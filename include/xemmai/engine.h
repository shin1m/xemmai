#ifndef XEMMAI__ENGINE_H
#define XEMMAI__ENGINE_H

#include <vector>

#include "module.h"
#include "fiber.h"
#include "thread.h"

namespace xemmai
{

class t_symbol;
class t_global;

class t_engine : public t_pointer::t_collector
{
	friend class t_hash;
	friend class t_hash::t_entry;
	friend struct t_hash::t_table;
	friend class t_object;
	friend struct t_type_of<t_type>;
	friend struct t_module;
	friend class t_module::t_scoped_lock;
	friend struct t_library;
	friend struct t_fiber;
	friend struct t_fiber::t_context;
	friend struct t_fiber::t_try;
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend class t_symbol;
	friend struct t_type_of<t_symbol>;
	friend class t_global;

	struct t_synchronizer
	{
		t_engine* v_engine;
		size_t v_cpu;
		bool v_wake;
		portable::t_mutex v_mutex;
		portable::t_condition v_condition;
		t_synchronizer* v_next;

		t_synchronizer(t_engine* a_engine, size_t a_cpu) :
		v_engine(a_engine), v_cpu(a_cpu), v_wake(false), v_next(a_engine->v_synchronizers)
		{
			v_engine->v_synchronizers = this;
		}
		~t_synchronizer()
		{
			v_engine->v_synchronizers = v_next;
		}
		void f_run();
	};

	static const size_t V_POOL__ALLOCATION__UNIT = 4096;
	static const size_t V_COLLECTOR__SKIP = 1024;

	static void* f_synchronizer(void* a_p);
	static void* f_collector(void* a_p);
	static void f_instance__hash__entry__pool__return(t_hash::t_entry* a_p)
	{
		static_cast<t_engine*>(t_pointer::v_collector)->v_hash__entry__pool.f_free(a_p);
	}
	static void f_instance__object__pool__return(t_object* a_p)
	{
		static_cast<t_engine*>(t_pointer::v_collector)->v_object__pool.f_free(a_p);
	}
	static void f_instance__fiber__context__pool__return(t_fiber::t_context* a_p)
	{
		static_cast<t_engine*>(t_pointer::v_collector)->v_fiber__context__pool.f_free(a_p);
	}
	static void f_instance__fiber__try__pool__return(t_fiber::t_try* a_p)
	{
		static_cast<t_engine*>(t_pointer::v_collector)->v_fiber__try__pool.f_free(a_p);
	}

	t_shared_pool<t_fixed_pool<t_hash::t_entry, 4096> > v_hash__entry__pool;
	t_shared_pool<t_variable_pool<t_hash::t_table> > v_hash__table__pools[t_hash::t_table::V_POOLS__SIZE];
	t_shared_pool<t_fixed_pool<t_object, 65536> > v_object__pool;
	t_object* v_object__cycle;
	std::vector<t_object*> v_object__cycles;
	bool v_object__reviving;
	portable::t_mutex v_object__reviving__mutex;
	size_t v_object__release;
	size_t v_object__collect;
	t_shared_pool<t_fixed_pool<t_fiber::t_context, 256> > v_fiber__context__pool;
	t_shared_pool<t_fixed_pool<t_fiber::t_try, 256> > v_fiber__try__pool;
	void* v_fiber__instructions[10];
	t_thread::t_internal* v_thread__internals;
	portable::t_mutex v_thread__mutex;
	portable::t_condition v_thread__condition;
	t_synchronizer* v_synchronizers;
	size_t v_synchronizer__wake;
	portable::t_mutex v_synchronizer__mutex;
	portable::t_condition v_synchronizer__condition;
	t_object* v_type_class;
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
	t_slot v_module_system;
	t_slot v_module_threading;
	t_slot v_module_io;
	t_slot v_module_math;
	t_slot v_module_os;
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
	t_engine(bool a_verbose, size_t a_count, char** a_arguments);
	~t_engine();
	void f_collect()
	{
		v_collector__skip = V_COLLECTOR__SKIP;
		f_wait();
	}
	t_object* f_module_global() const
	{
		return v_module_global;
	}
	t_object* f_module_system() const
	{
		return v_module_system;
	}
	t_object* f_module_threading() const
	{
		return v_module_threading;
	}
	t_object* f_module_io() const
	{
		return v_module_io;
	}
	t_object* f_module_math() const
	{
		return v_module_math;
	}
	t_object* f_module_os() const
	{
		return v_module_os;
	}
	int f_run();
};

inline t_engine* f_engine()
{
	return static_cast<t_engine*>(t_pointer::v_collector);
}

}

#endif
