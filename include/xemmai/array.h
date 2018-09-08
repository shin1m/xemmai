#ifndef XEMMAI__ARRAY_H
#define XEMMAI__ARRAY_H

#include "tuple.h"

namespace xemmai
{

class t_array
{
	friend struct t_finalizes<t_bears<t_array>>;
	friend struct t_type_of<t_array>;

	static t_slot* f_move_forward(t_slot* a_p, t_slot* a_q)
	{
		while (a_p < a_q) {
			t_slot& s = *a_q;
			s = std::move(*--a_q);
		}
		return a_q;
	}
	static t_slot* f_move_backward(t_slot* a_p, t_slot* a_q)
	{
		while (a_p < a_q) {
			t_slot& s = *a_p;
			s = std::move(*++a_p);
		}
		return a_p;
	}

	t_tuple* v_tuple;
	size_t v_head = 0;
	size_t v_size = 0;
	size_t v_mask = 0;
	t_slot v_slot;

	t_array() = default;
	~t_array() = default;
	void f_resize();
	void f_grow();
	void f_shrink();
	void f_validate(intptr_t& a_index) const
	{
		if (a_index < 0) {
			a_index += v_size;
			if (a_index < 0) f_throw(L"out of range."sv);
		} else {
			if (a_index >= static_cast<intptr_t>(v_size)) f_throw(L"out of range."sv);
		}
	}

public:
	static XEMMAI__PORTABLE__EXPORT t_scoped f_instantiate();

	void f_clear()
	{
		v_tuple = nullptr;
		v_head = v_size = v_mask = 0;
		v_slot = nullptr;
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_slot& operator[](intptr_t a_index) const
	{
		f_validate(a_index);
		return (*v_tuple)[v_head + a_index & v_mask];
	}
	t_slot& operator[](intptr_t a_index)
	{
		f_validate(a_index);
		return (*v_tuple)[v_head + a_index & v_mask];
	}
	void f_push(t_scoped&& a_value)
	{
		f_grow();
		(*v_tuple)[v_head + v_size & v_mask].f_construct(std::move(a_value));
		++v_size;
	}
	t_scoped f_pop()
	{
		if (v_size <= 0) f_throw(L"empty array."sv);
		t_scoped p = std::move((*v_tuple)[v_head + --v_size & v_mask]);
		f_shrink();
		return p;
	}
	void f_unshift(t_scoped&& a_value)
	{
		f_grow();
		v_head = v_head - 1 & v_mask;
		(*v_tuple)[v_head].f_construct(std::move(a_value));
		++v_size;
	}
	t_scoped f_shift()
	{
		if (v_size <= 0) f_throw(L"empty array."sv);
		t_scoped p = std::move((*v_tuple)[v_head]);
		v_head = v_head + 1 & v_mask;
		--v_size;
		f_shrink();
		return p;
	}
	void f_insert(intptr_t a_index, t_scoped&& a_value);
	t_scoped f_remove(intptr_t a_index);
};

template<>
struct t_type_of<t_array> : t_derivable<t_holds<t_array>>
{
	static void f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n);
	static t_scoped f_string(const t_value& a_self);
	static void f_clear(const t_value& a_self);
	static size_t f_size(const t_value& a_self);
	static t_scoped f__get_at(const t_value& a_self, intptr_t a_index);
	static t_scoped f__set_at(const t_value& a_self, intptr_t a_index, t_scoped&& a_value);
	static void f_push(const t_value& a_self, t_scoped&& a_value);
	static t_scoped f_pop(const t_value& a_self);
	static void f_unshift(const t_value& a_self, t_scoped&& a_value);
	static t_scoped f_shift(const t_value& a_self);
	static void f_insert(const t_value& a_self, intptr_t a_index, t_scoped&& a_value);
	static t_scoped f_remove(const t_value& a_self, intptr_t a_index);
	static void f_each(const t_value& a_self, const t_value& a_callable);
	static void f_sort(const t_value& a_self, const t_value& a_callable);
	static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_stacked* a_stack);
};

}

#endif
