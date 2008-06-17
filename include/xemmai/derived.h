#ifndef XEMMAI__DERIVED_H
#define XEMMAI__DERIVED_H

#include "global.h"

namespace xemmai
{

template<typename T>
struct t_derived : T
{
	t_derived(const t_transfer& a_module, const t_transfer& a_super) : T(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_hash(t_object* a_this, t_stack& a_stack);
	virtual void f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack);
	virtual void f_get_at(t_object* a_this, t_stack& a_stack);
	virtual void f_set_at(t_object* a_this, t_stack& a_stack);
	virtual void f_plus(t_object* a_this, t_stack& a_stack);
	virtual void f_minus(t_object* a_this, t_stack& a_stack);
	virtual void f_not(t_object* a_this, t_stack& a_stack);
	virtual void f_complement(t_object* a_this, t_stack& a_stack);
	virtual void f_multiply(t_object* a_this, t_stack& a_stack);
	virtual void f_divide(t_object* a_this, t_stack& a_stack);
	virtual void f_modulus(t_object* a_this, t_stack& a_stack);
	virtual void f_add(t_object* a_this, t_stack& a_stack);
	virtual void f_subtract(t_object* a_this, t_stack& a_stack);
	virtual void f_left_shift(t_object* a_this, t_stack& a_stack);
	virtual void f_right_shift(t_object* a_this, t_stack& a_stack);
	virtual void f_less(t_object* a_this, t_stack& a_stack);
	virtual void f_less_equal(t_object* a_this, t_stack& a_stack);
	virtual void f_greater(t_object* a_this, t_stack& a_stack);
	virtual void f_greater_equal(t_object* a_this, t_stack& a_stack);
	virtual void f_equals(t_object* a_this, t_stack& a_stack);
	virtual void f_not_equals(t_object* a_this, t_stack& a_stack);
	virtual void f_and(t_object* a_this, t_stack& a_stack);
	virtual void f_xor(t_object* a_this, t_stack& a_stack);
	virtual void f_or(t_object* a_this, t_stack& a_stack);
};

template<typename T>
t_type* t_derived<T>::f_derive(t_object* a_this)
{
	return new t_derived(T::v_module, a_this);
}

template<typename T>
void t_derived<T>::f_hash(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash())->f_call(0, 0, a_stack);
}

template<typename T>
void t_derived<T>::f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_call())->f_call(0, a_n, a_stack);
}

template<typename T>
void t_derived<T>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_get_at())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_set_at(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_set_at())->f_call(0, 2, a_stack);
}

template<typename T>
void t_derived<T>::f_plus(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_plus())->f_call(0, 0, a_stack);
}

template<typename T>
void t_derived<T>::f_minus(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_minus())->f_call(0, 0, a_stack);
}

template<typename T>
void t_derived<T>::f_not(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_not())->f_call(0, 0, a_stack);
}

template<typename T>
void t_derived<T>::f_complement(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_complement())->f_call(0, 0, a_stack);
}

template<typename T>
void t_derived<T>::f_multiply(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_multiply())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_divide(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_divide())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_modulus(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_modulus())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_add(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_add())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_subtract(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_subtract())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_left_shift(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_left_shift())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_right_shift(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_right_shift())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_less(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_less())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_less_equal(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_less_equal())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_greater(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_greater_equal(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater_equal())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_equals(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_equals())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_not_equals(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_not_equals())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_and(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_and())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_xor(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_xor())->f_call(0, 1, a_stack);
}

template<typename T>
void t_derived<T>::f_or(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_or())->f_call(0, 1, a_stack);
}

}

#endif
