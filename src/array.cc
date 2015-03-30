#include <xemmai/array.h>

#include <algorithm>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_slot* t_array::f_move_forward(t_slot* a_p, t_slot* a_q)
{
	while (a_p < a_q) {
		t_slot& s = *a_q;
		s = std::move(*--a_q);
	}
	return a_q;
}

t_slot* t_array::f_move_backward(t_slot* a_p, t_slot* a_q)
{
	while (a_p < a_q) {
		t_slot& s = *a_p;
		s = std::move(*++a_p);
	}
	return a_p;
}

void t_array::f_resize()
{
	t_tuple& tuple0 = f_as<t_tuple&>(v_tuple);
	t_scoped p = t_tuple::f_instantiate(v_size * 2);
	t_tuple& tuple1 = f_as<t_tuple&>(p);
	for (size_t i = 0; i < v_size; ++i) tuple1[i] = tuple0[(v_head + i) % tuple0.f_size()];
	v_tuple = std::move(p);
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
		v_tuple = nullptr;
		v_head = 0;
	}
}

void t_array::f_validate(intptr_t& a_index) const
{
	if (a_index < 0) {
		a_index += v_size;
		if (a_index < 0) t_throwable::f_throw(L"out of range.");
	} else {
		if (a_index >= static_cast<intptr_t>(v_size)) t_throwable::f_throw(L"out of range.");
	}
}

t_scoped t_array::f_instantiate()
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_array>());
	object.f_pointer__(new t_array());
	return object;
}

void t_array::f_swap(t_scoped& a_tuple, size_t& a_head, size_t& a_size)
{
	t_scoped tuple = std::move(v_tuple);
	v_tuple = std::move(a_tuple);
	a_tuple = std::move(tuple);
	std::swap(v_head, a_head);
	std::swap(v_size, a_size);
}

const t_slot& t_array::operator[](intptr_t a_index) const
{
	f_validate(a_index);
	const t_tuple& tuple = f_as<const t_tuple&>(v_tuple);
	return tuple[(v_head + a_index) % tuple.f_size()];
}

t_slot& t_array::operator[](intptr_t a_index)
{
	f_validate(a_index);
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	return tuple[(v_head + a_index) % tuple.f_size()];
}

void t_array::f_push(t_scoped&& a_value)
{
	f_grow();
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	tuple[(v_head + v_size) % tuple.f_size()] = std::move(a_value);
	++v_size;
}

t_scoped t_array::f_pop()
{
	if (v_size <= 0) t_throwable::f_throw(L"empty array.");
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	t_scoped p = std::move(tuple[(v_head + --v_size) % tuple.f_size()]);
	f_shrink();
	return p;
}

void t_array::f_unshift(t_scoped&& a_value)
{
	f_grow();
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	v_head += tuple.f_size() - 1;
	v_head %= tuple.f_size();
	tuple[v_head] = std::move(a_value);
	++v_size;
}

t_scoped t_array::f_shift()
{
	if (v_size <= 0) t_throwable::f_throw(L"empty array.");
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	t_scoped p = std::move(tuple[v_head]);
	++v_head;
	v_head %= tuple.f_size();
	--v_size;
	f_shrink();
	return p;
}

void t_array::f_insert(intptr_t a_index, t_scoped&& a_value)
{
	if (a_index < 0) {
		a_index += v_size;
		if (a_index < 0) t_throwable::f_throw(L"out of range.");
	} else {
		if (a_index > static_cast<intptr_t>(v_size)) t_throwable::f_throw(L"out of range.");
	}
	f_grow();
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	size_t n = tuple.f_size();
	t_slot* p = &tuple[0];
	if (a_index < static_cast<intptr_t>(v_size / 2)) {
		if (i >= n) {
			*f_move_backward(p + --v_head, p + n - 1) = std::move(*p);
			*f_move_backward(p, p + --i - n) = std::move(a_value);
		} else {
			if (v_head > 0) {
				*f_move_backward(p + --v_head, p + --i) = std::move(a_value);
			} else {
				v_head = n - 1;
				p[v_head] = std::move(*p);
				*f_move_backward(p, p + --i) = std::move(a_value);
			}
		}
	} else {
		if (i >= n) {
			*f_move_forward(p + i - n, p + j - n) = std::move(a_value);
		} else if (j >= n) {
			*f_move_forward(p, p + j - n) = std::move(p[n - 1]);
			*f_move_forward(p + i, p + n - 1) = std::move(a_value);
		} else {
			*f_move_forward(p + i, p + j) = std::move(a_value);
		}
	}
	++v_size;
}

t_scoped t_array::f_remove(intptr_t a_index)
{
	f_validate(a_index);
	t_tuple& tuple = f_as<t_tuple&>(v_tuple);
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	size_t n = tuple.f_size();
	t_slot* p = &tuple[0];
	t_scoped q = std::move(p[i % n]);
	if (a_index < static_cast<intptr_t>(v_size / 2)) {
		if (i >= n) {
			*f_move_forward(p, p + i - n) = std::move(p[n - 1]);
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
			*f_move_backward(p + i, p + n - 1) = std::move(*p);
			f_move_backward(p, p + --j - n);
		} else {
			f_move_backward(p + i, p + --j);
		}
	}
	--v_size;
	f_shrink();
	return q;
}

void t_type_of<t_array>::f__construct(t_object* a_module, t_scoped* a_stack, size_t a_n)
{
	t_scoped self = std::move(a_stack[0]);
	if (self.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	t_scoped p = t_object::f_allocate(std::move(self));
	t_array* array = new t_array();
	p.f_pointer__(array);
	for (size_t i = 1; i <= a_n; ++i) array->f_push(std::move(a_stack[i]));
	a_stack[0].f_construct(std::move(p));
}

std::wstring t_type_of<t_array>::f_string(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	const t_array& array = f_as<const t_array&>(a_self);
	t_scoped x;
	{
		t_with_lock_for_read lock(a_self);
		if (array.f_size() <= 0) return L"[]";
		x = array[0];
	}
	x = x.f_get(f_global()->f_symbol_string())();
	f_check<const std::wstring&>(x, L"value");
	std::wstring s = f_as<const std::wstring&>(x);
	size_t i = 1;
	while (true) {
		{
			t_with_lock_for_read lock(a_self);
			if (i >= array.f_size()) break;
			x = array[i];
		}
		x = x.f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(x, L"value");
		s += L", " + f_as<const std::wstring&>(x);
		++i;
	}
	return L'[' + s + L']';
}

intptr_t t_type_of<t_array>::f_hash(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	const t_array& array = f_as<const t_array&>(a_self);
	intptr_t n = 0;
	size_t i = 0;
	while (true) {
		t_scoped x;
		{
			t_with_lock_for_read lock(a_self);
			if (i >= array.f_size()) break;
			x = array[i];
		}
		x = x.f_hash();
		f_check<intptr_t>(x, L"value");
		n ^= f_as<intptr_t>(x);
		++i;
	}
	return n;
}

bool t_type_of<t_array>::f_less(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return false;
	f_check<t_array>(a_self, L"this");
	f_check<t_array>(a_other, L"other");
	const t_array& a0 = f_as<const t_array&>(a_self);
	const t_array& a1 = f_as<const t_array&>(a_other);
	size_t i = 0;
	while (true) {
		t_scoped x;
		t_scoped y;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		{
			t_with_lock_for_read lock1(a_other);
			if (i >= a1.f_size()) return false;
			y = a1[i];
		}
		if (!f_as<bool>(x.f_equals(t_value(y)))) return f_as<bool>(x.f_less(y));
		++i;
	}
	t_with_lock_for_read lock1(a_other);
	return i < a1.f_size();
}

bool t_type_of<t_array>::f_less_equal(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_array>(a_self, L"this");
	f_check<t_array>(a_other, L"other");
	const t_array& a0 = f_as<const t_array&>(a_self);
	const t_array& a1 = f_as<const t_array&>(a_other);
	size_t i = 0;
	while (true) {
		t_scoped x;
		t_scoped y;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		{
			t_with_lock_for_read lock1(a_other);
			if (i >= a1.f_size()) return false;
			y = a1[i];
		}
		if (!f_as<bool>(x.f_equals(t_value(y)))) return f_as<bool>(x.f_less(y));
		++i;
	}
	return true;
}

bool t_type_of<t_array>::f_greater(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return false;
	f_check<t_array>(a_self, L"this");
	f_check<t_array>(a_other, L"other");
	const t_array& a0 = f_as<const t_array&>(a_self);
	const t_array& a1 = f_as<const t_array&>(a_other);
	size_t i = 0;
	while (true) {
		t_scoped x;
		t_scoped y;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		{
			t_with_lock_for_read lock1(a_other);
			if (i >= a1.f_size()) return true;
			y = a1[i];
		}
		if (!f_as<bool>(x.f_equals(t_value(y)))) return f_as<bool>(x.f_greater(y));
		++i;
	}
	return false;
}

bool t_type_of<t_array>::f_greater_equal(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_array>(a_self, L"this");
	f_check<t_array>(a_other, L"other");
	const t_array& a0 = f_as<const t_array&>(a_self);
	const t_array& a1 = f_as<const t_array&>(a_other);
	size_t i = 0;
	while (true) {
		t_scoped x;
		t_scoped y;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		{
			t_with_lock_for_read lock1(a_other);
			if (i >= a1.f_size()) return true;
			y = a1[i];
		}
		if (!f_as<bool>(x.f_equals(t_value(y)))) return f_as<bool>(x.f_greater(y));
		++i;
	}
	t_with_lock_for_read lock1(a_other);
	return i >= a1.f_size();
}

bool t_type_of<t_array>::f_equals(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_array>(a_self, L"this");
	if (!f_is<t_array>(a_other)) return false;
	const t_array& a0 = f_as<const t_array&>(a_self);
	const t_array& a1 = f_as<const t_array&>(a_other);
	if (a0.f_size() != a1.f_size()) return false;
	size_t i = 0;
	while (true) {
		t_scoped x;
		t_scoped y;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		{
			t_with_lock_for_read lock1(a_other);
			if (i >= a1.f_size()) return false;
			y = a1[i];
		}
		if (!f_as<bool>(x.f_equals(y))) return false;
		++i;
	}
	return true;
}

void t_type_of<t_array>::f_each(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_array>(a_self, L"this");
	const t_array& a0 = f_as<const t_array&>(a_self);
	size_t i = 0;
	while (true) {
		t_scoped x;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		a_callable(std::move(x));
		++i;
	}
}

void t_type_of<t_array>::f_sort(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_array>(a_self, L"this");
	t_array& a0 = f_as<t_array&>(a_self);
	t_scoped tuple;
	size_t head = 0;
	size_t size = 0;
	{
		t_with_lock_for_read lock0(a_self);
		a0.f_swap(tuple, head, size);
	}
	if (!tuple) return;
	t_tuple& t = f_as<t_tuple&>(tuple);
	std::vector<t_scoped> a(size);
	for (size_t i = 0; i < size; ++i) a[i] = std::move(t[(head + i) % t.f_size()]);
	head = 0;
	std::sort(a.begin(), a.end(), [&a_callable](const t_scoped& a_x, const t_scoped& a_y)
	{
		return f_as<bool>(a_callable(a_x, a_y));
	});
	for (size_t i = 0; i < size; ++i) t[i] = std::move(a[i]);
	{
		t_with_lock_for_read lock0(a_self);
		a0.f_swap(tuple, head, size);
	}
}

void t_type_of<t_array>::f_define()
{
	t_define<t_array, t_object>(f_global(), L"Array")
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t (*)(const t_value&), f_hash>())
		(f_global()->f_symbol_get_at(), t_member<const t_value& (t_array::*)(intptr_t) const, &t_array::f_get_at, t_with_lock_for_read>())
		(f_global()->f_symbol_set_at(), t_member<const t_value& (t_array::*)(intptr_t, t_scoped&&), &t_array::f_set_at, t_with_lock_for_write>())
		(f_global()->f_symbol_less(), t_member<bool (*)(const t_value&, const t_value&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(const t_value&, const t_value&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(const t_value&, const t_value&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(const t_value&, const t_value&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
		(L"clear", t_member<void (t_array::*)(), &t_array::f_clear, t_with_lock_for_write>())
		(f_global()->f_symbol_size(), t_member<size_t (t_array::*)() const, &t_array::f_size>())
		(f_global()->f_symbol_push(), t_member<void (t_array::*)(t_scoped&&), &t_array::f_push, t_with_lock_for_write>())
		(L"pop", t_member<t_scoped (t_array::*)(), &t_array::f_pop, t_with_lock_for_write>())
		(L"unshift", t_member<void (t_array::*)(t_scoped&&), &t_array::f_unshift, t_with_lock_for_write>())
		(L"shift", t_member<t_scoped (t_array::*)(), &t_array::f_shift, t_with_lock_for_write>())
		(L"insert", t_member<void (t_array::*)(intptr_t, t_scoped&&), &t_array::f_insert, t_with_lock_for_write>())
		(L"remove", t_member<t_scoped (t_array::*)(intptr_t), &t_array::f_remove, t_with_lock_for_write>())
		(L"each", t_member<void (*)(const t_value&, const t_value&), f_each>())
		(L"sort", t_member<void (*)(const t_value&, const t_value&), f_sort>())
	;
}

t_type* t_type_of<t_array>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

void t_type_of<t_array>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_array&>(a_this).v_tuple);
}

void t_type_of<t_array>::f_finalize(t_object* a_this)
{
	delete &f_as<t_array&>(a_this);
}

t_scoped t_type_of<t_array>::f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	t_scoped p = t_object::f_allocate(a_class);
	t_array* array = new t_array();
	p.f_pointer__(array);
	for (size_t i = 1; i <= a_n; ++i) array->f_push(std::move(a_stack[i]));
	return p;
}

void t_type_of<t_array>::f_hash(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	a_stack[0].f_construct(f_hash(t_value(a_this)));
	context.f_done();
}

void t_type_of<t_array>::f_get_at(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	f_check<intptr_t>(a0, L"index");
	t_with_lock_for_read lock(a_this);
	a_stack[0].f_construct(f_as<const t_array&>(a_this).f_get_at(f_as<intptr_t>(a0)));
	context.f_done();
}

void t_type_of<t_array>::f_set_at(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	t_scoped a1 = std::move(a_stack[2]);
	f_check<intptr_t>(a0, L"index");
	t_with_lock_for_write lock(a_this);
	a_stack[0].f_construct(f_as<t_array&>(a_this).f_set_at(f_as<intptr_t>(a0), std::move(a1)));
	context.f_done();
}

void t_type_of<t_array>::f_less(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_less(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_less_equal(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_less_equal(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_greater(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_greater(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_greater_equal(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_greater_equal(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_equals(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_equals(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_not_equals(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_not_equals(a_this, a0));
	context.f_done();
}

}
