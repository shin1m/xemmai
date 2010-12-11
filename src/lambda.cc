#include <xemmai/lambda.h>

#include <xemmai/method.h>
#include <xemmai/global.h>

namespace xemmai
{

t_transfer t_lambda::f_instantiate(const t_transfer& a_scope, const t_transfer& a_code)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_lambda>());
	object.f_pointer__(new t_lambda(a_scope, a_code));
	return object;
}

t_type* t_type_of<t_lambda>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_lambda& p = f_as<t_lambda&>(a_this);
	a_scan(p.v_scope);
	a_scan(p.v_code);
}

void t_type_of<t_lambda>::f_finalize(t_object* a_this)
{
	delete &f_as<t_lambda&>(a_this);
}

void t_type_of<t_lambda>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_lambda>::f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	t_lambda& p = f_as<t_lambda&>(a_this);
	t_code& code = f_as<t_code&>(p.v_code);
	if (a_n != code.v_arguments) t_throwable::f_throw(L"invalid number of arguments.");
	t_fiber::t_context::f_push(p.v_code, p.v_scope, a_self, a_stack, a_n);
}

void t_type_of<t_lambda>::f_get_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(t_method::f_instantiate(a_this, a0));
	context.f_done();
}

}
