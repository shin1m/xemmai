#ifndef XEMMAI__FLOAT_H
#define XEMMAI__FLOAT_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<double> : t_type
{
	static t_transfer f_construct(t_object* a_class, double a_value)
	{
		t_transfer object = t_object::f_allocate_uninitialized(a_class);
		object->v_float = a_value;
		return object;
	}
	static std::wstring f_string(double a_self)
	{
		wchar_t cs[32];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%g", a_self);
		return cs;
	}
	static int f_hash(double a_self)
	{
		union
		{
			double v_d;
			int v_is[sizeof(double) / sizeof(int)];
		} u;
		u.v_d = a_self;
		int n = 0;
		int i = sizeof(double) / sizeof(int);
		while (i > 0) n ^= u.v_is[--i];
		return n;
	}
	static double f_plus(double a_self)
	{
		return a_self;
	}
	static double f_minus(double a_self)
	{
		return -a_self;
	}
	static double f_multiply(double a_self, double a_value)
	{
		return a_self * a_value;
	}
	static double f_divide(double a_self, double a_value)
	{
		return a_self / a_value;
	}
	static double f_add(double a_self, double a_value)
	{
		return a_self + a_value;
	}
	static double f_subtract(double a_self, double a_value)
	{
		return a_self - a_value;
	}
	static bool f_less(double a_self, double a_value)
	{
		return a_self < a_value;
	}
	static bool f_less_equal(double a_self, double a_value)
	{
		return a_self <= a_value;
	}
	static bool f_greater(double a_self, double a_value)
	{
		return a_self > a_value;
	}
	static bool f_greater_equal(double a_self, double a_value)
	{
		return a_self >= a_value;
	}
	static bool f_equals(double a_self, t_object* a_value);
	static bool f_not_equals(double a_self, t_object* a_value);
	static t_transfer f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_hash(t_object* a_this, t_stack& a_stack);
	virtual void f_plus(t_object* a_this, t_stack& a_stack);
	virtual void f_minus(t_object* a_this, t_stack& a_stack);
	virtual void f_multiply(t_object* a_this, t_stack& a_stack);
	virtual void f_divide(t_object* a_this, t_stack& a_stack);
	virtual void f_add(t_object* a_this, t_stack& a_stack);
	virtual void f_subtract(t_object* a_this, t_stack& a_stack);
	virtual void f_less(t_object* a_this, t_stack& a_stack);
	virtual void f_less_equal(t_object* a_this, t_stack& a_stack);
	virtual void f_greater(t_object* a_this, t_stack& a_stack);
	virtual void f_greater_equal(t_object* a_this, t_stack& a_stack);
	virtual void f_equals(t_object* a_this, t_stack& a_stack);
	virtual void f_not_equals(t_object* a_this, t_stack& a_stack);
};

template<typename T>
struct t_as<double, T>
{
	typedef double t_type;

	static double f_call(T a_object)
	{
		return a_object->v_float;
	}
};

}

#endif
