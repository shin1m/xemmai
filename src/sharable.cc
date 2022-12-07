#include <xemmai/convert.h>

namespace xemmai
{

void t_sharable::f_own()
{
	t_lock_with_safe_region lock(v_mutex);
	if (v_owner) f_throw(L"already owned."sv);
	v_owner = t_slot::t_increments::v_instance;
}

void t_sharable::f_share()
{
	if (v_owner != t_slot::t_increments::v_instance) f_throw(L"not owned."sv);
	t_lock_with_safe_region lock(v_mutex);
	v_owner = nullptr;
}

void t_type_of<t_sharable>::f_define()
{
	t_define{f_global()}
		(L"own"sv, t_member<void(t_sharable::*)(), &t_sharable::f_own>())
		(L"share"sv, t_member<void(t_sharable::*)(), &t_sharable::f_share>())
	.f_derive<t_sharable, t_object>();
}

}
