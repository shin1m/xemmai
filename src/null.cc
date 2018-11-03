#include <xemmai/null.h>

#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<std::nullptr_t>::f_define()
{
	t_define<std::nullptr_t, t_object>(f_global(), L"Null"sv)
		(f_global()->f_symbol_string(), t_member<t_scoped(*)(const t_value&), f_string>())
	;
}

}
