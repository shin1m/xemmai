#include <xemmai/engine.h>

#include <xemmai/portable/thread.h>
#include <xemmai/structure.h>
#include <xemmai/array.h>
#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

void t_engine::t_synchronizer::f_run()
{
	{
		portable::t_affinity affinity;
		affinity.f_clear();
		affinity.f_add(v_cpu);
		affinity.f_to_thread();
	}
	if (v_engine->v_verbose) std::fprintf(stderr, "synchronizer(%" PRIuPTR ") starting...\n", static_cast<uintptr_t>(v_cpu));
	{
		std::unique_lock<std::mutex> lock(v_mutex);
		while (true) {
			while (!v_wake) v_condition.wait(lock);
			v_wake = false;
			if (v_engine->v_collector__quitting) break;
			{
				std::lock_guard<std::mutex> lock(v_engine->v_synchronizer__mutex);
				if (--v_engine->v_synchronizer__wake > 0) continue;
			}
//			v_engine->v_synchronizer__condition.notify_one();
		}
	}
	if (v_engine->v_verbose) std::fprintf(stderr, "synchronizer(%" PRIuPTR ") quitting...\n", static_cast<uintptr_t>(v_cpu));
	v_condition.notify_one();
}

XEMMAI__PORTABLE__THREAD size_t t_engine::v_local_object__allocated;

void t_engine::f_pools__return()
{
	v_object__pool0.f_return_all();
	v_object__pool1.f_return_all();
	v_object__pool2.f_return_all();
	v_object__pool3.f_return_all();
	v_object__allocated += v_local_object__allocated;
	v_local_object__allocated = 0;
}

void t_engine::f_signal_synchronizers()
{
	if (!v_synchronizers) return;
	size_t cpu = portable::f_cpu();
	for (t_synchronizer* p = v_synchronizers; p; p = p->v_next) if (p->v_cpu != cpu) ++v_synchronizer__wake;
	for (t_synchronizer* p = v_synchronizers; p; p = p->v_next) {
		if (p->v_cpu == cpu) continue;
		{
			std::lock_guard<std::mutex> lock(p->v_mutex);
			p->v_wake = true;
		}
		p->v_condition.notify_one();
	}
}

void t_engine::f_wait_synchronizers()
{
/*	if (v_synchronizers) {
		std::unique_lock<std::mutex> lock(v_synchronizer__mutex);
		while (v_synchronizer__wake > 0) v_synchronizer__condition.wait(lock);
	}*/
	while (v_synchronizer__wake > 0) std::this_thread::yield();
}

void t_engine::f_collector()
{
	t_value::v_collector = this;
	if (v_verbose) std::fprintf(stderr, "collector starting...\n");
	while (true) {
		{
			std::unique_lock<std::mutex> lock(v_collector__mutex);
			v_collector__running = false;
			v_collector__done.notify_all();
			do v_collector__wake.wait(lock); while (!v_collector__running);
		}
		if (v_collector__quitting) {
			if (v_verbose) std::fprintf(stderr, "collector quitting...\n");
			std::lock_guard<std::mutex> lock(v_collector__mutex);
			v_collector__running = false;
			v_collector__done.notify_one();
			break;
		}
		++v_collector__epoch;
		{
			std::lock_guard<std::mutex> lock(v_object__reviving__mutex);
			v_object__reviving = false;
		}
		{
			std::lock_guard<std::mutex> lock(v_thread__mutex);
			for (auto p = v_thread__internals; p; p = p->v_next) {
				p->v_increments.f_epoch();
				p->v_decrements.f_epoch();
			}
			f_signal_synchronizers();
			{
				std::lock_guard<std::mutex> lock(v_object__reviving__mutex);
				for (auto p = v_thread__internals; p; p = p->v_next) {
					if (p->v_done > 0) ++p->v_done;
					if (!p->v_reviving) continue;
					t_object* volatile* tail = p->v_increments.v_tail + 1;
					size_t epoch = (p->v_increments.v_epoch + t_value::t_increments::V_SIZE - tail) % t_value::t_increments::V_SIZE;
					size_t reviving = (p->v_reviving + t_value::t_increments::V_SIZE - tail) % t_value::t_increments::V_SIZE;
					if (epoch > reviving)
						p->v_reviving = nullptr;
					else
						v_object__reviving = true;
				}
			}
			f_wait_synchronizers();
			for (auto p = &v_thread__internals; *p;) {
				auto q = *p;
				q->v_increments.f_flush();
				q->v_decrements.f_flush();
				if (q->v_done < 3) {
					p = &(*p)->v_next;
				} else {
					*p = q->v_next;
					v_thread__cache_hit += q->v_cache_hit;
					v_thread__cache_missed += q->v_cache_missed;
					delete q;
				}
			}
		}
		t_object::f_collect();
		if (v_object__pool0.v_freed > 0) v_object__pool0.f_return();
		if (v_object__pool1.v_freed > 0) v_object__pool1.f_return();
		if (v_object__pool2.v_freed > 0) v_object__pool2.f_return();
		if (v_object__pool3.v_freed > 0) v_object__pool3.f_return();
	}
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
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	f_debug_break_point(lock, &t_debugger::f_loaded);
}

void t_engine::f_debug_safe_region_leave(std::unique_lock<std::mutex>& a_lock)
{
	while (v_debug__stopping) v_thread__condition.wait(a_lock);
	assert(v_debug__safe > 0);
	--v_debug__safe;
}

t_engine::t_engine(size_t a_stack, bool a_verbose, size_t a_count, char** a_arguments) : v_collector__threshold0(1024 * 8), v_collector__threshold1(1024 * 16), v_stack_size(a_stack), v_verbose(a_verbose)
{
	v_thread__internals->f_initialize();
	auto structure_root = f_object__allocate_on_boot(sizeof(t_structure));
	t_value::v_increments->f_push(structure_root);
	v_structure_root = new(structure_root->f_data()) t_structure();
	t_value::v_increments->f_push(structure_root);
	structure_root->v_structure = v_structure_root;
	auto type_object = f_object__allocate_on_boot(sizeof(t_type));
	t_value::v_increments->f_push(structure_root);
	auto type = new(type_object->f_data()) t_type(t_type::V_ids);
	auto type_class = f_object__allocate_on_boot(sizeof(t_class));
	t_value::v_increments->f_push(structure_root);
	v_type_class = new(type_class->f_data()) t_class(t_class::V_ids, type);
	auto type_structure = f_object__allocate_on_boot(sizeof(t_type_of<t_structure>));
	t_value::v_increments->f_push(structure_root);
	v_type_structure = new(type_structure->f_data()) t_type_of<t_structure>(t_type_of<t_structure>::V_ids, type);
	t_value::v_increments->f_push(type_structure);
	structure_root->v_type = v_type_structure;
	v_object__pool0.f_grow();
	v_object__pool1.f_grow();
	v_object__pool2.f_grow();
	v_object__pool3.f_grow();
	auto type_module = type->f_derive<t_type_of<t_module>>();
	type_module->f_as<t_type>().v_revive = true;
	v_module_global = type_module->f_as<t_type>().f_new<t_library>(true, L""sv, nullptr);
	auto& library = v_module_global->f_as<t_library>();
	v_module__instances__null = v_module__instances.emplace(L""sv, t_slot()).first;
	library.v_iterator = v_module__instances.emplace(L"__global"sv, t_slot()).first;
	library.v_iterator->second = v_module_global;
	type->v_module = v_module_global;
	v_type_class->v_module = v_module_global;
	v_type_structure->v_module = v_module_global;
	type_module->f_as<t_type>().v_module = v_module_global;
	auto type_fiber = type->f_derive<t_type_of<t_fiber>>();
	v_fiber_exit = t_object::f_allocate(type, true, 0);
	auto type_thread = type->f_derive<t_type_of<t_thread>>();
	{
		auto fiber = type_fiber->f_as<t_type>().f_new<t_fiber>(false, nullptr, v_stack_size, true, true);
		v_thread = type_thread->f_as<t_type>().f_new<t_thread>(true, v_thread__internals, t_scoped(fiber));
		v_thread__internals->v_thread = t_thread::v_current = v_thread;
		v_thread->f_as<t_thread>().v_active = std::move(fiber);
	}
	{
		portable::t_affinity affinity;
		affinity.f_from_thread();
		size_t n = 0;
		for (size_t i = 0; i < portable::t_affinity::V_SIZE; ++i) if (affinity.f_contains(i)) ++n;
		if (n > 1) {
			n = portable::t_affinity::V_SIZE;
			while (n > 0) {
				if (!affinity.f_contains(--n)) continue;
				auto p = new t_synchronizer(this, n);
				std::thread(&t_synchronizer::f_run, p).detach();
			}
		}
	}
	{
		std::unique_lock<std::mutex> lock(v_collector__mutex);
		std::thread(&t_engine::f_collector, this).detach();
		while (v_collector__running) v_collector__done.wait(lock);
	}
	library.v_extension = new t_global(v_module_global, std::move(type_object), std::move(type_class), std::move(type_structure), std::move(type_module), std::move(type_fiber), std::move(type_thread));
	v_module_system = t_module::f_new<t_module>(L"system"sv, L""sv);
	auto path = t_array::f_instantiate();
	static_cast<t_object*>(path)->v_owner = nullptr;
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
	if (a_count > 0) {
		v_module_system.f_put(f_global()->f_symbol_executable(), f_global()->f_as(static_cast<const std::wstring&>(portable::t_path(portable::f_convert(a_arguments[0])))));
		if (a_count > 1) {
			portable::t_path script(portable::f_convert(a_arguments[1]));
			v_module_system.f_put(f_global()->f_symbol_script(), f_global()->f_as(static_cast<const std::wstring&>(script)));
			f_as<t_array&>(path).f_push(f_global()->f_as(static_cast<const std::wstring&>(script / L".."sv)));
			t_scoped arguments = t_array::f_instantiate();
			auto& p = f_as<t_array&>(arguments);
			for (size_t i = 2; i < a_count; ++i) p.f_push(f_global()->f_as(portable::f_convert(a_arguments[i])));
			v_module_system.f_put(f_global()->f_symbol_arguments(), std::move(arguments));
		}
	}
	v_module_system.f_put(f_global()->f_symbol_path(), std::move(path));
	v_module_io = t_module::f_new<t_library>(L"io"sv, L""sv, nullptr);
	v_module_io->f_as<t_library>().v_extension = new t_io(v_module_io);
	{
		auto file = io::t_file::f_instantiate(stdin);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"raw_in"sv), t_scoped(file));
		bool tty = f_as<io::t_file&>(file).f_tty();
		auto reader = io::t_reader::f_instantiate(std::move(file), L""sv, tty ? 1 : 1024);
		static_cast<t_object*>(reader)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"in"sv), std::move(reader));
	}
	{
		auto file = io::t_file::f_instantiate(stdout);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"raw_out"sv), t_scoped(file));
		auto writer = io::t_writer::f_instantiate(std::move(file), L""sv);
		static_cast<t_object*>(writer)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"out"sv), std::move(writer));
	}
	{
		auto file = io::t_file::f_instantiate(stderr);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"raw_error"sv), t_scoped(file));
		auto writer = io::t_writer::f_instantiate(std::move(file), L""sv);
		static_cast<t_object*>(writer)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"error"sv), std::move(writer));
	}
}

t_engine::~t_engine()
{
	t_thread::f_cache_clear();
	t_value::v_decrements->f_push(t_object::f_of(v_structure_root));
	v_module_global = nullptr;
	v_module_system = nullptr;
	v_module_io = nullptr;
	v_fiber_exit = nullptr;
	{
		auto& thread = f_as<t_thread&>(v_thread);
		thread.v_active = nullptr;
		v_thread = nullptr;
		std::lock_guard<std::mutex> lock(v_thread__mutex);
		auto internal = thread.v_internal;
		++internal->v_done;
		internal->v_cache_hit = t_thread::v_cache_hit;
		internal->v_cache_missed = t_thread::v_cache_missed;
	}
	f_pools__return();
	v_collector__threshold1 = 0;
	f_wait();
	f_wait();
	f_wait();
	f_wait();
	{
		std::unique_lock<std::mutex> lock(v_collector__mutex);
		v_collector__running = v_collector__quitting = true;
		v_collector__wake.notify_one();
		do v_collector__done.wait(lock); while (v_collector__running);
	}
	if (v_synchronizers) {
		do {
			{
				std::lock_guard<std::mutex> lock(v_synchronizers->v_mutex);
				v_synchronizers->v_wake = true;
			}
			v_synchronizers->v_condition.notify_one();
			{
				std::unique_lock<std::mutex> lock(v_synchronizers->v_mutex);
				while (v_synchronizers->v_wake) v_synchronizers->v_condition.wait(lock);
			}
			delete v_synchronizers;
		} while (v_synchronizers);
	}
	assert(!v_thread__internals);
	v_object__pool0.f_clear();
	v_object__pool1.f_clear();
	v_object__pool2.f_clear();
	v_object__pool3.f_clear();
	if (v_verbose) {
		std::fprintf(stderr, "statistics:\n\tt_object:\n");
		size_t allocated = 0;
		size_t freed = 0;
		auto f = [&](auto& a_pool, size_t a_rank)
		{
			size_t x = a_pool.f_allocated();
			size_t y = a_pool.f_freed();
			std::fprintf(stderr, "\t\trank%d: %" PRIuPTR " - %" PRIuPTR " = %" PRIuPTR "\n", a_rank, static_cast<uintptr_t>(x), static_cast<uintptr_t>(y), static_cast<uintptr_t>(x - y));
			allocated += x;
			freed += y;
		};
		f(v_object__pool0, 0);
		f(v_object__pool1, 1);
		f(v_object__pool2, 2);
		f(v_object__pool3, 3);
		std::fprintf(stderr, "\t\trank4: %" PRIuPTR " - %" PRIuPTR " = %" PRIuPTR "\n", static_cast<uintptr_t>(v_object__allocated), static_cast<uintptr_t>(v_object__freed), static_cast<uintptr_t>(v_object__allocated - v_object__freed));
		allocated += v_object__allocated;
		freed += v_object__freed;
		std::fprintf(stderr, "\t\ttotal: %" PRIuPTR " - %" PRIuPTR " = %" PRIuPTR ", release = %" PRIuPTR ", collect = %" PRIuPTR "\n", static_cast<uintptr_t>(allocated), static_cast<uintptr_t>(freed), static_cast<uintptr_t>(allocated - freed), static_cast<uintptr_t>(v_object__release), static_cast<uintptr_t>(v_object__collect));
		std::fprintf(stderr, "\tcollector: tick = %" PRIuPTR ", wait = %" PRIuPTR ", epoch = %" PRIuPTR ", release = %" PRIuPTR ", collect = %" PRIuPTR "\n", static_cast<uintptr_t>(v_collector__tick), static_cast<uintptr_t>(v_collector__wait), static_cast<uintptr_t>(v_collector__epoch), static_cast<uintptr_t>(v_collector__release), static_cast<uintptr_t>(v_collector__collect));
		{
			size_t base = v_thread__cache_hit + v_thread__cache_missed;
			std::fprintf(stderr, "\tfield cache: hit = %" PRIuPTR ", missed = %" PRIuPTR ", ratio = %.1f%%\n", static_cast<uintptr_t>(v_thread__cache_hit), static_cast<uintptr_t>(v_thread__cache_missed), base > 0 ? v_thread__cache_hit * 100.0 / base : 0.0);
		}
		if (allocated > freed) throw std::exception();
	}
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
		std::unique_lock<std::mutex> lock(v_thread__mutex);
		v_debugger = a_debugger;
		v_debug__stopping = true;
		++v_debug__safe;
		v_debugger->f_stopped(v_thread);
		f_debug_wait_and_leave(lock);
	} else {
		f_initialize_calls<t_context>();
	}
	intptr_t n = v_debugger ? t_fiber::f_main<t_debug_context>(t_module::f_main) : t_fiber::f_main<t_context>(t_module::f_main);
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	if (v_debugger) {
		if (v_debug__stepping == t_thread::f_current()) v_debug__stepping = nullptr;
		f_debug_enter_and_notify();
	}
	auto& thread = f_as<t_thread&>(v_thread);
	auto internal = thread.v_internal;
	while (true) {
		auto p = v_thread__internals;
		while (p == internal || p && p->v_done > 0) p = p->v_next;
		if (!p) break;
		v_thread__condition.wait(lock);
	}
	auto& fiber = f_as<t_fiber&>(thread.v_fiber);
	fiber.v_return = ++fiber.v_stack.v_used;
	while (!v_fiber__runnings.empty()) {
		t_object* x = v_fiber__runnings.front();
		auto& p = f_as<t_fiber&>(x);
		p.v_active = true;
		fiber.v_active = false;
		thread.v_active = x;
		t_stack::v_instance = &p.v_stack;
		p.v_throw = true;
		p.v_return->f_construct(v_fiber_exit);
		p.v_fiber.f_swap(fiber.v_fiber);
		fiber.v_return->f_destruct();
	}
	if (v_debugger) {
		f_debug_safe_region_leave(lock);
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
			std::fprintf(a_out, ":%" PRIuPTR ":%" PRIuPTR "\n", static_cast<uintptr_t>(at->f_line()), static_cast<uintptr_t>(at->f_column()));
			f_print_with_caret(a_out, path, at->f_position(), at->f_column());
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
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	if (v_debug__stepping == t_thread::f_current())
		f_debug_break_point(lock);
	else
		f_debug_safe_point(lock);
}

void t_engine::f_debug_break_point()
{
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	f_debug_break_point(lock);
}

void t_engine::f_debug_safe_region_enter()
{
	std::lock_guard<std::mutex> lock(v_thread__mutex);
	f_debug_enter_and_notify();
}

void t_engine::f_debug_safe_region_leave()
{
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	f_debug_safe_region_leave(lock);
}

void t_engine::f_debug_stop()
{
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	if (v_debug__stopping) return;
	f_debug_stop_and_wait(lock);
	v_debugger->f_stopped(v_thread);
}

void t_engine::f_debug_continue(t_object* a_stepping)
{
	std::lock_guard<std::mutex> lock(v_thread__mutex);
	if (!v_debugger) f_throw(L"not in debug mode"sv);
	if (!v_debug__stopping) f_throw(L"already running"sv);
	v_debug__stopping = false;
	v_debug__stepping = a_stepping;
	v_thread__condition.notify_all();
}

}
