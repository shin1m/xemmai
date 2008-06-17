#ifndef XEMMAI__ENGINE_H
#define XEMMAI__ENGINE_H

#include <vector>

#include "portable/path.h"
#include "fiber.h"
#include "thread.h"
#include "global.h"

namespace xemmai
{

struct t_class;
struct t_symbol;

t_engine* f_engine();

class t_engine
{
	friend class t_hash;
	friend class t_hash::t_entry;
	friend struct t_hash::t_table;
	friend class t_object;
	friend struct t_class;
	friend struct t_module;
	friend class t_module::t_scoped_lock;
	friend struct t_library;
	friend struct t_fiber;
	friend struct t_fiber::t_context;
	friend struct t_fiber::t_try;
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend struct t_symbol;
	friend class t_global;
	friend t_engine* f_engine();

	struct t_synchronizer
	{
		size_t v_cpu;
		portable::t_condition v_condition;
		t_synchronizer* v_next;

		t_synchronizer(size_t a_cpu, t_synchronizer* a_next) : v_cpu(a_cpu), v_next(a_next)
		{
		}
		void f_run();
	};

	static const size_t V_POOL__ALLOCATION__UNIT = 1024;
	static const size_t V_COLLECTOR__SKIP = 1024;
	static t_engine* v_instance;

	static void* f_synchronizer(void* a_p);
	static void* f_collector(void* a_p);
	static void f_instance__hash__entry__pool__return(t_hash::t_entry* a_p)
	{
		v_instance->v_hash__entry__pool.f_free(a_p);
	}
	static void f_instance__object__pool__return(t_object* a_p)
	{
		v_instance->v_object__pool.f_free(a_p);
	}
	static void f_instance__fiber__context__pool__return(t_fiber::t_context* a_p)
	{
		v_instance->v_fiber__context__pool.f_free(a_p);
	}
	static void f_instance__fiber__try__pool__return(t_fiber::t_try* a_p)
	{
		v_instance->v_fiber__try__pool.f_free(a_p);
	}

	t_shared_pool<t_fixed_pool<t_hash::t_entry> > v_hash__entry__pool;
	t_shared_pool<t_variable_pool<t_hash::t_table> > v_hash__table__pools[t_hash::t_table::V_POOLS__SIZE];
	t_shared_pool<t_fixed_pool<t_object> > v_object__pool;
	t_object* v_object__cycle;
	std::vector<t_object*> v_object__cycles;
	bool v_object__reviving;
	portable::t_mutex v_object__reviving__mutex;
	size_t v_object__release;
	size_t v_object__collect;
	t_shared_pool<t_fixed_pool<t_fiber::t_context> > v_fiber__context__pool;
	t_shared_pool<t_fixed_pool<t_fiber::t_try> > v_fiber__try__pool;
	void* v_fiber__instructions[10];
	t_thread::t_queues* v_thread__queueses;
	portable::t_mutex v_thread__mutex;
	portable::t_condition v_thread__condition;
	t_synchronizer* v_synchronizers;
	portable::t_mutex v_synchronizer__mutex;
	portable::t_condition v_synchronizer__condition;
	bool v_collector__running;
	portable::t_mutex v_collector__mutex;
	portable::t_condition v_collector__condition;
	size_t v_collector__skip;
	size_t v_collector__epoch;
	size_t v_collector__collect;
	size_t v_collector__tick;
	size_t v_collector__wait;
	t_object* v_type_class;
	std::vector<portable::t_path> v_paths;
	std::map<std::wstring, t_slot> v_module__instances;
	std::map<std::wstring, t_slot>::iterator v_module__instances__null;
	portable::t_mutex v_module__mutex;
	portable::t_condition v_module__condition;
	t_object* v_module__thread;
	t_library::t_handle* v_library__handle__finalizing;
	std::map<std::wstring, t_slot> v_symbol__instances;
	portable::t_mutex v_symbol__instances__mutex;
	portable::t_mutex v_symbol__instantiate__mutex;
	t_slot v_module_global;
	t_slot v_thread;
	bool v_verbose;

	void f_pools__return();
	t_hash::t_table* f_hash__table__allocate(size_t a_rank);
	void f_hash__table__free(t_hash::t_table* a_p);
	t_object* f_object__pool__allocate()
	{
		t_object* p = v_object__pool.f_allocate(V_POOL__ALLOCATION__UNIT, false);
		if (!p) {
			f_wait();
			p = v_object__pool.f_allocate(V_POOL__ALLOCATION__UNIT);
		}
		return p;
	}
	void f_collector();

public:
	t_engine(int& argc, char** argv);
	~t_engine();
	void f_tick()
	{
		portable::t_scoped_lock lock(v_collector__mutex);
		++v_collector__tick;
		if (!v_collector__running) {
			v_collector__running = true;
			v_collector__condition.f_signal();
		}
	}
	void f_wait()
	{
		portable::t_scoped_lock lock(v_collector__mutex);
		++v_collector__wait;
		if (!v_collector__running) {
			v_collector__running = true;
			v_collector__condition.f_signal();
		}
		v_collector__condition.f_wait(v_collector__mutex);
	}
	void f_collect()
	{
		v_collector__skip = V_COLLECTOR__SKIP;
		f_wait();
	}
	t_object* f_module_global() const
	{
		return v_module_global;
	}
};

inline t_engine* f_engine()
{
	return t_engine::v_instance;
}

}

#endif
