#ifndef XEMMAI__LAMBDA_H
#define XEMMAI__LAMBDA_H

#include "code.h"
#include "tuple.h"

namespace xemmai
{

struct t_fiber;

class t_lambda
{
	friend struct t_code;
	friend struct t_context;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_lambda>;

protected:
	t_slot* v_scope_entries;
	t_slot v_scope;
	t_slot v_code;
	size_t v_arguments;
#ifdef XEMMAI_ENABLE_JIT
	size_t (*v_jit_loop)(t_context*);
#endif
	void** v_instructions;
	size_t v_size;
	size_t v_privates;
	t_object* v_this;

	t_lambda(t_slot* a_scope, t_scoped&& a_code, t_object* a_this) : v_scope_entries(a_scope), v_scope(t_scope::f_this(a_scope)), v_code(std::move(a_code)), v_this(a_this)
	{
		auto& code = f_as<t_code&>(v_code);
		v_size = code.v_size;
		v_arguments = code.v_arguments;
		v_privates = code.v_privates;
		v_instructions = &code.v_instructions[0];
#ifdef XEMMAI_ENABLE_JIT
		v_jit_loop = code.v_jit_loop;
#endif
	}
	~t_lambda() = default;
	template<typename T_context>
	size_t f_call(t_stacked* a_stack)
	{
		T_context context(this, a_stack);
		return t_code::f_loop(context);
	}

public:
	static t_scoped f_instantiate(t_slot* a_scope, t_scoped&& a_code);
	static t_scoped f_instantiate(t_slot* a_scope, t_scoped&& a_code, t_stacked* a_stack);

	const t_slot& f_code() const
	{
		return v_code;
	}
};

template<>
struct t_type_of<t_lambda> : t_override<t_uninstantiatable<t_underivable<t_with_traits<t_holds<t_lambda>, false, true>>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	template<typename T, typename T_context>
	static size_t f__do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
	{
		auto& p = f_as<T&>(a_this);
		if (a_n != p.v_arguments) f_throw(a_stack, a_n, L"invalid number of arguments.");
		return p.template f_call<T_context>(a_stack);
	}
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
};

class t_lambda_shared : public t_lambda
{
	friend struct t_lambda;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_lambda>;
	friend struct t_type_of<t_lambda_shared>;

	size_t v_shareds;

protected:
	t_lambda_shared(t_slot* a_scope, t_scoped&& a_code, t_object* a_this) : t_lambda(a_scope, std::move(a_code), a_this), v_shareds(f_as<t_code&>(v_code).v_shareds)
	{
	}
	~t_lambda_shared() = default;
	template<typename T_context>
	size_t f_call(t_stacked* a_stack)
	{
		auto p = new(v_shareds) t_scope(v_shareds, v_scope_entries);
		t_scoped scope = t_scope::f_instantiate(p);
		T_context context(this, a_stack);
		context.v_scope = p->f_entries();
		return t_code::f_loop(context);
	}
};

template<>
struct t_type_of<t_lambda_shared> : t_holds<t_lambda_shared, t_type_of<t_lambda>>
{
	using t_base::t_base;
};

template<typename T_base>
class t_advanced_lambda : public T_base
{
	friend struct t_lambda;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_advanced_lambda>;

	t_slot v_defaults;
	bool v_variadic;
	size_t v_minimum;

	t_advanced_lambda(t_slot* a_scope, t_scoped&& a_code, t_object* a_this, t_scoped&& a_defaults) : T_base(a_scope, std::move(a_code), a_this), v_defaults(std::move(a_defaults))
	{
		auto& code = f_as<t_code&>(this->v_code);
		v_variadic = code.v_variadic;
		v_minimum = code.v_minimum;
	}
	~t_advanced_lambda() = default;
};

template<typename T_base>
struct t_type_of<t_advanced_lambda<T_base>> : t_override<t_holds<t_advanced_lambda<T_base>, t_type_of<T_base>>>
{
	using t_type_of::t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		t_type_of::t_base::f_do_scan(a_this, a_scan);
		a_scan(f_as<t_advanced_lambda<T_base>&>(a_this).v_defaults);
	}
	template<typename T_context>
	static size_t f__do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
	{
		auto& p = f_as<t_advanced_lambda<T_base>&>(a_this);
		if (a_n < p.v_minimum) f_throw(a_stack, a_n, L"too few arguments.");
		size_t arguments = p.v_arguments;
		if (p.v_variadic)
			--arguments;
		else if (a_n > arguments)
			f_throw(a_stack, a_n, L"too many arguments.");
		if (a_n < arguments) {
			auto& t0 = f_as<const t_tuple&>(p.v_defaults);
			t_stacked* t1 = a_stack + p.v_minimum + 2;
			for (size_t i = a_n - p.v_minimum; i < t0.f_size(); ++i) t1[i].f_construct(t0[i]);
			if (p.v_variadic) a_stack[p.v_arguments + 1].f_construct(t_tuple::f_instantiate(0));
		} else if (p.v_variadic) {
			size_t n = a_n - arguments;
			t_scoped x = t_tuple::f_instantiate(n);
			t_stacked* t0 = a_stack + arguments + 2;
			auto& t1 = f_as<t_tuple&>(x);
			for (size_t i = 0; i < n; ++i) t1[i].f_construct(std::move(t0[i]));
			t0[0].f_construct(std::move(x));
		}
		return p.template f_call<T_context>(a_stack);
	}
};

}

#endif
