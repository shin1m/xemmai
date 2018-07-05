#ifndef XEMMAI__LAMBDA_H
#define XEMMAI__LAMBDA_H

#include "code.h"

namespace xemmai
{

struct t_fiber;

class t_lambda
{
	friend struct t_code;
	friend struct t_context;
	friend struct t_type_of<t_lambda>;
	friend struct t_module;

protected:
	t_slot v_scope;
	t_slot v_code;
	t_scope& v_as_scope;
#ifdef XEMMAI_ENABLE_JIT
	size_t (*v_jit_loop)(t_context*);
#endif
	bool v_shared;
	size_t v_size;
	size_t v_privates;
	size_t v_shareds;
	size_t v_arguments;
	void** v_instructions;

	t_lambda(t_scoped&& a_scope, t_scoped&& a_code) : v_scope(std::move(a_scope)), v_code(std::move(a_code)), v_as_scope(f_as<t_scope&>(v_scope))
	{
		auto& code = f_as<t_code&>(v_code);
#ifdef XEMMAI_ENABLE_JIT
		v_jit_loop = code.v_jit_loop;
#endif
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

	const t_slot& f_code() const
	{
		return v_code;
	}
};

template<>
struct t_type_of<t_lambda> : t_with_traits<t_with_ids<t_lambda>, false, true>
{
	using t_with_traits<t_with_ids<t_lambda>, false, true>::t_with_traits;
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	virtual size_t f_get_at(t_object* a_this, t_stacked* a_stack);
};

class t_advanced_lambda : public t_lambda
{
	friend struct t_type_of<t_advanced_lambda>;

	t_slot v_defaults;
	bool v_variadic;
	size_t v_minimum;

	t_advanced_lambda(t_scoped&& a_scope, t_scoped&& a_code, t_scoped&& a_defaults) : t_lambda(std::move(a_scope), std::move(a_code)), v_defaults(std::move(a_defaults))
	{
		auto& code = f_as<t_code&>(v_code);
		v_variadic = code.v_variadic;
		v_minimum = code.v_minimum;
	}
	~t_advanced_lambda() = default;

public:
	static t_scoped f_instantiate(t_scoped&& a_scope, t_scoped&& a_code, t_stacked* a_stack);
};

template<>
struct t_type_of<t_advanced_lambda> : t_with_ids<t_advanced_lambda, t_type_of<t_lambda>>
{
	using t_with_ids<t_advanced_lambda, t_type_of<t_lambda>>::t_with_ids;
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
};

}

#endif
