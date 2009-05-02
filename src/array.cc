#include <xemmai/array.h>

#include <xemmai/boolean.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_slot* t_array::f_move_forward(t_slot* a_p, t_slot* a_q)
{
	while (a_p < a_q) {
		t_slot& s = *a_q;
		s = (--a_q)->f_transfer();
	}
	return a_q;
}

t_slot* t_array::f_move_backward(t_slot* a_p, t_slot* a_q)
{
	while (a_p < a_q) {
		t_slot& s = *a_p;
		s = (++a_p)->f_transfer();
	}
	return a_p;
}

void t_array::f_resize()
{
	t_tuple& tuple0 = f_as<t_tuple&>(v_tuple);
	t_transfer p = t_tuple::f_instantiate(v_size * 2);
	t_tuple& tuple1 = f_as<t_tuple&>(p);
	for (size_t i = 0; i < v_size; ++i) tuple1[i] = tuple0[(v_head + i) % tuple0.f_size()];
	v_tuple = p;
	v_head = 0;
}

void t_array::f_grow()
{
	if (v_size <= 0)
		v_tuple = t_tuple::f_instantiate(4);
	else
		if (v_size >= f_as<t_tuple&>(v_tuple).f_size()) f_resize();
}

void t_array::f_shrink()
{
	if (v_size * 4 >= f_as<t_tuple&>(v_tuple).f_size()) return;
	if (v_size > 1) {
		f_resize();
	} else if (v_size <= 0) {
		v_tuple = 0;
		v_head = 0;
	}
}

t_transfer t_array::f_instantiate()
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_array>());
	object->v_pointer = new t_array();
	return object;
}

const t_slot& t_array::operator[](int a_index) const
{
	if (a_index < 0) {
		if (a_index < -static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
		a_index = v_size - a_index;
	} else {
		if (a_index >= static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	}
	const t_tuple& tuple = f_as<const t_tuple&>(v_tuple);
	return tuple[(v_head + a_index) % tuple.f_size()];
}

t_slot& t_array::operator[](int a_index)
{
	if (a_index < 0) {
		if (a_index < -static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
		a_index = v_size - a_index;
	} else {
		if (a_index >= static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	}
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	return tuple[(v_head + a_index) % tuple.f_size()];
}

void t_array::f_push(const t_transfer& a_value)
{
	f_grow();
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	tuple[(v_head + v_size) % tuple.f_size()] = a_value;
	++v_size;
}

t_transfer t_array::f_pop()
{
	if (v_size <= 0) t_throwable::f_throw(L"empty array.");
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	t_transfer p = tuple[(v_head + --v_size) % tuple.f_size()].f_transfer();
	f_shrink();
	return p;
}

void t_array::f_unshift(const t_transfer& a_value)
{
	f_grow();
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	v_head += tuple.f_size() - 1;
	v_head %= tuple.f_size();
	tuple[v_head] = a_value;
	++v_size;
}

t_transfer t_array::f_shift()
{
	if (v_size <= 0) t_throwable::f_throw(L"empty array.");
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	t_transfer p = tuple[v_head].f_transfer();
	++v_head;
	v_head %= tuple.f_size();
	--v_size;
	f_shrink();
	return p;
}

void t_array::f_insert(int a_index, const t_transfer& a_value)
{
	if (a_index < 0) {
		if (a_index < -static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
		a_index = v_size - a_index;
	} else {
		if (a_index > static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	}
	f_grow();
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	size_t n = tuple.f_size();
	t_slot* p = &tuple[0];
	if (a_index < static_cast<int>(v_size / 2)) {
		if (i >= n) {
			*f_move_backward(p + --v_head, p + n - 1) = p->f_transfer();
			*f_move_backward(p, p + --i - n) = a_value;
		} else {
			if (v_head > 0) {
				*f_move_backward(p + --v_head, p + --i) = a_value;
			} else {
				v_head = n - 1;
				p[v_head] = p->f_transfer();
				*f_move_backward(p, p + --i) = a_value;
			}
		}
	} else {
		if (i >= n) {
			*f_move_forward(p + i - n, p + j - n) = a_value;
		} else if (j >= n) {
			*f_move_forward(p, p + j - n) = p[n - 1].f_transfer();
			*f_move_forward(p + i, p + n - 1) = a_value;
		} else {
			*f_move_forward(p + i, p + j) = a_value;
		}
	}
	++v_size;
}

t_transfer t_array::f_remove(int a_index)
{
	if (a_index < 0) {
		if (a_index < -static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
		a_index = v_size - a_index;
	} else {
		if (a_index >= static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	}
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	size_t n = tuple.f_size();
	t_slot* p = &tuple[0];
	t_transfer q = p[i % n].f_transfer();
	if (a_index < static_cast<int>(v_size / 2)) {
		if (i >= n) {
			*f_move_forward(p, p + i - n) = p[n - 1].f_transfer();
			f_move_forward(p + v_head, p + n - 1);
			if (++v_head >= n) v_head = 0;
		} else {
			f_move_forward(p + v_head, p + i);
			++v_head;
		}
	} else {
		if (i >= n) {
			f_move_backward(p + i - n, p + --j - n);
		} else if (j > n) {
			*f_move_backward(p + i, p + n - 1) = p->f_transfer();
			f_move_backward(p, p + --j - n);
		} else {
			f_move_backward(p + i, p + --j);
		}
	}
	--v_size;
	f_shrink();
	return q;
}

std::wstring t_type_of<t_array>::f_string(t_object* a_self)
{
	f_check<t_array>(a_self, L"this");
	const t_array& array = f_as<const t_array&>(a_self);
	t_transfer x;
	{
		portable::t_scoped_lock_for_read lock(a_self->v_lock);
		if (array.f_size() <= 0) return L"[]";
		x = array[0];
	}
	x = x->f_get(f_global()->f_symbol_string())->f_call();
	if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
	std::wstring s = f_as<std::wstring>(x);
	size_t i = 1;
	while (true) {
		{
			portable::t_scoped_lock_for_read lock(a_self->v_lock);
			if (i >= array.f_size()) break;
			x = array[i];
		}
		x = x->f_get(f_global()->f_symbol_string())->f_call();
		if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
		s += L", " + f_as<std::wstring>(x);
		++i;
	}
	return L'[' + s + L']';
}

int t_type_of<t_array>::f_hash(t_object* a_self)
{
	f_check<t_array>(a_self, L"this");
	const t_array& array = f_as<const t_array&>(a_self);
	int n = 0;
	size_t i = 0;
	while (true) {
		t_transfer x;
		{
			portable::t_scoped_lock_for_read lock(a_self->v_lock);
			if (i >= array.f_size()) break;
			x = array[i];
		}
		x = x->f_get(f_global()->f_symbol_hash())->f_call();
		if (!f_is<int>(x)) t_throwable::f_throw(L"argument must be integer.");
		n ^= f_as<int>(x);
		++i;
	}
	return n;
}

t_object* t_type_of<t_array>::f_get_at(t_object* a_self, int a_index)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_read lock(a_self->v_lock);
	return f_as<const t_array&>(a_self)[a_index];
}

t_object* t_type_of<t_array>::f_set_at(t_object* a_self, int a_index, const t_transfer& a_value)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_array&>(a_self)[a_index] = a_value;
}

bool t_type_of<t_array>::f_equals(t_object* a_self, t_object* a_other)
{
	if (a_self == a_other) return true;
	f_check<t_array>(a_self, L"this");
	if (!f_is<t_array>(a_other)) return false;
	const t_array& a0 = f_as<const t_array&>(a_self);
	const t_array& a1 = f_as<const t_array&>(a_other);
	{
		portable::t_scoped_lock_for_read lock0(a_self->v_lock);
		portable::t_scoped_lock_for_read lock1(a_other->v_lock);
		if (a0.f_size() != a1.f_size()) return false;
	}
	size_t i = 0;
	while (true) {
		t_transfer x;
		t_transfer y;
		{
			portable::t_scoped_lock_for_read lock0(a_self->v_lock);
			portable::t_scoped_lock_for_read lock1(a_other->v_lock);
			if (i >= a0.f_size()) break;
			if (i >= a1.f_size()) return false;
			x = a0[i];
			y = a1[i];
		}
		if (!f_as<bool>(x->f_get(f_global()->f_symbol_equals())->f_call(y))) return false;
		++i;
	}
	return true;
}

void t_type_of<t_array>::f_clear(t_object* a_self)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_array&>(a_self).f_clear();
}

size_t t_type_of<t_array>::f_size(t_object* a_self)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_read lock(a_self->v_lock);
	return f_as<t_array&>(a_self).f_size();
}

void t_type_of<t_array>::f_push(t_object* a_self, const t_transfer& a_value)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_array&>(a_self).f_push(a_value);
}

t_transfer t_type_of<t_array>::f_pop(t_object* a_self)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_array&>(a_self).f_pop();
}

void t_type_of<t_array>::f_unshift(t_object* a_self, const t_transfer& a_value)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_array&>(a_self).f_unshift(a_value);
}

t_transfer t_type_of<t_array>::f_shift(t_object* a_self)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_array&>(a_self).f_shift();
}

void t_type_of<t_array>::f_insert(t_object* a_self, int a_index, const t_transfer& a_value)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_array&>(a_self).f_insert(a_index, a_value);
}

t_transfer t_type_of<t_array>::f_remove(t_object* a_self, int a_index)
{
	f_check<t_array>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_array&>(a_self).f_remove(a_index);
}

t_transfer t_type_of<t_array>::f_define()
{
	return t_define<t_array, t_object>(f_global(), L"Array")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(t_object*), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(t_object*), f_hash>())
		(f_global()->f_symbol_get_at(), t_member<t_object* (*)(t_object*, int), f_get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_object* (*)(t_object*, int, const t_transfer&), f_set_at>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(t_object*, t_object*), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(t_object*, t_object*), f_not_equals>())
		(L"clear", t_member<void (*)(t_object*), f_clear>())
		(L"size", t_member<size_t (*)(t_object*), f_size>())
		(L"push", t_member<void (*)(t_object*, const t_transfer&), f_push>())
		(L"pop", t_member<t_transfer (*)(t_object*), f_pop>())
		(L"unshift", t_member<void (*)(t_object*, const t_transfer&), f_unshift>())
		(L"shift", t_member<t_transfer (*)(t_object*), f_shift>())
		(L"insert", t_member<void (*)(t_object*, int, const t_transfer&), f_insert>())
		(L"remove", t_member<t_transfer (*)(t_object*, int), f_remove>())
	;
}

t_type* t_type_of<t_array>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_array>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_array* p = f_as<t_array*>(a_this);
	a_scan(p->v_tuple);
}

void t_type_of<t_array>::f_finalize(t_object* a_this)
{
	delete f_as<t_array*>(a_this);
}

void t_type_of<t_array>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	while (a_n > 0) {
		array.f_unshift(a_stack.f_pop());
		--a_n;
	}
	a_stack.f_return(p);
}

void t_type_of<t_array>::f_hash(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(f_hash(a_this)));
	context.f_done();
}

void t_type_of<t_array>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"index");
	a_stack.f_return(f_get_at(a_this, f_as<int>(a0)));
	context.f_done();
}

void t_type_of<t_array>::f_set_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a1 = a_stack.f_pop();
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"index");
	a_stack.f_return(f_set_at(a_this, f_as<int>(a0), a1));
	context.f_done();
}

void t_type_of<t_array>::f_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_equals(a_this, a0)));
	context.f_done();
}

void t_type_of<t_array>::f_not_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_not_equals(a_this, a0)));
	context.f_done();
}

}
