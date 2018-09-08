#include <xemmai/io/path.h>

#include <xemmai/convert.h>
#include <xemmai/io.h>

namespace xemmai
{

void t_type_of<portable::t_path>::f_define(t_io* a_extension)
{
	t_define<portable::t_path, t_object>(a_extension, L"Path"sv)
		(t_construct_with<t_scoped(*)(t_type*, const t_string&), f_construct>())
		(f_global()->f_symbol_string(), t_member<const std::wstring&(portable::t_path::*)() const, &portable::t_path::operator const std::wstring&>())
		(f_global()->f_symbol_divide(), t_member<portable::t_path(*)(const portable::t_path&, const t_string&), f__divide>())
	;
}

t_scoped t_type_of<portable::t_path>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*, const t_string&), f_construct>::t_bind<portable::t_path>::f_do(this, a_stack, a_n);
}

size_t t_type_of<portable::t_path>::f_do_divide(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<t_string>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_extension<t_io>(a_this->f_type()->v_module)->f_as(f_as<const portable::t_path&>(a_this) / f_as<std::wstring>(a0.v_p)));
	return -1;
}

}
