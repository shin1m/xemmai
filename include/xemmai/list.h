#ifndef XEMMAI__LIST_H
#define XEMMAI__LIST_H

#include "sharable.h"
#include "tuple.h"

namespace xemmai
{

class t_list : public t_sharable
{
	friend struct t_finalizes<t_bears<t_list, t_type_of<t_sharable>>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_list>;

	static t_svalue* f_move_forward(t_svalue* a_p, t_svalue* a_q)
	{
		while (a_p < a_q) {
			auto& s = *a_q;
			s = *--a_q;
		}
		return a_q;
	}
	static t_svalue* f_move_backward(t_svalue* a_p, t_svalue* a_q)
	{
		while (a_p < a_q) {
			auto& s = *a_p;
			s = *++a_p;
		}
		return a_p;
	}

	t_slot v_tuple;
	size_t v_head = 0;
	size_t v_size = 0;
	size_t v_grow = 0;
	size_t v_shrink = 0;

	t_list() = default;
	~t_list() = default;
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
	t_svalue& f_at(size_t a_index) const
	{
		a_index += v_head;
		auto& tuple = v_tuple->f_as<t_tuple>();
		auto n = tuple.f_size();
		return tuple[a_index < n ? a_index : a_index - n];
	}

public:
	static XEMMAI__PORTABLE__EXPORT t_object* f_instantiate();

	void f_clear()
	{
		v_tuple = nullptr;
		v_head = v_size = v_grow = v_shrink = 0;
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_svalue& operator[](intptr_t a_index) const
	{
		f_validate(a_index);
		return f_at(a_index);
	}
	t_svalue& operator[](intptr_t a_index)
	{
		f_validate(a_index);
		return f_at(a_index);
	}
	void f_push(const t_pvalue& a_value)
	{
		if (v_size >= v_grow) f_grow();
		new(&f_at(v_size)) t_svalue(a_value);
		++v_size;
	}
	t_pvalue f_pop()
	{
		if (v_size <= 0) f_throw(L"empty list."sv);
		auto& x = f_at(--v_size);
		t_pvalue p = x;
		x = nullptr;
		if (v_size <= v_shrink) f_shrink();
		return p;
	}
	void f_unshift(const t_pvalue& a_value)
	{
		if (v_size >= v_grow) f_grow();
		auto& tuple = v_tuple->f_as<t_tuple>();
		v_head = (v_head > 0 ? v_head : tuple.f_size()) - 1;
		new(&tuple[v_head]) t_svalue(a_value);
		++v_size;
	}
	t_pvalue f_shift()
	{
		if (v_size <= 0) f_throw(L"empty list."sv);
		auto& tuple = v_tuple->f_as<t_tuple>();
		auto& x = tuple[v_head];
		t_pvalue p = x;
		x = nullptr;
		if (++v_head >= tuple.f_size()) v_head = 0;
		if (--v_size <= v_shrink) f_shrink();
		return p;
	}
	void f_insert(intptr_t a_index, const t_pvalue& a_value);
	t_pvalue f_remove(intptr_t a_index);
};

template<>
struct t_type_of<t_list> : t_derivable<t_holds<t_list, t_type_of<t_sharable>>>
{
	static t_object* f__string(t_list& a_self);
	static void f_clear(t_list& a_self);
	static size_t f_size(t_list& a_self);
	static t_pvalue f__get_at(t_list& a_self, intptr_t a_index);
	static t_pvalue f__set_at(t_list& a_self, intptr_t a_index, const t_pvalue& a_value);
	static void f_push(t_list& a_self, const t_pvalue& a_value);
	static t_pvalue f_pop(t_list& a_self);
	static void f_unshift(t_list& a_self, const t_pvalue& a_value);
	static t_pvalue f_shift(t_list& a_self);
	static void f_insert(t_list& a_self, intptr_t a_index, const t_pvalue& a_value);
	static t_pvalue f_remove(t_list& a_self, intptr_t a_index);
	static void f_each(t_list& a_self, const t_pvalue& a_callable);
	static void f_sort(t_list& a_self, const t_pvalue& a_callable);
	static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_list>().v_tuple);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
};

}

#endif
