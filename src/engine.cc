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

void t_engine::f_pools__return()
{
	f_return(v_object__pool);
	f_return(v_dictionary__entry__pool);
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
		if (v_object__freed > 0) f_return(v_object__pool, v_object__freed);
		if (v_dictionary__entry__freed > 0) f_return(v_dictionary__entry__pool, v_dictionary__entry__freed);
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
	v_object__pool.f_grow();
	auto thread = new t_thread(nullptr);
	thread->v_internal->f_initialize();
	thread->v_internal->v_next = v_thread__internals;
	v_thread__internals = thread->v_internal;
	auto type_object = new t_type(t_type::V_ids);
	v_type_class = new t_class(t_class::V_ids, type_object);
	auto type_structure = new t_type_of<t_structure>(t_type_of<t_structure>::V_ids, type_object);
	v_structure_root = t_object::f_allocate_on_boot();
	t_value::v_increments->f_push(type_structure->v_this);
	static_cast<t_object*>(v_structure_root)->v_type = type_structure;
	auto root = new(0) t_structure(t_scoped(v_structure_root));
	v_structure_root.f_pointer__(root);
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(type_object->v_this)->v_structure = root;
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(v_type_class->v_this)->v_structure = root;
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(type_structure->v_this)->v_structure = root;
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(v_structure_root)->v_structure = root;
	auto type_module = new t_type_of<t_module>(t_type_of<t_module>::V_ids, type_object, nullptr);
	v_module_global = t_object::f_allocate(type_module);
	auto library = new t_library(std::wstring(), nullptr);
	v_module_global.f_pointer__(library);
	v_module__instances__null = v_module__instances.emplace(std::wstring(), t_slot()).first;
	library->v_iterator = v_module__instances.emplace(L"__global", t_slot()).first;
	library->v_iterator->second = v_module_global;
	type_object->v_module = v_module_global;
	v_type_class->v_module = v_module_global;
	type_structure->v_module = v_module_global;
	type_module->v_module = v_module_global;
	auto type_fiber = new t_type_of<t_fiber>(t_type_of<t_fiber>::V_ids, type_object, t_scoped(v_module_global));
	v_fiber_exit = t_object::f_allocate(type_object);
	auto type_thread = new t_type_of<t_thread>(t_type_of<t_thread>::V_ids, type_object, t_scoped(v_module_global));
	v_thread = t_object::f_allocate(type_thread);
	v_thread.f_pointer__(thread);
	thread->v_internal->v_thread = t_thread::v_current = v_thread;
	thread->v_fiber = t_object::f_allocate(type_fiber);
	thread->v_fiber.f_pointer__(new t_fiber(nullptr, v_stack_size, true, true));
	thread->v_active = thread->v_fiber;
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
	library->v_extension = new t_global(v_module_global, type_object, v_type_class, type_structure, type_module, type_fiber, type_thread);
	v_module_system = t_module::f_instantiate(L"system", new t_module(std::wstring()));
	t_scoped path = t_array::f_instantiate();
	static_cast<t_object*>(path)->v_owner = nullptr;
	{
		char* p = std::getenv("XEMMAI_MODULE_PATH");
		if (p != NULL) {
			std::wstring s = portable::f_convert(p);
#ifdef _WIN32
			s.erase(std::remove(s.begin(), s.end(), L'"'), s.end());
#endif
			size_t i = 0;
			while (true) {
#ifdef __unix__
				size_t j = s.find(L':', i);
#endif
#ifdef _WIN32
				size_t j = s.find(L';', i);
#endif
				if (j == std::wstring::npos) break;
				if (i < j) f_as<t_array&>(path).f_push(f_global()->f_as(s.substr(i, j - i)));
				i = j + 1;
			}
			if (i < s.size()) f_as<t_array&>(path).f_push(f_global()->f_as(s.substr(i)));
		}
	}
#ifdef XEMMAI_MODULE_PATH
	f_as<t_array&>(path).f_push(f_global()->f_as(std::wstring(XEMMAI__MACRO__LQ(XEMMAI_MODULE_PATH))));
#endif
	if (a_count > 0) {
		v_module_system.f_put(f_global()->f_symbol_executable(), f_global()->f_as(static_cast<const std::wstring&>(portable::t_path(portable::f_convert(a_arguments[0])))));
		if (a_count > 1) {
			portable::t_path script(portable::f_convert(a_arguments[1]));
			v_module_system.f_put(f_global()->f_symbol_script(), f_global()->f_as(static_cast<const std::wstring&>(script)));
			f_as<t_array&>(path).f_push(f_global()->f_as(static_cast<const std::wstring&>(script / L"..")));
			t_scoped arguments = t_array::f_instantiate();
			auto& p = f_as<t_array&>(arguments);
			for (size_t i = 2; i < a_count; ++i) p.f_push(f_global()->f_as(portable::f_convert(a_arguments[i])));
			v_module_system.f_put(f_global()->f_symbol_arguments(), std::move(arguments));
		}
	}
	v_module_system.f_put(f_global()->f_symbol_path(), std::move(path));
	{
		auto library = new t_library(std::wstring(), nullptr);
		v_module_io = t_module::f_instantiate(L"io", library);
		library->v_extension = new t_io(v_module_io);
	}
	{
		t_scoped file = io::t_file::f_instantiate(stdin);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"raw_in"), t_scoped(file));
		bool tty = f_as<io::t_file&>(file).f_tty();
		t_scoped reader = io::t_reader::f_instantiate(std::move(file), L"", tty ? 1 : 1024);
		static_cast<t_object*>(reader)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"in"), std::move(reader));
	}
	{
		t_scoped file = io::t_file::f_instantiate(stdout);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"raw_out"), t_scoped(file));
		t_scoped writer = io::t_writer::f_instantiate(std::move(file), L"");
		static_cast<t_object*>(writer)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"out"), std::move(writer));
	}
	{
		t_scoped file = io::t_file::f_instantiate(stderr);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"raw_error"), t_scoped(file));
		t_scoped writer = io::t_writer::f_instantiate(std::move(file), L"");
		static_cast<t_object*>(writer)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"error"), std::move(writer));
	}
#ifdef XEMMAI_ENABLE_JIT
	f_jit_construct();
#endif
}

t_engine::~t_engine()
{
	t_thread::f_cache_clear();
	v_structure_root = nullptr;
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
	v_dictionary__entry__pool.f_clear();
	v_object__pool.f_clear();
#ifdef XEMMAI_ENABLE_JIT
	f_jit_destruct();
#endif
	if (v_verbose) {
		bool b = false;
		std::fprintf(stderr, "statistics:\n");
		{
			size_t allocated = v_object__pool.f_allocated();
			size_t freed = v_object__pool.f_freed();
			std::fprintf(stderr, "\tobject: %" PRIuPTR " - %" PRIuPTR " = %" PRIuPTR ", release = %" PRIuPTR ", collect = %" PRIuPTR "\n", static_cast<uintptr_t>(allocated), static_cast<uintptr_t>(freed), static_cast<uintptr_t>(allocated - freed), static_cast<uintptr_t>(v_object__release), static_cast<uintptr_t>(v_object__collect));
			if (allocated > freed) b = true;
		}
		{
			size_t allocated = v_dictionary__entry__pool.f_allocated();
			size_t freed = v_dictionary__entry__pool.f_freed();
			std::fprintf(stderr, "\tdictionary entry: %" PRIuPTR " - %" PRIuPTR " = %" PRIuPTR "\n", static_cast<uintptr_t>(allocated), static_cast<uintptr_t>(freed), static_cast<uintptr_t>(allocated - freed));
			if (allocated > freed) b = true;
		}
		std::fprintf(stderr, "\tcollector: tick = %" PRIuPTR ", wait = %" PRIuPTR ", epoch = %" PRIuPTR ", release = %" PRIuPTR ", collect = %" PRIuPTR "\n", static_cast<uintptr_t>(v_collector__tick), static_cast<uintptr_t>(v_collector__wait), static_cast<uintptr_t>(v_collector__epoch), static_cast<uintptr_t>(v_collector__release), static_cast<uintptr_t>(v_collector__collect));
		{
			size_t base = v_thread__cache_hit + v_thread__cache_missed;
			std::fprintf(stderr, "\tfield cache: hit = %" PRIuPTR ", missed = %" PRIuPTR ", ratio = %.1f%%\n", static_cast<uintptr_t>(v_thread__cache_hit), static_cast<uintptr_t>(v_thread__cache_missed), base > 0 ? v_thread__cache_hit * 100.0 / base : 0.0);
		}
		if (b) throw std::exception();
	}
}

namespace
{

template<typename T_context>
void f_initialize_calls()
{
	f_global()->f_type<t_lambda>()->f_call = t_type_of<t_lambda>::f_do_call<t_lambda, T_context>;
	f_global()->f_type<t_lambda_shared>()->f_call = t_type_of<t_lambda>::f_do_call<t_lambda_shared, T_context>;
	f_global()->f_type<t_advanced_lambda<t_lambda>>()->f_call = t_type_of<t_advanced_lambda<t_lambda>>::f_do_call<T_context>;
	f_global()->f_type<t_advanced_lambda<t_lambda_shared>>()->f_call = t_type_of<t_advanced_lambda<t_lambda_shared>>::f_do_call<T_context>;
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
			f_print_with_caret(a_out, path.c_str(), at->f_position(), at->f_column());
		} else {
			std::fputc('\n', a_out);
		}
	} else {
		std::fputs("<fiber>\n", a_out);
	}
}

void t_engine::f_debug_safe_point()
{
	if (!v_debugger) f_throw(L"not in debug mode");
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	if (v_debug__stepping == t_thread::f_current())
		f_debug_break_point(lock);
	else
		f_debug_safe_point(lock);
}

void t_engine::f_debug_break_point()
{
	if (!v_debugger) f_throw(L"not in debug mode");
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
	if (!v_debugger) f_throw(L"not in debug mode");
	if (v_debug__stopping) return;
	f_debug_stop_and_wait(lock);
	v_debugger->f_stopped(v_thread);
}

void t_engine::f_debug_continue(t_object* a_stepping)
{
	std::lock_guard<std::mutex> lock(v_thread__mutex);
	if (!v_debugger) f_throw(L"not in debug mode");
	if (!v_debug__stopping) f_throw(L"already running");
	v_debug__stopping = false;
	v_debug__stepping = a_stepping;
	v_thread__condition.notify_all();
}

}
