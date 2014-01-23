#ifndef XEMMAI__LAMBDA_H
#define XEMMAI__LAMBDA_H

#include "code.h"

namespace xemmai
{

struct t_code;
struct t_fiber;

class t_lambda
{
	friend struct t_code;
	friend struct t_fiber;
	friend struct t_type_of<t_lambda>;

protected:
	t_slot v_scope;
	t_slot v_code;
	t_scope& v_as_scope;
	bool v_shared;
	size_t v_size;
	size_t v_privates;
	size_t v_shareds;
	size_t v_arguments;
	void** v_instructions;

	t_lambda(t_scoped&& a_scope, t_scoped&& a_code) : v_scope(std::move(a_scope)), v_code(std::move(a_code)), v_as_scope(f_as<t_scope&>(v_scope))
	{
		t_code& code = f_as<t_code&>(v_code);
		v_shared = code.v_shared;
		v_size = code.v_size;
		v_privates = code.v_privates;
		v_shareds = code.v_shareds;
		v_arguments = code.v_arguments;
		v_instructions = &code.v_instructions[0];
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
	bool v_variadic;
	size_t v_minimum;

	t_advanced_lambda(t_scoped&& a_scope, t_scoped&& a_code, t_scoped&& a_defaults) : t_lambda(std::move(a_scope), std::move(a_code)), v_defaults(std::move(a_defaults))
	{
		t_code& code = f_as<t_code&>(v_code);
		v_variadic = code.v_variadic;
		v_minimum = code.v_minimum;
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
