#include <xemmai/io/path.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>
#include <xemmai/io.h>

namespace xemmai
{

void t_type_of<portable::t_path>::f_define(t_io* a_extension)
{
	t_define<portable::t_path, t_object>(a_extension, L"Path")
		(t_construct<const std::wstring&>())
		(f_global()->f_symbol_string(), t_member<const std::wstring& (portable::t_path::*)() const, &portable::t_path::operator const std::wstring&>())
		(f_global()->f_symbol_divide(), t_member<portable::t_path (portable::t_path::*)(const std::wstring&) const, &portable::t_path::operator/>())
	;
}

t_type* t_type_of<portable::t_path>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

void t_type_of<portable::t_path>::f_finalize(t_object* a_this)
{
	delete &f_as<portable::t_path&>(a_this);
}

t_scoped t_type_of<portable::t_path>::f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	return t_construct<const std::wstring&>::t_bind<portable::t_path>::f_do(a_class, a_stack, a_n);
}

size_t t_type_of<portable::t_path>::f_divide(t_object* a_this, t_scoped* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(f_extension<t_io>(v_module)->f_as(f_as<const portable::t_path&>(a_this) / f_as<const std::wstring&>(a0)));
	return -1;
}

}
