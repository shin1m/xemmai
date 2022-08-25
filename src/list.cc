#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

inline size_t f_capacity(size_t a_size)
{
	return (a_size - t_object::f_fields_offset(sizeof(t_tuple))) / sizeof(t_slot);
}

}

void t_list::f_resize()
{
	auto size = (t_object::f_fields_offset(sizeof(t_tuple)) + v_size * sizeof(t_slot) + sizeof(t_object) - 1) / sizeof(t_object) * sizeof(t_object);
	v_grow = f_capacity(size * 2);
	v_shrink = size < sizeof(t_object) * 2 ? 0 : f_capacity(size / 2);
	auto& tuple0 = v_tuple->f_as<t_tuple>();
	v_tuple = t_tuple::f_instantiate(v_grow, [&](auto& tuple1)
	{
		size_t n = tuple0.f_size();
		size_t i = 0;
		if (v_head + v_size > n) {
			do new(&tuple1[i++]) t_svalue(tuple0[v_head++]); while (v_head < n);
			v_head = 0;
		}
		do new(&tuple1[i++]) t_svalue(tuple0[v_head++]); while (i < v_size);
		do new(&tuple1[i++]) t_svalue; while (i < v_grow);
	});
	v_head = 0;
}

void t_list::f_grow()
{
	if (v_size <= 0) {
		v_grow = f_capacity(sizeof(t_object));
		v_tuple = t_tuple::f_instantiate(v_grow, [&](auto& tuple)
		{
			std::uninitialized_default_construct_n(&tuple[0], v_grow);
		});
	} else {
		f_resize();
	}
}

void t_list::f_shrink()
{
	if (v_size <= 0) {
		v_tuple = nullptr;
		v_head = v_grow = 0;
	} else {
		f_resize();
	}
}

t_object* t_list::f_instantiate()
{
	return f_new<t_list>(f_global());
}

void t_list::f_insert(intptr_t a_index, const t_pvalue& a_value)
{
	if (a_index == static_cast<intptr_t>(v_size)) return f_push(a_value);
	f_validate(a_index);
	if (v_size >= v_grow) f_grow();
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	auto& tuple = v_tuple->f_as<t_tuple>();
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

t_pvalue t_list::f_remove(intptr_t a_index)
{
	f_validate(a_index);
	size_t i = v_head + a_index;
	size_t j = v_head + v_size;
	auto& tuple = v_tuple->f_as<t_tuple>();
	size_t n = tuple.f_size();
	auto p = &tuple[0];
	t_pvalue q = p[i < n ? i : i - n];
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
	if (--v_size <= v_shrink) f_shrink();
	return q;
}

t_object* t_type_of<t_list>::f__string(t_list& a_self)
{
	std::vector<wchar_t> cs{L'['};
	t_pvalue x;
	if (a_self.f_owned_or_shared<std::shared_lock>([&]
	{
		if (a_self.f_size() <= 0) return false;
		x = a_self[0];
		return true;
	})) for (size_t i = 0;;) {
		x = x.f_string();
		f_check<t_string>(x, L"value");
		auto& s = x->f_as<t_string>();
		auto p = static_cast<const wchar_t*>(s);
		cs.insert(cs.end(), p, p + s.f_size());
		++i;
		if (!a_self.f_owned_or_shared<std::shared_lock>([&]
		{
			if (i >= a_self.f_size()) return false;
			x = a_self[i];
			return true;
		})) break;
		cs.push_back(L',');
		cs.push_back(L' ');
	}
	cs.push_back(L']');
	return t_string::f_instantiate(cs.data(), cs.size());
}

void t_type_of<t_list>::f_clear(t_list& a_self)
{
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		a_self.f_clear();
	});
}

size_t t_type_of<t_list>::f_size(t_list& a_self)
{
	return a_self.f_owned_or_shared<std::shared_lock>([&]
	{
		return a_self.f_size();
	});
}

t_pvalue t_type_of<t_list>::f__get_at(t_list& a_self, intptr_t a_index)
{
	return a_self.f_owned_or_shared<std::shared_lock>([&]
	{
		return t_pvalue(a_self[a_index]);
	});
}

t_pvalue t_type_of<t_list>::f__set_at(t_list& a_self, intptr_t a_index, const t_pvalue& a_value)
{
	return a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		return t_pvalue(a_self[a_index] = a_value);
	});
}

void t_type_of<t_list>::f_push(t_list& a_self, const t_pvalue& a_value)
{
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		a_self.f_push(std::move(a_value));
	});
}

t_pvalue t_type_of<t_list>::f_pop(t_list& a_self)
{
	return a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		return a_self.f_pop();
	});
}

void t_type_of<t_list>::f_unshift(t_list& a_self, const t_pvalue& a_value)
{
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		a_self.f_unshift(a_value);
	});
}

t_pvalue t_type_of<t_list>::f_shift(t_list& a_self)
{
	return a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		return a_self.f_shift();
	});
}

void t_type_of<t_list>::f_insert(t_list& a_self, intptr_t a_index, const t_pvalue& a_value)
{
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		a_self.f_insert(a_index, a_value);
	});
}

t_pvalue t_type_of<t_list>::f_remove(t_list& a_self, intptr_t a_index)
{
	return a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		return a_self.f_remove(a_index);
	});
}

void t_type_of<t_list>::f_each(t_list& a_self, const t_pvalue& a_callable)
{
	size_t i = 0;
	while (true) {
		t_pvalue x;
		if (!a_self.f_owned_or_shared<std::shared_lock>([&]
		{
			if (i >= a_self.f_size()) return false;
			x = a_self[i];
			return true;
		})) break;
		a_callable(x);
		++i;
	}
}

void t_type_of<t_list>::f_sort(t_list& a_self, const t_pvalue& a_callable)
{
	t_object* object;
	size_t head = 0;
	size_t size = 0;
	size_t grow = 0;
	size_t shrink = 0;
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		object = a_self.v_tuple;
		a_self.v_tuple = nullptr;
		std::swap(a_self.v_head, head);
		std::swap(a_self.v_size, size);
		std::swap(a_self.v_grow, grow);
		std::swap(a_self.v_shrink, shrink);
	});
	if (!object) return;
	auto& tuple = object->f_as<t_tuple>();
	auto p = reinterpret_cast<t_rvalue*>(&tuple[0]);
	if (head + size > tuple.f_size()) {
		std::rotate(p, p + head, p + tuple.f_size());
		head = 0;
	} else {
		p += head;
	}
	std::sort(p, p + size, [&](const auto& x, const auto& y)
	{
		return f_as<bool>(a_callable(x, y));
	});
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		a_self.v_tuple = object;
		a_self.v_head = head;
		a_self.v_size = size;
		a_self.v_grow = grow;
		a_self.v_shrink = shrink;
	});
}

void t_type_of<t_list>::f_define()
{
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(*)(t_list&), f__string>())
		(L"clear"sv, t_member<void(*)(t_list&), f_clear>())
		(f_global()->f_symbol_size(), t_member<size_t(*)(t_list&), f_size>())
		(f_global()->f_symbol_get_at(), t_member<t_pvalue(*)(t_list&, intptr_t), f__get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_pvalue(*)(t_list&, intptr_t, const t_pvalue&), f__set_at>())
		(L"push"sv, t_member<void(*)(t_list&, const t_pvalue&), f_push>())
		(L"pop"sv, t_member<t_pvalue(*)(t_list&), f_pop>())
		(L"unshift"sv, t_member<void(*)(t_list&, const t_pvalue&), f_unshift>())
		(L"shift"sv, t_member<t_pvalue(*)(t_list&), f_shift>())
		(L"insert"sv, t_member<void(*)(t_list&, intptr_t, const t_pvalue&), f_insert>())
		(L"remove"sv, t_member<t_pvalue(*)(t_list&, intptr_t), f_remove>())
		(L"each"sv, t_member<void(*)(t_list&, const t_pvalue&), f_each>())
		(L"sort"sv, t_member<void(*)(t_list&, const t_pvalue&), f_sort>())
	.f_derive<t_list, t_sharable>();
}

t_pvalue t_type_of<t_list>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	auto object = f_new<t_list>();
	auto& list = object->f_as<t_list>();
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) list.f_push(a_stack[i]);
	return object;
}

size_t t_type_of<t_list>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<intptr_t>(a_stack[2], L"index");
	auto& list = a_this->f_as<t_list>();
	list.f_owned_or_shared<std::shared_lock>([&]
	{
		a_stack[0] = list[f_as<intptr_t>(a_stack[2])];
	});
	return -1;
}

size_t t_type_of<t_list>::f_do_set_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<intptr_t>(a_stack[2], L"index");
	auto& list = a_this->f_as<t_list>();
	list.f_owned_or_shared<std::lock_guard>([&]
	{
		a_stack[0] = list[f_as<intptr_t>(a_stack[2])] = a_stack[3];
	});
	return -1;
}

}
