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
		static thread_local t_context* v_instance;

		static t_context* f_instantiate(t_slot* a_p, t_context* a_next, t_slot* a_base)
		{
			t_context* p = reinterpret_cast<t_context*>(a_p);
			p->f_next() = a_next;
			p->f_base() = a_base;
			p->f_native() = 0;
			return p;
		}
		static void f_initiate(void** a_pc);
		XEMMAI__PORTABLE__FORCE_INLINE static void f_initiate(const t_value& a_code, const t_value& a_callable, t_scoped&& a_x);
		static void f_terminate();
		XEMMAI__PORTABLE__FORCE_INLINE static void f_push(const t_value& a_code, const t_value& a_scope, t_slot* a_stack);
		XEMMAI__PORTABLE__ALWAYS_INLINE static void f_pop();
		static void f_pop(t_slot* a_stack, size_t a_n);
		static void f_backtrace(const t_value& a_value);

		t_slot v_next;
		t_slot v_base;
		t_slot v_scope;
		t_slot v_outer;
		t_slot v_code;

		t_context*& f_next()
		{
			return *reinterpret_cast<t_context**>(&v_next.v_pointer);
		}
		t_slot*& f_base()
		{
			return *reinterpret_cast<t_slot**>(&v_base.v_pointer);
		}
		t_slot*& f_previous()
		{
			return *reinterpret_cast<t_slot**>(&v_scope.v_pointer);
		}
		size_t& f_native()
		{
			return *reinterpret_cast<size_t*>(&v_outer.v_pointer);
		}
		void**& f_pc()
		{
			return *reinterpret_cast<void***>(&v_code.v_pointer);
		}
	};
	struct t_backtrace
	{
		static void f_push(const t_value& a_throwable, size_t a_native, const t_slot& a_code, void** a_pc);

		t_backtrace* v_next;
		size_t v_native;
		t_slot v_code;

		t_backtrace(t_backtrace* a_next, size_t a_native, const t_slot& a_code, void** a_pc) : v_next(a_next), v_native(a_native), v_code(a_code)
		{
			*reinterpret_cast<void***>(&v_code.v_pointer) = a_pc;
		}
		void** const& f_pc() const
		{
			return *reinterpret_cast<void** const*>(&v_code.v_pointer);
		}
		void f_scan(t_scan a_scan)
		{
			a_scan(v_code);
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

	static thread_local t_object* v_current;

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	static t_object* f_current()
	{
		return v_current;
	}
#else
	static XEMMAI__PORTABLE__EXPORT t_object* f_current();
#endif
	static void f_throw(const t_scoped& a_value);
	static t_scoped f_instantiate(t_scoped&& a_callable, size_t a_stack, bool a_main = false, bool a_active = false);
	static void f_define(t_object* a_class);
	template<typename T_main>
	static ptrdiff_t f_main(T_main a_main);

	t_slot v_callable;
	t_stack v_stack;
	t_context* v_context = nullptr;
	t_try* v_try = nullptr;
	size_t v_native = 0;
	size_t v_undone = 0;
	void** v_caught;
	bool v_main;
	bool v_active;
	t_slot* v_used;
	t_slot* v_return;

	t_fiber(t_scoped&& a_callable, size_t a_stack, bool a_main, bool a_active) : v_callable(std::move(a_callable)), v_stack(a_stack), v_main(a_main), v_active(a_active), v_used(v_stack.v_used)
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
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_call(t_object* a_this, t_slot* a_stack, size_t a_n);
};

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_fiber::t_context* f_context()
{
	return t_fiber::t_context::v_instance;
}
#else
XEMMAI__PORTABLE__EXPORT t_fiber::t_context* f_context();
#endif

class t_native_context
{
	size_t& v_native;
	bool v_done = false;

public:
	t_native_context() : v_native(f_context()->f_native())
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
	t_slot* stack = fiber.v_stack.f_head();
	v_instance = f_instantiate(stack, nullptr, stack);
	v_instance->f_pc() = a_pc;
}

inline void t_fiber::t_context::f_initiate(const t_value& a_code, const t_value& a_callable, t_scoped&& a_x)
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_stack::v_instance = &fiber.v_stack;
	t_code& code = f_as<t_code&>(a_code);
	t_slot* stack = fiber.v_stack.f_head();
	t_slot* used = stack + code.v_size;
	fiber.v_stack.f_allocate(used);
	fiber.v_stack.v_used = used;
	v_instance = f_instantiate(stack, nullptr, stack);
	v_instance->v_code.f_construct(static_cast<t_object*>(a_code));
	v_instance->f_pc() = &code.v_instructions[0];
	t_slot* privates = stack + sizeof(t_context) / sizeof(t_slot);
	privates[0].f_construct(a_callable);
	privates[1].f_construct(std::move(a_x));
}

inline void t_fiber::t_context::f_terminate()
{
	assert(!v_instance->f_next());
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	assert(fiber.v_stack.v_used == fiber.v_stack.f_head());
	v_instance = f_as<t_fiber&>(v_current).v_context = nullptr;
}

inline void t_fiber::t_context::f_push(const t_value& a_code, const t_value& a_scope, t_slot* a_stack)
{
	t_stack* stack = f_stack();
	t_code& code = f_as<t_code&>(a_code);
	t_slot* previous = stack->v_used;
	t_slot* used = ++a_stack + code.v_size;
	stack->f_allocate(used);
	stack->v_used = used;
	t_context* p = v_instance;
	if (p->f_native() > 0) ++f_as<t_fiber&>(v_current).v_native;
	v_instance = p = f_instantiate(a_stack + code.v_arguments, p, a_stack);
	if (code.v_shared) p->v_scope.f_construct(t_scope::f_instantiate(code.v_shareds, t_scoped(a_scope)));
	p->f_previous() = previous;
	p->v_outer.f_construct(static_cast<t_object*>(a_scope));
	p->v_code.f_construct(static_cast<t_object*>(a_code));
	p->f_pc() = &code.v_instructions[0];
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_fiber::t_context::f_pop()
{
	t_context* p = v_instance;
	v_instance = p->f_next();
	size_t n = f_as<t_code&>(p->v_code).v_privates;
	t_slot* base = p->f_base();
	for (size_t i = 0; i < n; ++i) base[i] = nullptr;
	f_stack()->v_used = p->f_previous();
	if (v_instance->f_native() > 0) --f_as<t_fiber&>(v_current).v_native;
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
