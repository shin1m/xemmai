#ifndef XEMMAI__LAMBDA_H
#define XEMMAI__LAMBDA_H

#include "object.h"

namespace xemmai
{

class t_lambda
{
	friend struct t_type_of<t_lambda>;

protected:
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

class t_advanced_lambda : public t_lambda
{
	friend struct t_type_of<t_advanced_lambda>;

	t_slot v_defaults;

	t_advanced_lambda(const t_transfer& a_scope, const t_transfer& a_code, const t_transfer& a_defaults) : t_lambda(a_scope, a_code), v_defaults(a_defaults)
	{
	}

public:
	static t_transfer f_instantiate(const t_transfer& a_scope, const t_transfer& a_code, t_slot* a_stack);
};

template<>
struct t_type_of<t_advanced_lambda> : t_type_of<t_lambda>
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<t_lambda>(a_module, a_super)
	{
	}
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n);
};

}

#endif
