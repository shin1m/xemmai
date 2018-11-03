#ifndef XEMMAI__STRUCTURE_H
#define XEMMAI__STRUCTURE_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_structure::t_discard> : t_uninstantiatable<t_underivable<t_finalizes<t_derives<t_structure::t_discard>>>>
{
	using t_base::t_base;
};

template<>
struct t_type_of<t_structure> : t_uninstantiatable<t_underivable<t_derives<t_structure>>>
{
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_base(a_ids, a_super, {})
	{
		auto type = static_cast<t_slot&>(v_super)->v_type;
		t_value::f_increments()->f_push(t_object::f_of(type));
		t_object::f_of(this)->v_type = type;
		v_revive = true;
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_structure>().f_scan(a_scan);
	}
	static void f_do_finalize(t_object* a_this);
};

}

#endif
