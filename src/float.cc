#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_type_of<double>::f_string(double a_self)
{
	wchar_t cs[32];
	size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%g", a_self);
	return t_string::f_instantiate(cs, n);
}

bool t_type_of<double>::f__equals(double a_self, const t_pvalue& a_value)
{
	return f_is<double>(a_value) && a_self == f_as<double>(a_value);
}

bool t_type_of<double>::f__not_equals(double a_self, const t_pvalue& a_value)
{
	return !f_is<double>(a_value) || a_self != f_as<double>(a_value);
}

void t_type_of<double>::f_define()
{
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(*)(double), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(double), f__hash>())
		(f_global()->f_symbol_plus(), t_member<double(*)(double), f__plus>())
		(f_global()->f_symbol_minus(), t_member<double(*)(double), f__minus>())
		(f_global()->f_symbol_multiply(), t_member<double(*)(double, double), f__multiply>())
		(f_global()->f_symbol_divide(), t_member<double(*)(double, double), f__divide>())
		(f_global()->f_symbol_add(), t_member<double(*)(double, double), f__add>())
		(f_global()->f_symbol_subtract(), t_member<double(*)(double, double), f__subtract>())
		(f_global()->f_symbol_less(), t_member<bool(*)(double, double), f__less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(*)(double, double), f__less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(*)(double, double), f__greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(*)(double, double), f__greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(double, const t_pvalue&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(double, const t_pvalue&), f__not_equals>())
	.f_derive<double, t_object>();
}

t_pvalue t_type_of<double>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, double), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, intptr_t), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, const t_string&), f_construct>
	>::t_bind<double>::f_do(this, a_stack, a_n);
}

}
