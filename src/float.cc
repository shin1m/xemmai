#include <xemmai/float.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

bool t_type_of<double>::f_equals(double a_self, const t_value& a_value)
{
	return f_is<double>(a_value) && a_self == f_as<double>(a_value);
}

bool t_type_of<double>::f_not_equals(double a_self, const t_value& a_value)
{
	return !f_is<double>(a_value) || a_self != f_as<double>(a_value);
}

void t_type_of<double>::f_define()
{
	t_define<double, t_object>(f_global(), L"Float")
		(t_construct_with<t_scoped(*)(t_object*, double), f_construct_derived>())
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(double), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(double), f_hash>())
		(f_global()->f_symbol_plus(), t_member<double(*)(double), f_plus>())
		(f_global()->f_symbol_minus(), t_member<double(*)(double), f_minus>())
		(f_global()->f_symbol_multiply(), t_member<double(*)(double, double), f_multiply>())
		(f_global()->f_symbol_divide(), t_member<double(*)(double, double), f_divide>())
		(f_global()->f_symbol_add(), t_member<double(*)(double, double), f_add>())
		(f_global()->f_symbol_subtract(), t_member<double(*)(double, double), f_subtract>())
		(f_global()->f_symbol_less(), t_member<bool(*)(double, double), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(*)(double, double), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(*)(double, double), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(*)(double, double), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(double, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(double, const t_value&), f_not_equals>())
	;
}

t_type* t_type_of<double>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

t_scoped t_type_of<double>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped(*)(t_object*, double), f_construct>,
		t_construct_with<t_scoped(*)(t_object*, intptr_t), f_construct>,
		t_construct_with<t_scoped(*)(t_object*, const std::wstring&), f_construct>
	>::t_bind<double>::f_do(a_class, a_stack, a_n);
}

}
