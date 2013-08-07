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
		typedef typename t_fundamental<T0>::t_type t_type;

		static t_type f_call(T1 a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__INTEGER:
				return a_object.f_integer();
			case t_value::e_tag__FLOAT:
				return a_object.f_float();
			default:
				return f_object(a_object)->f_float();
			}
		}
	};
	template<typename T>
	struct t_as<T, t_object*>
	{
		typedef typename t_fundamental<T>::t_type t_type;

		static t_type f_call(t_object* a_object)
		{
			return a_object->f_float();
		}
	};
	template<typename T0, typename T1>
	struct t_of
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != nullptr;
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
				return dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != nullptr;
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
	static t_scoped f_transfer(T_extension* a_extension, T a_value)
	{
		return t_value(static_cast<double>(a_value));
	}
	static t_scoped f_construct(t_object* a_class, double a_value)
	{
		return t_value(a_value);
	}
	static t_scoped f_construct(t_object* a_class, ptrdiff_t a_value)
	{
		return f_construct(a_class, static_cast<double>(a_value));
	}
	static t_scoped f_construct(t_object* a_class, const std::wstring& a_value)
	{
		return f_construct(a_class, f_parse(a_value.c_str()));
	}
	static t_scoped f_construct_derived(t_object* a_class, double a_value)
	{
		t_scoped object = t_object::f_allocate_uninitialized(a_class);
		object.f_float__(a_value);
		return object;
	}
	static double f_parse(const wchar_t* a_value)
	{
		wchar_t* p;
		return std::wcstod(a_value, &p);
	}
	static std::wstring f_string(double a_self)
	{
		wchar_t cs[32];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%g", a_self);
		return cs;
	}
	static ptrdiff_t f_hash(double a_self)
	{
		union
		{
			double v_d;
			ptrdiff_t v_is[sizeof(double) / sizeof(ptrdiff_t)];
		} u;
		u.v_d = a_self;
		ptrdiff_t n = 0;
		ptrdiff_t i = sizeof(double) / sizeof(ptrdiff_t);
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

	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
