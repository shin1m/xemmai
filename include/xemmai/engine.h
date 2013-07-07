#ifndef XEMMAI__ENGINE_H
#define XEMMAI__ENGINE_H

#include "module.h"
#include "fiber.h"
#include "thread.h"
#include "dictionary.h"

namespace xemmai
{

class t_symbol;
class t_global;

class t_engine : public t_value::t_collector
{
	friend class t_object;
	friend struct t_type_of<t_type>;
	friend class t_structure;
	friend struct t_type_of<t_structure>;
	friend struct t_module;
	friend class t_module::t_scoped_lock;
	friend struct t_library;
	friend struct t_fiber;
	friend struct t_fiber::t_context;
	friend struct t_fiber::t_try;
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend struct t_type_of<t_thread>;
	friend class t_symbol;
	friend struct t_type_of<t_symbol>;
	friend class t_dictionary;
	friend class t_dictionary::t_entry;
	friend class t_global;

	struct t_synchronizer
	{
		t_engine* v_engine;
		size_t v_cpu;
		bool v_wake;
		std::mutex v_mutex;
		std::condition_variable v_condition;
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

	static const size_t V_COLLECTOR__SKIP = 1024;

	template<typename T, size_t A_size>
	static void f_return(t_shared_pool<T, A_size>& a_pool)
	{
		T* p = t_local_pool<T>::f_detach();
		while (p) {
			T* q = p;
			size_t n = 0;
			while (++n < A_size && q->v_next) q = q->v_next;
			T* p0 = p;
			p = q->v_next;
			q->v_next = 0;
			a_pool.f_free(p0, n);
		}
	}
	template<typename T, size_t A_size>
	static void f_return(t_shared_pool<T, A_size>& a_pool, size_t& a_freed)
	{
		a_pool.f_free(t_local_pool<T>::f_detach(), a_freed);
		a_freed = 0;
	}
	template<typename T, size_t A_size>
	static void f_free(t_shared_pool<T, A_size>& a_pool, size_t& a_freed, T* a_p)
	{
		assert(t_thread::v_current == 0);
		t_local_pool<T>::f_free(a_p);
		if (++a_freed >= A_size) f_return(a_pool, a_freed);
	}

	t_shared_pool<t_object, 4096> v_object__pool;
	size_t v_object__freed;
	t_object* v_object__cycle;
	std::list<t_object*> v_object__cycles;
	bool v_object__reviving;
	std::mutex v_object__reviving__mutex;
	size_t v_object__release;
	size_t v_object__collect;
	t_structure* v_structure__finalizing;
	t_shared_pool<t_fiber::t_context, 256> v_fiber__context__pool;
	size_t v_fiber__context__freed;
	t_shared_pool<t_fiber::t_try, 64> v_fiber__try__pool;
	size_t v_fiber__try__freed;
	t_thread::t_internal* v_thread__internals;
	size_t v_thread__cache_hit;
	size_t v_thread__cache_missed;
	std::mutex v_thread__mutex;
	std::condition_variable v_thread__condition;
	t_synchronizer* v_synchronizers;
	volatile size_t v_synchronizer__wake;
	std::mutex v_synchronizer__mutex;
	std::condition_variable v_synchronizer__condition;
	t_object* v_type_class;
	std::map<std::wstring, t_slot> v_module__instances;
	std::map<std::wstring, t_slot>::iterator v_module__instances__null;
	std::mutex v_module__mutex;
	std::condition_variable v_module__condition;
	t_object* v_module__thread;
	t_library::t_handle* v_library__handle__finalizing;
	std::map<std::wstring, t_slot> v_symbol__instances;
	std::mutex v_symbol__instantiate__mutex;
	t_shared_pool<t_dictionary::t_entry, 1024> v_dictionary__entry__pool;
	size_t v_dictionary__entry__freed;
	t_slot v_structure_root;
	t_slot v_module_global;
	t_slot v_module_system;
	t_slot v_module_io;
	t_slot v_thread;
	t_slot v_code_fiber;
	size_t v_stack_size;
	bool v_verbose;

	void f_pools__return();
	t_object* f_object__pool__allocate()
	{
		t_object* p = v_object__pool.f_allocate(false);
		if (!p) {
			f_wait();
			p = v_object__pool.f_allocate();
		}
		return p;
	}
	void f_free_as_release(t_object* a_p)
	{
		++v_object__release;
		f_free(v_object__pool, v_object__freed, a_p);
	}
	void f_free_as_collect(t_object* a_p)
	{
		++v_object__collect;
		f_free(v_object__pool, v_object__freed, a_p);
	}
	void f_free(t_fiber::t_context* a_p)
	{
		f_free(v_fiber__context__pool, v_fiber__context__freed, a_p);
	}
	void f_free(t_fiber::t_try* a_p)
	{
		f_free(v_fiber__try__pool, v_fiber__try__freed, a_p);
	}
	void f_free(t_dictionary::t_entry* a_p)
	{
		f_free(v_dictionary__entry__pool, v_dictionary__entry__freed, a_p);
	}
	void f_signal_synchronizers();
	void f_wait_synchronizers();
	void f_collector();

public:
	t_engine(size_t a_stack, bool a_verbose, size_t a_count, char** a_arguments);
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
	t_object* f_module_io() const
	{
		return v_module_io;
	}
	ptrdiff_t f_run();
	void f_synchronize()
	{
		std::lock_guard<std::mutex> lock(v_thread__mutex);
		f_signal_synchronizers();
		f_wait_synchronizers();
	}
};

inline t_engine* f_engine()
{
	return static_cast<t_engine*>(t_value::v_collector);
}

inline t_object* t_object::f_pool__allocate()
{
	return f_engine()->f_object__pool__allocate();
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_object::f_decrement()
{
	assert(v_count > 0);
	if (--v_count > 0) {
#ifdef XEMMAI__OBJECT__CALL_SCAN_BLACK
		f_scan_black();
#endif
		v_color = e_color__PURPLE;
		if (!v_next) f_append(v_roots, this);
	} else {
		static_cast<t_object*>(v_structure->v_this)->f_decrement_member();
		if (v_fields) {
			v_fields->f_scan(f_decrement);
			delete v_fields;
			v_fields = 0;
		}
		t_type* type = f_type_as_type();
		if (!type->v_primitive) {
			type->f_scan(this, f_decrement);
			type->f_finalize(this);
		}
		f_type()->f_decrement_member();
		v_type.v_p = 0;
		v_color = e_color__BLACK;
		if (!v_next) f_engine()->f_free_as_release(this);
	}
}

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_transfer t_object::f_allocate_uninitialized(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	t_value::v_increments->f_push(f_engine()->v_structure_root);
	p->v_structure = static_cast<t_structure*>(static_cast<t_object*>(f_engine()->v_structure_root)->f_pointer());
	p->v_owner = static_cast<t_type*>(a_type->f_pointer())->v_shared ? 0 : t_value::v_increments;
	return t_transfer(p, t_transfer::t_pass());
}

inline t_transfer t_object::f_allocate(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	p->v_type.v_pointer = 0;
	t_value::v_increments->f_push(f_engine()->v_structure_root);
	p->v_structure = static_cast<t_structure*>(static_cast<t_object*>(f_engine()->v_structure_root)->f_pointer());
	p->v_owner = static_cast<t_type*>(a_type->f_pointer())->v_shared ? 0 : t_value::v_increments;
	return t_transfer(p, t_transfer::t_pass());
}
#endif

inline t_fiber::t_context* t_fiber::t_context::f_allocate()
{
	return f_engine()->v_fiber__context__pool.f_allocate();
}

inline void t_fiber::t_context::f_finalize(t_context* a_p)
{
	while (a_p) {
		t_context* p = a_p;
		a_p = p->v_next;
		f_engine()->f_free(p);
	}
}

inline t_fiber::t_try* t_fiber::t_try::f_allocate()
{
	return f_engine()->v_fiber__try__pool.f_allocate();
}

inline t_dictionary::t_entry* t_dictionary::t_entry::f_allocate()
{
	return f_engine()->v_dictionary__entry__pool.f_allocate();
}

inline t_dictionary::t_table::~t_table()
{
	t_entry** entries = f_entries();
	for (size_t i = 0; i < v_capacity; ++i) {
		t_entry* p = entries[i];
		while (p) {
			t_entry* q = p->v_next;
			f_engine()->f_free(p);
			p = q;
		}
	}
}

}

#endif
