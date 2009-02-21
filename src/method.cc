#include <xemmai/method.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_transfer t_method::f_instantiate(const t_transfer& a_function, const t_transfer& a_self)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_method>());
	object->v_pointer = new t_method(a_function, a_self);
	return object;
}

void t_method::f_define(t_object* a_class)
{
	t_define<t_method, t_object>(f_global(), L"Method", a_class)
		(f_global()->f_symbol_get_at(), t_member<t_transfer (t_method::*)(const t_transfer&) const, &t_method::f_bind>())
	;
}

t_type* t_type_of<t_method>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_method>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_method* p = f_as<t_method*>(a_this);
	a_scan(p->v_function);
	a_scan(p->v_self);
}

void t_type_of<t_method>::f_finalize(t_object* a_this)
{
	delete f_as<t_method*>(a_this);
}

void t_type_of<t_method>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_method>::f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	t_method* p = f_as<t_method*>(a_this);
	p->v_function->f_call(p->v_self, a_n, a_stack);
}

void t_type_of<t_method>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_as<t_method*>(a_this)->f_bind(a0));
	context.f_done();
}

}
