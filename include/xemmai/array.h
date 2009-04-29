#ifndef XEMMAI__ARRAY_H
#define XEMMAI__ARRAY_H

#include "tuple.h"

namespace xemmai
{

class t_array
{
	friend struct t_type_of<t_array>;

	static t_slot* f_move_forward(t_slot* a_p, t_slot* a_q);
	static t_slot* f_move_backward(t_slot* a_p, t_slot* a_q);

	t_slot v_tuple;
	size_t v_head;
	size_t v_size;

	t_array() : v_head(0), v_size(0)
	{
	}
	~t_array()
	{
	}
	void f_resize();
	void f_grow();
	void f_shrink();

public:
	static t_transfer f_instantiate();

	void f_clear()
	{
		v_tuple = 0;
		v_head = v_size = 0;
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_slot& operator[](int a_index) const;
	t_slot& operator[](int a_index);
	void f_push(const t_transfer& a_value);
	t_transfer f_pop();
	void f_unshift(const t_transfer& a_value);
	t_transfer f_shift();
	void f_insert(int a_index, const t_transfer& a_value);
	t_transfer f_remove(int a_index);
};

template<>
struct t_type_of<t_array> : t_type
{
	static std::wstring f_string(t_object* a_self);
	static int f_hash(t_object* a_self);
	static t_object* f_get_at(t_object* a_self, int a_index);
	static t_object* f_set_at(t_object* a_self, int a_index, const t_transfer& a_value);
	static bool f_equals(t_object* a_self, t_object* a_other);
	static bool f_not_equals(t_object* a_self, t_object* a_other)
	{
		return !f_equals(a_self, a_other);
	}
	static void f_clear(t_object* a_self);
	static size_t f_size(t_object* a_self);
	static void f_push(t_object* a_self, const t_transfer& a_value);
	static t_transfer f_pop(t_object* a_self);
	static void f_unshift(t_object* a_self, const t_transfer& a_value);
	static t_transfer f_shift(t_object* a_self);
	static void f_insert(t_object* a_self, int a_index, const t_transfer& a_value);
	static t_transfer f_remove(t_object* a_self, int a_index);
	static t_transfer f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_hash(t_object* a_this, t_stack& a_stack);
	virtual void f_get_at(t_object* a_this, t_stack& a_stack);
	virtual void f_set_at(t_object* a_this, t_stack& a_stack);
	virtual void f_equals(t_object* a_this, t_stack& a_stack);
	virtual void f_not_equals(t_object* a_this, t_stack& a_stack);
};

}

#endif
