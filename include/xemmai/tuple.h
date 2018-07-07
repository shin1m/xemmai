#ifndef XEMMAI__TUPLE_H
#define XEMMAI__TUPLE_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_tuple> : t_derivable<t_tuple, t_with_ids<t_tuple, t_type_immutable>>
{
	static void f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n);
	void f_define();

	using t_base::t_base;
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_get_at(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_less(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_less_equal(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_greater(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_greater_equal(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_equals(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_not_equals(t_object* a_this, t_stacked* a_stack);
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
