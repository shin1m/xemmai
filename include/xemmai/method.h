#ifndef XEMMAI__METHOD_H
#define XEMMAI__METHOD_H

#include "object.h"

namespace xemmai
{

class t_method
{
	friend struct t_finalizes<t_bears<t_method, t_type_immutable>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_method>;

	t_slot v_function;
	t_slot v_self;

	t_method(t_scoped&& a_function, t_scoped&& a_self) : v_function(std::move(a_function)), v_self(std::move(a_self))
	{
	}
	~t_method() = default;

public:
	static t_scoped f_instantiate(t_type* a_type, t_scoped&& a_function, t_scoped&& a_self)
	{
		return a_type->f_new<t_method>(true, std::move(a_function), std::move(a_self));
	}
	template<typename T>
	static t_scoped f_bind(t_scoped&& a_value, T&& a_target);

	const t_slot& f_function() const
	{
		return v_function;
	}
};

template<>
struct t_type_of<t_method> : t_uninstantiatable<t_underivable<t_holds<t_method, t_type_immutable>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		auto& p = a_this->f_as<t_method>();
		a_scan(p.v_function);
		a_scan(p.v_self);
	}
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
	{
		auto& p = a_this->f_as<t_method>();
		a_stack[1].f_construct(p.v_self);
		return static_cast<t_object*>(p.v_function)->f_call_without_loop(a_stack, a_n);
	}
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack)
	{
		t_scoped a0 = std::move(a_stack[2]);
		a_stack[0].f_construct(t_method::f_instantiate(a_this->f_type(), t_scoped(a_this->f_as<t_method>().v_function), std::move(a0)));
		return -1;
	}
};

}

#endif
