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

void t_type_of<t_native>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_native>::f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	t_native_context context;
	t_native& p = f_as<t_native&>(a_this);
	p.v_function(p.v_module, a_self, a_stack, a_n);
	context.f_done();
}

void t_type_of<t_native>::f_get_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(t_method::f_instantiate(a_this, a0));
	context.f_done();
}

}
