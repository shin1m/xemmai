#include <xemmai/value.h>

#include <xemmai/engine.h>

namespace xemmai
{

void t_value::t_increments::f_flush()
{
	auto end = v_objects + V_SIZE - 1;
	auto tail = v_tail.load(std::memory_order_relaxed);
	auto epoch = v_head.load(std::memory_order_acquire);
	if (epoch > v_objects)
		--epoch;
	else
		epoch = end;
	while (tail != epoch) {
		auto next = epoch;
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
	v_tail.store(tail, std::memory_order_release);
}

void t_value::t_decrements::f_flush()
{
	auto end = v_objects + V_SIZE - 1;
	auto tail = v_tail.load(std::memory_order_relaxed);
	auto epoch = v_last;
	if (epoch > v_objects)
		--epoch;
	else
		epoch = end;
	while (tail != epoch) {
		auto next = epoch;
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
	v_tail.store(tail, std::memory_order_release);
	v_last = v_head.load(std::memory_order_acquire);
}

XEMMAI__PORTABLE__THREAD t_value::t_collector* t_value::v_collector;
XEMMAI__PORTABLE__THREAD t_value::t_increments* t_value::v_increments;
XEMMAI__PORTABLE__THREAD t_value::t_decrements* t_value::v_decrements;

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

const t_value t_value::v_null;
const t_value t_value::v_true(true);
const t_value t_value::v_false(false);

XEMMAI__PORTABLE__THREAD t_stack* t_stack::v_instance;

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_stack* f_stack()
{
	return t_stack::v_instance;
}
#endif

}
