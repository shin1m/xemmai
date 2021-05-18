#include <xemmai/convert.h>

namespace xemmai
{

class t_threading;

template<>
struct t_type_of<std::mutex> : t_holds<std::mutex>
{
	using t_library = t_threading;

	static void f_acquire(std::mutex& a_self);
	static void f_release(std::mutex& a_self);
	static void f_define(t_threading* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<std::condition_variable> : t_holds<std::condition_variable>
{
	using t_library = t_threading;

	static void f_wait(std::condition_variable& a_self, std::mutex& a_mutex);
	static void f_wait(std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds);
	static void f_signal(std::condition_variable& a_self);
	static void f_broadcast(std::condition_variable& a_self);
	static void f_define(t_threading* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

class t_threading : public t_library
{
	t_slot_of<t_type> v_type_mutex;
	t_slot_of<t_type> v_type_condition;

public:
	using t_library::t_library;
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_type_mutex);
		a_scan(v_type_condition);
	}
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
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
	t_pvalue f_as(T&& a_value) const
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
}

void t_type_of<std::mutex>::f_release(std::mutex& a_self)
{
	a_self.unlock();
}

void t_type_of<std::mutex>::f_define(t_threading* a_library)
{
	t_define{a_library}
		(L"acquire"sv, t_member<void(*)(std::mutex&), f_acquire>())
		(L"release"sv, t_member<void(*)(std::mutex&), f_release>())
	.f_derive<std::mutex, t_object>();
}

t_pvalue t_type_of<std::mutex>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::mutex>::f_do(this, a_stack, a_n);
}

void t_type_of<std::condition_variable>::f_wait(std::condition_variable& a_self, std::mutex& a_mutex)
{
	t_safe_region region;
	std::unique_lock lock(a_mutex, std::defer_lock);
	a_self.wait(lock);
}

void t_type_of<std::condition_variable>::f_wait(std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds)
{
	t_safe_region region;
	std::unique_lock lock(a_mutex, std::defer_lock);
	a_self.wait_for(lock, std::chrono::milliseconds(a_milliseconds));
}

void t_type_of<std::condition_variable>::f_signal(std::condition_variable& a_self)
{
	a_self.notify_one();
}

void t_type_of<std::condition_variable>::f_broadcast(std::condition_variable& a_self)
{
	a_self.notify_all();
}

void t_type_of<std::condition_variable>::f_define(t_threading* a_library)
{
	t_define{a_library}
		(L"wait"sv,
			t_member<void(*)(std::condition_variable&, std::mutex&), f_wait>(),
			t_member<void(*)(std::condition_variable&, std::mutex&, size_t), f_wait>()
		)
		(L"signal"sv, t_member<void(*)(std::condition_variable&), f_signal>())
		(L"broadcast"sv, t_member<void(*)(std::condition_variable&), f_broadcast>())
	.f_derive<std::condition_variable, t_object>();
}

t_pvalue t_type_of<std::condition_variable>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::condition_variable>::f_do(this, a_stack, a_n);
}

std::vector<std::pair<t_root, t_rvalue>> t_threading::f_define()
{
	t_type_of<std::mutex>::f_define(this);
	t_type_of<std::condition_variable>::f_define(this);
	return t_define(this)
		(L"Mutex"sv, t_object::f_of(v_type_mutex))
		(L"Condition"sv, t_object::f_of(v_type_condition))
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmai::t_threading>(a_handle);
}
