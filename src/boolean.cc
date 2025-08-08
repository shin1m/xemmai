#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<bool>::f_define()
{
	auto global = f_global();
	t_define{global}
	(global->f_symbol___string(), t_member<t_object*(*)(const t_pvalue&), f__string>())
	.f_derive<bool, t_object>();
}

}
