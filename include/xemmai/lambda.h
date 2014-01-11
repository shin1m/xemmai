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

	t_lambda(t_scoped&& a_scope, t_scoped&& a_code) : v_scope(std::move(a_scope)), v_code(std::move(a_code))
	{
	}
	~t_lambda() = default;

public:
	static t_scoped f_instantiate(t_scoped&& a_scope, t_scoped&& a_code);
};

template<>
struct t_type_of<t_lambda> : t_type
{
	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_call(t_object* a_this, t_slot* a_stack, size_t a_n);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
};

class t_advanced_lambda : public t_lambda
{
	friend struct t_type_of<t_advanced_lambda>;

	t_slot v_defaults;

	t_advanced_lambda(t_scoped&& a_scope, t_scoped&& a_code, t_scoped&& a_defaults) : t_lambda(std::move(a_scope), std::move(a_code)), v_defaults(std::move(a_defaults))
	{
	}
	~t_advanced_lambda() = default;

public:
	static t_scoped f_instantiate(t_scoped&& a_scope, t_scoped&& a_code, t_slot* a_stack);
};

template<>
struct t_type_of<t_advanced_lambda> : t_type_of<t_lambda>
{
	using t_type_of<t_lambda>::t_type_of;
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_call(t_object* a_this, t_slot* a_stack, size_t a_n);
};

}

#endif
