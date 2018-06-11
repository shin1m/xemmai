#include <xemmai/value.h>

#include <xemmai/global.h>

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

t_scoped t_value::f_get_primitive(t_object* a_key) const
{
	assert(f_tag() < t_value::e_tag__OBJECT);
	size_t i = t_thread::t_cache::f_index(*this, a_key);
	auto& cache = t_thread::v_cache[i];
	auto& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == *this && static_cast<t_object*>(cache.v_key) == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	t_scoped value = static_cast<t_object*>(f_type()->v_this)->f_get(a_key);
	if (value.f_type() == f_global()->f_type<t_method>()) value = f_as<t_method&>(value).f_bind(*this);
	cache.v_object = *this;
	cache.v_key = a_key;
	return cache.v_value = std::move(value);
}

}
