#include <xemmai/pointer.h>

#include <xemmai/engine.h>

namespace xemmai
{

void t_pointer::t_increments::f_flush()
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

void t_pointer::t_decrements::f_flush()
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

XEMMAI__PORTABLE__THREAD t_pointer::t_collector* t_pointer::v_collector;
XEMMAI__PORTABLE__THREAD t_pointer::t_increments* t_pointer::v_increments;
XEMMAI__PORTABLE__THREAD t_pointer::t_decrements* t_pointer::v_decrements;

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_pointer::t_pointer(t_object* a_p) : v_p(a_p)
{
	if (v_p) v_increments->f_push(v_p);
}

t_pointer::t_pointer(const t_pointer& a_p) : v_p(a_p.v_p)
{
	if (v_p) v_increments->f_push(v_p);
}

t_transfer::~t_transfer()
{
	if (v_p) v_decrements->f_push(v_p);
}

t_transfer& t_transfer::operator=(t_object* a_p)
{
	if (a_p) v_increments->f_push(a_p);
	t_object* p = v_p;
	v_p = a_p;
	if (p) v_decrements->f_push(p);
	return *this;
}

t_transfer& t_transfer::operator=(const t_transfer& a_p)
{
	t_object* p = v_p;
	v_p = a_p;
	a_p.v_p = 0;
	if (p) v_decrements->f_push(p);
	return *this;
}

t_shared& t_shared::operator=(t_object* a_p)
{
	if (a_p) v_increments->f_push(a_p);
	t_object* p = v_p;
	v_p = a_p;
	if (p) v_decrements->f_push(p);
	return *this;
}

t_shared& t_shared::operator=(const t_transfer& a_p)
{
	t_object* p = v_p;
	v_p = a_p;
	a_p.v_p = 0;
	if (p) v_decrements->f_push(p);
	return *this;
}

t_scoped::~t_scoped()
{
	if (v_p) v_decrements->f_push(v_p);
}

void t_slot::f_construct(t_object* a_p)
{
	assert(!v_p);
	if (a_p) v_increments->f_push(a_p);
	v_p = a_p;
}
#endif

}
