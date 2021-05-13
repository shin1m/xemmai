#include <xemmai/object.h>

namespace xemmai
{

void t_sharable::f_own()
{
	t_scoped_lock_for_write lock(v_lock);
	if (v_owner) f_throw(L"already owned."sv);
	v_owner = t_slot::t_increments::v_instance;
}

void t_sharable::f_share()
{
	if (v_owner != t_slot::t_increments::v_instance) f_throw(L"not owned."sv);
	t_scoped_lock_for_write lock(v_lock);
	v_owner = nullptr;
}

}
