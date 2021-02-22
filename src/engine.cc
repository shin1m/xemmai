#include <xemmai/engine.h>
#include <xemmai/structure.h>
#include <xemmai/array.h>
#include <xemmai/io.h>
#include <xemmai/convert.h>

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
					v_thread__cache_hit += q->v_cache_hit;
					v_thread__cache_missed += q->v_cache_missed;
					delete q;
				}
			}
		}
		while (v_cycles) {
			std::lock_guard lock(v_object__reviving__mutex);
			auto cycle = v_cycles;
			v_cycles = cycle->v_next_cycle;
			auto p = cycle;
			auto mutated = [&]
			{
				if (v_object__reviving)
					do if (p->v_color != e_color__ORANGE || p->v_cyclic > 0 || p->v_type->v_revive) return true; while ((p = p->v_next) != cycle);
				else
					do if (p->v_color != e_color__ORANGE || p->v_cyclic > 0) return true; while ((p = p->v_next) != cycle);
				return false;
			};
			if (mutated()) {
				p = cycle;
				auto q = p->v_next;
				if (p->v_color == e_color__ORANGE) {
					p->v_color = e_color__PURPLE;
					t_object::f_append(p);
				} else if (p->v_color == e_color__PURPLE) {
					t_object::f_append(p);
				} else {
					p->v_color = e_color__BLACK;
					p->v_next = nullptr;
				}
				while (q != cycle) {
					p = q;
					q = p->v_next;
					if (p->v_color == e_color__PURPLE) {
						t_object::f_append(p);
					} else {
						p->v_color = e_color__BLACK;
						p->v_next = nullptr;
					}
				}
			} else {
				do p->v_color = e_color__RED; while ((p = p->v_next) != cycle);
				do p->f_cyclic_decrement(); while ((p = p->v_next) != cycle);
				do {
					auto q = p->v_next;
					if (p->v_type == v_type_class)
						p->f_as<t_type>().f_destruct();
					else if (p->v_type == v_type_structure)
						p->f_as<t_structure>().~t_structure();
					f_free_as_collect(p);
					p = q;
				} while (p != cycle);
			}
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
				if (roots->v_next == roots) {
					roots->v_previous = roots;
				} else {
					{
						auto p = roots->v_next;
						do {
							p->f_scan_gray();
							p = p->v_next;
						} while (p != roots);
					}
					do {
						auto p = roots->v_next;
						roots->v_next = p->v_next;
						if (p->v_color == e_color__WHITE) {
							p->f_collect_white();
							t_object::v_cycle->v_next_cycle = v_cycles;
							v_cycles = t_object::v_cycle;
						} else {
							p->v_next = nullptr;
						}
					} while (roots->v_next != roots);
					roots->v_previous = roots;
					for (auto cycle = v_cycles; cycle; cycle = cycle->v_next_cycle) {
						auto p = cycle;
						do {
							p->v_color = e_color__RED;
							p->v_cyclic = p->v_count;
						} while ((p = p->v_next) != cycle);
						do p->f_step<&t_object::f_scan_red>(); while ((p = p->v_next) != cycle);
						do p->v_color = e_color__ORANGE; while ((p = p->v_next) != cycle);
					}
				}
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
	for (auto p = v_thread__internals; p; p = p->v_next) if (p->v_done <= 0 && p->v_thread) ++n;
	while (v_debug__safe < n) v_thread__condition.wait(a_lock);
}

void t_engine::f_debug_safe_point(std::unique_lock<std::mutex>& a_lock)
{
	if (v_debug__stopping) f_debug_enter_leave(a_lock);
}

void t_engine::f_debug_break_point(std::unique_lock<std::mutex>& a_lock)
{
	f_debug_break_point(a_lock, &t_debugger::f_stopped);
}

void t_engine::f_debug_script_loaded()
{
	std::unique_lock lock(v_thread__mutex);
	f_debug_break_point(lock, &t_debugger::f_loaded);
}

void t_engine::f_debug_safe_region_leave(std::unique_lock<std::mutex>& a_lock)
{
	while (v_debug__stopping) v_thread__condition.wait(a_lock);
	assert(v_debug__safe > 0);
	--v_debug__safe;
}

t_engine::t_engine(const t_options& a_options, size_t a_count, char** a_arguments) : v_object__heap([]
{
	v_instance->f_wait();
}), v_options(a_options)
{
	v_instance = this;
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
		t_thread::v_current->f_epoch_get();
		v_instance->f_epoch_suspend();
	};
	sigaddset(&sa.sa_mask, SIGUSR2);
	if (sigaction(SIGUSR1, &sa, &v_epoch__old_sigusr1) == -1) throw std::system_error(errno, std::generic_category());
	v_thread__internals->f_initialize();
	{
		std::unique_lock lock(v_collector__mutex);
		std::thread(&t_engine::f_collector, this).detach();
		do v_collector__done.wait(lock); while (v_collector__running);
	}
	auto structure_root = f_allocate_on_boot(sizeof(t_structure));
	t_slot::t_increments::f_push(structure_root);
	v_structure_root = new(structure_root->f_data()) t_structure();
	t_slot::t_increments::f_push(structure_root);
	structure_root->v_structure = v_structure_root;
	auto type_object = f_allocate_on_boot(sizeof(t_type));
	t_slot::t_increments::f_push(structure_root);
	auto type = new(type_object->f_data()) t_type(t_type::V_ids);
	auto type_class = f_allocate_on_boot(sizeof(t_class));
	t_slot::t_increments::f_push(structure_root);
	v_type_class = new(type_class->f_data()) t_class(t_class::V_ids, type);
	type_object->f_be(v_type_class);
	type_class->f_be(v_type_class);
	auto type_structure = f_allocate_on_boot(sizeof(t_type_of<t_structure>));
	t_slot::t_increments::f_push(structure_root);
	v_type_structure = new(type_structure->f_data()) t_type_of<t_structure>(t_type_of<t_structure>::V_ids, type);
	type_structure->f_be(v_type_class);
	structure_root->f_be(v_type_structure);
	auto type_module = type->f_derive<t_type_of<t_module>>();
	type_module->f_as<t_type>().v_revive = true;
	v_module_global = type_module->f_as<t_type>().f_new<t_library>(true, v_module__instances.emplace(L"__global"sv, nullptr).first, L""sv, nullptr);
	type->v_module = v_module_global;
	v_type_class->v_module = v_module_global;
	v_type_structure->v_module = v_module_global;
	type_module->f_as<t_type>().v_module = v_module_global;
	auto type_fiber = type->f_derive<t_type_of<t_fiber>>();
	v_fiber_exit = f_allocate(true, 0);
	v_fiber_exit->f_be(type);
	auto type_thread = type->f_derive<t_type_of<t_thread>>();
	v_thread = type_thread->f_as<t_type>().f_new<t_thread>(true, v_thread__internals, type_fiber->f_as<t_type>().f_new<t_fiber>(false, nullptr, v_options.v_stack_size));
	v_thread__internals->f_initialize(&f_as<t_thread&>(v_thread), this);
	v_module_global->f_as<t_library>().v_extension = new t_global(v_module_global, type_object, type_class, type_structure, type_module, type_fiber, type_thread);
	v_module_system = t_module::f_new<t_module>(L"system"sv, L""sv);
	auto path = t_array::f_instantiate();
	path->v_owner = nullptr;
	{
		char* p = std::getenv("XEMMAI_MODULE_PATH");
		if (p != NULL) {
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
				if (i < j) f_as<t_array&>(path).f_push(f_global()->f_as(sv.substr(i, j - i)));
				i = j + 1;
			}
			if (i < sv.size()) f_as<t_array&>(path).f_push(f_global()->f_as(sv.substr(i)));
		}
	}
#ifdef XEMMAI_MODULE_PATH
	f_as<t_array&>(path).f_push(f_global()->f_as(std::wstring_view(XEMMAI__MACRO__LQ(XEMMAI_MODULE_PATH))));
#endif
	t_pvalue system{v_module_system};
	if (a_count > 0) {
		system.f_put(f_global()->f_symbol_executable(), f_global()->f_as(static_cast<const std::wstring&>(portable::t_path(portable::f_convert(a_arguments[0])))));
		if (a_count > 1) {
			portable::t_path script(portable::f_convert(a_arguments[1]));
			system.f_put(f_global()->f_symbol_script(), f_global()->f_as(static_cast<const std::wstring&>(script)));
			f_as<t_array&>(path).f_push(f_global()->f_as(static_cast<const std::wstring&>(script / L".."sv)));
			auto arguments = t_array::f_instantiate();
			auto& p = f_as<t_array&>(arguments);
			for (size_t i = 2; i < a_count; ++i) p.f_push(f_global()->f_as(portable::f_convert(a_arguments[i])));
			system.f_put(f_global()->f_symbol_arguments(), arguments);
		}
	}
	system.f_put(f_global()->f_symbol_path(), path);
	v_module_io = t_module::f_new<t_library>(L"io"sv, L""sv, nullptr);
	v_module_io->f_as<t_library>().v_extension = new t_io(v_module_io);
	{
		auto file = io::t_file::f_instantiate(stdin);
		file->v_owner = nullptr;
		system.f_put(t_symbol::f_instantiate(L"raw_in"sv), file);
		bool tty = f_as<io::t_file&>(file).f_tty();
		auto reader = io::t_reader::f_instantiate(file, L""sv, tty ? 1 : 1024);
		reader->v_owner = nullptr;
		system.f_put(t_symbol::f_instantiate(L"in"sv), reader);
	}
	{
		auto file = io::t_file::f_instantiate(stdout);
		file->v_owner = nullptr;
		system.f_put(t_symbol::f_instantiate(L"raw_out"sv), file);
		auto writer = io::t_writer::f_instantiate(file, L""sv);
		writer->v_owner = nullptr;
		system.f_put(t_symbol::f_instantiate(L"out"sv), writer);
	}
	{
		auto file = io::t_file::f_instantiate(stderr);
		file->v_owner = nullptr;
		system.f_put(t_symbol::f_instantiate(L"raw_error"sv), file);
		auto writer = io::t_writer::f_instantiate(file, L""sv);
		writer->v_owner = nullptr;
		system.f_put(t_symbol::f_instantiate(L"error"sv), writer);
	}
}

t_engine::~t_engine()
{
	t_thread::f_cache_clear();
	t_slot::t_decrements::f_push(t_object::f_of(v_structure_root));
	v_module_global = nullptr;
	v_module_system = nullptr;
	v_module_io = nullptr;
	v_fiber_exit = nullptr;
	{
		auto internal = f_as<t_thread&>(v_thread).v_internal;
		v_thread = nullptr;
		internal->f_epoch_get();
		std::lock_guard lock(v_thread__mutex);
		internal->v_active->v_internal->v_thread = nullptr;
		++internal->v_done;
		internal->v_cache_hit = t_thread::v_cache_hit;
		internal->v_cache_missed = t_thread::v_cache_missed;
	}
	f_object__return();
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
	if (sem_destroy(&v_epoch__received) == -1) std::exit(errno);
	if (sigaction(SIGUSR1, &v_epoch__old_sigusr1, NULL) == -1) std::exit(errno);
	if (sigaction(SIGUSR2, &v_epoch__old_sigusr2, NULL) == -1) std::exit(errno);
	if (v_options.v_verbose) {
		std::fprintf(stderr, "statistics:\n\tt_object:\n");
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
		{
			size_t base = v_thread__cache_hit + v_thread__cache_missed;
			std::fprintf(stderr, "\tfield cache: hit = %zu, missed = %zu, ratio = %.1f%%\n", v_thread__cache_hit, v_thread__cache_missed, base > 0 ? v_thread__cache_hit * 100.0 / base : 0.0);
		}
		if (allocated != freed) std::terminate();
	}
}

t_object* t_engine::f_fork(const t_pvalue& a_callable, size_t a_stack)
{
	auto internal = new t_thread::t_internal();
	internal->v_thread = nullptr;
	{
		std::lock_guard lock(v_thread__mutex);
		internal->v_next = v_thread__internals;
		v_thread__internals = internal;
	}
	auto thread = f_new<t_thread>(f_global(), true, internal, t_fiber::f_instantiate(a_callable, a_stack));
	t_slot::t_increments::f_push(thread);
	t_thread::f_cache_release();
	try {
		std::thread([this, thread]
		{
			v_instance = this;
			auto& p = f_as<t_thread&>(thread);
			auto internal = p.v_internal;
			{
				std::unique_lock lock(v_thread__mutex);
				if (v_debugger) f_debug_safe_point(lock);
				internal->f_initialize();
				internal->f_initialize(&p, &internal);
			}
			t_global::v_instance = f_extension<t_global>(f_module_global());
			auto main = []
			{
				f_as<t_fiber&>(t_fiber::f_current()).v_callable();
			};
			if (v_debugger)
				t_fiber::f_main<t_debug_context>(main);
			else
				t_fiber::f_main<t_context>(main);
			t_thread::f_cache_clear();
			f_object__return();
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
			internal->v_active->v_internal->v_thread = nullptr;
			++internal->v_done;
			internal->v_cache_hit = t_thread::v_cache_hit;
			internal->v_cache_missed = t_thread::v_cache_missed;
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
		v_debugger->f_stopped(&f_as<t_thread&>(v_thread));
		f_debug_wait_and_leave(lock);
	} else {
		f_initialize_calls<t_context>();
	}
	intptr_t n = v_debugger ? t_fiber::f_main<t_debug_context>(t_module::f_main) : t_fiber::f_main<t_context>(t_module::f_main);
	auto& thread = f_as<t_thread&>(v_thread);
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
		auto& code = f_as<t_code&>(a_lambda->f_code());
		auto& path = f_as<t_module&>(code.v_module).v_path;
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
	v_debugger->f_stopped(&f_as<t_thread&>(v_thread));
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

}
