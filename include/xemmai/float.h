#ifndef XEMMAI__FLOAT_H
#define XEMMAI__FLOAT_H

#include "object.h"

namespace xemmai
{

template<>
struct t_fundamental<float>
{
	typedef double t_type;
};

template<>
struct t_type_of<double> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__INTEGER:
				return static_cast<T0>(a_object.f_integer());
			case t_value::e_tag__FLOAT:
				return static_cast<T0>(a_object.f_float());
			default:
				return static_cast<T0>(f_object(a_object)->f_float());
			}
		}
	};
	template<typename T>
	struct t_as<T, t_object*>
	{
		typedef T t_type;

		static T f_call(t_object* a_object)
		{
			return static_cast<T>(a_object->f_float());
		}
	};
	template<typename T0, typename T1>
	struct t_of
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
		}
	};
	template<typename T>
	struct t_of<double, T>
	{
		static bool f_call(T a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__NULL:
			case t_value::e_tag__BOOLEAN:
				return false;
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return true;
			default:
				return dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
			}
		}
	};
	template<typename T0, typename T1>
	struct t_is
	{
		static bool f_call(T1 a_object)
		{
			return t_of<typename t_fundamental<T0>::t_type, T1>::f_call(a_object);
		}
	};

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		return t_transfer(static_cast<double>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, double a_value)
	{
		return t_transfer(a_value);
	}
	static t_transfer f_construct(t_object* a_class, int a_value)
	{
		return f_construct(a_class, static_cast<double>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, const std::wstring& a_value)
	{
		wchar_t* p;
		return f_construct(a_class, std::wcstod(a_value.c_str(), &p));
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
	static bool f_equals(double a_self, const t_value& a_value);
	static bool f_not_equals(double a_self, const t_value& a_value);
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
