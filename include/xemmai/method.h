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

	t_method(t_scoped&& a_function, t_scoped&& a_self) : v_function(std::move(a_function)), v_self(std::move(a_self))
	{
	}
	~t_method() = default;

public:
	static t_scoped f_instantiate(t_scoped&& a_function, t_scoped&& a_self);

	const t_slot& f_function() const
	{
		return v_function;
	}
	t_scoped f_bind(t_scoped&& a_target) const
	{
		return f_instantiate(t_scoped(v_function), std::move(a_target));
	}
};

template<>
struct t_type_of<t_method> : t_type
{
	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_fixed = v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_scoped* a_stack, size_t a_n);
	virtual size_t f_get_at(t_object* a_this, t_scoped* a_stack);
};

}

#endif
