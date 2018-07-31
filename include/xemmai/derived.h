#ifndef XEMMAI__DERIVED_H
#define XEMMAI__DERIVED_H

#include "global.h"

namespace xemmai
{

template<typename T>
struct t_derived : T
{
	template<size_t A_n>
	t_derived(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : T(a_ids, a_super, std::move(a_module))
	{
		this->f_call = t_type::f_do_call;
	}
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n)
	{
		return static_cast<t_object*>(T::v_this)->f_call_preserved(f_global()->f_symbol_construct(), a_stack, a_n);
	}
	virtual void f_hash(t_object* a_this, t_stacked* a_stack)
	{
		a_this->f_get(f_global()->f_symbol_hash(), a_stack);
		t_scoped x = std::move(a_stack[0]);
		x.f_call(a_stack, 0);
	}
#define XEMMAI__DERIVED__METHOD(a_method, a_n)\
	virtual size_t f_##a_method(t_object* a_this, t_stacked* a_stack)\
	{\
		try {\
			a_this->f_get(f_global()->f_symbol_##a_method(), a_stack);\
			return a_n;\
		} catch (...) {\
			for (size_t i = 2; i < a_n + 2; ++i) a_stack[i].f_destruct();\
			throw;\
		}\
	}
	XEMMAI__DERIVED__METHOD(get_at, 1)
	XEMMAI__DERIVED__METHOD(set_at, 2)
	XEMMAI__DERIVED__METHOD(plus, 0)
	XEMMAI__DERIVED__METHOD(minus, 0)
	XEMMAI__DERIVED__METHOD(not, 0)
	XEMMAI__DERIVED__METHOD(complement, 0)
	XEMMAI__DERIVED__METHOD(multiply, 1)
	XEMMAI__DERIVED__METHOD(divide, 1)
	XEMMAI__DERIVED__METHOD(modulus, 1)
	XEMMAI__DERIVED__METHOD(add, 1)
	XEMMAI__DERIVED__METHOD(subtract, 1)
	XEMMAI__DERIVED__METHOD(left_shift, 1)
	XEMMAI__DERIVED__METHOD(right_shift, 1)
	XEMMAI__DERIVED__METHOD(less, 1)
	XEMMAI__DERIVED__METHOD(less_equal, 1)
	XEMMAI__DERIVED__METHOD(greater, 1)
	XEMMAI__DERIVED__METHOD(greater_equal, 1)
	XEMMAI__DERIVED__METHOD(equals, 1)
	XEMMAI__DERIVED__METHOD(not_equals, 1)
	XEMMAI__DERIVED__METHOD(and, 1)
	XEMMAI__DERIVED__METHOD(xor, 1)
	XEMMAI__DERIVED__METHOD(or, 1)
	XEMMAI__DERIVED__METHOD(send, 1)
};

template<typename T_base>
t_type* t_derivable<T_base>::f_derive()
{
	return new t_derived<t_type_of<typename T_base::t_what>>(t_type_of<typename T_base::t_what>::V_ids, this, this->v_module);
}

}

#endif
