#include <xemmai/array.h>

#include <algorithm>
#include <xemmai/convert.h>

namespace xemmai
{

void t_array::f_resize()
{
	t_scoped p = t_tuple::f_instantiate(v_size * 2);
	auto& tuple = f_as<t_tuple&>(p);
	for (size_t i = 0; i < v_size; ++i) tuple[i].f_construct((*v_tuple)[v_head + i & v_mask]);
	v_tuple = &tuple;
	v_head = 0;
	v_mask = tuple.f_size() - 1;
	v_slot = std::move(p);
}

void t_array::f_grow()
{
	if (v_size <= 0) {
		t_scoped p = t_tuple::f_instantiate(4);
		v_tuple = &f_as<t_tuple&>(p);
		v_mask = 3;
		v_slot.f_construct(std::move(p));
	} else if (v_size >= v_tuple->f_size()) {
		f_resize();
	}
}

void t_array::f_shrink()
{
	if (v_size * 4 > v_tuple->f_size()) return;
	if (v_size > 1) {
		f_resize();
	} else if (v_size <= 0) {
		v_tuple = nullptr;
		v_head = 0;
		v_slot = nullptr;
	}
}

t_scoped t_array::f_instantiate()
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_array>());
	object.f_pointer__(new t_array());
	return object;
}

void t_array::f_insert(intptr_t a_index, t_scoped&& a_value)
{
	f_validate(a_index);
	f_grow();
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	size_t n = v_tuple->f_size();
	t_slot* p = &(*v_tuple)[0];
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
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	size_t n = v_tuple->f_size();
	t_slot* p = &(*v_tuple)[0];
	t_scoped q = std::move(p[i & v_mask]);
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

void t_type_of<t_array>::f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n)
{
	if (a_stack[1].f_type() != f_global()->f_type<t_class>()) f_throw(a_stack, a_n, L"must be class.");
	t_scoped p = t_object::f_allocate(&f_as<t_type&>(a_stack[1]));
	a_stack[1].f_destruct();
	auto array = new t_array();
	p.f_pointer__(array);
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) array->f_push(std::move(a_stack[i]));
	a_stack[0].f_construct(std::move(p));
}

std::wstring t_type_of<t_array>::f_string(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	auto& array = f_as<const t_array&>(a_self);
	t_scoped x;
	if (!f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		if (array.f_size() <= 0) return false;
		x = array[0];
		return true;
	})) return L"[]";
	x = x.f_invoke(f_global()->f_symbol_string());
	f_check<const std::wstring&>(x, L"value");
	std::wstring s = f_as<const std::wstring&>(x);
	size_t i = 1;
	while (f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		if (i >= array.f_size()) return false;
		x = array[i];
		return true;
	})) {
		x = x.f_invoke(f_global()->f_symbol_string());
		f_check<const std::wstring&>(x, L"value");
		s += L", " + f_as<const std::wstring&>(x);
		++i;
	}
	return L'[' + s + L']';
}

void t_type_of<t_array>::f_clear(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_clear();
	});
}

size_t t_type_of<t_array>::f_size(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return f_as<const t_array&>(a_self).f_size();
	});
}

t_scoped t_type_of<t_array>::f__get_at(const t_value& a_self, intptr_t a_index)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return t_scoped(f_as<const t_array&>(a_self)[a_index]);
	});
}

t_scoped t_type_of<t_array>::f__set_at(const t_value& a_self, intptr_t a_index, t_scoped&& a_value)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return t_scoped(f_as<t_array&>(a_self)[a_index] = std::move(a_value));
	});
}

void t_type_of<t_array>::f_push(const t_value& a_self, t_scoped&& a_value)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_push(std::move(a_value));
	});
}

t_scoped t_type_of<t_array>::f_pop(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_array&>(a_self).f_pop();
	});
}

void t_type_of<t_array>::f_unshift(const t_value& a_self, t_scoped&& a_value)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_unshift(std::move(a_value));
	});
}

t_scoped t_type_of<t_array>::f_shift(const t_value& a_self)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_array&>(a_self).f_shift();
	});
}

void t_type_of<t_array>::f_insert(const t_value& a_self, intptr_t a_index, t_scoped&& a_value)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_insert(a_index, std::move(a_value));
	});
}

t_scoped t_type_of<t_array>::f_remove(const t_value& a_self, intptr_t a_index)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_array&>(a_self).f_remove(a_index);
	});
}

void t_type_of<t_array>::f_each(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_array>(a_self, L"this");
	auto& a0 = f_as<const t_array&>(a_self);
	size_t i = 0;
	while (true) {
		t_scoped x;
		if (!f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
		{
			if (i >= a0.f_size()) return false;
			x = a0[i];
			return true;
		})) break;
		a_callable(std::move(x));
		++i;
	}
}

void t_type_of<t_array>::f_sort(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_array>(a_self, L"this");
	auto& a0 = f_as<t_array&>(a_self);
	t_tuple* tuple = nullptr;
	size_t head = 0;
	size_t size = 0;
	size_t mask = 0;
	t_scoped p;
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		std::swap(a0.v_tuple, tuple);
		std::swap(a0.v_head, head);
		std::swap(a0.v_size, size);
		std::swap(a0.v_mask, mask);
		p = std::move(a0.v_slot);
	});
	if (!p) return;
	std::vector<t_scoped> a(size);
	for (size_t i = 0; i < size; ++i) a[i] = std::move((*tuple)[head + i & mask]);
	std::sort(a.begin(), a.end(), [&](const t_scoped& x, const t_scoped& y)
	{
		return f_as<bool>(a_callable(x, y));
	});
	for (size_t i = 0; i < size; ++i) (*tuple)[i].f_construct(std::move(a[i]));
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		a0.v_tuple = tuple;
		a0.v_head = 0;
		a0.v_size = size;
		a0.v_mask = mask;
		a0.v_slot.f_construct(std::move(p));
	});
}

void t_type_of<t_array>::f_define()
{
	t_define<t_array, t_object>(f_global(), L"Array")
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(const t_value&), f_string>())
		(L"clear", t_member<void(*)(const t_value&), f_clear>())
		(f_global()->f_symbol_size(), t_member<size_t(*)(const t_value&), f_size>())
		(f_global()->f_symbol_get_at(), t_member<t_scoped(*)(const t_value&, intptr_t), f__get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_scoped(*)(const t_value&, intptr_t, t_scoped&&), f__set_at>())
		(L"push", t_member<void(*)(const t_value&, t_scoped&&), f_push>())
		(L"pop", t_member<t_scoped(*)(const t_value&), f_pop>())
		(L"unshift", t_member<void(*)(const t_value&, t_scoped&&), f_unshift>())
		(L"shift", t_member<t_scoped(*)(const t_value&), f_shift>())
		(L"insert", t_member<void(*)(const t_value&, intptr_t, t_scoped&&), f_insert>())
		(L"remove", t_member<t_scoped(*)(const t_value&, intptr_t), f_remove>())
		(L"each", t_member<void(*)(const t_value&, const t_value&), f_each>())
		(L"sort", t_member<void(*)(const t_value&, const t_value&), f_sort>())
	;
}

void t_type_of<t_array>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_array&>(a_this).v_slot);
}

t_scoped t_type_of<t_array>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	t_scoped p = t_object::f_allocate(this);
	auto array = new t_array();
	p.f_pointer__(array);
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) array->f_push(t_scoped(a_stack[i]));
	return p;
}

size_t t_type_of<t_array>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<intptr_t>(a0.v_p, L"index");
	f_owned_or_shared<t_with_lock_for_read>(a_this, [&]
	{
		a_stack[0].f_construct(f_as<const t_array&>(a_this)[f_as<intptr_t>(a0.v_p)]);
	});
	return -1;
}

size_t t_type_of<t_array>::f_do_set_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	t_scoped a1 = std::move(a_stack[3]);
	f_check<intptr_t>(a0.v_p, L"index");
	f_owned_or_shared<t_with_lock_for_write>(a_this, [&]
	{
		a_stack[0].f_construct(f_as<t_array&>(a_this)[f_as<intptr_t>(a0.v_p)] = std::move(a1));
	});
	return -1;
}

}
