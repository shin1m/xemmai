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
		this->v_construct = static_cast<t_scoped (t_type::*)(t_stacked*, size_t)>(&t_derived::f_do_construct);
		this->f_call = t_type::f_do_call;
		this->f_hash = t_type::f_do_hash;
		this->f_get_at = t_type::f_do_get_at;
		this->f_set_at = t_type::f_do_set_at;
		this->f_plus = t_type::f_do_plus;
		this->f_minus = t_type::f_do_minus;
		this->f_not = t_type::f_do_not;
		this->f_complement = t_type::f_do_complement;
		this->f_multiply = t_type::f_do_multiply;
		this->f_divide = t_type::f_do_divide;
		this->f_modulus = t_type::f_do_modulus;
		this->f_add = t_type::f_do_add;
		this->f_subtract = t_type::f_do_subtract;
		this->f_left_shift = t_type::f_do_left_shift;
		this->f_right_shift = t_type::f_do_right_shift;
		this->f_less = t_type::f_do_less;
		this->f_less_equal = t_type::f_do_less_equal;
		this->f_greater = t_type::f_do_greater;
		this->f_greater_equal = t_type::f_do_greater_equal;
		this->f_equals = t_type::f_do_equals;
		this->f_not_equals = t_type::f_do_not_equals;
		this->f_and = t_type::f_do_and;
		this->f_xor = t_type::f_do_xor;
		this->f_or = t_type::f_do_or;
		this->f_send = t_type::f_do_send;
	}
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n)
	{
		return static_cast<t_object*>(this->v_this)->f_call_preserved(f_global()->f_symbol_construct(), a_stack, a_n);
	}
};

template<typename T_base>
t_type* t_derivable<T_base>::f_do_derive()
{
	return new t_derived<t_type_of<typename T_base::t_what>>(t_type_of<typename T_base::t_what>::V_ids, this, this->v_module);
}

}

#endif
