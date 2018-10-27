#ifndef XEMMAI__ENGINE_H
#define XEMMAI__ENGINE_H

#include "module.h"
#include "fiber.h"
#include "thread.h"

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
	friend struct t_type_of<t_object>;
	friend class t_structure;
	friend struct t_type_of<t_structure>;
	friend struct t_module;
	friend class t_module::t_scoped_lock;
	friend struct t_script;
	friend struct t_library;
	friend struct t_fiber;
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend struct t_type_of<t_thread>;
	friend class t_symbol;
	friend struct t_type_of<t_symbol>;
	friend class t_code;
	friend struct t_type_of<t_lambda>;
	friend struct t_type_of<t_lambda_shared>;
	friend struct t_type_of<t_advanced_lambda<t_lambda>>;
	friend struct t_type_of<t_advanced_lambda<t_lambda_shared>>;
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
	template<typename T, size_t A_size>
	struct t_pool : t_shared_pool<T, A_size>
	{
		size_t v_freed = 0;

		void f_return_all()
		{
			auto p = t_local_pool<T>::f_detach();
			while (p) {
				auto q = p;
				size_t n = 0;
				while (++n < A_size && q->v_next) q = q->v_next;
				auto p0 = p;
				p = q->v_next;
				q->v_next = nullptr;
				t_shared_pool<T, A_size>::f_free(p0, n);
			}
		}
		void f_return()
		{
			t_shared_pool<T, A_size>::f_free(t_local_pool<T>::f_detach(), v_freed);
			v_freed = 0;
		}
		void f_free(decltype(T::v_next) a_p)
		{
			assert(t_thread::v_current == nullptr);
			t_local_pool<T>::f_free(a_p);
			if (++v_freed >= A_size) f_return();
		}
		size_t f_live() const
		{
			return this->f_allocated() - this->f_freed() - v_freed;
		}
	};

	static XEMMAI__PORTABLE__THREAD size_t v_local_object__allocated;

	size_t v_collector__threshold0;
	size_t v_collector__threshold1;
	t_pool<t_object_and<0>, 4096> v_object__pool0;
	t_pool<t_object_and<1>, 4096> v_object__pool1;
	t_pool<t_object_and<2>, 4096> v_object__pool2;
	t_pool<t_object_and<3>, 4096> v_object__pool3;
	std::atomic<size_t> v_object__allocated = 0;
	size_t v_object__freed = 0;
	size_t v_object__lower0 = 0;
	size_t v_object__lower1 = 0;
	t_object* v_object__cycle = nullptr;
	std::list<t_object*> v_object__cycles;
	bool v_object__reviving = false;
	std::mutex v_object__reviving__mutex;
	size_t v_object__release = 0;
	size_t v_object__collect = 0;
	std::list<t_object*> v_fiber__runnings;
	std::mutex v_fiber__mutex;
	t_thread::t_internal* v_thread__internals = new t_thread::t_internal();
	size_t v_thread__cache_hit = 0;
	size_t v_thread__cache_missed = 0;
	std::mutex v_thread__mutex;
	std::condition_variable v_thread__condition;
	t_synchronizer* v_synchronizers = nullptr;
	volatile size_t v_synchronizer__wake = 0;
	std::mutex v_synchronizer__mutex;
	//std::condition_variable v_synchronizer__condition;
	t_type* v_type_class;
	t_type* v_type_structure;
	std::map<std::wstring, t_slot, std::less<>> v_module__instances;
	std::map<std::wstring, t_slot, std::less<>>::iterator v_module__instances__null;
	std::mutex v_module__mutex;
	std::condition_variable v_module__condition;
	t_object* v_module__thread = nullptr;
	t_library::t_handle* v_library__handle__finalizing = nullptr;
	std::map<std::wstring, t_slot, std::less<>> v_symbol__instances;
	std::mutex v_symbol__instantiate__mutex;
	t_structure* v_structure_root;
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
	decltype(auto) f_object__pool(std::integral_constant<size_t, 0>)
	{
		return (v_object__pool0);
	}
	decltype(auto) f_object__pool(std::integral_constant<size_t, 1>)
	{
		return (v_object__pool1);
	}
	decltype(auto) f_object__pool(std::integral_constant<size_t, 2>)
	{
		return (v_object__pool2);
	}
	decltype(auto) f_object__pool(std::integral_constant<size_t, 3>)
	{
		return (v_object__pool3);
	}
	template<size_t A_rank>
	t_object* f_object__pool__allocate();
	t_object* f_object__allocate(size_t a_size)
	{
		if (++v_local_object__allocated >= 1024) {
			v_object__allocated += 1024;
			v_local_object__allocated = 0;
		}
		return new(new char[sizeof(t_object) + a_size]) t_object(4);
	}
	t_scoped f_object__allocate_on_boot(size_t a_size)
	{
		auto p = f_object__allocate(a_size);
		p->v_next = nullptr;
		return {p, t_scoped::t_pass()};
	}
	void f_free(t_object* a_p)
	{
		a_p->v_owner = nullptr;
		a_p->v_structure = v_structure_root;
		a_p->v_count = 1;
		switch (a_p->v_rank) {
		case 0:
			v_object__pool0.f_free(a_p);
			break;
		case 1:
			v_object__pool1.f_free(a_p);
			break;
		case 2:
			v_object__pool2.f_free(a_p);
			break;
		case 3:
			v_object__pool3.f_free(a_p);
			break;
		default:
			++v_object__freed;
			delete a_p;
		}
	}
	void f_free_as_release(t_object* a_p)
	{
		++v_object__release;
		f_free(a_p);
	}
	void f_free_as_collect(t_object* a_p)
	{
		++v_object__collect;
		f_free(a_p);
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
	const std::map<std::wstring, t_slot, std::less<>>& f_modules() const
	{
		return v_module__instances;
	}
	void f_context_print(std::FILE* a_out, t_lambda* a_lambda, void** a_pc);
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

template<size_t A_rank>
inline t_object* t_engine::f_object__pool__allocate()
{
	auto p = f_object__pool(std::integral_constant<size_t, A_rank>()).f_allocate(false);
	if (!p) {
		f_wait();
		p = f_object__pool(std::integral_constant<size_t, A_rank>()).f_allocate();
	}
	return p;
}

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

inline t_object::t_object(size_t a_rank) : v_rank(a_rank), v_structure(f_engine()->v_structure_root)
{
}

template<size_t A_rank>
inline t_object* t_object::f_pool__allocate()
{
	return f_engine()->f_object__pool__allocate<A_rank>();
}

inline t_object* t_object::f_local_pool__allocate(size_t a_size)
{
	switch ((sizeof(t_object) - sizeof(v_data) + a_size - 1) / (sizeof(void*) * 8)) {
	case 0:
		return t_local_pool<t_object_and<0>>::f_allocate(f_pool__allocate<0>);
	case 1:
		return t_local_pool<t_object_and<1>>::f_allocate(f_pool__allocate<1>);
	case 2:
		return t_local_pool<t_object_and<2>>::f_allocate(f_pool__allocate<2>);
	case 3:
		return t_local_pool<t_object_and<3>>::f_allocate(f_pool__allocate<3>);
	default:
		return f_engine()->f_object__allocate(a_size);
	}
}

inline void t_object::f_decrement_step()
{
	t_object::f_of(v_structure)->f_decrement_push();
	if (v_fields) {
		v_fields->f_scan(f_push_and_clear<&t_object::f_decrement_push>);
		delete v_fields;
		v_fields = nullptr;
	}
	if (!v_type->v_primitive) {
		v_type->f_scan(this, f_push_and_clear<&t_object::f_decrement_push>);
		v_type->f_finalize(this);
	}
	t_object::f_of(v_type)->f_decrement_push();
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

inline t_object* t_fiber::f_current()
{
	return f_as<t_thread&>(t_thread::v_current).v_active;
}

}

#endif
