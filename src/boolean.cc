#include <xemmai/boolean.h>

#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<bool>::f_define()
{
	t_define<bool, t_object>(f_global(), L"Boolean")
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(bool), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(bool), f__hash>())
		(f_global()->f_symbol_not(), t_member<bool(*)(bool), f_not>())
		(f_global()->f_symbol_and(), t_member<bool(*)(bool, bool), f_and>())
		(f_global()->f_symbol_xor(), t_member<bool(*)(bool, bool), f_xor>())
		(f_global()->f_symbol_or(), t_member<bool(*)(bool, bool), f_or>())
	;
}

}
