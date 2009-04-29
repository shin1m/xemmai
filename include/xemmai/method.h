#ifndef XEMMAI__METHOD_H
#define XEMMAI__METHOD_H

#include "object.h"

namespace xemmai
{

class t_method
{
	friend struct t_type_of<t_method>;

	t_slot v_function;
	t_slot v_self;

	t_method(const t_transfer& a_function, const t_transfer& a_self) : v_function(a_function), v_self(a_self)
	{
	}
	~t_method()
	{
	}

public:
	static t_transfer f_instantiate(const t_transfer& a_function, const t_transfer& a_self);
	static void f_define(t_object* a_class);

	t_transfer f_bind(const t_transfer& a_target) const
	{
		return f_instantiate(v_function, a_target);
	}
};

template<>
struct t_type_of<t_method> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack);
	virtual void f_get_at(t_object* a_this, t_stack& a_stack);
};

}

#endif
