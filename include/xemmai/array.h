#ifndef XEMMAI__ARRAY_H
#define XEMMAI__ARRAY_H

#include "tuple.h"

namespace xemmai
{

class t_array : public t_sharable
{
	friend struct t_finalizes<t_bears<t_array>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_array>;

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
	size_t v_mask = 0;

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
	static XEMMAI__PORTABLE__EXPORT t_object* f_instantiate();

	void f_clear()
	{
		v_tuple = nullptr;
		v_head = v_size = v_mask = 0;
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_svalue& operator[](intptr_t a_index) const
	{
		f_validate(a_index);
		return f_as<t_tuple&>(v_tuple)[v_head + a_index & v_mask];
	}
	t_svalue& operator[](intptr_t a_index)
	{
		f_validate(a_index);
		return f_as<t_tuple&>(v_tuple)[v_head + a_index & v_mask];
	}
	void f_push(const t_pvalue& a_value)
	{
		f_grow();
		new(&f_as<t_tuple&>(v_tuple)[v_head + v_size & v_mask]) t_svalue(a_value);
		++v_size;
	}
	t_pvalue f_pop()
	{
		if (v_size <= 0) f_throw(L"empty array."sv);
		auto& x = f_as<t_tuple&>(v_tuple)[v_head + --v_size & v_mask];
		t_pvalue p = x;
		x = nullptr;
		f_shrink();
		return p;
	}
	void f_unshift(const t_pvalue& a_value)
	{
		f_grow();
		v_head = v_head - 1 & v_mask;
		new(&f_as<t_tuple&>(v_tuple)[v_head]) t_svalue(a_value);
		++v_size;
	}
	t_pvalue f_shift()
	{
		if (v_size <= 0) f_throw(L"empty array."sv);
		auto& x = f_as<t_tuple&>(v_tuple)[v_head];
		t_pvalue p = x;
		x = nullptr;
		v_head = v_head + 1 & v_mask;
		--v_size;
		f_shrink();
		return p;
	}
	void f_insert(intptr_t a_index, const t_pvalue& a_value);
	t_pvalue f_remove(intptr_t a_index);
};

template<>
struct t_type_of<t_array> : t_derivable<t_holds<t_array>>
{
	static void f__construct(xemmai::t_library* a_library, t_pvalue* a_stack, size_t a_n);
	static void f_own(t_array& a_self)
	{
		a_self.f_own();
	}
	static void f_share(t_array& a_self)
	{
		a_self.f_share();
	}
	static t_object* f_string(const t_pvalue& a_self);
	static void f_clear(const t_pvalue& a_self);
	static size_t f_size(const t_pvalue& a_self);
	static t_pvalue f__get_at(const t_pvalue& a_self, intptr_t a_index);
	static t_pvalue f__set_at(const t_pvalue& a_self, intptr_t a_index, const t_pvalue& a_value);
	static void f_push(const t_pvalue& a_self, const t_pvalue& a_value);
	static t_pvalue f_pop(const t_pvalue& a_self);
	static void f_unshift(const t_pvalue& a_self, const t_pvalue& a_value);
	static t_pvalue f_shift(const t_pvalue& a_self);
	static void f_insert(const t_pvalue& a_self, intptr_t a_index, const t_pvalue& a_value);
	static t_pvalue f_remove(const t_pvalue& a_self, intptr_t a_index);
	static void f_each(const t_pvalue& a_self, const t_pvalue& a_callable);
	static void f_sort(const t_pvalue& a_self, const t_pvalue& a_callable);
	static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_array>().v_tuple);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
};

}

#endif
