#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<std::nullptr_t>::f_define()
{
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), f__string>())
	.f_derive<std::nullptr_t, t_object>();
}

}
