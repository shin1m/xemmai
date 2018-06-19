#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_type> : t_type
{
	t_type_of(t_type* a_super);
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
	virtual void f_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack);
	virtual t_scoped f_get(t_object* a_this, t_object* a_key);
	virtual void f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	virtual t_scoped f_remove(t_object* a_this, t_object* a_key);
	virtual void f_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	virtual size_t f_send(t_object* a_this, t_stacked* a_stack);
};

typedef t_type_of<t_type> t_class;

}

#endif
