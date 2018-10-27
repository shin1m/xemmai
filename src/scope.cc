#include <xemmai/scope.h>

namespace xemmai
{

void t_type_of<t_scope>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_scope&>(a_this).f_scan(a_scan);
}

}
