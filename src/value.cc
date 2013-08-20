#include <xemmai/value.h>

#include <xemmai/engine.h>
#include <xemmai/throwable.h>

namespace xemmai
{

void t_value::t_increments::f_flush()
{
	t_object* volatile* end = v_objects + V_SIZE - 1;
	t_object* volatile* tail = v_tail;
	t_object* volatile* epoch = v_epoch;
	if (epoch > v_objects)
		--epoch;
	else
		epoch = end;
	while (tail != epoch) {
		t_object* volatile* next = epoch;
		if (tail < end) {
			if (next < tail) next = end;
			++tail;
		} else {
			tail = v_objects;
		}
		while (true) {
			(*tail)->f_increment();
			if (tail == next) break;
			++tail;
		}
	}
	v_tail = tail;
}

void t_value::t_decrements::f_flush()
{
	t_object* volatile* end = v_objects + V_SIZE - 1;
	t_object* volatile* tail = v_tail;
	t_object* volatile* epoch = v_last;
	if (epoch > v_objects)
		--epoch;
	else
		epoch = end;
	while (tail != epoch) {
		t_object* volatile* next = epoch;
		if (tail < end) {
			if (next < tail) next = end;
			++tail;
		} else {
			tail = v_objects;
		}
		while (true) {
			(*tail)->f_decrement();
			if (tail == next) break;
			++tail;
		}
	}
	v_tail = tail;
	v_last = v_epoch;
}

thread_local t_value::t_collector* t_value::v_collector;
thread_local t_value::t_increments* t_value::v_increments;
thread_local t_value::t_decrements* t_value::v_decrements;

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_increments* t_value::f_increments()
{
	return v_increments;
}

t_decrements* t_value::f_decrements()
{
	return v_decrements;
}
#endif

thread_local t_stack* t_stack::v_instance;

void t_stack::f_expand(t_slot* a_p)
{
	if (a_p > f_head() + v_size) t_throwable::f_throw(L"stack overflow.");
	while (v_tail < a_p) new(v_tail++) t_slot();
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_stack* f_stack()
{
	return t_stack::v_instance;
}
#endif

}
