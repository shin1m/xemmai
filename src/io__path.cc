#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

void t_type_of<portable::t_path>::f_define(t_io* a_library)
{
	t_define{a_library}
		(f_global()->f_symbol_string(), t_member<const std::wstring&(portable::t_path::*)() const, &portable::t_path::operator const std::wstring&>())
		(f_global()->f_symbol_divide(), t_member<portable::t_path(*)(const portable::t_path&, const t_string&), f__divide>())
	.f_derive<portable::t_path, t_object>();
}

t_pvalue t_type_of<portable::t_path>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, const t_string&), f_construct>::t_bind<portable::t_path>::f_do(this, a_stack, a_n);
}

size_t t_type_of<portable::t_path>::f_do_divide(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_type()->v_module->f_as<t_io>().f_as(a_this->f_as<portable::t_path>() / f_as<std::wstring>(a_stack[2]));
	return -1;
}

}
