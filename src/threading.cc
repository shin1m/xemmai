#include <xemmai/threading.h>

#include <xemmai/thread.h>
#include <xemmai/convert.h>

namespace xemmai
{

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
	delete f_as<portable::t_mutex*>(a_this);
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
	delete f_as<portable::t_condition*>(a_this);
}

void t_type_of<portable::t_condition>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct<portable::t_condition>::f_call(a_class, a_n, a_stack);
}

t_threading::t_threading(t_object* a_module) : t_extension(a_module)
{
	t_type_of<portable::t_mutex>::f_define(this);
	xemmai::f_as<t_type*>(v_type_mutex)->v_builtin = true;
	t_type_of<portable::t_condition>::f_define(this);
	xemmai::f_as<t_type*>(v_type_condition)->v_builtin = true;
}

void t_threading::f_scan(t_scan a_scan)
{
	a_scan(v_type_mutex);
	a_scan(v_type_condition);
}

}
