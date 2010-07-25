#include <xemmai/null.h>

#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<t_null>::f_define()
{
	t_define<t_null, t_object>(f_global(), L"Null")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
	;
}

t_type* t_type_of<t_null>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_null>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
