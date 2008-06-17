#include <xemmai/scope.h>

#include <xemmai/throwable.h>
#include <xemmai/global.h>

namespace xemmai
{

t_transfer t_scope::f_instantiate(size_t a_size, const t_transfer& a_outer, const t_transfer& a_self)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_scope>());
	object->v_pointer = new(a_size) t_scope(a_outer, a_self);
	return object;
}

t_type* t_type_of<t_scope>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_scope>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_scope*>(a_this)->f_scan(a_scan);
}

void t_type_of<t_scope>::f_finalize(t_object* a_this)
{
	delete f_as<t_scope*>(a_this);
}

void t_type_of<t_scope>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
