#ifndef XEMMAI__STRUCTURE_H
#define XEMMAI__STRUCTURE_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_structure> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_revive = v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
