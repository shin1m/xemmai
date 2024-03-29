#ifndef XEMMAI__LAMBDA_H
#define XEMMAI__LAMBDA_H

#include "scope.h"
#include "code.h"
#include "tuple.h"

namespace xemmai
{

class t_lambda
{
	friend struct t_context;
	friend struct t_code;
	friend struct t_finalizes<t_bears<t_lambda>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_lambda>;

protected:
	t_slot v_scope;
	t_slot v_code;
	size_t v_arguments;
	void** v_instructions;
	size_t v_size;
	size_t v_privates;

	t_lambda(t_object* a_scope, t_object* a_code) : v_scope(a_scope), v_code(a_code)
	{
		auto& code = v_code->f_as<t_code>();
		v_size = code.v_size;
		v_arguments = code.v_arguments;
		v_privates = code.v_privates;
		v_instructions = &code.v_instructions[0];
	}
	~t_lambda() = default;
	template<typename T_context>
	size_t f_call(t_pvalue* a_stack)
	{
		T_context context(t_object::f_of(this), a_stack);
		return t_code::f_loop(context);
	}

public:
	XEMMAI__LOCAL static t_object* f_instantiate(t_object* a_scope, t_object* a_code);
	XEMMAI__LOCAL static t_object* f_instantiate(t_object* a_scope, t_object* a_code, t_pvalue* a_stack);

	const t_slot& f_code() const
	{
		return v_code;
	}
};

template<>
struct t_type_of<t_lambda> : t_uninstantiatable<t_holds<t_lambda>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		auto& p = a_this->f_as<t_lambda>();
		a_scan(p.v_scope);
		a_scan(p.v_code);
	}
	template<typename T, typename T_context>
	static size_t f__do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
	{
		auto& p = a_this->f_as<T>();
		if (a_n != p.v_arguments) f_throw(L"invalid number of arguments."sv);
		return p.template f_call<T_context>(a_stack);
	}
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
};

class t_lambda_shared : public t_lambda
{
	friend struct t_finalizes<t_bears<t_lambda_shared, t_type_of<t_lambda>>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_lambda>;

	size_t v_shareds;

protected:
	t_lambda_shared(t_object* a_scope, t_object* a_code) : t_lambda(a_scope, a_code), v_shareds(v_code->f_as<t_code>().v_shareds)
	{
	}
	~t_lambda_shared() = default;
	template<typename T_context>
	size_t f_call(t_pvalue* a_stack);
};

template<>
struct t_type_of<t_lambda_shared> : t_holds<t_lambda_shared, t_type_of<t_lambda>>
{
	using t_base::t_base;
};

template<typename T_base>
class t_advanced_lambda : public T_base
{
	friend struct t_finalizes<t_bears<t_advanced_lambda, t_type_of<T_base>>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_advanced_lambda>;

	t_slot v_defaults;
	bool v_variadic;
	size_t v_minimum;

	t_advanced_lambda(t_object* a_scope, t_object* a_code, t_object* a_defaults) : T_base(a_scope, a_code), v_defaults(a_defaults)
	{
		auto& code = this->v_code->template f_as<t_code>();
		v_variadic = code.v_variadic;
		v_minimum = code.v_minimum;
	}
	~t_advanced_lambda() = default;
};

template<typename T_base>
struct t_type_of<t_advanced_lambda<T_base>> : t_holds<t_advanced_lambda<T_base>, t_type_of<T_base>>
{
	using t_holds<t_advanced_lambda<T_base>, t_type_of<T_base>>::t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		t_type_of::t_base::f_do_scan(a_this, a_scan);
		a_scan(a_this->f_as<t_advanced_lambda<T_base>>().v_defaults);
	}
	template<typename T_context>
	static size_t f__do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
	{
		auto& p = a_this->f_as<t_advanced_lambda<T_base>>();
		if (a_n < p.v_minimum) f_throw(L"too few arguments."sv);
		size_t arguments = p.v_arguments;
		if (p.v_variadic)
			--arguments;
		else if (a_n > arguments)
			f_throw(L"too many arguments."sv);
		if (a_n < arguments) {
			auto& t0 = p.v_defaults->template f_as<t_tuple>();
			auto t1 = a_stack + p.v_minimum + 2;
			for (size_t i = a_n - p.v_minimum; i < t0.f_size(); ++i) t1[i] = t0[i];
			if (p.v_variadic) a_stack[p.v_arguments + 1] = t_tuple::f_instantiate(0, [](auto&)
			{
			});
		} else if (p.v_variadic) {
			size_t n = a_n - arguments;
			auto t0 = a_stack + arguments + 2;
			t0[0] = t_tuple::f_instantiate(n, [&](auto& t1)
			{
				std::uninitialized_copy_n(&t0[0], n, &t1[0]);
			});
		}
		return p.template f_call<T_context>(a_stack);
	}
};

}

#endif
