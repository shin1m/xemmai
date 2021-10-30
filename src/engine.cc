#include <xemmai/convert.h>
#include <xemmai/io.h>
#include <thread>

namespace xemmai
{

void t_engine::f_collector()
{
	v_instance = this;
	if (v_options.v_verbose) std::fprintf(stderr, "collector starting...\n");
	t_object::v_roots.v_next = t_object::v_roots.v_previous = reinterpret_cast<t_object*>(&t_object::v_roots);
	while (true) {
		{
			std::unique_lock lock(v_collector__mutex);
			v_collector__running = false;
			v_collector__done.notify_all();
			do v_collector__wake.wait(lock); while (!v_collector__running);
			if (v_collector__quitting) break;
		}
		++v_collector__epoch;
		{
			std::lock_guard lock(v_object__reviving__mutex);
			v_object__reviving = false;
		}
		{
			std::lock_guard lock(v_thread__mutex);
			for (auto p = &v_thread__internals; *p;) {
				auto q = *p;
				if (q->v_done >= 0) {
					auto tail = q->v_increments.v_tail;
					q->f_epoch();
					std::lock_guard lock(v_object__reviving__mutex);
					if (q->v_reviving) {
						size_t n = t_slot::t_increments::V_SIZE;
						size_t epoch = (q->v_increments.v_tail + n - tail) % n;
						size_t reviving = (q->v_reviving + n - tail) % n;
						if (epoch < reviving)
							v_object__reviving = true;
						else
							q->v_reviving = nullptr;
					}
				}
				if (q->v_done < 3) {
					p = &q->v_next;
				} else {
					*p = q->v_next;
					delete q;
				}
			}
		}
		t_object* garbage = nullptr;
		while (v_cycles) {
			std::lock_guard lock(v_object__reviving__mutex);
			auto cycle = v_cycles;
			v_cycles = cycle->v_next_cycle;
			auto failed = false;
			auto p = cycle;
			while (true) {
				auto q = p->v_next;
				if (q->v_type) {
					if (q->v_color != e_color__ORANGE || q->v_cyclic > 0 || v_object__reviving && q->v_type->v_revive) failed = true;
					p = q;
					if (p == cycle) break;
				} else {
					p->v_next = q->v_next;
					f_free_as_collect(q);
					if (q == cycle) {
						cycle = p == q ? nullptr : p;
						break;
					}
				}
			}
			if (!cycle) continue;
			if (failed) {
				p = cycle;
				if (p->v_color == e_color__ORANGE) p->v_color = e_color__PURPLE;
				do {
					auto q = p->v_next;
					if (p->v_count <= 0) {
						p->v_next = garbage;
						garbage = p;
					} else if (p->v_color == e_color__PURPLE) {
						t_object::f_append(p);
					} else {
						p->v_color = e_color__BLACK;
						p->v_next = nullptr;
					}
					p = q;
				} while (p != cycle);
			} else {
				do p->v_color = e_color__RED; while ((p = p->v_next) != cycle);
				do p->f_cyclic_decrement(); while ((p = p->v_next) != cycle);
				do {
					auto q = p->v_next;
					if (p->v_type == v_type_type) p->f_as<t_type>().~t_type();
					f_free_as_collect(p);
					p = q;
				} while (p != cycle);
			}
		}
		while (garbage) {
			auto p = garbage;
			garbage = p->v_next;
			p->v_next = nullptr;
			p->f_loop<&t_object::f_decrement_step>();
		}
		for (auto& p = v_library__handle__finalizing; p;) {
			auto q = p;
			p = q->v_next;
			delete q;
		}
		auto roots = reinterpret_cast<t_object*>(&t_object::v_roots);
		if (roots->v_next != roots) {
			auto live = v_object__heap.f_live();
			if (live < v_object__lower) v_object__lower = live;
			if (live - v_object__lower >= v_options.v_collector__threshold) {
				v_object__lower = live;
				++v_collector__collect;
				{
					auto p = roots;
					auto q = p->v_next;
					do {
						assert(q->v_count > 0);
						if (q->v_color == e_color__PURPLE) {
							q->f_mark_gray();
							p = q;
						} else {
							p->v_next = q->v_next;
							q->v_next = nullptr;
						}
						q = p->v_next;
					} while (q != roots);
				}
				if (roots->v_next != roots) {
					{
						auto p = roots->v_next;
						do p->f_scan_gray(); while ((p = p->v_next) != roots);
					}
					do {
						auto p = roots->v_next;
						roots->v_next = p->v_next;
						if (p->v_color == e_color__WHITE) {
							p->f_collect_white();
							auto cycle = t_object::v_cycle;
							auto q = cycle;
							do q->f_step<&t_object::f_scan_red>(); while ((q = q->v_next) != cycle);
							do q->v_color = e_color__ORANGE; while ((q = q->v_next) != cycle);
							cycle->v_next_cycle = v_cycles;
							v_cycles = cycle;
						} else {
							p->v_next = nullptr;
						}
					} while (roots->v_next != roots);
				}
				roots->v_previous = roots;
			}
		}
		v_object__heap.f_flush();
	}
	if (v_options.v_verbose) std::fprintf(stderr, "collector quitting...\n");
	std::lock_guard lock(v_collector__mutex);
	v_collector__running = false;
	v_collector__done.notify_one();
}

void t_engine::f_debug_stop_and_wait(std::unique_lock<std::mutex>& a_lock)
{
	v_debug__stopping = true;
	size_t n = 0;
	for (auto p = v_thread__internals; p; p = p->v_next) if (p->v_done == 0) ++n;
	while (v_debug__safe < n) v_thread__condition.wait(a_lock);
}

void t_engine::f_debug_safe_point(std::unique_lock<std::mutex>& a_lock)
{
	if (v_debug__stopping) f_debug_enter_leave(a_lock);
}

void t_engine::f_debug_break_point(std::unique_lock<std::mutex>& a_lock)
{
	f_debug_break_point(a_lock, [this]
	{
		v_debugger->f_stopped(t_thread::v_current->v_thread);
	});
}

void t_engine::f_debug_script_loaded(t_debug_script& a_debug)
{
	std::unique_lock lock(v_thread__mutex);
	f_debug_break_point(lock, [&]
	{
		v_debugger->f_loaded(a_debug);
	});
}

void t_engine::f_debug_safe_region_leave(std::unique_lock<std::mutex>& a_lock)
{
	while (v_debug__stopping) v_thread__condition.wait(a_lock);
	assert(v_debug__safe > 0);
	--v_debug__safe;
}

t_engine::t_engine(const t_options& a_options, size_t a_count, char** a_arguments) : v_object__heap([]
{
	v_instance->f_tick();
}), v_options(a_options)
{
	v_instance = this;
#ifdef __unix__
	if (sem_init(&v_epoch__received, 0, 0) == -1) throw std::system_error(errno, std::generic_category());
	sigfillset(&v_epoch__notsigusr2);
	sigdelset(&v_epoch__notsigusr2, SIGUSR2);
	struct sigaction sa;
	sa.sa_handler = [](int)
	{
	};
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGUSR2, &sa, &v_epoch__old_sigusr2) == -1) throw std::system_error(errno, std::generic_category());
	sa.sa_handler = [](int)
	{
		t_thread::v_current->v_active->f_epoch_get();
		t_thread::v_current->f_epoch_get();
		v_instance->f_epoch_suspend();
	};
	sigaddset(&sa.sa_mask, SIGUSR2);
	if (sigaction(SIGUSR1, &sa, &v_epoch__old_sigusr1) == -1) throw std::system_error(errno, std::generic_category());
#endif
	v_thread__internals->f_initialize(v_options.v_stack_size, this);
	{
		std::unique_lock lock(v_collector__mutex);
		std::thread(&t_engine::f_collector, this).detach();
		do v_collector__done.wait(lock); while (v_collector__running);
	}
	auto type_object = f_allocate_for_type<t_type>(26);
	auto type = new(type_object->f_data()) t_type();
	type->v_derive = &t_type::f_do_derive;
	std::uninitialized_default_construct_n(type->f_fields(), 26);
	auto type_type = f_allocate_for_type<t_class>(0);
	v_type_type = new(type_type->f_data()) t_class(t_class::V_ids, type);
	type_object->f_be(v_type_type);
	type_type->f_be(v_type_type);
	{
		auto type_module__body = f_new_type_on_boot<t_module::t_body>(26, type, nullptr);
		auto global = type_module__body->f_as<t_type>().f_new<t_global>(type_object, type_type, type_module__body);
		v_module_global = t_module::f_new(L"__global"sv, global, global->f_as<t_global>().f_define());
	}
	v_fiber_exit = f_allocate(0);
	v_fiber_exit->f_be(type);
	v_thread = f_new<t_thread>(f_global(), v_thread__internals, f_new<t_fiber>(f_global(), nullptr, v_options.v_stack_size));
	v_thread__internals->f_initialize(&v_thread->f_as<t_thread>());
	auto path = t_list::f_instantiate();
	path->f_as<t_list>().v_owner = nullptr;
	if (auto p = std::getenv("XEMMAI_MODULE_PATH")) {
		std::wstring s = portable::f_convert(p);
#ifdef _WIN32
		s.erase(std::remove(s.begin(), s.end(), L'"'), s.end());
#endif
		std::wstring_view sv = s;
		size_t i = 0;
		while (true) {
#ifdef __unix__
			size_t j = sv.find(L':', i);
#endif
#ifdef _WIN32
			size_t j = sv.find(L';', i);
#endif
			if (j == std::wstring_view::npos) break;
			if (i < j) path->f_as<t_list>().f_push(f_global()->f_as(sv.substr(i, j - i)));
			i = j + 1;
		}
		if (i < sv.size()) path->f_as<t_list>().f_push(f_global()->f_as(sv.substr(i)));
	}
	std::wstring executable;
	portable::t_path script({});
	auto arguments = t_list::f_instantiate();
	if (a_count > 0) {
		executable = portable::f_convert(a_arguments[0]);
#ifdef XEMMAI_MODULE_PATH
		path->f_as<t_list>().f_push(f_global()->f_as(static_cast<const std::wstring&>(portable::t_path(portable::f_executable_path()) / std::wstring_view(L"../" XEMMAI__MACRO__LQ(XEMMAI_MODULE_PATH)))));
#endif
		if (a_count > 1) {
			script = {portable::f_convert(a_arguments[1])};
			path->f_as<t_list>().f_push(f_global()->f_as(static_cast<const std::wstring&>(script / L".."sv)));
			auto& as = arguments->f_as<t_list>();
			for (size_t i = 2; i < a_count; ++i) as.f_push(f_global()->f_as(portable::f_convert(a_arguments[i])));
		}
	}
	t_define system(f_global());
	system(L"path"sv, path);
	system(L"executable"sv, executable);
	system(L"script"sv, static_cast<const std::wstring&>(script));
	system(L"arguments"sv, arguments);
	{
		auto io = f_global()->f_type<t_module::t_body>()->f_new<t_io>();
		v_module_io = t_module::f_new(L"io"sv, io, io->f_as<t_io>().f_define());
	}
	{
		auto file = io::t_file::f_instantiate(stdin);
		system(L"raw_in"sv, file);
		auto tty = file->f_as<io::t_file>().f_tty();
		system(L"in"sv, io::t_reader::f_instantiate(file, L""sv, tty ? 1 : 1024));
	}
	{
		auto file = io::t_file::f_instantiate(stdout);
		system(L"raw_out"sv, file);
		system(L"out"sv, io::t_writer::f_instantiate(file, L""sv));
	}
	{
		auto file = io::t_file::f_instantiate(stderr);
		system(L"raw_error"sv, file);
		system(L"error"sv, io::t_writer::f_instantiate(file, L""sv));
	}
	v_module_system = t_module::f_new(L"system"sv, nullptr, system);
}

t_engine::~t_engine()
{
	v_module_global = nullptr;
	v_module_system = nullptr;
	v_module_io = nullptr;
	v_fiber_exit = nullptr;
	{
		auto internal = v_thread->f_as<t_thread>().v_internal;
		v_thread = nullptr;
		internal->f_epoch_get();
		std::lock_guard lock(v_thread__mutex);
		internal->v_active->v_thread = nullptr;
		++internal->v_done;
	}
	v_object__heap.f_return();
	v_options.v_collector__threshold = 0;
	f_wait();
	f_wait();
	f_wait();
	f_wait();
	{
		std::unique_lock lock(v_collector__mutex);
		v_collector__running = v_collector__quitting = true;
		v_collector__wake.notify_one();
		do v_collector__done.wait(lock); while (v_collector__running);
	}
	assert(!v_thread__internals);
#ifdef __unix__
	if (sem_destroy(&v_epoch__received) == -1) std::exit(errno);
	if (sigaction(SIGUSR1, &v_epoch__old_sigusr1, NULL) == -1) std::exit(errno);
	if (sigaction(SIGUSR2, &v_epoch__old_sigusr2, NULL) == -1) std::exit(errno);
#endif
	if (v_options.v_verbose) {
		std::fprintf(stderr, "statistics:\n\tobject:\n");
		size_t allocated = 0;
		size_t freed = 0;
		v_object__heap.f_statistics([&](auto a_rank, auto a_grown, auto a_allocated, auto a_freed)
		{
			std::fprintf(stderr, "\t\trank%zu: %zu: %zu - %zu = %zu\n", a_rank, a_grown, a_allocated, a_freed, a_allocated - a_freed);
			allocated += a_allocated;
			freed += a_freed;
		});
		std::fprintf(stderr, "\t\ttotal: %zu - %zu = %zu, release = %zu, collect = %zu\n", allocated, freed, allocated - freed, v_object__release, v_object__collect);
		std::fprintf(stderr, "\tcollector: tick = %zu, wait = %zu, epoch = %zu, collect = %zu\n", v_collector__tick, v_collector__wait, v_collector__epoch, v_collector__collect);
		if (allocated != freed) std::terminate();
	}
}

t_object* t_engine::f_fork(const t_pvalue& a_callable, size_t a_stack)
{
	auto internal = new t_thread::t_internal();
	{
		std::lock_guard lock(v_thread__mutex);
		internal->v_next = v_thread__internals;
		v_thread__internals = internal;
	}
	auto thread = f_new<t_thread>(f_global(), internal, t_fiber::f_instantiate(a_callable, a_stack));
	t_slot::t_increments::f_push(thread);
	try {
		std::thread([this, thread]
		{
			v_instance = this;
			auto& p = thread->f_as<t_thread>();
			auto internal = p.v_internal;
			{
				std::unique_lock lock(v_thread__mutex);
				if (v_debugger) f_debug_safe_point(lock);
				internal->f_initialize(p.v_fiber->f_as<t_fiber>().v_stack, &internal);
				internal->f_initialize(&p);
			}
			t_global::v_instance = &v_module_global->f_as<t_module>().v_body->f_as<t_global>();
			auto main = []
			{
				t_fiber::f_current()->f_as<t_fiber>().v_callable();
			};
			if (v_debugger)
				t_fiber::f_main<t_debug_context>(main);
			else
				t_fiber::f_main<t_context>(main);
			v_object__heap.f_return();
			{
				std::unique_lock lock(v_thread__mutex);
				if (v_debugger) {
					if (v_debug__stepping == &p) v_debug__stepping = nullptr;
					f_debug_safe_point(lock);
				}
				p.v_internal = nullptr;
			}
			t_slot::t_decrements::f_push(thread);
			internal->f_epoch_get();
			std::unique_lock lock(v_thread__mutex);
			internal->v_active->v_thread = nullptr;
			++internal->v_done;
			v_thread__condition.notify_all();
		}).detach();
	} catch (std::system_error&) {
		t_slot::t_decrements::f_push(thread);
		{
			std::lock_guard lock(v_thread__mutex);
			internal->v_done = 1;
		}
		f_throw(L"failed to create thread."sv);
	}
	return thread;
}

namespace
{

template<typename T_context>
void f_initialize_calls()
{
	f_global()->f_type<t_lambda>()->f_call = t_type_of<t_lambda>::f__do_call<t_lambda, T_context>;
	f_global()->f_type<t_lambda_shared>()->f_call = t_type_of<t_lambda>::f__do_call<t_lambda_shared, T_context>;
	f_global()->f_type<t_advanced_lambda<t_lambda>>()->f_call = t_type_of<t_advanced_lambda<t_lambda>>::f__do_call<T_context>;
	f_global()->f_type<t_advanced_lambda<t_lambda_shared>>()->f_call = t_type_of<t_advanced_lambda<t_lambda_shared>>::f__do_call<T_context>;
}

}

intptr_t t_engine::f_run(t_debugger* a_debugger)
{
	if (a_debugger) {
		f_initialize_calls<t_debug_context>();
		std::unique_lock lock(v_thread__mutex);
		v_debugger = a_debugger;
		v_debug__stopping = true;
		++v_debug__safe;
		v_debugger->f_stopped(&v_thread->f_as<t_thread>());
		f_debug_wait_and_leave(lock);
	} else {
		f_initialize_calls<t_context>();
	}
	intptr_t n = v_debugger ? t_fiber::f_main<t_debug_context>(t_module::f_main) : t_fiber::f_main<t_context>(t_module::f_main);
	auto& thread = v_thread->f_as<t_thread>();
	{
		std::unique_lock lock(v_thread__mutex);
		if (v_debugger) {
			if (v_debug__stepping == t_thread::v_current->v_thread) v_debug__stepping = nullptr;
			f_debug_enter_and_notify();
		}
		auto internal = thread.v_internal;
		while (true) {
			auto p = v_thread__internals;
			while (p == internal || p && p->v_done > 0) p = p->v_next;
			if (!p) break;
			v_thread__condition.wait(lock);
		}
	}
	if (v_debugger) {
		f_debug_safe_region_leave();
		v_debugger = nullptr;
		assert(v_debug__safe <= 0);
	}
	return n;
}

void t_engine::f_context_print(std::FILE* a_out, t_lambda* a_lambda, void** a_pc)
{
	if (a_lambda) {
		auto& code = a_lambda->f_code()->f_as<t_code>();
		auto& path = code.v_module->f_as<t_script>().v_path;
		std::fprintf(a_out, "%ls", path.c_str());
		const t_at* at = code.f_at(a_pc);
		if (at) {
			std::fprintf(a_out, ":%zu:%zu\n", at->v_line, at->v_column);
			f_print_with_caret(a_out, path, at->v_position, at->v_column);
		} else {
			std::fputc('\n', a_out);
		}
	} else {
		std::fputs("<fiber>\n", a_out);
	}
}

void t_engine::f_debug_safe_point()
{
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	std::unique_lock lock(v_thread__mutex);
	if (v_debug__stepping == t_thread::v_current->v_thread)
		f_debug_break_point(lock);
	else
		f_debug_safe_point(lock);
}

void t_engine::f_debug_break_point()
{
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	std::unique_lock lock(v_thread__mutex);
	f_debug_break_point(lock);
}

void t_engine::f_debug_safe_region_enter()
{
	std::lock_guard lock(v_thread__mutex);
	f_debug_enter_and_notify();
}

void t_engine::f_debug_safe_region_leave()
{
	std::unique_lock lock(v_thread__mutex);
	f_debug_safe_region_leave(lock);
}

void t_engine::f_debug_stop()
{
	std::unique_lock lock(v_thread__mutex);
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	if (v_debug__stopping) return;
	f_debug_stop_and_wait(lock);
	v_debugger->f_stopped(&v_thread->f_as<t_thread>());
}

void t_engine::f_debug_continue(t_thread* a_stepping)
{
	std::lock_guard lock(v_thread__mutex);
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	if (!v_debug__stopping) f_throw(L"already running"sv);
	v_debug__stopping = false;
	v_debug__stepping = a_stepping;
	v_thread__condition.notify_all();
}

#ifdef _WIN32
void t_slot::t_increments::f_push(t_object* a_object)
{
	f__push(a_object);
}

void t_slot::t_decrements::f_push(t_object* a_object)
{
	f__push(a_object);
}

t_object* t_engine::f_allocate(size_t a_size)
{
	return f__allocate(a_size);
}

t_engine* f_engine()
{
	return t_engine::v_instance;
}
#endif

}
