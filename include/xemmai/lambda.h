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
	friend struct t_finalizes<t_lambda, t_bears<t_lambda>>;
	friend struct t_type_of<t_lambda>;
	friend struct t_module;

protected:
	t_slot* v_scope_entries;
	t_slot v_scope;
	t_slot v_code;
	size_t v_arguments;
#ifdef XEMMAI_ENABLE_JIT
	size_t (*v_jit_loop)(t_context*);
#endif
	size_t (t_lambda::*v_call)(t_stacked*);
	void** v_instructions;
	size_t v_size;
	size_t v_privates;
	size_t v_shareds;
	t_object* v_this;

	t_lambda(t_slot* a_scope, t_scoped&& a_code, t_object* a_this);
	~t_lambda() = default;
	template<typename T_context>
	size_t f_call_own(t_stacked* a_stack)
	{
		T_context context(this, a_stack);
		return t_code::f_loop(context);
	}
	template<typename T_context>
	size_t f_call_shared(t_stacked* a_stack)
	{
		auto p = new(v_shareds) t_scope(v_shareds, v_scope_entries);
		t_scoped scope = t_scope::f_instantiate(p);
		T_context context(this, a_stack);
		context.v_scope = p->f_entries();
		return t_code::f_loop(context);
	}

public:
	static t_scoped f_instantiate(t_slot* a_scope, t_scoped&& a_code);

	const t_slot& f_code() const
	{
		return v_code;
	}
};

template<>
struct t_type_of<t_lambda> : t_uninstantiatable<t_underivable<t_with_traits<t_holds<t_lambda>, false, true>>>
{
	using t_base::t_base;
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	virtual size_t f_get_at(t_object* a_this, t_stacked* a_stack);
};

class t_advanced_lambda : public t_lambda
{
	friend struct t_finalizes<t_advanced_lambda, t_bears<t_advanced_lambda, t_type_of<t_lambda>>>;
	friend struct t_type_of<t_advanced_lambda>;

	t_slot v_defaults;
	bool v_variadic;
	size_t v_minimum;

	t_advanced_lambda(t_slot* a_scope, t_scoped&& a_code, t_object* a_this, t_scoped&& a_defaults) : t_lambda(a_scope, std::move(a_code), a_this), v_defaults(std::move(a_defaults))
	{
		auto& code = f_as<t_code&>(v_code);
		v_variadic = code.v_variadic;
		v_minimum = code.v_minimum;
	}
	~t_advanced_lambda() = default;

public:
	static t_scoped f_instantiate(t_slot* a_scope, t_scoped&& a_code, t_stacked* a_stack);
};

template<>
struct t_type_of<t_advanced_lambda> : t_underivable<t_holds<t_advanced_lambda, t_type_of<t_lambda>>>
{
	using t_base::t_base;
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
};

}

#endif
