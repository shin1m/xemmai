#include <xemmai/math.h>

#include <cmath>
#include <xemmai/convert.h>

namespace xemmai
{

t_math::t_math(t_object* a_module) : t_extension(a_module)
{
	f_define<double (*)(double), std::fabs>(this, L"fabs");
	f_define<double (*)(double), std::sqrt>(this, L"sqrt");
}

void t_math::f_scan(t_scan a_scan)
{
}

}
