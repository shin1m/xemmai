#include <xemmai/method.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_method::f_instantiate(t_scoped&& a_function, t_scoped&& a_self)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_method>());
	object.f_pointer__(new t_method(std::move(a_function), std::move(a_self)));
	return object;
}

t_type* t_type_of<t_method>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_method>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_method& p = f_as<t_method&>(a_this);
	a_scan(p.v_function);
	a_scan(p.v_self);
}

void t_type_of<t_method>::f_finalize(t_object* a_this)
{
	delete &f_as<t_method&>(a_this);
}

void t_type_of<t_method>::f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

size_t t_type_of<t_method>::f_call(t_object* a_this, t_scoped* a_stack, size_t a_n)
{
	t_method& p = f_as<t_method&>(a_this);
	a_stack[0].f_construct(p.v_self);
	return static_cast<t_object*>(p.v_function)->f_call_without_loop(a_stack, a_n);
}

void t_type_of<t_method>::f_get_at(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[1]);
	a_stack[0].f_construct(f_as<t_method&>(a_this).f_bind(std::move(a0)));
	context.f_done();
}

}
