#ifndef XEMMAI__ATOMIC_H
#define XEMMAI__ATOMIC_H

#include <cstddef>
#include <atomic>
#include <thread>

#include "portable/define.h"

namespace xemmai
{

inline size_t f_atomic_increment(volatile size_t& a_n)
{
	return ++reinterpret_cast<volatile std::atomic<size_t>&>(a_n);
}

class t_lock
{
	std::atomic<ptrdiff_t> v_lock{0};

public:
	void f_acquire_for_read()
	{
		if (v_lock.fetch_add(1, std::memory_order_acquire) >= 0) return;
		do std::this_thread::yield(); while (v_lock.load(std::memory_order_acquire) <= 0);
	}
	void f_release_for_read()
	{
		v_lock.fetch_sub(1, std::memory_order_release);
	}
	void f_acquire_for_write()
	{
		while (true) {
			ptrdiff_t value = 0;
			if (v_lock.compare_exchange_weak(value, ~(~size_t(0) >> 1), std::memory_order_acquire, std::memory_order_relaxed)) break;
			do std::this_thread::yield(); while (v_lock.load(std::memory_order_relaxed) != 0);
		}
	}
	void f_release_for_write()
	{
		v_lock.fetch_add(~(~size_t(0) >> 1), std::memory_order_release);
	}
};

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

#endif
