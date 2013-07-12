#include <xemmai/null.h>

#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<std::nullptr_t>::f_define()
{
	t_define<std::nullptr_t, t_object>(f_global(), L"Null")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
	;
}

t_type* t_type_of<std::nullptr_t>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<std::nullptr_t>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
