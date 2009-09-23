#ifndef XEMMAI__INTEGER_H
#define XEMMAI__INTEGER_H

#include "object.h"

namespace xemmai
{

template<>
struct t_fundamental<size_t>
{
	typedef int t_type;
};

template<>
struct t_type_of<int> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			return static_cast<T0>(a_object->v_integer);
		}
	};

	static t_transfer f_construct(t_object* a_class, int a_value)
	{
		t_transfer object = t_object::f_allocate_uninitialized(a_class);
		object->v_integer = a_value;
		return object;
	}
	static t_transfer f_construct(t_object* a_class, double a_value)
	{
		return f_construct(a_class, static_cast<int>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, const std::wstring& a_value)
	{
		wchar_t* p;
		return f_construct(a_class, static_cast<int>(std::wcstol(a_value.c_str(), &p, 10)));
	}
	static std::wstring f_string(int a_self)
	{
		wchar_t cs[16];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%d", a_self);
		return cs;
	}
	static int f_hash(int a_self)
	{
		return a_self;
	}
	static int f_plus(int a_self)
	{
		return a_self;
	}
	static int f_minus(int a_self)
	{
		return -a_self;
	}
	static int f_complement(int a_self)
	{
		return ~a_self;
	}
	static int f_multiply(int a_self, int a_value)
	{
		return a_self * a_value;
	}
	static int f_divide(int a_self, int a_value)
	{
		return a_self / a_value;
	}
	static int f_modulus(int a_self, int a_value)
	{
		return a_self % a_value;
	}
	static int f_add(int a_self, int a_value)
	{
		return a_self + a_value;
	}
	static int f_subtract(int a_self, int a_value)
	{
		return a_self - a_value;
	}
	static int f_left_shift(int a_self, int a_value)
	{
		return a_self << a_value;
	}
	static int f_right_shift(int a_self, int a_value)
	{
		return a_self >> a_value;
	}
	static bool f_less(int a_self, int a_value)
	{
		return a_self < a_value;
	}
	static bool f_less_equal(int a_self, int a_value)
	{
		return a_self <= a_value;
	}
	static bool f_greater(int a_self, int a_value)
	{
		return a_self > a_value;
	}
	static bool f_greater_equal(int a_self, int a_value)
	{
		return a_self >= a_value;
	}
	static bool f_equals(int a_self, t_object* a_value);
	static bool f_not_equals(int a_self, t_object* a_value);
	static int f_and(int a_self, int a_value)
	{
		return a_self & a_value;
	}
	static int f_xor(int a_self, int a_value)
	{
		return a_self ^ a_value;
	}
	static int f_or(int a_self, int a_value)
	{
		return a_self | a_value;
	}
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_hash(t_object* a_this, t_stack& a_stack);
	virtual void f_plus(t_object* a_this, t_stack& a_stack);
	virtual void f_minus(t_object* a_this, t_stack& a_stack);
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

}

#endif
