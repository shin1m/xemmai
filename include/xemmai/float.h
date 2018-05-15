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
	template<typename T0>
	struct t_as
	{
		typedef typename t_fundamental<T0>::t_type t_type;

		template<typename T1>
		static t_type f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__INTEGER:
				return a_object.f_integer();
			case t_value::e_tag__FLOAT:
				return a_object.f_float();
			default:
				return p->f_float();
			}
		}
		static t_type f_call(t_object* a_object)
		{
			return a_object->f_float();
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			if (!std::is_same<typename t_fundamental<T0>::t_type, double>::value) return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(&f_as<t_type&>(p->f_type())) != nullptr;
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__NULL:
			case t_value::e_tag__BOOLEAN:
				return false;
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return true;
			default:
				return dynamic_cast<t_type_of*>(&f_as<t_type&>(p->f_type())) != nullptr;
			}
		}
	};

	static t_scoped f_construct(t_object* a_class, double a_value)
	{
		return t_scoped(a_value);
	}
	static t_scoped f_construct(t_object* a_class, intptr_t a_value)
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
	static intptr_t f_hash(double a_self)
	{
		union
		{
			double v_d;
			intptr_t v_is[sizeof(double) / sizeof(intptr_t)];
		} u;
		u.v_d = a_self;
		intptr_t n = 0;
		intptr_t i = sizeof(double) / sizeof(intptr_t);
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
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
