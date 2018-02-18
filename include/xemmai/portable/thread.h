#ifndef XEMMAI__PORTABLE__THREAD_H
#define XEMMAI__PORTABLE__THREAD_H

#include "define.h"

#ifdef __unix__
#include <pthread.h>
#endif
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif

namespace xemmai::portable
{

#ifdef __unix__
inline size_t f_cpu()
{
	return sched_getcpu();
}

class t_affinity
{
	cpu_set_t v_set;

public:
	static const size_t V_SIZE = CPU_SETSIZE;

	void f_from_thread()
	{
		pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &v_set);
	}
	void f_to_thread()
	{
		pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &v_set);
	}
	bool f_contains(size_t a_cpu) const
	{
		return CPU_ISSET(static_cast<int>(a_cpu), &v_set) != 0;
	}
	void f_clear()
	{
		CPU_ZERO(&v_set);
	}
	void f_add(size_t a_cpu)
	{
		CPU_SET(static_cast<int>(a_cpu), &v_set);
	}
	void f_remove(size_t a_cpu)
	{
		CPU_CLR(static_cast<int>(a_cpu), &v_set);
	}
};
#endif

#ifdef _WIN32
inline size_t f_cpu()
{
	_asm {mov eax, 1}
	_asm {cpuid}
	_asm {shr ebx, 24}
	_asm {mov eax, ebx}
}

class t_affinity
{
	DWORD_PTR v_mask;

public:
	static const size_t V_SIZE = sizeof(DWORD_PTR) * 8;

	void f_from_thread()
	{
		DWORD_PTR mask;
		GetProcessAffinityMask(GetCurrentProcess(), &v_mask, &mask);
	}
	void f_to_thread()
	{
		SetThreadAffinityMask(GetCurrentThread(), v_mask);
	}
	bool f_contains(size_t a_cpu) const
	{
		return (v_mask & 1 << a_cpu) != 0;
	}
	void f_clear()
	{
		v_mask = 0;
	}
	void f_add(size_t a_cpu)
	{
		v_mask |= 1 << a_cpu;
	}
	void f_remove(size_t a_cpu)
	{
		v_mask &= ~(1 << a_cpu);
	}
};
#endif

}

#endif
