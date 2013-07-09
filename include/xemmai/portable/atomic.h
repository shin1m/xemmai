#ifndef XEMMAI__PORTABLE__ATOMIC_H
#define XEMMAI__PORTABLE__ATOMIC_H

#include "define.h"

#if defined(__unix__) && (defined(__i386__) || defined(__x86_64__)) && defined(__GNUC__)
#include <pthread.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#endif

namespace xemmai
{

namespace portable
{

#if 0
inline size_t f_atomic_increment(volatile size_t& a_n)
{
	return ++a_n;
}

class t_lock
{
	volatile size_t v_lock = 0;

public:
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
		v_lock += ~(~size_t(0) >> 1);
	}
	void f_release_for_write()
	{
		v_lock -= ~(~size_t(0) >> 1);
	}
};
#endif

#if defined(__unix__) && (defined(__i386__) || defined(__x86_64__)) && defined(__GNUC__)
inline size_t f_atomic_increment(volatile size_t& a_n)
{
	size_t value;
	asm volatile (
		"lock; xadd %0, %1"
		: "=r" (value)
		: "m" (a_n), "0" (1)
		: "memory", "cc"
	);
	return ++value;
}

class t_lock
{
	volatile std::ptrdiff_t v_lock = 0;

public:
	void f_acquire_for_read()
	{
		std::ptrdiff_t value;
		asm volatile (
			"lock; xadd %0, %1"
			: "=r" (value)
			: "m" (v_lock), "0" (1)
			: "memory", "cc"
		);
		if (value >= 0) return;
		do sched_yield(); while (v_lock <= 0);
	}
	void f_release_for_read()
	{
		asm volatile (
#ifdef __x86_64__
			"lock; decq %0"
#else
			"lock; decl %0"
#endif
			:
			: "m" (v_lock)
			: "memory", "cc"
		);
	}
	void f_acquire_for_write()
	{
		while (true) {
			std::ptrdiff_t value;
			asm volatile (
				"lock; cmpxchg %1, %2"
				: "=a" (value)
				: "r" (~(~size_t(0) >> 1)), "m" (v_lock), "0" (0)
				: "memory", "cc"
			);
			if (value == 0) break;
			do sched_yield(); while (v_lock != 0);
		}
	}
	void f_release_for_write()
	{
		asm volatile (
			"lock; add %0, %1"
			:
			: "r" (~(~size_t(0) >> 1)), "m" (v_lock)
			: "memory", "cc"
		);
	}
};
#endif

#ifdef _MSC_VER
inline size_t f_atomic_increment(volatile size_t& a_n)
{
	return _InterlockedIncrement(reinterpret_cast<volatile long*>(&a_n));
}

class t_lock
{
	volatile long v_lock = 0;

public:
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
		while (_InterlockedCompareExchange(&v_lock, ~(~size_t(0) >> 1), 0) != 0) SwitchToThread();
	}
	XEMMAI__PORTABLE__FORCE_INLINE void f_release_for_write()
	{
		_InterlockedExchangeAdd(&v_lock, ~(~size_t(0) >> 1));
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
