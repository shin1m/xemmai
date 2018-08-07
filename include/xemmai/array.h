#ifndef XEMMAI__ARRAY_H
#define XEMMAI__ARRAY_H

#include "tuple.h"

namespace xemmai
{

class t_array
{
	friend struct t_type_of<t_object>;
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

	t_slot v_tuple;
	size_t v_head = 0;
	size_t v_size = 0;

	t_array() = default;
	~t_array() = default;
	void f_resize();
	void f_grow();
	void f_shrink();
	void f_validate(intptr_t& a_index) const
	{
		if (a_index < 0) {
			a_index += v_size;
			if (a_index < 0) f_throw(L"out of range.");
		} else {
			if (a_index >= static_cast<intptr_t>(v_size)) f_throw(L"out of range.");
		}
	}

public:
	static XEMMAI__PORTABLE__EXPORT t_scoped f_instantiate();

	void f_clear()
	{
		v_tuple = nullptr;
		v_head = v_size = 0;
	}
	size_t f_size() const
	{
		return v_size;
	}
	void f_swap(t_scoped& a_tuple, size_t& a_head, size_t& a_size)
	{
		t_scoped tuple = std::move(v_tuple);
		v_tuple = std::move(a_tuple);
		a_tuple = std::move(tuple);
		std::swap(v_head, a_head);
		std::swap(v_size, a_size);
	}
	const t_slot& operator[](intptr_t a_index) const
	{
		f_validate(a_index);
		auto& tuple = f_as<const t_tuple&>(v_tuple);
		return tuple[(v_head + a_index) % tuple.f_size()];
	}
	t_slot& operator[](intptr_t a_index)
	{
		f_validate(a_index);
		auto& tuple = f_as<t_tuple&>(v_tuple);
		return tuple[(v_head + a_index) % tuple.f_size()];
	}
	const t_value& f_get_at(intptr_t a_index) const
	{
		return (*this)[a_index];
	}
	const t_value& f_set_at(intptr_t a_index, t_scoped&& a_value)
	{
		return (*this)[a_index] = std::move(a_value);
	}
	void f_push(t_scoped&& a_value)
	{
		f_grow();
		auto& tuple = f_as<t_tuple&>(v_tuple);
		tuple[(v_head + v_size) % tuple.f_size()] = std::move(a_value);
		++v_size;
	}
	t_scoped f_pop()
	{
		if (v_size <= 0) f_throw(L"empty array.");
		auto& tuple = f_as<t_tuple&>(v_tuple);
		t_scoped p = std::move(tuple[(v_head + --v_size) % tuple.f_size()]);
		f_shrink();
		return p;
	}
	void f_unshift(t_scoped&& a_value)
	{
		f_grow();
		auto& tuple = f_as<t_tuple&>(v_tuple);
		v_head += tuple.f_size() - 1;
		v_head %= tuple.f_size();
		tuple[v_head] = std::move(a_value);
		++v_size;
	}
	t_scoped f_shift()
	{
		if (v_size <= 0) f_throw(L"empty array.");
		auto& tuple = f_as<t_tuple&>(v_tuple);
		t_scoped p = std::move(tuple[v_head]);
		++v_head;
		v_head %= tuple.f_size();
		--v_size;
		f_shrink();
		return p;
	}
	XEMMAI__PORTABLE__EXPORT void f_insert(intptr_t a_index, t_scoped&& a_value);
	XEMMAI__PORTABLE__EXPORT t_scoped f_remove(intptr_t a_index);
};

template<>
struct t_type_of<t_array> : t_override<t_derivable<t_holds<t_array>>>
{
	static void f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n);
	static std::wstring f_string(const t_value& a_self);
	static intptr_t f__hash(const t_value& a_self);
	static bool f__less(const t_value& a_self, const t_value& a_other);
	static bool f__less_equal(const t_value& a_self, const t_value& a_other);
	static bool f__greater(const t_value& a_self, const t_value& a_other);
	static bool f__greater_equal(const t_value& a_self, const t_value& a_other);
	static bool f__equals(const t_value& a_self, const t_value& a_other);
	static bool f__not_equals(const t_value& a_self, const t_value& a_other)
	{
		return !f__equals(a_self, a_other);
	}
	static void f_each(const t_value& a_self, const t_value& a_callable);
	static void f_sort(const t_value& a_self, const t_value& a_callable);
	static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static void f_do_hash(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_less(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_less_equal(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_greater(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_greater_equal(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_equals(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_not_equals(t_object* a_this, t_stacked* a_stack);
};

}

#endif
