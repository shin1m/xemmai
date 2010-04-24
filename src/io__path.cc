#include <xemmai/io/path.h>

#include <xemmai/engine.h>
#include <xemmai/convert.h>
#include <xemmai/io.h>

namespace xemmai
{

void t_type_of<portable::t_path>::f_define(t_io* a_extension)
{
	t_define<portable::t_path, t_object>(a_extension, L"Path")
		(f_global()->f_symbol_string(), t_member<const std::wstring& (portable::t_path::*)() const, &portable::t_path::operator const std::wstring&>())
		(f_global()->f_symbol_divide(), t_member<portable::t_path (portable::t_path::*)(const std::wstring&) const, &portable::t_path::operator/>())
	;
}

t_type* t_type_of<portable::t_path>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<portable::t_path>::f_finalize(t_object* a_this)
{
	delete f_as<portable::t_path*>(a_this);
}

void t_type_of<portable::t_path>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct<portable::t_path, const std::wstring&>::f_call(a_class, a_n, a_stack);
}

void t_type_of<portable::t_path>::f_divide(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<std::wstring>(a0, L"argument0");
	a_stack.f_return(f_extension<t_io>(v_module)->f_as(f_as<const portable::t_path&>(a_this) / f_as<const std::wstring&>(a0)));
	context.f_done();
}

}
