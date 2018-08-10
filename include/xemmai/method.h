#ifndef XEMMAI__METHOD_H
#define XEMMAI__METHOD_H

#include "object.h"

namespace xemmai
{

class t_method
{
	friend struct t_finalizes<t_bears<t_method, t_type_immutable>>;
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
struct t_type_of<t_method> : t_uninstantiatable<t_underivable<t_holds<t_method, t_type_immutable>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
};

}

#endif
