#include <xemmai/thread.h>
#include <xemmai/convert.h>

namespace xemmai
{

class t_threading;

template<>
struct t_type_of<portable::t_mutex> : t_type
{
	typedef t_threading t_extension;

	static void f_acquire(portable::t_mutex& a_self);
	static void f_release(portable::t_mutex& a_self);
	static void f_define(t_threading* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

template<>
struct t_type_of<portable::t_condition> : t_type
{
	typedef t_threading t_extension;

	static void f_wait(portable::t_condition& a_self, portable::t_mutex& a_mutex);
	static void f_wait(portable::t_condition& a_self, portable::t_mutex& a_mutex, size_t a_milliseconds);
	static void f_signal(portable::t_condition& a_self);
	static void f_broadcast(portable::t_condition& a_self);
	static void f_define(t_threading* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
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
inline void t_threading::f_type__<portable::t_mutex>(const t_transfer& a_type)
{
	v_type_mutex = a_type;
}

template<>
inline void t_threading::f_type__<portable::t_condition>(const t_transfer& a_type)
{
	v_type_condition = a_type;
}

template<>
inline t_object* t_threading::f_type<portable::t_mutex>() const
{
	return &*v_type_mutex;
}

template<>
inline t_object* t_threading::f_type<portable::t_condition>() const
{
	return &*v_type_condition;
}

void t_type_of<portable::t_mutex>::f_acquire(portable::t_mutex& a_self)
{
	a_self.f_acquire();
	t_thread::f_cache_acquire();
}

void t_type_of<portable::t_mutex>::f_release(portable::t_mutex& a_self)
{
	t_thread::f_cache_release();
	a_self.f_release();
}

void t_type_of<portable::t_mutex>::f_define(t_threading* a_extension)
{
	t_define<portable::t_mutex, t_object>(a_extension, L"Mutex")
		(L"acquire", t_member<void (*)(portable::t_mutex&), f_acquire>())
		(L"release", t_member<void (*)(portable::t_mutex&), f_release>())
	;
}

t_type* t_type_of<portable::t_mutex>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<portable::t_mutex>::f_finalize(t_object* a_this)
{
	delete &f_as<portable::t_mutex&>(a_this);
}

void t_type_of<portable::t_mutex>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct<portable::t_mutex>::f_call(a_class, a_n, a_stack);
}

void t_type_of<portable::t_condition>::f_wait(portable::t_condition& a_self, portable::t_mutex& a_mutex)
{
	t_thread::f_cache_release();
	a_self.f_wait(a_mutex);
	t_thread::f_cache_acquire();
}

void t_type_of<portable::t_condition>::f_wait(portable::t_condition& a_self, portable::t_mutex& a_mutex, size_t a_milliseconds)
{
	t_thread::f_cache_release();
	a_self.f_wait(a_mutex, a_milliseconds);
	t_thread::f_cache_acquire();
}

void t_type_of<portable::t_condition>::f_signal(portable::t_condition& a_self)
{
	t_thread::f_cache_release();
	a_self.f_signal();
}

void t_type_of<portable::t_condition>::f_broadcast(portable::t_condition& a_self)
{
	t_thread::f_cache_release();
	a_self.f_broadcast();
}

void t_type_of<portable::t_condition>::f_define(t_threading* a_extension)
{
	t_define<portable::t_condition, t_object>(a_extension, L"Condition")
		(L"wait",
			t_member<void (*)(portable::t_condition&, portable::t_mutex&), f_wait>(),
			t_member<void (*)(portable::t_condition&, portable::t_mutex&, size_t), f_wait>()
		)
		(L"signal", t_member<void (*)(portable::t_condition&), f_signal>())
		(L"broadcast", t_member<void (*)(portable::t_condition&), f_broadcast>())
	;
}

t_type* t_type_of<portable::t_condition>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<portable::t_condition>::f_finalize(t_object* a_this)
{
	delete &f_as<portable::t_condition&>(a_this);
}

void t_type_of<portable::t_condition>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct<portable::t_condition>::f_call(a_class, a_n, a_stack);
}

t_threading::t_threading(t_object* a_module) : t_extension(a_module)
{
	t_type_of<portable::t_mutex>::f_define(this);
	t_type_of<portable::t_condition>::f_define(this);
}

void t_threading::f_scan(t_scan a_scan)
{
	a_scan(v_type_mutex);
	a_scan(v_type_condition);
}

}

#ifdef _MSC_VER
extern "C" __declspec(dllexport) xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#else
extern "C" xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#endif
{
	return new xemmai::t_threading(a_module);
}
