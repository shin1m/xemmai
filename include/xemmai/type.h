#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include <string>

#include "pointer.h"

namespace xemmai
{

class t_global;

struct t_stack
{
	t_slot* v_top;

	XEMMAI__PORTABLE__ALWAYS_INLINE void f_push(t_object* a_value)
	{
		(--v_top)->f_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_push(const t_transfer& a_value)
	{
		(--v_top)->f_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_push(const t_shared& a_value)
	{
		f_push(static_cast<t_object*>(a_value));
	}
	t_transfer f_pop()
	{
		t_transfer p = v_top->f_transfer();
		++v_top;
		return p;
	}
	t_slot& f_top()
	{
		return *v_top;
	}
	t_slot& f_at(size_t a_i)
	{
		return v_top[a_i];
	}
	void f_return(t_object* a_value)
	{
		v_top->f_construct(a_value);
	}
	void f_return(const t_transfer& a_value)
	{
		v_top->f_construct(a_value);
	}
	void f_return(const t_shared& a_value)
	{
		f_return(static_cast<t_object*>(a_value));
	}
};

struct t_type
{
	typedef t_global t_extension;

	t_slot v_module;
	t_slot v_super;
	bool v_builtin;
	bool v_primitive;
	bool v_revive;

	XEMMAI__PORTABLE__EXPORT static bool f_derives(t_object* a_this, t_object* a_type);
	static void f_construct(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack);
	static void f_initialize(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT static std::wstring f_string(t_object* a_self)
	{
		wchar_t cs[13 + sizeof(t_object*) * 2];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", a_self);
		return cs;
	}
	XEMMAI__PORTABLE__EXPORT static int f_hash(t_object* a_self)
	{
		return reinterpret_cast<int>(a_self);
	}
	XEMMAI__PORTABLE__EXPORT static bool f_equals(t_object* a_self, t_object* a_other)
	{
		return a_self == a_other;
	}
	XEMMAI__PORTABLE__EXPORT static bool f_not_equals(t_object* a_self, t_object* a_other)
	{
		return a_self != a_other;
	}
	XEMMAI__PORTABLE__EXPORT static void f_define(t_object* a_class);

	t_type(const t_transfer& a_module, const t_transfer& a_super) : v_module(a_module), v_super(a_super), v_builtin(false), v_primitive(false), v_revive(false)
	{
	}
	XEMMAI__PORTABLE__EXPORT virtual ~t_type();
	XEMMAI__PORTABLE__EXPORT virtual t_type* f_derive(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual t_transfer f_get(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_put(t_object* a_this, t_object* a_key, const t_transfer& a_value);
	XEMMAI__PORTABLE__EXPORT virtual t_transfer f_remove(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_hash(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_get_at(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_set_at(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_plus(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_minus(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_not(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_complement(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_multiply(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_divide(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_modulus(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_add(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_subtract(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_left_shift(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_right_shift(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_less(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_less_equal(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_greater(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_greater_equal(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_equals(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_not_equals(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_and(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_xor(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_or(t_object* a_this, t_stack& a_stack);
};

template<typename T>
struct t_type_of
{
	typedef t_global t_extension;
};

}

#endif
