#include <cmath>
#include <xemmai/convert.h>
#include <xemmai/array.h>

namespace xemmai
{

struct t_math : t_extension
{
	t_math(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(T a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

namespace
{

t_transfer f_frexp(double a_value)
{
	int e;
	double m = std::frexp(a_value, &e);
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	array.f_push(f_global()->f_as(m));
	array.f_push(f_global()->f_as(e));
	return p;
}

t_transfer f_modf(double a_value)
{
	double i;
	double f = std::modf(a_value, &i);
	t_transfer p = t_array::f_instantiate();
	t_array& array = f_as<t_array&>(p);
	array.f_push(f_global()->f_as(f));
	array.f_push(f_global()->f_as(i));
	return p;
}

template<int (*A_function)(double)>
bool f_boolean(double a_value)
{
	return A_function(a_value);
}

}

t_math::t_math(t_object* a_module) : t_extension(a_module)
{
	f_define<double (*)(double), std::acos>(this, L"acos");
	f_define<double (*)(double), std::asin>(this, L"asin");
	f_define<double (*)(double), std::atan>(this, L"atan");
	f_define<double (*)(double, double), std::atan2>(this, L"atan2");
	f_define<double (*)(double), std::ceil>(this, L"ceil");
	f_define<double (*)(double), std::cos>(this, L"cos");
	f_define<double (*)(double), std::cosh>(this, L"cosh");
	f_define<double (*)(double), std::exp>(this, L"exp");
	f_define<double (*)(double), std::fabs>(this, L"fabs");
	f_define<double (*)(double), std::floor>(this, L"floor");
	f_define<double (*)(double, double), std::fmod>(this, L"fmod");
	f_define<t_transfer (*)(double), f_frexp>(this, L"frexp");
	f_define<double (*)(double, int), std::ldexp>(this, L"ldexp");
	f_define<double (*)(double), std::log>(this, L"log");
	f_define<double (*)(double), std::log10>(this, L"log10");
	f_define<t_transfer (*)(double), f_modf>(this, L"modf");
	f_define<double (*)(double, double), std::pow>(this, L"pow");
	f_define<double (*)(double), std::sin>(this, L"sin");
	f_define<double (*)(double), std::sinh>(this, L"sinh");
	f_define<double (*)(double), std::sqrt>(this, L"sqrt");
	f_define<double (*)(double), std::tan>(this, L"tan");
	f_define<double (*)(double), std::tanh>(this, L"tanh");
	f_define<bool (*)(double), f_boolean<std::isfinite> >(this, L"isfinite");
	f_define<bool (*)(double), f_boolean<std::isinf> >(this, L"isinf");
	f_define<bool (*)(double), f_boolean<std::isnan> >(this, L"isnan");
	f_define<bool (*)(double), f_boolean<std::isnormal> >(this, L"isnormal");
	f_define<bool (*)(double), f_boolean<std::signbit> >(this, L"signbit");
	a_module->f_put(t_symbol::f_instantiate(L"E"), f_as(M_E));
	a_module->f_put(t_symbol::f_instantiate(L"PI"), f_as(M_PI));
}

void t_math::f_scan(t_scan a_scan)
{
}

}

#ifdef _MSC_VER
extern "C" __declspec(dllexport) xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#else
extern "C" xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#endif
{
	return new xemmai::t_math(a_module);
}
