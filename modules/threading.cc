#include <xemmai/convert.h>

namespace xemmai
{

class t_threading;

template<>
struct t_type_of<std::mutex> : t_underivable<t_with_traits<t_holds<std::mutex>, true, true>>
{
	typedef t_threading t_extension;

	static void f_acquire(std::mutex& a_self);
	static void f_release(std::mutex& a_self);
	static void f_define(t_threading* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<std::condition_variable> : t_underivable<t_with_traits<t_holds<std::condition_variable>, true, true>>
{
	typedef t_threading t_extension;

	static void f_wait(std::condition_variable& a_self, std::mutex& a_mutex);
	static void f_wait(std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds);
	static void f_signal(std::condition_variable& a_self);
	static void f_broadcast(std::condition_variable& a_self);
	static void f_define(t_threading* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

class t_threading : public t_extension
{
	t_slot_of<t_type> v_type_mutex;
	t_slot_of<t_type> v_type_condition;

public:
	t_threading(t_object* a_module);
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_type_mutex);
		a_scan(v_type_condition);
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_threading*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
};

template<>
inline t_slot_of<t_type>& t_threading::f_type_slot<std::mutex>()
{
	return v_type_mutex;
}

template<>
inline t_slot_of<t_type>& t_threading::f_type_slot<std::condition_variable>()
{
	return v_type_condition;
}

void t_type_of<std::mutex>::f_acquire(std::mutex& a_self)
{
	t_safe_region region;
	a_self.lock();
	t_thread::f_cache_acquire();
}

void t_type_of<std::mutex>::f_release(std::mutex& a_self)
{
	t_thread::f_cache_release();
	a_self.unlock();
}

void t_type_of<std::mutex>::f_define(t_threading* a_extension)
{
	t_define<std::mutex, t_object>(a_extension, L"Mutex")
		(L"acquire", t_member<void(*)(std::mutex&), f_acquire>())
		(L"release", t_member<void(*)(std::mutex&), f_release>())
	;
}

t_scoped t_type_of<std::mutex>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::mutex>::f_do(this, a_stack, a_n);
}

void t_type_of<std::condition_variable>::f_wait(std::condition_variable& a_self, std::mutex& a_mutex)
{
	t_safe_region region;
	t_thread::f_cache_release();
	{
		std::unique_lock<std::mutex> lock(a_mutex, std::defer_lock);
		a_self.wait(lock);
	}
	t_thread::f_cache_acquire();
}

void t_type_of<std::condition_variable>::f_wait(std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds)
{
	t_safe_region region;
	t_thread::f_cache_release();
	{
		std::unique_lock<std::mutex> lock(a_mutex, std::defer_lock);
		a_self.wait_for(lock, std::chrono::milliseconds(a_milliseconds));
	}
	t_thread::f_cache_acquire();
}

void t_type_of<std::condition_variable>::f_signal(std::condition_variable& a_self)
{
	t_thread::f_cache_release();
	a_self.notify_one();
}

void t_type_of<std::condition_variable>::f_broadcast(std::condition_variable& a_self)
{
	t_thread::f_cache_release();
	a_self.notify_all();
}

void t_type_of<std::condition_variable>::f_define(t_threading* a_extension)
{
	t_define<std::condition_variable, t_object>(a_extension, L"Condition")
		(L"wait",
			t_member<void(*)(std::condition_variable&, std::mutex&), f_wait>(),
			t_member<void(*)(std::condition_variable&, std::mutex&, size_t), f_wait>()
		)
		(L"signal", t_member<void(*)(std::condition_variable&), f_signal>())
		(L"broadcast", t_member<void(*)(std::condition_variable&), f_broadcast>())
	;
}

t_scoped t_type_of<std::condition_variable>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::condition_variable>::f_do(this, a_stack, a_n);
}

t_threading::t_threading(t_object* a_module) : t_extension(a_module)
{
	t_type_of<std::mutex>::f_define(this);
	t_type_of<std::condition_variable>::f_define(this);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmai::t_threading(a_module);
}
