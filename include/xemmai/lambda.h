#ifndef XEMMAI__LAMBDA_H
#define XEMMAI__LAMBDA_H

#include "object.h"

namespace xemmai
{

struct t_type_of_variadic_lambda;

class t_lambda
{
	friend struct t_type_of<t_lambda>;
	friend struct t_type_of_variadic_lambda;

	t_slot v_scope;
	t_slot v_code;

	t_lambda(const t_transfer& a_scope, const t_transfer& a_code) : v_scope(a_scope), v_code(a_code)
	{
	}
	~t_lambda()
	{
	}

public:
	static t_transfer f_instantiate(const t_transfer& a_scope, const t_transfer& a_code);
};

template<>
struct t_type_of<t_lambda> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
};

struct t_type_of_variadic_lambda : t_type_of<t_lambda>
{
	t_type_of_variadic_lambda(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<t_lambda>(a_module, a_super)
	{
	}
	virtual void f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n);
};

}

#endif
