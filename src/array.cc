#include <xemmai/array.h>
#include <xemmai/convert.h>

namespace xemmai
{

void t_array::f_resize()
{
	auto object = t_tuple::f_instantiate(v_size * 2);
	auto& tuple0 = f_as<t_tuple&>(v_tuple);
	auto& tuple1 = f_as<t_tuple&>(object);
	for (size_t i = 0; i < v_size; ++i) new(&tuple1[i]) t_svalue(tuple0[v_head + i & v_mask]);
	v_tuple = object;
	v_head = 0;
	v_mask = tuple1.f_size() - 1;
}

void t_array::f_grow()
{
	if (v_size <= 0) {
		v_tuple = t_tuple::f_instantiate(4);
		v_mask = 3;
	} else if (v_size >= f_as<t_tuple&>(v_tuple).f_size()) {
		f_resize();
	}
}

void t_array::f_shrink()
{
	if (v_size * 4 > f_as<t_tuple&>(v_tuple).f_size()) return;
	if (v_size > 1) {
		f_resize();
	} else if (v_size <= 0) {
		v_tuple = nullptr;
		v_head = 0;
	}
}

t_object* t_array::f_instantiate()
{
	return f_new<t_array>(f_global(), false);
}

void t_array::f_insert(intptr_t a_index, const t_pvalue& a_value)
{
	f_validate(a_index);
	f_grow();
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	auto& tuple = f_as<t_tuple&>(v_tuple);
	size_t n = tuple.f_size();
	auto p = &tuple[0];
	if (a_index < static_cast<intptr_t>(v_size / 2)) {
		if (i >= n) {
			*f_move_backward(p + --v_head, p + n - 1) = *p;
			*f_move_backward(p, p + --i - n) = a_value;
		} else {
			if (v_head > 0) {
				*f_move_backward(p + --v_head, p + --i) = a_value;
			} else {
				v_head = n - 1;
				p[v_head] = *p;
				*f_move_backward(p, p + --i) = a_value;
			}
		}
	} else {
		if (i >= n) {
			*f_move_forward(p + i - n, p + j - n) = a_value;
		} else if (j >= n) {
			*f_move_forward(p, p + j - n) = p[n - 1];
			*f_move_forward(p + i, p + n - 1) = a_value;
		} else {
			*f_move_forward(p + i, p + j) = a_value;
		}
	}
	++v_size;
}

t_pvalue t_array::f_remove(intptr_t a_index)
{
	f_validate(a_index);
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	auto& tuple = f_as<t_tuple&>(v_tuple);
	size_t n = tuple.f_size();
	auto p = &tuple[0];
	t_pvalue q = p[i & v_mask];
	if (a_index < static_cast<intptr_t>(v_size / 2)) {
		if (i >= n) {
			*f_move_forward(p, p + i - n) = p[n - 1];
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
			*f_move_backward(p + i, p + n - 1) = *p;
			f_move_backward(p, p + --j - n);
		} else {
			f_move_backward(p + i, p + --j);
		}
	}
	--v_size;
	f_shrink();
	return q;
}

void t_type_of<t_array>::f__construct(xemmai::t_extension* a_extension, t_pvalue* a_stack, size_t a_n)
{
	if (a_stack[1].f_type() != f_global()->f_type<t_class>()) f_throw(L"must be class."sv);
	auto object = f_as<t_type&>(a_stack[1]).f_new<t_array>(false);
	auto& array = f_as<t_array&>(object);
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) array.f_push(a_stack[i]);
	a_stack[0] = object;
}

t_object* t_type_of<t_array>::f_string(const t_pvalue& a_self)
{
	f_check<t_array>(a_self, L"this");
	auto& array = f_as<const t_array&>(a_self);
	std::vector<wchar_t> cs{L'['};
	t_pvalue x;
	if (f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		if (array.f_size() <= 0) return false;
		x = array[0];
		return true;
	})) for (size_t i = 0;;) {
		x = x.f_invoke(f_global()->f_symbol_string());
		f_check<t_string>(x, L"value");
		auto& s = f_as<const t_string&>(x);
		auto p = static_cast<const wchar_t*>(s);
		cs.insert(cs.end(), p, p + s.f_size());
		++i;
		if (!f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
		{
			if (i >= array.f_size()) return false;
			x = array[i];
			return true;
		})) break;
		cs.push_back(L',');
		cs.push_back(L' ');
	}
	cs.push_back(L']');
	return t_string::f_instantiate(cs.data(), cs.size());
}

void t_type_of<t_array>::f_clear(const t_pvalue& a_self)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_clear();
	});
}

size_t t_type_of<t_array>::f_size(const t_pvalue& a_self)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return f_as<const t_array&>(a_self).f_size();
	});
}

t_pvalue t_type_of<t_array>::f__get_at(const t_pvalue& a_self, intptr_t a_index)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return t_pvalue(f_as<const t_array&>(a_self)[a_index]);
	});
}

t_pvalue t_type_of<t_array>::f__set_at(const t_pvalue& a_self, intptr_t a_index, const t_pvalue& a_value)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return t_pvalue(f_as<t_array&>(a_self)[a_index] = a_value);
	});
}

void t_type_of<t_array>::f_push(const t_pvalue& a_self, const t_pvalue& a_value)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_push(std::move(a_value));
	});
}

t_pvalue t_type_of<t_array>::f_pop(const t_pvalue& a_self)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_array&>(a_self).f_pop();
	});
}

void t_type_of<t_array>::f_unshift(const t_pvalue& a_self, const t_pvalue& a_value)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_unshift(a_value);
	});
}

t_pvalue t_type_of<t_array>::f_shift(const t_pvalue& a_self)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_array&>(a_self).f_shift();
	});
}

void t_type_of<t_array>::f_insert(const t_pvalue& a_self, intptr_t a_index, const t_pvalue& a_value)
{
	f_check<t_array>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_array&>(a_self).f_insert(a_index, a_value);
	});
}

t_pvalue t_type_of<t_array>::f_remove(const t_pvalue& a_self, intptr_t a_index)
{
	f_check<t_array>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_array&>(a_self).f_remove(a_index);
	});
}

void t_type_of<t_array>::f_each(const t_pvalue& a_self, const t_pvalue& a_callable)
{
	f_check<t_array>(a_self, L"this");
	auto& a0 = f_as<const t_array&>(a_self);
	size_t i = 0;
	while (true) {
		t_pvalue x;
		if (!f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
		{
			if (i >= a0.f_size()) return false;
			x = a0[i];
			return true;
		})) break;
		a_callable(x);
		++i;
	}
}

void t_type_of<t_array>::f_sort(const t_pvalue& a_self, const t_pvalue& a_callable)
{
	f_check<t_array>(a_self, L"this");
	auto& a0 = f_as<t_array&>(a_self);
	t_object* object;
	size_t head = 0;
	size_t size = 0;
	size_t mask = 0;
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		object = a0.v_tuple;
		a0.v_tuple = nullptr;
		std::swap(a0.v_head, head);
		std::swap(a0.v_size, size);
		std::swap(a0.v_mask, mask);
	});
	if (!object) return;
	std::vector<t_rvalue> a(size);
	auto& tuple = f_as<t_tuple&>(object);
	for (size_t i = 0; i < size; ++i) a[i] = tuple[head + i & mask];
	std::sort(a.begin(), a.end(), [&](const auto& x, const auto& y)
	{
		return f_as<bool>(a_callable(x, y));
	});
	for (size_t i = 0; i < size; ++i) tuple[i] = a[i];
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		a0.v_tuple = object;
		a0.v_head = 0;
		a0.v_size = size;
		a0.v_mask = mask;
	});
}

void t_type_of<t_array>::f_define()
{
	t_define<t_array, t_object>(f_global(), L"Array"sv)
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), f_string>())
		(L"clear"sv, t_member<void(*)(const t_pvalue&), f_clear>())
		(f_global()->f_symbol_size(), t_member<size_t(*)(const t_pvalue&), f_size>())
		(f_global()->f_symbol_get_at(), t_member<t_pvalue(*)(const t_pvalue&, intptr_t), f__get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_pvalue(*)(const t_pvalue&, intptr_t, const t_pvalue&), f__set_at>())
		(L"push"sv, t_member<void(*)(const t_pvalue&, const t_pvalue&), f_push>())
		(L"pop"sv, t_member<t_pvalue(*)(const t_pvalue&), f_pop>())
		(L"unshift"sv, t_member<void(*)(const t_pvalue&, const t_pvalue&), f_unshift>())
		(L"shift"sv, t_member<t_pvalue(*)(const t_pvalue&), f_shift>())
		(L"insert"sv, t_member<void(*)(const t_pvalue&, intptr_t, const t_pvalue&), f_insert>())
		(L"remove"sv, t_member<t_pvalue(*)(const t_pvalue&, intptr_t), f_remove>())
		(L"each"sv, t_member<void(*)(const t_pvalue&, const t_pvalue&), f_each>())
		(L"sort"sv, t_member<void(*)(const t_pvalue&, const t_pvalue&), f_sort>())
	;
}

t_pvalue t_type_of<t_array>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	auto object = f_new<t_array>(false);
	auto& array = f_as<t_array&>(object);
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) array.f_push(a_stack[i]);
	return object;
}

size_t t_type_of<t_array>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<intptr_t>(a_stack[2], L"index");
	f_owned_or_shared<t_with_lock_for_read>(a_this, [&]
	{
		a_stack[0] = f_as<const t_array&>(a_this)[f_as<intptr_t>(a_stack[2])];
	});
	return -1;
}

size_t t_type_of<t_array>::f_do_set_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<intptr_t>(a_stack[2], L"index");
	f_owned_or_shared<t_with_lock_for_write>(a_this, [&]
	{
		a_stack[0] = f_as<t_array&>(a_this)[f_as<intptr_t>(a_stack[2])] = a_stack[3];
	});
	return -1;
}

}
