#include <xemmai/method.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_method::f_instantiate(t_scoped&& a_function, t_scoped&& a_self)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_method>(), true);
	object.f_pointer__(new t_method(std::move(a_function), std::move(a_self)));
	return object;
}

void t_type_of<t_method>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	auto& p = f_as<t_method&>(a_this);
	a_scan(p.v_function);
	a_scan(p.v_self);
}

size_t t_type_of<t_method>::f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	auto& p = f_as<t_method&>(a_this);
	a_stack[1].f_construct(p.v_self);
	return static_cast<t_object*>(p.v_function)->f_call_without_loop(a_stack, a_n);
}

size_t t_type_of<t_method>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(f_as<t_method&>(a_this).f_bind(std::move(a0)));
	return -1;
}

}
