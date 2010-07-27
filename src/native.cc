#include <xemmai/native.h>

#include <xemmai/method.h>
#include <xemmai/global.h>

namespace xemmai
{

t_transfer t_native::f_instantiate(const t_transfer& a_module, t_function a_function)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_native>());
	object.f_pointer__(new t_native(a_module, a_function));
	return object;
}

t_type* t_type_of<t_native>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_native>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_native&>(a_this).v_module);
}

void t_type_of<t_native>::f_finalize(t_object* a_this)
{
	delete &f_as<t_native&>(a_this);
}

void t_type_of<t_native>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_native>::f_call(t_object* a_this, const t_value& a_self, size_t a_n, t_stack& a_stack)
{
	t_native_context context;
	t_native& p = f_as<t_native&>(a_this);
	p.v_function(p.v_module.f_object(), a_self, a_n, a_stack);
	context.f_done();
}

void t_type_of<t_native>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(t_method::f_instantiate(a_this, a0));
	context.f_done();
}

}
