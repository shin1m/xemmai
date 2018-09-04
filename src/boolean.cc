#include <xemmai/boolean.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_type_of<bool>::f_string(bool a_self)
{
	return a_self ? t_string::f_instantiate(L"true", 4) : t_string::f_instantiate(L"false", 5);
}

void t_type_of<bool>::f_define()
{
	t_define<bool, t_object>(f_global(), L"Boolean")
		(f_global()->f_symbol_string(), t_member<t_scoped(*)(bool), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(bool), f__hash>())
		(f_global()->f_symbol_not(), t_member<bool(*)(bool), f_not>())
		(f_global()->f_symbol_and(), t_member<bool(*)(bool, bool), f_and>())
		(f_global()->f_symbol_xor(), t_member<bool(*)(bool, bool), f_xor>())
		(f_global()->f_symbol_or(), t_member<bool(*)(bool, bool), f_or>())
	;
}

}
