#ifndef XEMMAI__FIBER_H
#define XEMMAI__FIBER_H

#include "lambda.h"
#include <memory>
#include <cassert>
#ifdef __unix__
#include <ucontext.h>
#endif

namespace xemmai
{

struct t_debug_context;
void f_print_with_caret(std::FILE* a_out, std::wstring_view a_path, long a_position, size_t a_column);

struct t_fiber
{
	struct t_internal
	{
		t_internal* v_next;
		void* v_thread;
		t_fiber* v_fiber;
		std::unique_ptr<t_pvalue[]> v_estack;
		t_pvalue* v_estack_used;
		std::unique_ptr<t_object*[]> v_estack_buffer;
		t_object** v_estack_last_head;
		t_object** v_estack_last_used;
		t_object** v_estack_decrements;
		t_object** v_stack_last_top;
		t_object** v_stack_last_bottom;
		t_object** v_stack_copy;
		t_object** v_stack_bottom;
		t_object** v_stack_top;
		t_object** v_stack_decrements;
#ifdef __unix__
		ucontext_t v_context;
#endif
#ifdef _WIN32
		LPVOID v_handle;
#endif

		XEMMAI__LOCAL t_internal(size_t a_stack, size_t a_n);
		XEMMAI__LOCAL t_internal(size_t a_stack, void* a_bottom);
		XEMMAI__LOCAL t_internal(t_fiber* a_fiber, void(*a_f)());
#ifdef _WIN32
		~t_internal()
		{
			if (v_handle != NULL) DeleteFiber(v_handle);
		}
#endif
		void f_epoch_get()
		{
#ifndef _WIN32
			v_estack_used = v_stack_used;
#endif
			t_object* dummy = nullptr;
			v_stack_top = &dummy;
		}
		XEMMAI__LOCAL void f_epoch_copy();
		XEMMAI__LOCAL void f_epoch_scan();
		XEMMAI__LOCAL void f_epoch_decrement();
	};

#ifdef _WIN32
	static inline XEMMAI__PORTABLE__THREAD t_internal* v_current;
#else
	static inline XEMMAI__PORTABLE__THREAD t_pvalue* v_stack_used;
#endif

	static t_object* f_current();
	XEMMAI__LOCAL static t_object* f_instantiate(const t_pvalue& a_callable, size_t a_stack);
	template<typename T_context>
	static intptr_t f_main(auto a_main);
	template<typename T_context>
	static void f_run();

	t_svalue v_callable;
	size_t v_stack;
	t_internal* v_internal = nullptr;
	bool v_throw = false;
	t_pvalue* v_return;
	t_debug_context* v_context = nullptr;

	t_fiber(const t_pvalue& a_callable, size_t a_stack) : v_callable(a_callable), v_stack(a_stack)
	{
	}
};

template<>
struct t_type_of<t_fiber> : t_holds<t_fiber>
{
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_fiber>().v_callable);
	}
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
};

#ifdef _WIN32
XEMMAI__PUBLIC t_pvalue* f_stack();
XEMMAI__PUBLIC void f_stack__(t_pvalue* a_p);
#else
inline t_pvalue* f_stack()
{
	return t_fiber::v_stack_used;
}

inline void f_stack__(t_pvalue* a_p)
{
	std::atomic_signal_fence(std::memory_order_release);
	t_fiber::v_stack_used = a_p;
}
#endif

struct t_context
{
	void** v_pc;
	t_pvalue* v_base;
	t_object* v_lambda;
	t_pvalue* v_previous;
	t_object* v_scope;

	t_context() : v_base(f_stack()), v_pc(nullptr), v_lambda(nullptr)
	{
	}
	t_context(t_object* a_lambda, t_pvalue* a_stack) : v_pc(a_lambda->f_as<t_lambda>().v_instructions), v_base(a_stack + 2), v_lambda(a_lambda)
	{
		v_previous = f_stack();
		f_stack__(v_base + v_lambda->f_as<t_lambda>().v_size);
#ifndef NDEBUG
		std::fill(v_base + v_lambda->f_as<t_lambda>().v_arguments, v_base + v_lambda->f_as<t_lambda>().v_privates, 0);
#endif
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_return(auto&& a_value)
	{
		v_base[-2] = std::forward<decltype(a_value)>(a_value);
		f_stack__(v_previous);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_tail(t_pvalue* a_stack, size_t a_n)
	{
		std::copy(a_stack, a_stack + a_n + 2, v_base - 2);
		f_stack__(std::max(v_previous, v_base + a_n));
	}
	template<size_t (*t_type::*A_function)(t_object*, t_pvalue*)>
	size_t f_tail(t_object* a_this);
	size_t f_loop();
	const t_pvalue* f_variable(std::wstring_view a_name) const;
};

template<size_t (*t_type::*A_function)(t_object*, t_pvalue*)>
size_t t_context::f_tail(t_object* a_this)
{
	auto stack = v_base + v_lambda->f_as<t_lambda>().v_privates;
	size_t n = (a_this->f_type()->*A_function)(a_this, stack);
	if (n == size_t(-1))
		f_return(stack[0]);
	else
		f_tail(stack, n);
	return n;
}

struct t_debug_context : t_context
{
	t_debug_context* v_next;

	t_debug_context() : v_next(nullptr)
	{
		t_fiber::f_current()->f_as<t_fiber>().v_context = this;
	}
	t_debug_context(t_object* a_lambda, t_pvalue* a_stack) : t_context(a_lambda, a_stack)
	{
		auto& fiber = t_fiber::f_current()->f_as<t_fiber>();
		v_next = fiber.v_context;
		fiber.v_context = this;
	}
	~t_debug_context()
	{
		t_fiber::f_current()->f_as<t_fiber>().v_context = v_next;
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
	t_scoped_stack(size_t a_n, auto&& a_x, auto&&... a_xs) : t_scoped_stack(a_n, std::forward<decltype(a_xs)>(a_xs)...)
	{
		v_p[a_n - sizeof...(a_xs) - 1] = std::forward<decltype(a_x)>(a_x);
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
inline t_pvalue t_value<T_tag>::operator()(auto&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<decltype(a_arguments)>(a_arguments)...);
	stack[1] = nullptr;
	f_call(stack, sizeof...(a_arguments));
	return stack[0];
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_invoke(t_object* a_key, size_t& a_index, auto&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<decltype(a_arguments)>(a_arguments)...);
	stack[1] = nullptr;
	f_call(a_key, a_index, stack, sizeof...(a_arguments));
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
#define XEMMAI__VALUE__UNARY_ARITHMETIC(a_name, a_operator)\
template<typename T_tag>\
inline t_pvalue t_value<T_tag>::f_##a_name() const\
{\
	auto p = static_cast<t_object*>(*this);\
	switch (reinterpret_cast<uintptr_t>(p)) {\
	case c_tag__NULL:\
	case c_tag__FALSE:\
	case c_tag__TRUE:\
		f_throw(L"not supported."sv);\
	case c_tag__INTEGER:\
		return a_operator(v_integer);\
	case c_tag__FLOAT:\
		return a_operator(v_float);\
	default:\
		XEMMAI__VALUE__UNARY(f_##a_name)\
	}\
}

XEMMAI__VALUE__UNARY_ARITHMETIC(plus, )
XEMMAI__VALUE__UNARY_ARITHMETIC(minus, -)

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_complement() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case c_tag__INTEGER:
		return ~v_integer;
	case c_tag__NULL:
	case c_tag__FALSE:
	case c_tag__TRUE:
	case c_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__UNARY(f_complement)
	}
}

inline t_pvalue t_object::f_invoke_class(size_t a_index, auto&&... a_arguments)
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<decltype(a_arguments)>(a_arguments)...);
	stack[1] = nullptr;
	v_type->f_invoke_class(this, a_index, stack, sizeof...(a_arguments));
	return stack[0];
}

inline t_pvalue t_object::f_invoke(t_object* a_key, size_t& a_index, auto&&... a_arguments)
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<decltype(a_arguments)>(a_arguments)...);
	stack[1] = nullptr;
	f_call(a_key, a_index, stack, sizeof...(a_arguments));
	return stack[0];
}

}

#endif
