#ifndef XEMMAI__FIBER_H
#define XEMMAI__FIBER_H

#include "lambda.h"
#include <cassert>
#ifdef __unix__
#include <ucontext.h>
#endif

namespace xemmai
{

class t_thread;
struct t_debug_context;
void f_print_with_caret(std::FILE* a_out, std::wstring_view a_path, long a_position, size_t a_column);

struct t_fiber
{
	struct t_internal
	{
		t_internal* v_next;
		t_thread* v_thread;
		t_fiber* v_fiber;
		std::unique_ptr<char[]> v_stack_buffer;
		t_object** v_stack_last_top;
		t_object** v_stack_last_bottom;
		t_object** v_stack_copy;
		t_object** v_stack_bottom;
		t_object** v_stack_top;
		t_object** v_stack_decrements;
		std::unique_ptr<t_pvalue[]> v_estack;
		t_pvalue* v_estack_used;
		std::unique_ptr<t_object*[]> v_estack_buffer;
		t_object** v_estack_last_head;
		t_object** v_estack_last_used;
		t_object** v_estack_decrements;
		ucontext_t v_context;

		t_internal(t_fiber* a_fiber, size_t a_stack, size_t a_n);
		t_internal(t_fiber* a_fiber, void* a_bottom);
		t_internal(t_fiber* a_fiber, void(*a_f)());
		void f_epoch_get()
		{
			t_object* dummy = nullptr;
			v_stack_top = &dummy;
			v_estack_used = v_stack_used;
		}
		void f_epoch_copy();
		void f_epoch_scan();
		void f_epoch_decrement();
	};

	static inline XEMMAI__PORTABLE__THREAD t_pvalue* v_stack_used;

	static t_object* f_current();
	static t_object* f_instantiate(const t_pvalue& a_callable, size_t a_stack);
	template<typename T_context, typename T_main>
	static intptr_t f_main(T_main a_main);
	template<typename T_context>
	static void f_run();

	t_svalue v_callable;
	size_t v_stack;
	t_internal* v_internal = nullptr;
	void** v_caught = nullptr;
	bool v_throw = false;
	t_pvalue* v_return;
	t_debug_context* v_context = nullptr;

	t_fiber(const t_pvalue& a_callable, size_t a_stack) : v_callable(a_callable), v_stack(a_stack)
	{
	}
	void f_caught(const t_pvalue& a_value, t_object* a_lambda, void** a_pc = nullptr);
};

template<>
struct t_type_of<t_fiber> : t_underivable<t_holds<t_fiber>>
{
	void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_fiber>().v_callable);
	}
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
};

inline t_pvalue* f_stack()
{
	return t_fiber::v_stack_used;
}

inline void f_stack__(t_pvalue* a_p)
{
	std::atomic_signal_fence(std::memory_order_release);
	t_fiber::v_stack_used = a_p;
}

struct t_context
{
	void** v_pc;
	t_pvalue* v_base;
	t_object* v_lambda;
	t_pvalue* v_previous;
	t_svalue* v_scope;

	t_context() : v_base(f_stack()), v_pc(nullptr), v_lambda(nullptr)
	{
	}
	t_context(t_object* a_lambda, t_pvalue* a_stack) : v_pc(f_as<t_lambda&>(a_lambda).v_instructions), v_base(a_stack + 2), v_lambda(a_lambda)
	{
		v_previous = f_stack();
		f_stack__(v_base + f_as<t_lambda&>(v_lambda).v_size);
	}
	template<typename T>
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return(T&& a_value)
	{
		v_base[-2] = std::forward<T>(a_value);
		f_stack__(v_previous);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return()
	{
		f_return(v_base[f_as<t_lambda&>(v_lambda).v_privates]);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return(size_t a_index)
	{
		v_base[-2] = v_base[a_index];
		f_stack__(v_previous);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_tail(size_t a_n)
	{
		auto p = reinterpret_cast<char*>(v_base + f_as<t_lambda&>(v_lambda).v_privates);
		std::copy(p, p + (a_n + 2) * sizeof(t_pvalue), reinterpret_cast<char*>(v_base - 2));
		f_stack__(std::max(v_previous, v_base + a_n));
	}
	template<size_t (*t_type::*A_function)(t_object*, t_pvalue*)>
	size_t f_tail(t_object* a_this);
	void f_backtrace(const t_pvalue& a_value);
	const t_pvalue* f_variable(std::wstring_view a_name) const;
};

template<size_t (*t_type::*A_function)(t_object*, t_pvalue*)>
size_t t_context::f_tail(t_object* a_this)
{
	size_t n = (a_this->f_type()->*A_function)(a_this, v_base + f_as<t_lambda&>(v_lambda).v_privates);
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
	t_debug_context(t_object* a_lambda, t_pvalue* a_stack) : t_context(a_lambda, a_stack)
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

class t_scoped_stack
{
	t_pvalue* v_p;

public:
	t_scoped_stack(size_t a_n)
	{
		v_p = f_stack();
		f_stack__(v_p + a_n);
	}
	template<typename T_x, typename... T>
	t_scoped_stack(size_t a_n, T_x&& a_x, T&&... a_xs) : t_scoped_stack(a_n, std::forward<T>(a_xs)...)
	{
		v_p[a_n - sizeof...(a_xs) - 1] = std::forward<T_x>(a_x);
	}
	~t_scoped_stack()
	{
		f_stack__(v_p);
	}
	operator t_pvalue*() const
	{
		return v_p;
	}
};

template<typename T_tag>
template<typename... T>
inline t_pvalue t_value<T_tag>::operator()(T&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
	stack[1] = nullptr;
	f_call(stack, sizeof...(a_arguments));
	return stack[0];
}

template<typename T_tag>
template<typename... T>
inline t_pvalue t_value<T_tag>::f_invoke(t_object* a_key, T&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
	stack[1] = nullptr;
	f_call(a_key, stack, sizeof...(a_arguments));
	return stack[0];
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_get_at(const t_pvalue& a_index) const
{
	auto p = f_object_or_throw();
	t_scoped_stack stack(3);
	stack[2] = a_index;
	size_t n = p->f_type()->f_get_at(p, stack);
	if (n != size_t(-1)) f_loop(stack, n);
	return stack[0];
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_set_at(const t_pvalue& a_index, const t_pvalue& a_value) const
{
	auto p = f_object_or_throw();
	t_scoped_stack stack(4);
	stack[2] = a_index;
	stack[3] = a_value;
	size_t n = p->f_type()->f_set_at(p, stack);
	if (n != size_t(-1)) f_loop(stack, n);
	return stack[0];
}

#define XEMMAI__VALUE__UNARY(a_method)\
		{\
			t_scoped_stack stack(2);\
			size_t n = p->f_type()->a_method(p, stack);\
			if (n != size_t(-1)) f_loop(stack, n);\
			return stack[0];\
		}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_plus() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return v_integer;
	case e_tag__FLOAT:
		return v_float;
	default:
		XEMMAI__VALUE__UNARY(f_plus)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_minus() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return -v_integer;
	case e_tag__FLOAT:
		return -v_float;
	default:
		XEMMAI__VALUE__UNARY(f_minus)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_not() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__BOOLEAN:
		return !v_boolean;
	case e_tag__NULL:
	case e_tag__INTEGER:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__UNARY(f_not)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_complement() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__INTEGER:
		return ~v_integer;
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__UNARY(f_complement)
	}
}

template<typename... T>
inline t_pvalue t_object::f_invoke(t_object* a_key, T&&... a_arguments)
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
	stack[1] = nullptr;
	f_call(a_key, stack, sizeof...(a_arguments));
	return stack[0];
}

inline t_pvalue t_object::f_call_preserved(t_object* a_key, t_pvalue* a_stack, size_t a_n)
{
	size_t n = a_n + 2;
	t_scoped_stack stack(n);
	stack[1] = nullptr;
	for (size_t i = 2; i < n; ++i) stack[i] = a_stack[i];
	f_call(a_key, stack, a_n);
	return stack[0];
}

}

#endif
