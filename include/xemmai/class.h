#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_type> : t_type
{
	XEMMAI__PORTABLE__EXPORT static t_transfer f_instantiate(t_type* a_type);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n);
	virtual t_transfer f_get(const t_value& a_this, t_object* a_key);
	virtual void f_put(t_object* a_this, t_object* a_key, const t_transfer& a_value);
	virtual t_transfer f_remove(t_object* a_this, t_object* a_key);
	virtual void f_call(t_object* a_this, const t_value& a_self, size_t a_n);
	virtual void f_send(t_object* a_this);
};

typedef t_type_of<t_type> t_class;

}

#endif
