#include <xemmai/scope.h>

#include <xemmai/global.h>

namespace xemmai
{

t_scoped t_scope::f_instantiate(size_t a_size, t_scoped&& a_outer)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_scope>());
	object.f_pointer__(new(a_size) t_scope(a_size, std::move(a_outer)));
	return object;
}

void t_type_of<t_scope>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_scope&>(a_this).f_scan(a_scan);
}

}
