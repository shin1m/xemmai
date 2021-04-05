#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <xemmai/convert.h>

namespace xemmai
{

struct t_math : t_extension
{
	t_math(t_object* a_module);
	virtual void f_scan(t_scan a_scan)
	{
	}
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
};

namespace
{

t_object* f_frexp(double a_value)
{
	int e;
	double m = std::frexp(a_value, &e);
	return f_tuple(m, e);
}

t_object* f_modf(double a_value)
{
	double i;
	double f = std::modf(a_value, &i);
	return f_tuple(f, i);
}

template<bool (*A_function)(double)>
bool f_boolean(double a_value)
{
	return A_function(a_value);
}

template<int (*A_function)(double)>
bool f_boolean(double a_value)
{
	return A_function(a_value);
}

}

t_math::t_math(t_object* a_module) : t_extension(a_module)
{
	f_define<double(*)(double), std::acos>(this, L"acos"sv);
	f_define<double(*)(double), std::asin>(this, L"asin"sv);
	f_define<double(*)(double), std::atan>(this, L"atan"sv);
	f_define<double(*)(double, double), std::atan2>(this, L"atan2"sv);
	f_define<double(*)(double), std::ceil>(this, L"ceil"sv);
	f_define<double(*)(double), std::cos>(this, L"cos"sv);
	f_define<double(*)(double), std::cosh>(this, L"cosh"sv);
	f_define<double(*)(double), std::exp>(this, L"exp"sv);
	f_define<double(*)(double), std::fabs>(this, L"fabs"sv);
	f_define<double(*)(double), std::floor>(this, L"floor"sv);
	f_define<double(*)(double, double), std::fmod>(this, L"fmod"sv);
	f_define<t_object*(*)(double), f_frexp>(this, L"frexp"sv);
	f_define<double(*)(double, int), std::ldexp>(this, L"ldexp"sv);
	f_define<double(*)(double), std::log>(this, L"log"sv);
	f_define<double(*)(double), std::log10>(this, L"log10"sv);
	f_define<t_object*(*)(double), f_modf>(this, L"modf"sv);
	f_define<double(*)(double, double), std::pow>(this, L"pow"sv);
	f_define<double(*)(double), std::sin>(this, L"sin"sv);
	f_define<double(*)(double), std::sinh>(this, L"sinh"sv);
	f_define<double(*)(double), std::sqrt>(this, L"sqrt"sv);
	f_define<double(*)(double), std::tan>(this, L"tan"sv);
	f_define<double(*)(double), std::tanh>(this, L"tanh"sv);
	f_define<bool(*)(double), f_boolean<std::isfinite>>(this, L"isfinite"sv);
	f_define<bool(*)(double), f_boolean<std::isinf>>(this, L"isinf"sv);
	f_define<bool(*)(double), f_boolean<std::isnan>>(this, L"isnan"sv);
	f_define<bool(*)(double), f_boolean<std::isnormal>>(this, L"isnormal"sv);
	f_define<bool(*)(double), f_boolean<std::signbit>>(this, L"signbit"sv);
	a_module->f_put(t_symbol::f_instantiate(L"E"sv), f_as(M_E));
	a_module->f_put(t_symbol::f_instantiate(L"PI"sv), f_as(M_PI));
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmai::t_math(a_module);
}
