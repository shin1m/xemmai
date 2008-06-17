#include <xemmai/boolean.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_transfer t_type_of<bool>::f_define()
{
	return t_define<bool, t_object>(f_global(), L"Boolean")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(bool), f_string>())
		(f_global()->f_symbol_not(), t_member<bool (*)(bool), f_not>())
		(f_global()->f_symbol_and(), t_member<bool (*)(bool, bool), f_and>())
		(f_global()->f_symbol_xor(), t_member<bool (*)(bool, bool), f_xor>())
		(f_global()->f_symbol_or(), t_member<bool (*)(bool, bool), f_or>())
	;
}

t_type* t_type_of<bool>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<bool>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<bool>::f_not(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(!f_as<bool>(a_this)));
	context.f_done();
}

void t_type_of<bool>::f_and(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<bool>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<bool>(a_this) & f_as<bool>(a0)));
	context.f_done();
}

void t_type_of<bool>::f_xor(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<bool>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<bool>(a_this) ^ f_as<bool>(a0)));
	context.f_done();
}

void t_type_of<bool>::f_or(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<bool>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<bool>(a_this) | f_as<bool>(a0)));
	context.f_done();
}

}
