#ifndef XEMMAI__NATIVE_H
#define XEMMAI__NATIVE_H

#include "object.h"

namespace xemmai
{

struct t_native
{
	typedef void (*t_function)(t_object*, t_object*, size_t, t_stack&);

	XEMMAI__PORTABLE__EXPORT static t_transfer f_instantiate(const t_transfer& a_module, t_function a_function);

	t_slot v_module;
	t_function v_function;

	t_native(const t_transfer& a_module, t_function a_function) : v_module(a_module), v_function(a_function)
	{
	}
};

template<>
struct t_type_of<t_native> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack);
};

}

#endif
