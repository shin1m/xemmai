#ifndef XEMMAI__FIBER_H
#define XEMMAI__FIBER_H

#include <cassert>

#include "portable/fiber.h"
#include "lambda.h"

namespace xemmai
{

struct t_debug_context;
void f_print_with_caret(std::FILE* a_out, std::wstring_view a_path, long a_position, size_t a_column);

struct t_fiber
{
	static t_object* f_current();
	static t_scoped f_instantiate(t_scoped&& a_callable, size_t a_stack, bool a_main = false, bool a_active = false);
	template<typename T_context, typename T_main>
	static intptr_t f_main(T_main a_main);
	template<typename T_context>
	static void f_run();

	t_slot v_callable;
	t_stack v_stack;
	void** v_caught = nullptr;
	bool v_throw = false;
	bool v_main;
	bool v_active;
	t_stacked* v_return;
	portable::t_fiber v_fiber;
	t_debug_context* v_context = nullptr;

	t_fiber(t_scoped&& a_callable, size_t a_stack, bool a_main, bool a_active) : v_callable(std::move(a_callable)), v_stack(a_stack), v_main(a_main), v_active(a_active), v_fiber(v_main)
	{
	}
	void f_caught(const t_value& a_value, t_object* a_lambda, void** a_pc = nullptr);
};

template<>
struct t_type_of<t_fiber> : t_underivable<t_holds<t_fiber>>
{
	void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	void f_do_instantiate(t_stacked* a_stack, size_t a_n);
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
};

struct t_context
{
	void** v_pc;
	t_stacked* v_base;
	t_lambda* v_lambda;
	t_stacked* v_previous;
	t_slot* v_scope;

	t_context() : v_pc(nullptr), v_lambda(nullptr)
	{
		t_stack* stack = t_stack::v_instance = &f_as<t_fiber&>(t_fiber::f_current()).v_stack;
		v_base = stack->f_head();
	}
	t_context(t_lambda* a_lambda, t_stacked* a_stack) : v_pc(a_lambda->v_instructions), v_base(a_stack + 2), v_lambda(a_lambda)
	{
		t_stack* stack = f_stack();
		v_previous = stack->v_used;
		stack->v_used = v_base + v_lambda->v_size;
	}
	template<typename T>
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return(T&& a_value)
	{
		v_base[-1].f_destruct();
		for (size_t i = 0; i < v_lambda->v_privates; ++i) v_base[i].f_destruct();
		v_base[-2].f_construct(std::forward<T>(a_value));
		f_stack()->v_used = v_previous;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return()
	{
		f_return(std::move(v_base[v_lambda->v_privates]));
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return(size_t a_index)
	{
		v_base[-1].f_destruct();
		size_t i = 0;
		while (i < a_index) v_base[i++].f_destruct();
		v_base[-2].f_construct(std::move(v_base[i++]));
		while (i < v_lambda->v_privates) v_base[i++].f_destruct();
		f_stack()->v_used = v_previous;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_tail(size_t a_n)
	{
		v_base[-1].f_destruct();
		for (size_t i = 0; i < v_lambda->v_privates; ++i) v_base[i].f_destruct();
		auto p = reinterpret_cast<char*>(v_base + v_lambda->v_privates);
		std::copy(p, p + (a_n + 2) * sizeof(t_stacked), reinterpret_cast<char*>(v_base - 2));
		f_stack()->v_used = std::max(v_previous, v_base + a_n);
	}
	template<size_t (*t_type::*A_function)(t_object*, t_stacked*)>
	size_t f_tail(t_object* a_this);
	void f_backtrace(const t_value& a_value);
	const t_value* f_variable(std::wstring_view a_name) const;
};

template<size_t (*t_type::*A_function)(t_object*, t_stacked*)>
size_t t_context::f_tail(t_object* a_this)
{
	size_t n = (a_this->f_type()->*A_function)(a_this, v_base + v_lambda->v_privates);
	if (n == size_t(-1))
		f_return();
	else
		f_tail(n);
	return n;
}

struct t_debug_context : t_context
{
	t_debug_context* v_next;

	t_debug_context() : v_next(nullptr)
	{
		f_as<t_fiber&>(t_fiber::f_current()).v_context = this;
	}
	t_debug_context(t_lambda* a_lambda, t_stacked* a_stack) : t_context(a_lambda, a_stack)
	{
		auto& fiber = f_as<t_fiber&>(t_fiber::f_current());
		v_next = fiber.v_context;
		fiber.v_context = this;
	}
	~t_debug_context()
	{
		f_as<t_fiber&>(t_fiber::f_current()).v_context = v_next;
	}
};

}

#endif
