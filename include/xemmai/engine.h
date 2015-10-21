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
struct t_safe_region;

struct t_debugger
{
	virtual void f_stopped(t_object* a_thread) = 0;
	virtual void f_loaded(t_object* a_thread) = 0;
};

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
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend struct t_type_of<t_thread>;
	friend class t_symbol;
	friend struct t_type_of<t_symbol>;
	friend class t_code;
	friend class t_dictionary;
	friend class t_dictionary::t_entry;
	friend class t_global;
	friend struct t_safe_region;

	struct t_synchronizer
	{
		t_engine* v_engine;
		size_t v_cpu;
		bool v_wake = false;
		std::mutex v_mutex;
		std::condition_variable v_condition;
		t_synchronizer* v_next;

		t_synchronizer(t_engine* a_engine, size_t a_cpu) : v_engine(a_engine), v_cpu(a_cpu), v_next(a_engine->v_synchronizers)
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
			q->v_next = nullptr;
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
		assert(t_thread::v_current == nullptr);
		t_local_pool<T>::f_free(a_p);
		if (++a_freed >= A_size) f_return(a_pool, a_freed);
	}

	t_shared_pool<t_object, 4096> v_object__pool;
	size_t v_object__freed = 0;
	t_object* v_object__cycle = nullptr;
	std::list<t_object*> v_object__cycles;
	bool v_object__reviving = false;
	std::mutex v_object__reviving__mutex;
	size_t v_object__release = 0;
	size_t v_object__collect = 0;
	t_structure* v_structure__finalizing = nullptr;
	std::list<t_object*> v_fiber__runnings;
	std::mutex v_fiber__mutex;
	t_thread::t_internal* v_thread__internals = nullptr;
	size_t v_thread__cache_hit = 0;
	size_t v_thread__cache_missed = 0;
	std::mutex v_thread__mutex;
	std::condition_variable v_thread__condition;
	t_synchronizer* v_synchronizers = nullptr;
	volatile size_t v_synchronizer__wake = 0;
	std::mutex v_synchronizer__mutex;
	std::condition_variable v_synchronizer__condition;
	t_object* v_type_class;
	std::map<std::wstring, t_slot> v_module__instances;
	std::map<std::wstring, t_slot>::iterator v_module__instances__null;
	std::mutex v_module__mutex;
	std::condition_variable v_module__condition;
	t_object* v_module__thread = nullptr;
	t_library::t_handle* v_library__handle__finalizing = nullptr;
	std::map<std::wstring, t_slot> v_symbol__instances;
	std::mutex v_symbol__instantiate__mutex;
	t_shared_pool<t_dictionary::t_entry, 1024> v_dictionary__entry__pool;
	size_t v_dictionary__entry__freed = 0;
	t_scoped v_structure_root;
	t_scoped v_module_global;
	t_scoped v_module_system;
	t_scoped v_module_io;
	t_scoped v_fiber_exit;
	t_scoped v_thread;
	size_t v_stack_size;
	bool v_verbose;
	t_debugger* v_debugger = nullptr;
	bool v_debug__stopping = false;
	size_t v_debug__safe = 0;
	t_object* v_debug__stepping = nullptr;

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
	void f_free(t_dictionary::t_entry* a_p)
	{
		f_free(v_dictionary__entry__pool, v_dictionary__entry__freed, a_p);
	}
	void f_signal_synchronizers();
	void f_wait_synchronizers();
	void f_collector();
	void f_debug_stop_and_wait(std::unique_lock<std::mutex>& a_lock);
	void f_debug_enter_and_notify()
	{
		++v_debug__safe;
		v_thread__condition.notify_all();
	}
	void f_debug_wait_and_leave(std::unique_lock<std::mutex>& a_lock)
	{
		do v_thread__condition.wait(a_lock); while (v_debug__stopping);
		assert(v_debug__safe > 0);
		--v_debug__safe;
	}
	void f_debug_enter_leave(std::unique_lock<std::mutex>& a_lock)
	{
		f_debug_enter_and_notify();
		f_debug_wait_and_leave(a_lock);
	}
	void f_debug_safe_point(std::unique_lock<std::mutex>& a_lock);
	template<typename T>
	void f_debug_break_point(std::unique_lock<std::mutex>& a_lock, T a_member);
	void f_debug_break_point(std::unique_lock<std::mutex>& a_lock);
	void f_debug_script_loaded();
	void f_debug_safe_region_leave(std::unique_lock<std::mutex>& a_lock);

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
	intptr_t f_run(t_debugger* a_debugger);
	void f_synchronize()
	{
		std::lock_guard<std::mutex> lock(v_thread__mutex);
		f_signal_synchronizers();
		f_wait_synchronizers();
	}
	template<typename T>
	void f_threads(T a_callback)
	{
		for (auto p = v_thread__internals; p; p = p->v_next) if (p->v_done <= 0 && p->v_thread) a_callback(p->v_thread);
	}
	const std::map<std::wstring, t_slot>& f_modules() const
	{
		return v_module__instances;
	}
	void f_context_print(std::FILE* a_out, t_object* a_lambda, void** a_pc);
	void f_debug_safe_point();
	void f_debug_break_point();
	void f_debug_safe_region_enter();
	void f_debug_safe_region_leave();
	void f_debug_stop();
	t_object* f_debug_stepping() const
	{
		return v_debug__stepping;
	}
	void f_debug_continue(t_object* a_stepping = nullptr);
};

template<typename T>
void t_engine::f_debug_break_point(std::unique_lock<std::mutex>& a_lock, T a_member)
{
	while (v_debug__stopping) f_debug_enter_leave(a_lock);
	++v_debug__safe;
	f_debug_stop_and_wait(a_lock);
	(v_debugger->*a_member)(t_thread::f_current());
	f_debug_wait_and_leave(a_lock);
}

inline t_engine* f_engine()
{
	return static_cast<t_engine*>(t_value::v_collector);
}

struct t_safe_region
{
	t_safe_region()
	{
		if (f_engine()->v_debugger) f_engine()->f_debug_safe_region_enter();
	}
	~t_safe_region()
	{
		if (f_engine()->v_debugger) f_engine()->f_debug_safe_region_leave();
	}
};

inline t_object* t_object::f_pool__allocate()
{
	return f_engine()->f_object__pool__allocate();
}

inline void t_object::f_decrement_step()
{
	static_cast<t_object*>(v_structure->v_this)->f_decrement_push();
	if (v_fields) {
		v_fields->f_scan(f_push_and_clear<&t_object::f_decrement_push>);
		delete v_fields;
		v_fields = nullptr;
	}
	t_type* type = f_type_as_type();
	if (!type->v_primitive) {
		type->f_scan(this, f_push_and_clear<&t_object::f_decrement_push>);
		type->f_finalize(this);
	}
	f_type()->f_decrement_push();
	v_type.v_p = nullptr;
	v_color = e_color__BLACK;
	if (!v_next) f_engine()->f_free_as_release(this);
}

inline void t_object::f_collect_white_push()
{
	if (v_color != e_color__WHITE) return;
	v_color = e_color__ORANGE;
	f_append(f_engine()->v_object__cycle, this);
	f_push(this);
}

inline void t_object::f_collect_white()
{
	v_color = e_color__ORANGE;
	f_append(f_engine()->v_object__cycle, this);
	f_loop<&t_object::f_step<&t_object::f_collect_white_push>>();
}

inline t_scoped t_object::f_allocate_uninitialized(t_object* a_type)
{
	t_object* p = f_local_pool__allocate();
	p->v_next = nullptr;
	p->v_count = 1;
	p->v_type.f_construct_nonnull(a_type);
	t_value::f_increments()->f_push(f_engine()->v_structure_root);
	p->v_structure = static_cast<t_structure*>(static_cast<t_object*>(f_engine()->v_structure_root)->f_pointer());
	p->v_owner = static_cast<t_type*>(a_type->f_pointer())->v_shared ? nullptr : t_value::f_increments();
	return t_scoped(p, t_scoped::t_pass());
}

inline t_scoped t_object::f_allocate(t_object* a_type)
{
	t_object* p = f_local_pool__allocate();
	p->v_next = nullptr;
	p->v_count = 1;
	p->v_type.f_construct_nonnull(a_type);
	p->v_type.v_pointer = nullptr;
	t_value::f_increments()->f_push(f_engine()->v_structure_root);
	p->v_structure = static_cast<t_structure*>(static_cast<t_object*>(f_engine()->v_structure_root)->f_pointer());
	p->v_owner = static_cast<t_type*>(a_type->f_pointer())->v_shared ? nullptr : t_value::f_increments();
	return t_scoped(p, t_scoped::t_pass());
}

inline t_object* t_fiber::f_current()
{
	return f_as<t_thread&>(t_thread::v_current).v_active;
}

inline t_context::t_context() : v_next(nullptr)
{
	t_stack* stack = t_stack::v_instance = &f_as<t_fiber&>(t_fiber::f_current()).v_stack;
	v_base = stack->f_head();
	f_pc() = nullptr;
	v_instance = this;
}

inline void t_context::f_terminate()
{
	assert(!v_next);
	t_fiber& fiber = f_as<t_fiber&>(t_fiber::f_current());
	assert(fiber.v_stack.v_used == fiber.v_stack.f_head());
	fiber.v_context = nullptr;
}

class t_native_context
{
	size_t& v_native;
	bool v_done = false;

public:
	t_native_context() : v_native(f_context()->v_native)
	{
		++v_native;
	}
	~t_native_context()
	{
		--v_native;
		if (!v_done) ++f_as<t_fiber&>(t_fiber::f_current()).v_undone;
	}
	void f_done()
	{
		v_done = true;
	}
};

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
