#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <xemmai/convert.h>

namespace xemmai
{

struct t_math : t_library
{
	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_math, t_global, f_global())

namespace
{

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

void t_math::f_scan(t_scan a_scan)
{
}

std::vector<std::pair<t_root, t_rvalue>> t_math::f_define()
{
	return t_define(this)
	(L"acos"sv, t_static<double(*)(double), std::acos>())
	(L"asin"sv, t_static<double(*)(double), std::asin>())
	(L"atan"sv, t_static<double(*)(double), std::atan>())
	(L"atan2"sv, t_static<double(*)(double, double), std::atan2>())
	(L"ceil"sv, t_static<double(*)(double), std::ceil>())
	(L"cos"sv, t_static<double(*)(double), std::cos>())
	(L"cosh"sv, t_static<double(*)(double), std::cosh>())
	(L"exp"sv, t_static<double(*)(double), std::exp>())
	(L"fabs"sv, t_static<double(*)(double), std::fabs>())
	(L"floor"sv, t_static<double(*)(double), std::floor>())
	(L"fmod"sv, t_static<double(*)(double, double), std::fmod>())
	(L"frexp"sv, t_static<t_object*(*)(double), [](double a_value)
	{
		int e;
		double m = std::frexp(a_value, &e);
		return f_tuple(m, e);
	}>())
	(L"ldexp"sv, t_static<double(*)(double, int), std::ldexp>())
	(L"log"sv, t_static<double(*)(double), std::log>())
	(L"log10"sv, t_static<double(*)(double), std::log10>())
	(L"modf"sv, t_static<t_object*(*)(double), [](double a_value)
	{
		double i;
		double f = std::modf(a_value, &i);
		return f_tuple(f, i);
	}>())
	(L"pow"sv, t_static<double(*)(double, double), std::pow>())
	(L"sin"sv, t_static<double(*)(double), std::sin>())
	(L"sinh"sv, t_static<double(*)(double), std::sinh>())
	(L"sqrt"sv, t_static<double(*)(double), std::sqrt>())
	(L"tan"sv, t_static<double(*)(double), std::tan>())
	(L"tanh"sv, t_static<double(*)(double), std::tanh>())
	(L"isfinite"sv, t_static<bool(*)(double), f_boolean<std::isfinite>>())
	(L"isinf"sv, t_static<bool(*)(double), f_boolean<std::isinf>>())
	(L"isnan"sv, t_static<bool(*)(double), f_boolean<std::isnan>>())
	(L"isnormal"sv, t_static<bool(*)(double), f_boolean<std::isnormal>>())
	(L"signbit"sv, t_static<bool(*)(double), f_boolean<std::signbit>>())
	(L"E"sv, M_E)
	(L"PI"sv, M_PI)
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmai::t_math>(a_handle);
}
