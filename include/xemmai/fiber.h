#ifndef XEMMAI__FIBER_H
#define XEMMAI__FIBER_H

#include <cassert>

#include "code.h"

namespace xemmai
{

void f_print_with_caret(const std::wstring& a_path, long a_position, size_t a_column);

struct t_fiber
{
	struct t_context
	{
		static XEMMAI__PORTABLE__THREAD t_context* v_instance;

		static t_context* f_allocate();
		XEMMAI__PORTABLE__FORCE_INLINE static t_context* f_instantiate(t_context* a_next, t_slot* a_base)
		{
			t_context* p = t_local_pool<t_context>::f_allocate(f_allocate);
			p->v_next = a_next;
			p->v_base = a_base;
			return p;
		}
		XEMMAI__PORTABLE__FORCE_INLINE static void f_finalize(t_context* a_p)
		{
			while (a_p) {
				t_context* p = a_p;
				a_p = p->v_next;
				p->f_finalize();
			}
		}
		static void f_initiate(void** a_pc);
		XEMMAI__PORTABLE__FORCE_INLINE static void f_initiate(const t_value& a_code, const t_value& a_callable, const t_transfer& a_x);
		static void f_terminate();
		XEMMAI__PORTABLE__FORCE_INLINE static void f_push(const t_value& a_code, const t_value& a_scope, const t_value& a_self, t_slot* a_stack, size_t a_n);
		XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE static void f_pop();
		XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE static void f_pop(t_slot* a_stack, size_t a_n);
		static void f_backtrace();

		t_slot* v_base;
		t_slot* v_previous;
		t_context* v_next;
		t_slot v_outer;
		t_slot v_self;
		t_slot v_scope;
		t_slot v_code;
		void** v_pc;
		size_t v_native;

		t_context() : v_native(0)
		{
		}
		void f_scan(t_scan a_scan)
		{
			a_scan(v_outer);
			a_scan(v_self);
			a_scan(v_scope);
			a_scan(v_code);
		}
		XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE void f_finalize()
		{
			v_outer = 0;
			v_self = 0;
			v_scope = 0;
			v_code = 0;
			v_native = 0;
			t_local_pool<t_context>::f_free(this);
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
		static void f_push(t_slot* a_stack, t_context* a_context, void** a_catch, void** a_finally);
		static void f_pop();

		t_try* v_next;
		t_slot* v_stack;
		t_context* v_context;
		t_state v_state;
		void** v_catch;
		void** v_finally;
	};

	static XEMMAI__PORTABLE__THREAD t_object* v_current;

	static t_object* f_current()
	{
		return v_current;
	}
	static void f_throw(const t_scoped& a_value);
	static t_transfer f_instantiate(const t_transfer& a_callable, size_t a_stack, bool a_main = false, bool a_active = false);
	static void f_define(t_object* a_class);

	t_slot v_callable;
	t_stack v_stack;
	t_context* v_context;
	t_try* v_try;
	size_t v_native;
	t_context* v_backtrace;
	size_t v_undone;
	void** v_caught;
	bool v_main;
	bool v_active;
	t_slot* v_used;
	t_slot* v_return;

	t_fiber(const t_transfer& a_callable, size_t a_stack, bool a_main, bool a_active) : v_callable(a_callable), v_stack(a_stack), v_context(0), v_try(0), v_native(0), v_backtrace(0), v_undone(0), v_main(a_main), v_active(a_active), v_used(v_stack.v_used)
	{
	}
	~t_fiber();
	void f_caught(const t_value& a_object);
};

template<>
struct t_type_of<t_fiber> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n);
};

inline t_fiber::t_context* f_context()
{
	return t_fiber::t_context::v_instance;
}

class t_native_context
{
	size_t& v_native;
	bool v_done;

public:
	t_native_context() : v_native(f_context()->v_native), v_done(false)
	{
		++v_native;
	}
	~t_native_context()
	{
		--v_native;
		if (!v_done) ++f_as<t_fiber&>(t_fiber::f_current()).v_undone;
	}
	void f_done()
	{
		v_done = true;
	}
};

inline void t_fiber::t_context::f_initiate(void** a_pc)
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_stack::v_instance = &fiber.v_stack;
	v_instance = f_instantiate(0, fiber.v_stack.f_head());
	v_instance->v_pc = a_pc;
}

inline void t_fiber::t_context::f_initiate(const t_value& a_code, const t_value& a_callable, const t_transfer& a_x)
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_stack::v_instance = &fiber.v_stack;
	t_code& code = f_as<t_code&>(a_code);
	t_slot* stack = fiber.v_stack.f_head();
	t_slot* used = stack + code.v_size;
	fiber.v_stack.f_allocate(used);
	fiber.v_stack.v_used = used;
	v_instance = f_instantiate(0, stack);
	v_instance->v_code.f_construct(a_code);
	v_instance->v_pc = &code.v_instructions[0];
	stack[0].f_construct(a_callable);
	stack[1].f_construct(a_x);
}

inline void t_fiber::t_context::f_terminate()
{
	assert(!v_instance->v_next);
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	assert(fiber.v_stack.v_used == fiber.v_stack.f_head());
	v_instance->f_finalize();
	v_instance = f_as<t_fiber&>(v_current).v_context = 0;
}

inline void t_fiber::t_context::f_push(const t_value& a_code, const t_value& a_scope, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	t_stack* stack = f_stack();
	t_code& code = f_as<t_code&>(a_code);
	t_slot* previous = stack->v_used;
	t_slot* used = ++a_stack + code.v_size;
	stack->f_allocate(used);
	stack->v_used = used;
	t_context* p = v_instance;
	if (p->v_native > 0) ++f_as<t_fiber&>(v_current).v_native;
	v_instance = p = f_instantiate(p, a_stack);
	p->v_previous = previous;
	p->v_outer.f_construct(a_scope);
	p->v_self.f_construct(a_self);
	if (code.v_shared) p->v_scope.f_construct(t_scope::f_instantiate(code.v_shareds, a_scope));
	p->v_code.f_construct(a_code);
	p->v_pc = &code.v_instructions[0];
}

inline void t_fiber::t_context::f_pop()
{
	t_stack* stack = f_stack();
	t_context* p = v_instance;
	t_code& code = f_as<t_code&>(p->v_code);
	for (size_t i = 0; i < code.v_privates; ++i) p->v_base[i] = 0;
	stack->v_used = p->v_previous;
	v_instance = p->v_next;
	p->f_finalize();
	if (v_instance->v_native > 0) --f_as<t_fiber&>(v_current).v_native;
}

inline void t_fiber::t_context::f_pop(t_slot* a_stack, size_t a_n)
{
	t_stack* stack = f_stack();
	t_context* p = v_instance;
	t_code& code = f_as<t_code&>(p->v_code);
	++a_stack;
	size_t i = 0;
	for (; i < a_n; ++i) p->v_base[i] = a_stack[i].f_transfer();
	for (; i < code.v_privates; ++i) p->v_base[i] = 0;
	stack->v_used = std::max(p->v_previous, p->v_base + a_n);
	v_instance = p->v_next;
	p->f_finalize();
	if (v_instance->v_native > 0) --f_as<t_fiber&>(v_current).v_native;
}

inline void t_fiber::t_try::f_push(t_slot* a_stack, t_context* a_context, void** a_catch, void** a_finally)
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_try* p = t_local_pool<t_try>::f_allocate(f_allocate);
	p->v_next = fiber.v_try;
	p->v_stack = a_stack;
	p->v_context = a_context;
	p->v_state = e_state__TRY;
	p->v_catch = a_catch;
	p->v_finally = a_finally;
	fiber.v_try = p;
}

inline void t_fiber::t_try::f_pop()
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_try* p = fiber.v_try;
	fiber.v_try = p->v_next;
	t_local_pool<t_try>::f_free(p);
}

}

#endif
