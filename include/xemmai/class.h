#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

struct t_class : t_type
{
	XEMMAI__PORTABLE__EXPORT static t_transfer f_instantiate(t_type* a_type);

	t_class(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual t_transfer f_get(t_object* a_this, t_object* a_key);
	virtual void f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack);
};

}

#endif
