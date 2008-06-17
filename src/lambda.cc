#include <xemmai/lambda.h>

#include <xemmai/code.h>
#include <xemmai/global.h>

namespace xemmai
{

t_transfer t_lambda::f_instantiate(const t_transfer& a_scope, const t_transfer& a_code)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_lambda>());
	object->v_pointer = new t_lambda(a_scope, a_code);
	return object;
}

t_type* t_type_of<t_lambda>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_lambda* p = f_as<t_lambda*>(a_this);
	a_scan(p->v_scope);
	a_scan(p->v_code);
}

void t_type_of<t_lambda>::f_finalize(t_object* a_this)
{
	delete f_as<t_lambda*>(a_this);
}

void t_type_of<t_lambda>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_lambda>::f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	t_lambda* p = f_as<t_lambda*>(a_this);
	f_as<t_code*>(p->v_code)->f_call(p->v_code, p->v_scope, a_self, a_n, a_stack);
}

}
