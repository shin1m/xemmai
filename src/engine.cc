#include <xemmai/engine.h>

#include <cassert>
#include <cstring>
#include <xemmai/class.h>

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
	if (v_engine->v_verbose) std::fwprintf(stderr, L"synchronizer(%d) starting...\n", v_cpu);
	{
		portable::t_scoped_lock lock(v_mutex);
		while (true) {
			while (!v_wake) v_condition.f_wait(v_mutex);
			v_wake = false;
			if (v_engine->v_collector__quitting) break;
			{
				portable::t_scoped_lock lock(v_engine->v_synchronizer__mutex);
				if (--v_engine->v_synchronizer__wake > 0) continue;
			}
//			v_engine->v_synchronizer__condition.f_signal();
		}
	}
	if (v_engine->v_verbose) std::fwprintf(stderr, L"synchronizer(%d) quitting...\n", v_cpu);
	v_condition.f_signal();
}

void* t_engine::f_synchronizer(void* a_p)
{
	static_cast<t_synchronizer*>(a_p)->f_run();
	return 0;
}

void* t_engine::f_collector(void* a_p)
{
	static_cast<t_engine*>(a_p)->f_collector();
	return 0;
}

void t_engine::f_pools__return()
{
	t_local_pool<t_hash::t_entry>::f_return(f_instance__hash__entry__pool__return);
	t_local_pool<t_object>::f_return(f_instance__object__pool__return);
	t_local_pool<t_fiber::t_context>::f_return(f_instance__fiber__context__pool__return);
	t_local_pool<t_fiber::t_try>::f_return(f_instance__fiber__try__pool__return);
}

t_hash::t_table* t_engine::f_hash__table__allocate(size_t a_rank)
{
	t_hash::t_table* p = v_hash__table__pools[a_rank].f_allocate();
	size_t n = p->v_capacity;
	for (size_t i = 0; i < n; ++i) p->v_entries[i] = 0;
	return p;
}

void t_engine::f_hash__table__free(t_hash::t_table* a_p)
{
	a_p->v_next = 0;
	v_hash__table__pools[a_p->v_rank].f_free(a_p);
}

void t_engine::f_collector()
{
	t_pointer::v_collector = this;
	if (v_verbose) std::fwprintf(stderr, L"collector starting...\n");
	while (true) {
		{
			portable::t_scoped_lock lock(v_collector__mutex);
			v_collector__running = false;
		}
		v_collector__done.f_broadcast();
		{
			portable::t_scoped_lock lock(v_collector__mutex);
			while (!v_collector__running) v_collector__wake.f_wait(v_collector__mutex);
		}
		if (v_collector__quitting) {
			v_object__release = t_object::v_release;
			v_object__collect = t_object::v_collect;
			if (v_verbose) std::fwprintf(stderr, L"collector quitting...\n");
			{
				portable::t_scoped_lock lock(v_collector__mutex);
				v_collector__running = false;
			}
			v_collector__done.f_signal();
			break;
		}
		++v_collector__epoch;
		{
			portable::t_scoped_lock lock(v_object__reviving__mutex);
			v_object__reviving = false;
		}
		{
			portable::t_scoped_lock lock(v_thread__mutex);
			t_thread::t_queues* p = v_thread__queueses;
			if (p) {
				do {
					p = p->v_next;
					p->v_increments.f_epoch();
					p->v_decrements.f_epoch();
				} while (p != v_thread__queueses);
				if (v_synchronizers) {
					size_t cpu = portable::f_cpu();
					for (t_synchronizer* p = v_synchronizers; p; p = p->v_next) if (p->v_cpu != cpu) ++v_synchronizer__wake;
					for (t_synchronizer* p = v_synchronizers; p; p = p->v_next) {
						if (p->v_cpu == cpu) continue;
						{
							portable::t_scoped_lock lock(p->v_mutex);
							p->v_wake = true;
						}
						p->v_condition.f_signal();
					}
				}
				{
					portable::t_scoped_lock lock(v_object__reviving__mutex);
					do {
						p = p->v_next;
						if (p->v_done > 0) ++p->v_done;
						if (p->v_reviving) {
							t_object* volatile* tail = p->v_increments.v_tail + 1;
							size_t epoch = (p->v_increments.v_epoch + t_pointer::t_increments::V_SIZE - tail) % t_pointer::t_increments::V_SIZE;
							size_t reviving = (p->v_reviving + t_pointer::t_increments::V_SIZE - tail) % t_pointer::t_increments::V_SIZE;
							if (epoch > reviving)
								p->v_reviving = 0;
							else
								v_object__reviving = true;
						}
					} while (p != v_thread__queueses);
				}
/*				if (v_synchronizers) {
					portable::t_scoped_lock lock(v_synchronizer__mutex);
					while (v_synchronizer__wake > 0) v_synchronizer__condition.f_wait(v_synchronizer__mutex);
				}*/
				while (v_synchronizer__wake > 0) portable::f_yield();
				while (true) {
					t_thread::t_queues* q = p->v_next;
					q->v_increments.f_flush();
					q->v_decrements.f_flush();
					if (q->v_done < 3) {
						if (q == v_thread__queueses) break;
						p = q;
					} else {
						p->v_next = q->v_next;
						if (q == v_thread__queueses) {
							if (p == q) p = 0;
							v_thread__queueses = p;
							delete q;
							break;
						}
						delete q;
					}
				}
			}
		}
		t_object::f_collect();
		f_pools__return();
	}
}

t_engine::t_engine(int& argc, char** argv) :
v_object__cycle(0),
v_object__reviving(false),
v_object__release(0),
v_object__collect(0),
v_thread__queueses(0),
v_synchronizers(0),
v_synchronizer__wake(0),
v_module__thread(0),
v_library__handle__finalizing(0),
v_verbose(false)
{
	{
		char** end = argv + argc;
		char** q = argv;
		for (char** p = argv; p < end; ++p) {
			if ((*p)[0] == '-' && (*p)[1] == '-') {
				if (std::strcmp(*p + 2, "verbose") == 0) v_verbose = true;
			} else {
				*q++ = *p;
			}
		}
		argc = q - argv;
	}
	v_hash__table__pools[0].f_initialize(0, 11);
	v_hash__table__pools[1].f_initialize(1, 31);
	v_hash__table__pools[2].f_initialize(2, 67);
	v_hash__table__pools[3].f_initialize(3, 127);
	v_object__pool.f_grow();
	v_fiber__instructions[0] = reinterpret_cast<void*>(e_instruction__TRY);
	v_fiber__instructions[1] = reinterpret_cast<void*>(v_fiber__instructions + 7);
	v_fiber__instructions[2] = reinterpret_cast<void*>(v_fiber__instructions + 9);
	v_fiber__instructions[3] = reinterpret_cast<void*>(e_instruction__CALL);
	v_fiber__instructions[4] = reinterpret_cast<void*>(1);
	v_fiber__instructions[5] = reinterpret_cast<void*>(e_instruction__FINALLY);
	v_fiber__instructions[6] = reinterpret_cast<void*>(t_fiber::t_try::e_state__STEP);
	v_fiber__instructions[7] = reinterpret_cast<void*>(e_instruction__FINALLY);
	v_fiber__instructions[8] = reinterpret_cast<void*>(t_fiber::t_try::e_state__THROW);
	v_fiber__instructions[9] = reinterpret_cast<void*>(e_instruction__FIBER_EXIT);
	t_code::f_generate(v_fiber__instructions, true);
	t_thread* thread = new t_thread(0);
	t_pointer::v_increments = &thread->v_queues->v_increments;
	t_pointer::v_decrements = &thread->v_queues->v_decrements;
	v_thread__queueses = thread->v_queues->v_next = thread->v_queues;
	t_scoped type_class = t_object::f_allocate(0);
	type_class->v_type = type_class;
	type_class->v_pointer = new t_class(0, 0);
	v_type_class = type_class;
	t_scoped type_object = t_class::f_instantiate(new t_type(0, 0));
	f_as<t_type*>(type_class)->v_super = type_object;
	t_scoped type_module = t_class::f_instantiate(new t_type_of<t_module>(0, type_object));
	v_module_global = t_object::f_allocate(type_module);
	t_library* library = new t_library(std::wstring(), 0);
	v_module_global->v_pointer = library;
	v_module__instances__null = v_module__instances.insert(std::make_pair(std::wstring(), t_slot())).first;
	library->v_iterator = v_module__instances.insert(std::make_pair(L"__global", t_slot())).first;
	library->v_iterator->second = v_module_global;
	f_as<t_type*>(type_object)->v_module = v_module_global;
	f_as<t_type*>(type_class)->v_module = v_module_global;
	f_as<t_type*>(type_module)->v_module = v_module_global;
	t_scoped type_fiber = t_class::f_instantiate(new t_type_of<t_fiber>(v_module_global, type_object));
	t_scoped type_thread = t_class::f_instantiate(new t_type_of<t_thread>(v_module_global, type_object));
	v_thread = t_object::f_allocate(type_thread);
	v_thread->v_pointer = thread;
	t_thread::v_current = v_thread;
	thread->v_fiber = t_object::f_allocate(type_fiber);
	thread->v_fiber->v_pointer = new t_fiber(0, true, true);
	thread->v_active = thread->v_fiber;
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
				new t_synchronizer(this, n);
				portable::f_thread(f_synchronizer, v_synchronizers);
			}
		}
	}
	{
		portable::t_scoped_lock lock(v_collector__mutex);
		portable::f_thread(f_collector, this);
		while (v_collector__running) v_collector__done.f_wait(v_collector__mutex);
	}
	library->v_extension = new t_global(v_module_global, type_object.f_transfer(), type_class.f_transfer(), type_module.f_transfer(), type_fiber.f_transfer(), type_thread.f_transfer());
}

t_engine::~t_engine()
{
	v_module_global = 0;
	f_pools__return();
	{
		t_thread* thread = f_as<t_thread*>(v_thread);
		thread->v_active = 0;
		t_thread::t_queues* queues = thread->v_queues;
		thread->v_queues = 0;
		v_thread = 0;
		portable::t_scoped_lock lock(v_thread__mutex);
		++queues->v_done;
	}
	f_collect();
	f_collect();
	f_collect();
	f_collect();
	{
		portable::t_scoped_lock lock(v_collector__mutex);
		v_collector__running = v_collector__quitting = true;
	}
	v_collector__wake.f_signal();
	{
		portable::t_scoped_lock lock(v_collector__mutex);
		while (v_collector__running) v_collector__done.f_wait(v_collector__mutex);
	}
	if (v_synchronizers) {
		do {
			{
				portable::t_scoped_lock lock(v_synchronizers->v_mutex);
				v_synchronizers->v_wake = true;
			}
			v_synchronizers->v_condition.f_signal();
			{
				portable::t_scoped_lock lock(v_synchronizers->v_mutex);
				while (v_synchronizers->v_wake) v_synchronizers->v_condition.f_wait(v_synchronizers->v_mutex);
			}
			delete v_synchronizers;
		} while (v_synchronizers);
	}
	assert(!v_thread__queueses);
	v_fiber__try__pool.f_clear();
	v_fiber__context__pool.f_clear();
	v_object__pool.f_clear();
	for (size_t i = 0; i < t_hash::t_table::V_POOLS__SIZE; ++i) v_hash__table__pools[i].f_clear();
	v_hash__entry__pool.f_clear();
	if (v_verbose) {
		bool b = false;
		std::fwprintf(stderr, L"statistics:\n\thash:\n");
		{
			size_t allocated = v_hash__entry__pool.f_allocated();
			size_t freed = v_hash__entry__pool.f_freed();
			std::fwprintf(stderr, L"\t\tentry: %d - %d = %d\n", allocated, freed, allocated - freed);
			if (allocated > freed) b = true;
		}
		for (size_t i = 0; i < t_hash::t_table::V_POOLS__SIZE; ++i) {
			size_t allocated = v_hash__table__pools[i].f_allocated();
			size_t freed = v_hash__table__pools[i].f_freed();
			std::fwprintf(stderr, L"\t\ttable[%d]: %d - %d = %d\n", i, allocated, freed, allocated - freed);
			if (allocated > freed) b = true;
		}
		{
			size_t allocated = v_object__pool.f_allocated();
			size_t freed = v_object__pool.f_freed();
			std::fwprintf(stderr, L"\tobject: %d - %d = %d, release = %d, collect = %d\n", allocated, freed, allocated - freed, v_object__release, v_object__collect);
			if (allocated > freed) b = true;
		}
		std::fwprintf(stderr, L"\tcollector: tick = %d, wait = %d, epoch = %d, collect = %d\n", v_collector__tick, v_collector__wait, v_collector__epoch, v_collector__collect);
		if (b) throw std::exception();
	}
}

}
