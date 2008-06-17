#ifndef XEMMAI__PORTABLE__ATOMIC_H
#define XEMMAI__PORTABLE__ATOMIC_H

#if defined(__unix__) && (defined(__i386__) || defined(__x86_64__)) && defined(__GNUC__)
#include <pthread.h>
#endif

#ifdef _MSC_VER
#define _WIN32_WINNT 0x0400
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#endif

#include "define.h"

namespace xemmai
{

namespace portable
{

#if 0
class t_lock
{
	volatile long v_lock;

public:
	t_lock() : v_lock(0)
	{
	}
	void f_acquire_for_read()
	{
		++v_lock;
		while (v_lock <= 0);
	}
	void f_release_for_read()
	{
		--v_lock;
	}
	void f_acquire_for_write()
	{
		while (v_lock != 0);
		v_lock += 0x80000000;
	}
	void f_release_for_write()
	{
		v_lock -= 0x80000000;
	}
};
#endif

#if defined(__unix__) && (defined(__i386__) || defined(__x86_64__)) && defined(__GNUC__)
class t_lock
{
	volatile long v_lock;

public:
	t_lock() : v_lock(0)
	{
	}
	void f_acquire_for_read()
	{
		int value;
		asm volatile (
			"lock; xaddl %0, %1"
			: "=r" (value)
			: "m" (v_lock), "0" (1)
			: "memory", "cc"
		);
		if (value >= 0) return;
		do pthread_yield(); while (v_lock <= 0);
	}
	void f_release_for_read()
	{
		asm volatile (
			"lock; decl %0"
			:
			: "m" (v_lock)
			: "memory", "cc"
		);
	}
	void f_acquire_for_write()
	{
		while (true) {
			int value;
			asm volatile (
				"lock; cmpxchgl %1, %2"
				: "=a" (value)
				: "r" (0x80000000), "m" (v_lock), "0" (0)
				: "memory", "cc"
			);
			if (value == 0) break;
			do pthread_yield(); while (v_lock != 0);
		}
	}
	void f_release_for_write()
	{
		asm volatile (
			"lock; addl $0x80000000, %0"
			:
			: "m" (v_lock)
			: "memory", "cc"
		);
	}
};
#endif

#ifdef _MSC_VER
class t_lock
{
	volatile long v_lock;

public:
	t_lock() : v_lock(0)
	{
	}
	XEMMAI__PORTABLE__FORCE_INLINE void f_acquire_for_read()
	{
		if (_InterlockedIncrement(&v_lock) > 0) return;
		do SwitchToThread(); while (v_lock <= 0);
	}
	XEMMAI__PORTABLE__FORCE_INLINE void f_release_for_read()
	{
		_InterlockedDecrement(&v_lock);
	}
	XEMMAI__PORTABLE__FORCE_INLINE void f_acquire_for_write()
	{
		while (_InterlockedCompareExchange(&v_lock, 0x80000000, 0) != 0) SwitchToThread();
	}
	XEMMAI__PORTABLE__FORCE_INLINE void f_release_for_write()
	{
		_InterlockedExchangeAdd(&v_lock, 0x80000000);
	}
};
#endif

class t_scoped_lock_for_read
{
	t_lock& v_lock;

public:
	XEMMAI__PORTABLE__FORCE_INLINE t_scoped_lock_for_read(t_lock& a_lock) : v_lock(a_lock)
	{
		v_lock.f_acquire_for_read();
	}
	XEMMAI__PORTABLE__FORCE_INLINE ~t_scoped_lock_for_read()
	{
		v_lock.f_release_for_read();
	}
};

class t_scoped_lock_for_write
{
	t_lock& v_lock;

public:
	XEMMAI__PORTABLE__FORCE_INLINE t_scoped_lock_for_write(t_lock& a_lock) : v_lock(a_lock)
	{
		v_lock.f_acquire_for_write();
	}
	XEMMAI__PORTABLE__FORCE_INLINE ~t_scoped_lock_for_write()
	{
		v_lock.f_release_for_write();
	}
};

}

}

#endif
