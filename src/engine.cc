#include <xemmai/engine.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <xemmai/portable/thread.h>
#include <xemmai/portable/path.h>
#include <xemmai/portable/convert.h>
#include <xemmai/structure.h>
#include <xemmai/array.h>
#include <xemmai/io.h>

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
	if (v_engine->v_verbose) std::fprintf(stderr, "synchronizer(%" XEMMAI__PORTABLE__FORMAT_SIZE_T "d) starting...\n", v_cpu);
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
	if (v_engine->v_verbose) std::fprintf(stderr, "synchronizer(%" XEMMAI__PORTABLE__FORMAT_SIZE_T "d) quitting...\n", v_cpu);
	v_condition.notify_one();
}

void t_engine::f_pools__return()
{
	f_return(v_object__pool);
	f_return(v_fiber__context__pool);
	f_return(v_fiber__try__pool);
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
			std::lock_guard<std::mutex> lock(v_collector__mutex);
			v_collector__running = false;
		}
		v_collector__done.notify_all();
		{
			std::unique_lock<std::mutex> lock(v_collector__mutex);
			while (!v_collector__running) v_collector__wake.wait(lock);
		}
		if (v_collector__quitting) {
			if (v_verbose) std::fprintf(stderr, "collector quitting...\n");
			{
				std::lock_guard<std::mutex> lock(v_collector__mutex);
				v_collector__running = false;
			}
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
			t_thread::t_internal* p = v_thread__internals;
			if (p) {
				do {
					p = p->v_next;
					p->v_increments.f_epoch();
					p->v_decrements.f_epoch();
				} while (p != v_thread__internals);
				f_signal_synchronizers();
				{
					std::lock_guard<std::mutex> lock(v_object__reviving__mutex);
					do {
						p = p->v_next;
						if (p->v_done > 0) ++p->v_done;
						if (p->v_reviving) {
							t_object* volatile* tail = p->v_increments.v_tail + 1;
							size_t epoch = (p->v_increments.v_epoch + t_value::t_increments::V_SIZE - tail) % t_value::t_increments::V_SIZE;
							size_t reviving = (p->v_reviving + t_value::t_increments::V_SIZE - tail) % t_value::t_increments::V_SIZE;
							if (epoch > reviving)
								p->v_reviving = nullptr;
							else
								v_object__reviving = true;
						}
					} while (p != v_thread__internals);
				}
				f_wait_synchronizers();
				while (true) {
					t_thread::t_internal* q = p->v_next;
					q->v_increments.f_flush();
					q->v_decrements.f_flush();
					if (q->v_done < 3) {
						if (q == v_thread__internals) break;
						p = q;
					} else {
						p->v_next = q->v_next;
						v_thread__cache_hit += q->v_cache_hit;
						v_thread__cache_missed += q->v_cache_missed;
						if (q == v_thread__internals) {
							if (p == q) p = nullptr;
							v_thread__internals = p;
							delete q;
							break;
						}
						delete q;
					}
				}
			}
		}
		t_object::f_collect();
		if (v_object__freed > 0) f_return(v_object__pool, v_object__freed);
		if (v_fiber__context__freed > 0) f_return(v_fiber__context__pool, v_fiber__context__freed);
		if (v_fiber__try__freed > 0) f_return(v_fiber__try__pool, v_fiber__try__freed);
		if (v_dictionary__entry__freed > 0) f_return(v_dictionary__entry__pool, v_dictionary__entry__freed);
	}
}

t_engine::t_engine(size_t a_stack, bool a_verbose, size_t a_count, char** a_arguments) : v_stack_size(a_stack), v_verbose(a_verbose)
{
	v_object__pool.f_grow();
	t_thread* thread = new t_thread(nullptr);
	thread->v_internal->f_initialize();
	v_thread__internals = thread->v_internal->v_next = thread->v_internal;
	t_scoped type_class = t_object::f_allocate_on_boot(nullptr);
	static_cast<t_object*>(type_class)->v_type = type_class;
	v_type_class = type_class;
	t_scoped type_object = t_object::f_allocate_on_boot(type_class);
	type_object.f_pointer__(new t_type(nullptr, nullptr));
	type_class.f_pointer__(new t_class(nullptr, type_object));
	t_scoped type_structure = t_object::f_allocate_on_boot(type_class);
	type_structure.f_pointer__(new t_type_of<t_structure>(nullptr, type_object));
	v_structure_root = t_object::f_allocate_on_boot(type_structure);
	t_structure* root = new(0) t_structure(v_structure_root);
	v_structure_root.f_pointer__(root);
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(type_object)->v_structure = root;
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(type_class)->v_structure = root;
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(type_structure)->v_structure = root;
	t_value::v_increments->f_push(v_structure_root);
	static_cast<t_object*>(v_structure_root)->v_structure = root;
	t_scoped type_module = t_class::f_instantiate(new t_type_of<t_module>(nullptr, type_object));
	v_module_global = t_object::f_allocate(type_module);
	t_library* library = new t_library(std::wstring(), nullptr);
	v_module_global.f_pointer__(library);
	v_module__instances__null = v_module__instances.insert(std::make_pair(std::wstring(), t_slot())).first;
	library->v_iterator = v_module__instances.insert(std::make_pair(L"__global", t_slot())).first;
	library->v_iterator->second = v_module_global;
	f_as<t_type&>(type_object).v_module = v_module_global;
	f_as<t_type&>(type_class).v_module = v_module_global;
	f_as<t_type&>(type_structure).v_module = v_module_global;
	f_as<t_type&>(type_module).v_module = v_module_global;
	t_scoped type_fiber = t_class::f_instantiate(new t_type_of<t_fiber>(v_module_global, type_object));
	t_scoped type_thread = t_class::f_instantiate(new t_type_of<t_thread>(v_module_global, type_object));
	v_thread = t_object::f_allocate(type_thread);
	v_thread.f_pointer__(thread);
	t_thread::v_current = v_thread;
	(*thread).v_fiber = t_object::f_allocate(type_fiber);
	(*thread).v_fiber.f_pointer__(new t_fiber(nullptr, v_stack_size, true, true));
	(*thread).v_active = thread->v_fiber;
	t_fiber::v_current = thread->v_active;
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
				std::thread([p]() {
					p->f_run();
				}).detach();
			}
		}
	}
	{
		std::unique_lock<std::mutex> lock(v_collector__mutex);
		std::thread([this]() {
			f_collector();
		}).detach();
		while (v_collector__running) v_collector__done.wait(lock);
	}
	library->v_extension = new t_global(v_module_global, type_object.f_transfer(), type_class.f_transfer(), type_structure.f_transfer(), type_module.f_transfer(), type_fiber.f_transfer(), type_thread.f_transfer());
	v_module_system = t_module::f_instantiate(L"system", new t_module(std::wstring()));
	t_transfer path = t_array::f_instantiate();
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
			t_transfer arguments = t_array::f_instantiate();
			t_array& p = f_as<t_array&>(arguments);
			for (size_t i = 2; i < a_count; ++i) p.f_push(f_global()->f_as(portable::f_convert(a_arguments[i])));
			v_module_system.f_put(f_global()->f_symbol_arguments(), arguments);
		}
	}
	v_module_system.f_put(f_global()->f_symbol_path(), path);
	{
		t_library* library = new t_library(std::wstring(), nullptr);
		v_module_io = t_module::f_instantiate(L"io", library);
		library->v_extension = new t_io(v_module_io);
	}
	{
		t_transfer file = io::t_file::f_instantiate(stdin);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"native_in"), t_value(file));
		t_transfer reader = io::t_reader::f_instantiate(file, L"");
		static_cast<t_object*>(reader)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"in"), reader);
	}
	{
		t_transfer file = io::t_file::f_instantiate(stdout);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"native_out"), t_value(file));
		t_transfer writer = io::t_writer::f_instantiate(file, L"");
		static_cast<t_object*>(writer)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"out"), writer);
	}
	{
		t_transfer file = io::t_file::f_instantiate(stderr);
		static_cast<t_object*>(file)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"native_error"), t_value(file));
		t_transfer writer = io::t_writer::f_instantiate(file, L"");
		static_cast<t_object*>(writer)->v_owner = nullptr;
		v_module_system.f_put(t_symbol::f_instantiate(L"error"), writer);
	}
	{
		v_code_fiber = t_code::f_instantiate(std::wstring(), false, false, 2, 0, 0, 0);
		t_code& code = f_as<t_code&>(v_code_fiber);
		t_code::t_label catch0;
		t_code::t_label finally0;
		code.f_emit(e_instruction__TRY);
		code.f_operand(0);
		code.f_operand(catch0);
		code.f_operand(finally0);
		code.f_emit(e_instruction__CALL);
		code.f_operand(0);
		code.f_operand(1);
		code.f_emit(e_instruction__FINALLY);
		code.f_operand(t_fiber::t_try::e_state__STEP);
		code.f_target(catch0);
		code.f_emit(e_instruction__FINALLY);
		code.f_operand(t_fiber::t_try::e_state__THROW);
		code.f_target(finally0);
		code.f_emit(e_instruction__FIBER_EXIT);
		code.f_resolve(catch0);
		code.f_resolve(finally0);
	}
}

t_engine::~t_engine()
{
	t_thread::f_cache_clear();
	v_structure_root = nullptr;
	v_module_global = nullptr;
	v_module_system = nullptr;
	v_module_io = nullptr;
	v_code_fiber = nullptr;
	{
		t_thread& thread = f_as<t_thread&>(v_thread);
		thread.v_active = nullptr;
		t_thread::t_internal* internal = thread.v_internal;
		thread.v_internal = nullptr;
		v_thread = nullptr;
		std::lock_guard<std::mutex> lock(v_thread__mutex);
		++internal->v_done;
		internal->v_cache_hit = t_thread::v_cache_hit;
		internal->v_cache_missed = t_thread::v_cache_missed;
	}
	f_pools__return();
	f_collect();
	f_collect();
	f_collect();
	f_collect();
	{
		std::lock_guard<std::mutex> lock(v_collector__mutex);
		v_collector__running = v_collector__quitting = true;
	}
	v_collector__wake.notify_one();
	{
		std::unique_lock<std::mutex> lock(v_collector__mutex);
		while (v_collector__running) v_collector__done.wait(lock);
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
	v_fiber__try__pool.f_clear();
	v_fiber__context__pool.f_clear();
	v_object__pool.f_clear();
	if (v_verbose) {
		bool b = false;
		std::fprintf(stderr, "statistics:\n");
		{
			size_t allocated = v_object__pool.f_allocated();
			size_t freed = v_object__pool.f_freed();
			std::fprintf(stderr, "\tobject: %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d - %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, release = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, collect = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d\n", allocated, freed, allocated - freed, v_object__release, v_object__collect);
			if (allocated > freed) b = true;
		}
		{
			size_t allocated = v_dictionary__entry__pool.f_allocated();
			size_t freed = v_dictionary__entry__pool.f_freed();
			std::fprintf(stderr, "\tdictionary entry: %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d - %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d\n", allocated, freed, allocated - freed);
			if (allocated > freed) b = true;
		}
		std::fprintf(stderr, "\tcollector: tick = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, wait = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, epoch = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, collect = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d\n", v_collector__tick, v_collector__wait, v_collector__epoch, v_collector__collect);
		{
			size_t base = v_thread__cache_hit + v_thread__cache_missed;
			std::fprintf(stderr, "\tfield cache: hit = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, missed = %" XEMMAI__PORTABLE__FORMAT_SIZE_T "d, ratio = %.1f%%\n", v_thread__cache_hit, v_thread__cache_missed, base > 0 ? v_thread__cache_hit * 100.0 / base : 0.0);
		}
		if (b) throw std::exception();
	}
}

ptrdiff_t t_engine::f_run()
{
	ptrdiff_t n = t_fiber::f_main(t_module::f_main);
	std::unique_lock<std::mutex> lock(v_thread__mutex);
	t_thread::t_internal*& internals = v_thread__internals;
	t_thread::t_internal* internal = f_as<t_thread&>(t_thread::f_current()).v_internal;
	while (true) {
		t_thread::t_internal* p = internals;
		do {
			p = p->v_next;
			if (p == internal || p->v_done > 0) continue;
			p = nullptr;
			break;
		} while (p != internals);
		if (p) break;
		v_thread__condition.wait(lock);
	}
	return n;
}

}
