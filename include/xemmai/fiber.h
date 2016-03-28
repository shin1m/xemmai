#ifndef XEMMAI__FIBER_H
#define XEMMAI__FIBER_H

#include <cassert>

#include "portable/fiber.h"
#include "lambda.h"

namespace xemmai
{

void f_print_with_caret(std::FILE* a_out, const std::wstring& a_path, long a_position, size_t a_column);

struct t_context
{
	static XEMMAI__PORTABLE__THREAD t_context* v_instance;

	t_context* v_next;
	t_scoped* v_base;
	t_scoped v_lambda;
	t_scoped v_scope;

	t_context();
	t_context(t_object* a_lambda, t_scoped* a_stack) : v_next(v_instance), v_base(a_stack + 2)
	{
		v_lambda.f_construct_nonnull(a_lambda);
		t_stack* stack = f_stack();
		f_previous() = stack->v_used;
		auto& lambda = f_as<t_lambda&>(a_lambda);
		stack->f_allocate(v_base + lambda.v_size);
		if (lambda.v_shared) v_scope.f_construct_nonnull(t_scope::f_instantiate(lambda.v_shareds, t_scoped(lambda.v_scope)));
		v_instance = this;
	}
	~t_context()
	{
		v_instance = v_next;
	}
	void f_terminate();
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_pop()
	{
		size_t n = f_as<t_lambda&>(v_lambda).v_privates;
		v_base[-1].f_destruct();
		for (size_t i = 0; i < n; ++i) v_base[i].f_destruct();
		f_stack()->v_used = f_previous();
	}
	void f_tail(t_scoped* a_stack, size_t a_n);
	void f_backtrace(const t_value& a_value);
	t_scoped*& f_previous()
	{
		return *reinterpret_cast<t_scoped**>(&v_lambda.v_pointer);
	}
	void**& f_pc()
	{
		return *reinterpret_cast<void***>(&v_scope.v_pointer);
	}
	const t_value* f_variable(const std::wstring& a_name) const;
};

struct t_backtrace
{
	static void f_push(const t_value& a_throwable, const t_scoped& a_lambda, void** a_pc);

	t_backtrace* v_next;
	t_slot v_lambda;

	t_backtrace(t_backtrace* a_next, const t_scoped& a_lambda, void** a_pc) : v_next(a_next), v_lambda(a_lambda)
	{
		*reinterpret_cast<void***>(&v_lambda.v_pointer) = a_pc;
	}
	void** const& f_pc() const
	{
		return *reinterpret_cast<void** const*>(&v_lambda.v_pointer);
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_lambda);
	}
	void f_dump() const;
};

struct t_fiber
{
	static t_object* f_current();
	static t_scoped f_instantiate(t_scoped&& a_callable, size_t a_stack, bool a_main = false, bool a_active = false);
	static void f_define(t_object* a_class);
	template<typename T_main>
	static intptr_t f_main(T_main a_main);
	static void f_run();

	t_slot v_callable;
	t_stack v_stack;
	t_context* v_context = nullptr;
	bool v_throw = false;
	bool v_main;
	bool v_active;
	t_scoped* v_used;
	t_scoped* v_return;
	portable::t_fiber v_fiber;

	t_fiber(t_scoped&& a_callable, size_t a_stack, bool a_main, bool a_active) : v_callable(std::move(a_callable)), v_stack(a_stack), v_main(a_main), v_active(a_active), v_used(v_stack.v_used), v_fiber(v_main)
	{
	}
	void f_caught(const t_value& a_value, void** a_pc = nullptr);
};

template<>
struct t_type_of<t_fiber> : t_type
{
	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_scoped* a_stack, size_t a_n);
};

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_context* f_context()
{
	return t_context::v_instance;
}
#else
XEMMAI__PORTABLE__EXPORT t_context* f_context();
#endif

}

#endif
