#ifndef XEMMAI__METHOD_H
#define XEMMAI__METHOD_H

#include "object.h"

namespace xemmai
{

class t_method
{
	friend struct t_finalizes<t_bears<t_method>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_method>;

	t_slot v_function;
	t_svalue v_self;

	t_method(t_object* a_function, const t_pvalue& a_self) : v_function(a_function), v_self(a_self)
	{
	}
	~t_method() = default;
};

template<>
struct t_type_of<t_method> : t_uninstantiatable<t_holds<t_method>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		auto& p = a_this->f_as<t_method>();
		a_scan(p.v_function);
		a_scan(p.v_self);
	}
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
	{
		auto& p = a_this->f_as<t_method>();
		a_stack[1] = p.v_self;
		return p.v_function->f_call_without_loop(a_stack, a_n);
	}
};

}

#endif
