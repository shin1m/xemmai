#include <xemmai/convert.h>

namespace xemmai
{

struct t_threading;

template<>
struct t_type_of<std::mutex> : t_holds<std::mutex>
{
	using t_library = t_threading;

	static void f_define(t_threading* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<std::condition_variable> : t_holds<std::condition_variable>
{
	using t_library = t_threading;

	static void f_define(t_threading* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

struct t_threading : t_library
{
	t_slot_of<t_type> v_type_mutex;
	t_slot_of<t_type> v_type_condition;

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_threading, t_global, f_global())
XEMMAI__LIBRARY__TYPE_AS(t_threading, std::mutex, mutex)
XEMMAI__LIBRARY__TYPE_AS(t_threading, std::condition_variable, condition)

void t_type_of<std::mutex>::f_define(t_threading* a_library)
{
	t_define{a_library}
	(L"acquire"sv, t_member<void(*)(std::mutex&), [](std::mutex& a_self)
	{
		t_safe_region region;
		a_self.lock();
	}>())
#ifdef _MSC_VER
	(L"release"sv, t_member<void(*)(std::mutex&), [](std::mutex& a_self)
	{
		a_self.unlock();
	}>())
#else
	(L"release"sv, t_member<void(std::mutex::*)(), &std::mutex::unlock>())
#endif
	.f_derive<std::mutex, t_object>();
}

t_pvalue t_type_of<std::mutex>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::mutex>::f_do(this, a_stack, a_n);
}

void t_type_of<std::condition_variable>::f_define(t_threading* a_library)
{
	t_define{a_library}
	(L"wait"sv,
		t_member<void(*)(std::condition_variable&, std::mutex&), [](std::condition_variable& a_self, std::mutex& a_mutex)
		{
			t_safe_region region;
			std::unique_lock lock(a_mutex, std::defer_lock);
			a_self.wait(lock);
		}>(),
		t_member<void(*)(std::condition_variable&, std::mutex&, size_t), [](std::condition_variable& a_self, std::mutex& a_mutex, size_t a_milliseconds)
		{
			t_safe_region region;
			std::unique_lock lock(a_mutex, std::defer_lock);
			a_self.wait_for(lock, std::chrono::milliseconds(a_milliseconds));
		}>()
	)
	(L"signal"sv, t_member<void(std::condition_variable::*)(), &std::condition_variable::notify_one>())
	(L"broadcast"sv, t_member<void(std::condition_variable::*)(), &std::condition_variable::notify_all>())
	.f_derive<std::condition_variable, t_object>();
}

t_pvalue t_type_of<std::condition_variable>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<std::condition_variable>::f_do(this, a_stack, a_n);
}

void t_threading::f_scan(t_scan a_scan)
{
	a_scan(v_type_mutex);
	a_scan(v_type_condition);
}

std::vector<std::pair<t_root, t_rvalue>> t_threading::f_define()
{
	t_type_of<std::mutex>::f_define(this);
	t_type_of<std::condition_variable>::f_define(this);
	return t_define(this)
	(L"Mutex"sv, static_cast<t_object*>(v_type_mutex))
	(L"Condition"sv, static_cast<t_object*>(v_type_condition))
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmai::t_threading>(a_handle);
}
