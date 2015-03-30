#ifndef XEMMAI__FIBER_H
#define XEMMAI__FIBER_H

#include <cassert>

#include "portable/fiber.h"
#include "lambda.h"

namespace xemmai
{

void f_print_with_caret(std::FILE* a_out, const std::wstring& a_path, long a_position, size_t a_column);

struct t_fiber
{
	struct t_context
	{
		static XEMMAI__PORTABLE__THREAD t_context* v_instance;

		static t_context* f_instantiate(t_scoped* a_p, t_context* a_next, t_scoped* a_base)
		{
			t_context* p = reinterpret_cast<t_context*>(a_p);
			p->f_next() = a_next;
			p->f_base() = a_base;
			p->f_native() = 0;
			return p;
		}
		static void f_initiate();
		static void f_terminate();
		XEMMAI__PORTABLE__FORCE_INLINE static void f_push(t_object* a_lambda, t_scoped* a_stack);
		static void f_pop();
		static void** f_pop(t_scoped* a_stack, size_t a_n);
		static void f_backtrace(const t_value& a_value);

		t_scoped v_next;
		t_scoped v_base;
		t_scoped v_native;
		t_scoped v_scope;
		t_scoped v_lambda;

		t_context*& f_next()
		{
			return *reinterpret_cast<t_context**>(&v_next.v_pointer);
		}
		t_scoped*& f_base()
		{
			return *reinterpret_cast<t_scoped**>(&v_base.v_pointer);
		}
		t_scoped* const& f_base() const
		{
			return *reinterpret_cast<t_scoped* const*>(&v_base.v_pointer);
		}
		size_t& f_native()
		{
			return *reinterpret_cast<size_t*>(&v_native.v_pointer);
		}
		t_scoped*& f_previous()
		{
			return *reinterpret_cast<t_scoped**>(&v_scope.v_pointer);
		}
		void**& f_pc()
		{
			return *reinterpret_cast<void***>(&v_lambda.v_pointer);
		}
		const t_value* f_variable(const std::wstring& a_name) const;
	};
	struct t_backtrace
	{
		static void f_push(const t_value& a_throwable, size_t a_native, const t_scoped& a_lambda, void** a_pc);

		t_backtrace* v_next;
		size_t v_native;
		t_slot v_lambda;

		t_backtrace(t_backtrace* a_next, size_t a_native, const t_scoped& a_lambda, void** a_pc) : v_next(a_next), v_native(a_native), v_lambda(a_lambda)
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
	struct t_try
	{
		enum t_state
		{
			e_state__TRY,
			e_state__CATCH,
			e_state__STEP,
			e_state__BREAK,
			e_state__CONTINUE,
			e_state__RETURN,
			e_state__THROW,
		};

		static t_try* f_allocate();
		static void f_push(t_scoped* a_stack, t_context* a_context, void** a_catch, void** a_finally);
		static void f_pop();

		t_try* v_next;
		t_scoped* v_stack;
		t_context* v_context;
		t_state v_state;
		void** v_catch;
		void** v_finally;
	};

	static t_object* f_current();
	static void f_throw(const t_scoped& a_value);
	static t_scoped f_instantiate(t_scoped&& a_callable, size_t a_stack, bool a_main = false, bool a_active = false);
	static void f_define(t_object* a_class);
	template<typename T_main>
	static intptr_t f_main(T_main a_main);
	static void f_run();

	t_slot v_callable;
	t_stack v_stack;
	t_context* v_context = nullptr;
	t_try* v_try = nullptr;
	size_t v_undone = 0;
	void** v_caught;
	bool v_throw = false;
	bool v_main;
	bool v_active;
	t_scoped* v_used;
	t_scoped* v_return;
	portable::t_fiber v_fiber;

	t_fiber(t_scoped&& a_callable, size_t a_stack, bool a_main, bool a_active) : v_callable(std::move(a_callable)), v_stack(a_stack), v_main(a_main), v_active(a_active), v_used(v_stack.v_used), v_fiber(v_main)
	{
	}
	~t_fiber();
	void f_caught(const t_value& a_value);
};

template<>
struct t_type_of<t_fiber> : t_type
{
	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual void f_call(t_object* a_this, t_scoped* a_stack, size_t a_n);
};

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_fiber::t_context* f_context()
{
	return t_fiber::t_context::v_instance;
}
#else
XEMMAI__PORTABLE__EXPORT t_fiber::t_context* f_context();
#endif

inline void t_fiber::t_context::f_push(t_object* a_lambda, t_scoped* a_stack)
{
	t_stack* stack = f_stack();
	t_lambda& lambda = f_as<t_lambda&>(a_lambda);
	t_scoped* previous = stack->v_used;
	t_scoped* used = ++a_stack + lambda.v_size;
	stack->f_allocate(used);
	stack->v_used = used;
	t_context* p = v_instance = f_instantiate(a_stack + lambda.v_arguments, v_instance, a_stack);
	p->f_previous() = previous;
	p->f_pc() = lambda.v_instructions;
	if (lambda.v_shared) p->v_scope.f_construct_nonnull(t_scope::f_instantiate(lambda.v_shareds, t_scoped(lambda.v_scope)));
	p->v_lambda.f_construct_nonnull(a_lambda);
}

}

#endif
