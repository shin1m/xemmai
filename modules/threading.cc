#include <xemmai/thread.h>
#include <xemmai/convert.h>

namespace xemmai
{

class t_threading;

template<>
struct t_type_of<std::mutex> : t_type
{
	typedef t_threading t_extension;

	static void f_acquire(std::mutex& a_self);
	static void f_release(std::mutex& a_self);
	static void f_define(t_threading* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_fixed = v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<>
struct t_type_of<std::condition_variable> : t_type
{
	typedef t_threading t_extension;

	static void f_wait(std::condition_variable& a_self, std::mutex& a_mutex);
	static void f_wait(std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds);
	static void f_signal(std::condition_variable& a_self);
	static void f_broadcast(std::condition_variable& a_self);
	static void f_define(t_threading* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_fixed = v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

class t_threading : public t_extension
{
	template<typename T, typename T_super> friend class t_define;

	t_slot v_type_mutex;
	t_slot v_type_condition;

	template<typename T>
	void f_type__(const t_transfer& a_type);

public:
	t_threading(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

template<>
inline void t_threading::f_type__<std::mutex>(const t_transfer& a_type)
{
	v_type_mutex = a_type;
}

template<>
inline void t_threading::f_type__<std::condition_variable>(const t_transfer& a_type)
{
	v_type_condition = a_type;
}

template<>
inline t_object* t_threading::f_type<std::mutex>() const
{
	return v_type_mutex;
}

template<>
inline t_object* t_threading::f_type<std::condition_variable>() const
{
	return v_type_condition;
}

void t_type_of<std::mutex>::f_acquire(std::mutex& a_self)
{
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
		(L"acquire", t_member<void (*)(std::mutex&), f_acquire>())
		(L"release", t_member<void (*)(std::mutex&), f_release>())
	;
}

t_type* t_type_of<std::mutex>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<std::mutex>::f_finalize(t_object* a_this)
{
	delete &f_as<std::mutex&>(a_this);
}

t_transfer t_type_of<std::mutex>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::mutex>::f_do(a_class, a_stack, a_n);
}

void t_type_of<std::condition_variable>::f_wait(std::condition_variable& a_self, std::mutex& a_mutex)
{
	t_thread::f_cache_release();
	{
		std::unique_lock<std::mutex> lock(a_mutex, std::adopt_lock);
		a_self.wait(lock);
	}
	t_thread::f_cache_acquire();
}

void t_type_of<std::condition_variable>::f_wait(std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds)
{
	t_thread::f_cache_release();
	{
		std::unique_lock<std::mutex> lock(a_mutex, std::adopt_lock);
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
			t_member<void (*)(std::condition_variable&, std::mutex&), f_wait>(),
			t_member<void (*)(std::condition_variable&, std::mutex&, size_t), f_wait>()
		)
		(L"signal", t_member<void (*)(std::condition_variable&), f_signal>())
		(L"broadcast", t_member<void (*)(std::condition_variable&), f_broadcast>())
	;
}

t_type* t_type_of<std::condition_variable>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<std::condition_variable>::f_finalize(t_object* a_this)
{
	delete &f_as<std::condition_variable&>(a_this);
}

t_transfer t_type_of<std::condition_variable>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::condition_variable>::f_do(a_class, a_stack, a_n);
}

t_threading::t_threading(t_object* a_module) : t_extension(a_module)
{
	t_type_of<std::mutex>::f_define(this);
	t_type_of<std::condition_variable>::f_define(this);
}

void t_threading::f_scan(t_scan a_scan)
{
	a_scan(v_type_mutex);
	a_scan(v_type_condition);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmai::t_threading(a_module);
}
