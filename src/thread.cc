#include <xemmai/thread.h>

#include <xemmai/engine.h>
#include <xemmai/module.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

void f_main(void* a_p)
{
	f_as<t_fiber*>(t_fiber::v_current)->v_callable->f_call();
}

}

XEMMAI__PORTABLE__THREAD t_object* t_thread::v_current;

void* t_thread::f_main(void* a_p)
{
	v_current = static_cast<t_object*>(a_p);
	t_thread* p = f_as<t_thread*>(v_current);
	t_thread::t_queues* queues = p->v_queues;
	t_pointer::v_increments = &queues->v_increments;
	t_pointer::v_decrements = &queues->v_decrements;
	p->v_active = p->v_fiber;
	t_fiber::v_current = p->v_active;
	t_module::f_main(xemmai::f_main, 0);
	p->v_active = 0;
	p->v_queues = 0;
	t_pointer::v_decrements->f_push(v_current);
	f_engine()->f_pools__return();
	{
		portable::t_scoped_lock lock(f_engine()->v_thread__mutex);
		++queues->v_done;
		f_engine()->v_thread__condition.f_broadcast();
	}
	return 0;
}

t_transfer t_thread::f_instantiate(const t_transfer& a_callable)
{
	t_transfer fiber = t_fiber::f_instantiate(a_callable, true, true);
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_thread>());
	t_thread* p = new t_thread(fiber);
	object->v_pointer = p;
	t_queues* queues = p->v_queues;
	{
		portable::t_scoped_lock lock(f_engine()->v_thread__mutex);
		t_queues*& queueses = f_engine()->v_thread__queueses;
		if (queueses) {
			queues->v_next = queueses->v_next;
			queueses->v_next = queues;
		} else {
			queues->v_next = queues;
		}
		queueses = queues;
	}
	t_pointer::v_increments->f_push(object);
	if (!portable::f_thread(f_main, object)) {
		p->v_queues = 0;
		t_pointer::v_decrements->f_push(object);
		portable::t_scoped_lock lock(f_engine()->v_thread__mutex);
		++queues->v_done;
	}
	return object;
}

void t_thread::f_define(t_object* a_class)
{
	t_define<t_thread, t_object>(f_global(), L"Thread", a_class)
		(L"current", t_static<t_object* (*)(), f_current>())
		(L"join", t_member<void (t_thread::*)(), &t_thread::f_join>())
	;
}

void t_thread::f_join()
{
	if (this == f_as<t_thread*>(v_current)) t_throwable::f_throw(L"current thread can not be joined.");
	if (this == f_as<t_thread*>(f_engine()->v_thread)) t_throwable::f_throw(L"engine thread can not be joined.");
	portable::t_scoped_lock lock(f_engine()->v_thread__mutex);
	while (v_queues) f_engine()->v_thread__condition.f_wait(f_engine()->v_thread__mutex);
}

t_type* t_type_of<t_thread>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_thread>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_thread*>(a_this)->v_fiber);
}

void t_type_of<t_thread>::f_finalize(t_object* a_this)
{
	delete f_as<t_thread*>(a_this);
}

void t_type_of<t_thread>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_transfer x = a_stack.f_pop();
	a_stack.f_return(t_thread::f_instantiate(x));
}

}
