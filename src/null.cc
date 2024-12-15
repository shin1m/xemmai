#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<std::nullptr_t>::f_define()
{
	auto global = f_global();
	t_define{global}
		(global->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), f__string>())
	.f_derive<std::nullptr_t, t_object>();
}

}
