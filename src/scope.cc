#include <xemmai/scope.h>

#include <xemmai/global.h>

namespace xemmai
{

t_scoped t_scope::f_instantiate(t_scope* a_scope)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_scope>());
	a_scope->v_this = object;
	object.f_pointer__(a_scope);
	return object;
}

void t_type_of<t_scope>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_scope&>(a_this).f_scan(a_scan);
}

}
