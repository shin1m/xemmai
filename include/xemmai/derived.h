#ifndef XEMMAI__DERIVED_H
#define XEMMAI__DERIVED_H

#include "global.h"

namespace xemmai
{

template<typename T>
struct t_derived : T
{
	using T::T;
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_call(t_object* a_this, t_scoped* a_stack, size_t a_n);
	virtual size_t f_get_at(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_set_at(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_plus(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_minus(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_not(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_complement(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_multiply(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_divide(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_modulus(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_add(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_subtract(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_left_shift(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_right_shift(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_less(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_less_equal(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_greater(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_greater_equal(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_equals(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_not_equals(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_and(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_xor(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_or(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_send(t_object* a_this, t_scoped* a_stack);
};

template<typename T>
t_type* t_derived<T>::f_derive(t_object* a_this)
{
	return new t_derived(t_scoped(T::v_module), a_this);
}

template<typename T>
t_scoped t_derived<T>::f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	return a_class->f_get(f_global()->f_symbol_construct()).f_call_with_same(a_stack, a_n);
}

template<typename T>
void t_derived<T>::f_hash(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash()).f_call(a_stack, 0);
}

template<typename T>
size_t t_derived<T>::f_call(t_object* a_this, t_scoped* a_stack, size_t a_n)
{
	return a_this->f_get(f_global()->f_symbol_call()).f_call_without_loop(a_stack, a_n);
}

template<typename T>
size_t t_derived<T>::f_get_at(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_get_at(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_set_at(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_set_at(), a_stack);
	return 2;
}

template<typename T>
size_t t_derived<T>::f_plus(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_plus(), a_stack);
	return 0;
}

template<typename T>
size_t t_derived<T>::f_minus(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_minus(), a_stack);
	return 0;
}

template<typename T>
size_t t_derived<T>::f_not(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_not(), a_stack);
	return 0;
}

template<typename T>
size_t t_derived<T>::f_complement(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_complement(), a_stack);
	return 0;
}

template<typename T>
size_t t_derived<T>::f_multiply(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_multiply(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_divide(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_divide(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_modulus(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_modulus(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_add(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_add(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_subtract(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_subtract(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_left_shift(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_left_shift(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_right_shift(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_right_shift(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_less(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_less(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_less_equal(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_less_equal(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_greater(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_greater_equal(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater_equal(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_equals(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_equals(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_not_equals(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_not_equals(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_and(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_and(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_xor(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_xor(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_or(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_or(), a_stack);
	return 1;
}

template<typename T>
size_t t_derived<T>::f_send(t_object* a_this, t_scoped* a_stack)
{
	a_this->f_get(f_global()->f_symbol_send(), a_stack);
	return 1;
}

}

#endif
