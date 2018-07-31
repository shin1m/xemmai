#ifndef XEMMAI__STRUCTURE_H
#define XEMMAI__STRUCTURE_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_structure> : t_underivable<t_type>
{
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_base(a_ids, a_super)
	{
		auto type = static_cast<t_object*>(static_cast<t_slot&>(v_super))->v_type;
		t_value::f_increments()->f_push(type->v_this);
		static_cast<t_object*>(v_this)->v_type = type;
		v_revive = v_shared = true;
		v_instantiate = &t_type::f_dont_instantiate;
	}
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
};

}

#endif
