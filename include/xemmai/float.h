#ifndef XEMMAI__FLOAT_H
#define XEMMAI__FLOAT_H

#include "class.h"
#include "string.h"

namespace xemmai
{

template<>
struct t_fundamental<float>
{
	using t_type = double;
};

template<>
struct t_type_of<double> : t_derivable<t_bears<double>, t_derived_primitive<double>>
{
	template<typename T>
	struct t_cast
	{
		using t_type = typename t_fundamental<T>::t_type;

		static t_type f_as(auto&& a_object)
		{
			auto p = static_cast<t_object*>(a_object);
			switch (reinterpret_cast<uintptr_t>(p)) {
			case c_tag__INTEGER:
				return a_object.f_integer();
			case c_tag__FLOAT:
				return a_object.f_float();
			default:
				return p->f_as<double>();
			}
		}
		static t_type f_as(t_object* a_object)
		{
			return a_object->f_as<double>();
		}
		static bool f_is(t_object* a_object)
		{
			if (!std::is_same_v<typename t_fundamental<T>::t_type, double>) return reinterpret_cast<uintptr_t>(a_object) >= c_tag__OBJECT && a_object->f_type()->f_derives<typename t_fundamental<T>::t_type>();
			switch (reinterpret_cast<uintptr_t>(a_object)) {
			case c_tag__NULL:
			case c_tag__FALSE:
			case c_tag__TRUE:
				return false;
			case c_tag__INTEGER:
			case c_tag__FLOAT:
				return true;
			default:
				return a_object->f_type()->f_derives<double>();
			}
		}
	};

	static t_pvalue f_construct(t_type* a_class, const t_string& a_value)
	{
		return std::wcstod(a_value, NULL);
	}
	XEMMAI__PUBLIC static t_object* f__string(double a_self);
	static intptr_t f__hash(double a_self)
	{
		return std::hash<double>{}(a_self);
	}
	static double f__plus(double a_self)
	{
		return a_self;
	}
	static double f__minus(double a_self)
	{
		return -a_self;
	}
	static double f__multiply(double a_self, double a_value)
	{
		return a_self * a_value;
	}
	static double f__divide(double a_self, double a_value)
	{
		return a_self / a_value;
	}
	static double f__add(double a_self, double a_value)
	{
		return a_self + a_value;
	}
	static double f__subtract(double a_self, double a_value)
	{
		return a_self - a_value;
	}
	static bool f__less(double a_self, double a_value)
	{
		return a_self < a_value;
	}
	static bool f__less_equal(double a_self, double a_value)
	{
		return a_self <= a_value;
	}
	static bool f__greater(double a_self, double a_value)
	{
		return a_self > a_value;
	}
	static bool f__greater_equal(double a_self, double a_value)
	{
		return a_self >= a_value;
	}
	XEMMAI__LOCAL static bool f__equals(double a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static bool f__not_equals(double a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
