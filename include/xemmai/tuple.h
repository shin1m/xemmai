#ifndef XEMMAI__TUPLE_H
#define XEMMAI__TUPLE_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_tuple> : t_derivable<t_holds<t_tuple, t_type_immutable>>
{
	static void f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n);
	void f_define();

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : t_base(a_ids, a_super, std::move(a_module))
	{
		f_scan = f_do_scan;
		v_construct = static_cast<t_scoped (t_type::*)(t_stacked*, size_t)>(&t_type_of::f_do_construct);
		f_hash = f_do_hash;
		f_get_at = f_do_get_at;
		f_less = f_do_less;
		f_less_equal = f_do_less_equal;
		f_greater = f_do_greater;
		f_greater_equal = f_do_greater_equal;
		f_equals = f_do_equals;
		f_not_equals = f_do_not_equals;
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static void f_do_hash(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_less(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_less_equal(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_greater(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_greater_equal(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_equals(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_not_equals(t_object* a_this, t_stacked* a_stack);
};

template<size_t A_i>
inline void f_tuple(t_tuple& a_tuple)
{
}

template<size_t A_i, typename T_x, typename... T_xs>
inline void f_tuple(t_tuple& a_tuple, T_x&& a_x, T_xs&&... a_xs)
{
	a_tuple[A_i].f_construct(std::forward<T_x>(a_x));
	f_tuple<A_i + 1>(a_tuple, a_xs...);
}

template<typename... T_xs>
inline t_scoped f_tuple(T_xs&&... a_xs)
{
	t_scoped p = t_tuple::f_instantiate(sizeof...(T_xs));
	f_tuple<0>(f_as<t_tuple&>(p), a_xs...);
	return p;
}

}

#endif
