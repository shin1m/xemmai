#ifndef XEMMAI__PORTABLE__THREAD_H
#define XEMMAI__PORTABLE__THREAD_H

#ifdef __unix__
#include <pthread.h>
#include <sched.h>
#endif
#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#define NOMINMAX
#include <windows.h>
#include <process.h>
#endif

namespace xemmai
{

namespace portable
{

class t_condition;

#ifdef __unix__
inline bool f_thread(void* (*a_main)(void*), void* a_p)
{
	pthread_t thread;
	pthread_attr_t attributes;
	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
	int n = pthread_create(&thread, &attributes, a_main, a_p);
	pthread_attr_destroy(&attributes);
	return n == 0;
}

class t_mutex
{
	friend class t_condition;

	pthread_mutex_t v_mutex;

public:
	t_mutex()
	{
		pthread_mutex_init(&v_mutex, NULL);
	}
	~t_mutex()
	{
		pthread_mutex_destroy(&v_mutex);
	}
	void f_acquire()
	{
		pthread_mutex_lock(&v_mutex);
	}
	void f_release()
	{
		pthread_mutex_unlock(&v_mutex);
	}
};

class t_condition
{
	pthread_cond_t v_condition;

public:
	t_condition()
	{
		pthread_cond_init(&v_condition, NULL);
	}
	~t_condition()
	{
		pthread_cond_destroy(&v_condition);
	}
	void f_wait(t_mutex& a_mutex)
	{
		pthread_cond_wait(&v_condition, &a_mutex.v_mutex);
	}
	void f_signal()
	{
		pthread_cond_signal(&v_condition);
	}
	void f_broadcast()
	{
		pthread_cond_broadcast(&v_condition);
	}
};

class t_affinity
{
	cpu_set_t v_set;

public:
	static const size_t V_SIZE = CPU_SETSIZE;

	void f_from_process();
	void f_to_thread();
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
bool f_thread(void* (*a_main)(void*), void* a_p);

class t_mutex
{
	CRITICAL_SECTION v_cs;

public:
	t_mutex()
	{
		InitializeCriticalSection(&v_cs);
	}
	~t_mutex()
	{
		DeleteCriticalSection(&v_cs);
	}
	void f_acquire()
	{
		EnterCriticalSection(&v_cs);
	}
	void f_release()
	{
		LeaveCriticalSection(&v_cs);
	}
};

class t_condition
{
	HANDLE v_handle;
	volatile long v_wait;
	volatile long v_signal;
	volatile long v_wake;

public:
	t_condition() : v_handle(CreateEvent(NULL, TRUE, FALSE, NULL)), v_wait(0), v_signal(0), v_wake(0)
	{
	}
	~t_condition()
	{
		CloseHandle(v_handle);
	}
	void f_wait(t_mutex& a_mutex)
	{
		long wait = ++v_wait;
		a_mutex.f_release();
		while (true) {
			WaitForSingleObject(v_handle, INFINITE);
			if (wait - v_signal <= 0) break;
			SwitchToThread();
		}
		a_mutex.f_acquire();
		if (--v_wake <= 0) ResetEvent(v_handle);
	}
	void f_signal()
	{
		long signal = v_signal;
		if (v_wait - signal <= 0) return;
		v_signal = ++signal;
		++v_wake;
		SetEvent(v_handle);
	}
	void f_broadcast()
	{
		long wait = v_wait;
		long signal = v_signal;
		if (wait - signal <= 0) return;
		v_signal = wait;
		v_wake += wait - signal;
		SetEvent(v_handle);
	}
};

class t_affinity
{
	DWORD_PTR v_mask;

public:
	static const size_t V_SIZE = sizeof(DWORD_PTR) * 8;

	void f_from_process()
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

class t_scoped_lock
{
	t_mutex& v_mutex;

public:
	t_scoped_lock(t_mutex& a_mutex) : v_mutex(a_mutex)
	{
		v_mutex.f_acquire();
	}
	~t_scoped_lock()
	{
		v_mutex.f_release();
	}
};

}

}

#endif
