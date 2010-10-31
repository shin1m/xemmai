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

void t_array::f_validate(int& a_index) const
{
	if (a_index < 0) {
		a_index += v_size;
		if (a_index < 0) t_throwable::f_throw(L"out of range.");
	} else {
		if (a_index >= static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	}
}

t_transfer t_array::f_instantiate()
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_array>());
	object.f_pointer__(new t_array());
	return object;
}

void t_array::f_swap(t_transfer& a_tuple, size_t& a_head, size_t& a_size)
{
	t_transfer tuple = v_tuple.f_transfer();
	v_tuple = a_tuple;
	a_tuple = tuple;
	std::swap(v_head, a_head);
	std::swap(v_size, a_size);
}

const t_slot& t_array::operator[](int a_index) const
{
	f_validate(a_index);
	const t_tuple& tuple = f_as<const t_tuple&>(v_tuple);
	return tuple[(v_head + a_index) % tuple.f_size()];
}

t_slot& t_array::operator[](int a_index)
{
	f_validate(a_index);
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
		a_index += v_size;
		if (a_index < 0) t_throwable::f_throw(L"out of range.");
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
	f_validate(a_index);
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

std::wstring t_type_of<t_array>::f_string(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	const t_array& array = f_as<const t_array&>(a_self);
	t_transfer x;
	{
		t_with_lock_for_read lock(a_self);
		if (array.f_size() <= 0) return L"[]";
		x = array[0];
	}
	x = x.f_get(f_global()->f_symbol_string())();
	if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
	std::wstring s = f_as<std::wstring>(x);
	size_t i = 1;
	while (true) {
		{
			t_with_lock_for_read lock(a_self);
			if (i >= array.f_size()) break;
			x = array[i];
		}
		x = x.f_get(f_global()->f_symbol_string())();
		if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
		s += L", " + f_as<std::wstring>(x);
		++i;
	}
	return L'[' + s + L']';
}

int t_type_of<t_array>::f_hash(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	const t_array& array = f_as<const t_array&>(a_self);
	int n = 0;
	size_t i = 0;
	while (true) {
		t_transfer x;
		{
			t_with_lock_for_read lock(a_self);
			if (i >= array.f_size()) break;
			x = array[i];
		}
		x = x.f_hash();
		if (!f_is<int>(x)) t_throwable::f_throw(L"argument must be integer.");
		n ^= f_as<int>(x);
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
		t_transfer x;
		t_transfer y;
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
		t_transfer x;
		t_transfer y;
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
		t_transfer x;
		t_transfer y;
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
		t_transfer x;
		t_transfer y;
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
		t_transfer x;
		t_transfer y;
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
		t_transfer x;
		{
			t_with_lock_for_read lock0(a_self);
			if (i >= a0.f_size()) break;
			x = a0[i];
		}
		a_callable(x);
		++i;
	}
}

namespace
{

struct t_less
{
	const t_value& v_callable;

	t_less(const t_value& a_callable) : v_callable(a_callable)
	{
	}
	bool operator()(const t_transfer& a_x, const t_transfer& a_y) const
	{
		return f_as<bool>(v_callable(a_x, a_y));
	}
};

}

void t_type_of<t_array>::f_sort(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_array>(a_self, L"this");
	t_array& a0 = f_as<t_array&>(a_self);
	t_transfer tuple;
	size_t head = 0;
	size_t size = 0;
	{
		t_with_lock_for_read lock0(a_self);
		a0.f_swap(tuple, head, size);
	}
	if (!tuple) return;
	t_tuple& t = f_as<t_tuple&>(tuple);
	std::vector<t_scoped> a(size);
	for (size_t i = 0; i < size; ++i) a[i] = t[(head + i) % t.f_size()].f_transfer();
	head = 0;
	std::sort(a.begin(), a.end(), t_less(a_callable));
	for (size_t i = 0; i < size; ++i) t[i] = a[i].f_transfer();
	{
		t_with_lock_for_read lock0(a_self);
		a0.f_swap(tuple, head, size);
	}
}

void t_type_of<t_array>::f_define()
{
	t_define<t_array, t_object>(f_global(), L"Array")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(const t_value&), f_hash>())
		(f_global()->f_symbol_get_at(), t_member<const t_value& (t_array::*)(int) const, &t_array::f_get_at, t_with_lock_for_read>())
		(f_global()->f_symbol_set_at(), t_member<const t_value& (t_array::*)(int, const t_transfer&), &t_array::f_set_at, t_with_lock_for_write>())
		(f_global()->f_symbol_less(), t_member<bool (*)(const t_value&, const t_value&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(const t_value&, const t_value&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(const t_value&, const t_value&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(const t_value&, const t_value&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
		(L"clear", t_member<void (t_array::*)(), &t_array::f_clear, t_with_lock_for_write>())
		(f_global()->f_symbol_size(), t_member<size_t (t_array::*)() const, &t_array::f_size>())
		(f_global()->f_symbol_push(), t_member<void (t_array::*)(const t_transfer&), &t_array::f_push, t_with_lock_for_write>())
		(L"pop", t_member<t_transfer (t_array::*)(), &t_array::f_pop, t_with_lock_for_write>())
		(L"unshift", t_member<void (t_array::*)(const t_transfer&), &t_array::f_unshift, t_with_lock_for_write>())
		(L"shift", t_member<t_transfer (t_array::*)(), &t_array::f_shift, t_with_lock_for_write>())
		(L"insert", t_member<void (t_array::*)(int, const t_transfer&), &t_array::f_insert, t_with_lock_for_write>())
		(L"remove", t_member<t_transfer (t_array::*)(int), &t_array::f_remove, t_with_lock_for_write>())
		(L"each", t_member<void (*)(const t_value&, const t_value&), f_each>())
		(L"sort", t_member<void (*)(const t_value&, const t_value&), f_sort>())
	;
}

t_type* t_type_of<t_array>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_array>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_array& p = f_as<t_array&>(a_this);
	a_scan(p.v_tuple);
}

void t_type_of<t_array>::f_finalize(t_object* a_this)
{
	delete &f_as<t_array&>(a_this);
}

void t_type_of<t_array>::f_construct(t_object* a_class, size_t a_n)
{
	t_stack* stack = f_stack();
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	while (a_n > 0) {
		array.f_unshift(stack->f_pop());
		--a_n;
	}
	stack->f_return(p);
}

void t_type_of<t_array>::f_hash(t_object* a_this)
{
	t_native_context context;
	f_stack()->f_return(f_hash(t_value(a_this)));
	context.f_done();
}

void t_type_of<t_array>::f_get_at(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	f_check<int>(a0, L"index");
	portable::t_scoped_lock_for_read lock(a_this->v_lock);
	stack->f_return(f_as<const t_array&>(a_this).f_get_at(f_as<int>(a0)));
	context.f_done();
}

void t_type_of<t_array>::f_set_at(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a1 = stack->f_pop();
	t_transfer a0 = stack->f_pop();
	f_check<int>(a0, L"index");
	portable::t_scoped_lock_for_write lock(a_this->v_lock);
	stack->f_return(f_as<t_array&>(a_this).f_set_at(f_as<int>(a0), a1));
	context.f_done();
}

void t_type_of<t_array>::f_less(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	stack->f_return(f_less(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_less_equal(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	stack->f_return(f_less_equal(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_greater(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	stack->f_return(f_greater(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_greater_equal(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	stack->f_return(f_greater_equal(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_equals(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	stack->f_return(f_equals(a_this, a0));
	context.f_done();
}

void t_type_of<t_array>::f_not_equals(t_object* a_this)
{
	t_native_context context;
	t_stack* stack = f_stack();
	t_transfer a0 = stack->f_pop();
	stack->f_return(f_not_equals(a_this, a0));
	context.f_done();
}

}
