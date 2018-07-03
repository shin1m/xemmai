#include <xemmai/null.h>

#include <xemmai/convert.h>

namespace xemmai
{

constexpr decltype(t_type_of<std::nullptr_t>::V_ids) t_type_of<std::nullptr_t>::V_ids;

void t_type_of<std::nullptr_t>::f_define()
{
	t_define<std::nullptr_t, t_object>(f_global(), L"Null")
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(const t_value&), f_string>())
	;
}

t_type* t_type_of<std::nullptr_t>::f_derive()
{
	return nullptr;
}

void t_type_of<std::nullptr_t>::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(a_stack, a_n, L"uninstantiatable.");
}

}
