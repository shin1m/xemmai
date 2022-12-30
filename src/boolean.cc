#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_type_of<bool>::f__string(bool a_self)
{
	return t_string::f_instantiate(a_self ? L"true"sv : L"false"sv);
}

void t_type_of<bool>::f_define()
{
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(*)(bool), f__string>())
	.f_derive<bool, t_object>();
}

}
