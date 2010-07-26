#include <xemmai/value.h>

#include <xemmai/engine.h>

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

XEMMAI__PORTABLE__THREAD t_value::t_collector* t_value::v_collector;
XEMMAI__PORTABLE__THREAD t_value::t_increments* t_value::v_increments;
XEMMAI__PORTABLE__THREAD t_value::t_decrements* t_value::v_decrements;

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_value::t_value(t_object* a_p, const t_own&) : v_p(a_p)
{
	if (v_p) v_increments->f_push(v_p);
}

t_value::t_value(const t_value& a_p, const t_own&) : v_p(a_p.v_p)
{
	switch (reinterpret_cast<size_t>(v_p)) {
	case e_tag__NULL:
		break;
	case e_tag__BOOLEAN:
		v_boolean = a_p.v_boolean;
		break;
	case e_tag__INTEGER:
		v_integer = a_p.v_integer;
		break;
	case e_tag__FLOAT:
		v_float = a_p.v_float;
		break;
	default:
		v_increments->f_push(v_p);
	}
}

void t_value::f_assign(t_object* a_p)
{
	if (a_p) v_increments->f_push(a_p);
	t_object* p = v_p;
	v_p = a_p;
	if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) v_decrements->f_push(p);
}

void t_value::f_assign(const t_value& a_p)
{
	switch (reinterpret_cast<size_t>(a_p.v_p)) {
	case e_tag__NULL:
		break;
	case e_tag__BOOLEAN:
		v_boolean = a_p.v_boolean;
		break;
	case e_tag__INTEGER:
		v_integer = a_p.v_integer;
		break;
	case e_tag__FLOAT:
		v_float = a_p.v_float;
		break;
	default:
		v_increments->f_push(a_p.v_p);
	}
	t_object* p = v_p;
	v_p = a_p.v_p;
	if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) v_decrements->f_push(p);
}

void t_value::f_assign(const t_transfer& a_p)
{
	t_object* p = v_p;
	v_p = a_p.v_p;
	switch (reinterpret_cast<size_t>(v_p)) {
	case e_tag__BOOLEAN:
		v_boolean = a_p.v_boolean;
		break;
	case e_tag__INTEGER:
		v_integer = a_p.v_integer;
		break;
	case e_tag__FLOAT:
		v_float = a_p.v_float;
		break;
	}
	a_p.v_p = 0;
	if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) v_decrements->f_push(p);
}

t_transfer::~t_transfer()
{
	if (reinterpret_cast<size_t>(v_p) >= e_tag__OBJECT) v_decrements->f_push(v_p);
}

t_scoped::~t_scoped()
{
	if (reinterpret_cast<size_t>(v_p) >= e_tag__OBJECT) v_decrements->f_push(v_p);
}

void t_slot::f_construct(t_object* a_p)
{
	assert(!v_p);
	if (a_p) v_increments->f_push(a_p);
	v_p = a_p;
}

void t_slot::f_construct(const t_value& a_p)
{
	assert(!v_p);
	switch (reinterpret_cast<size_t>(a_p.v_p)) {
	case e_tag__NULL:
		break;
	case e_tag__BOOLEAN:
		v_boolean = a_p.v_boolean;
		break;
	case e_tag__INTEGER:
		v_integer = a_p.v_integer;
		break;
	case e_tag__FLOAT:
		v_float = a_p.v_float;
		break;
	default:
		v_increments->f_push(a_p.v_p);
	}
	v_p = a_p.v_p;
}
#endif

}
