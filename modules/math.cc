#include <cmath>
#include <xemmai/convert.h>

namespace xemmai
{

class t_math : public t_extension
{
public:
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

t_math::t_math(t_object* a_module) : t_extension(a_module)
{
	f_define<double (*)(double), std::fabs>(this, L"fabs");
	f_define<double (*)(double), std::sqrt>(this, L"sqrt");
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
