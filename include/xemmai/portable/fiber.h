#ifndef XEMMAI__PORTABLE__FIBER_H
#define XEMMAI__PORTABLE__FIBER_H

#include "define.h"

#ifdef __unix__
#include <ucontext.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

namespace xemmai::portable
{

#ifdef __unix__
class t_fiber
{
	ucontext_t v_context;
	char* v_stack;

public:
	static const size_t V_SIZE = 8192 * 1024;

	t_fiber(bool a_main) : v_stack(a_main ? nullptr : new char[V_SIZE])
	{
	}
	~t_fiber()
	{
		delete v_stack;
	}
	void f_make(void (*a_f)())
	{
		getcontext(&v_context);
		v_context.uc_link = nullptr;
		v_context.uc_stack.ss_sp = v_stack;
		v_context.uc_stack.ss_size = V_SIZE;
		makecontext(&v_context, a_f, 0);
	}
	void f_swap(t_fiber& a_current)
	{
		swapcontext(&a_current.v_context, &v_context);
	}
	void f_set()
	{
		setcontext(&v_context);
	}
};
#endif

#ifdef _WIN32
class t_fiber
{
	static void CALLBACK f_start(PVOID a_f)
	{
		reinterpret_cast<void (*)()>(a_f)();
	}

	bool v_main;
	LPVOID v_handle = NULL;

public:
	t_fiber(bool a_main) : v_main(a_main)
	{
	}
	~t_fiber()
	{
		if (!v_main && v_handle != NULL) DeleteFiber(v_handle);
	}
	void f_make(void (*a_f)())
	{
		if (v_handle != NULL) DeleteFiber(v_handle);
		v_handle = CreateFiber(0, f_start, a_f);
	}
	void f_swap(t_fiber& a_current)
	{
		if (a_current.v_main && a_current.v_handle == NULL) a_current.v_handle = ConvertThreadToFiber(NULL);
		SwitchToFiber(v_handle);
	}
	void f_set()
	{
		SwitchToFiber(v_handle);
	}
};
#endif

}

#endif
