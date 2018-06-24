#ifndef XEMMAI__DERIVED_H
#define XEMMAI__DERIVED_H

#include "global.h"

namespace xemmai
{

template<typename T>
struct t_derived : T
{
	using T::T;
	virtual t_type* f_derive();
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	virtual size_t f_get_at(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_set_at(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_plus(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_minus(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_not(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_complement(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_multiply(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_divide(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_modulus(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_add(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_subtract(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_left_shift(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_right_shift(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_less(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_less_equal(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_greater(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_greater_equal(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_equals(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_not_equals(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_and(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_xor(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_or(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_send(t_object* a_this, t_stacked* a_stack);
};

template<typename T>
t_type* t_derived<T>::f_derive()
{
	return new t_derived(t_scoped(T::v_module), this);
}

template<typename T>
t_scoped t_derived<T>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return static_cast<t_object*>(T::v_this)->f_call_preserved(f_global()->f_symbol_construct(), a_stack, a_n);
}

template<typename T>
void t_derived<T>::f_hash(t_object* a_this, t_stacked* a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash(), a_stack);
	t_scoped x = std::move(a_stack[0]);
	x.f_call(a_stack, 0);
}

template<typename T>
size_t t_derived<T>::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	try {
		a_this->f_get(f_global()->f_symbol_call(), a_stack);
	} catch (...) {
		a_n += 2;
		for (size_t i = 2; i < a_n; ++i) a_stack[i].f_destruct();
		throw;
	}
	t_scoped x = std::move(a_stack[0]);
	return x.f_call_without_loop(a_stack, a_n);
}

#define XEMMAI__DERIVED__METHOD(a_method, a_n)\
template<typename T>\
size_t t_derived<T>::f_##a_method(t_object* a_this, t_stacked* a_stack)\
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

}

#endif
