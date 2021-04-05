#ifndef XEMMAI__THREAD_H
#define XEMMAI__THREAD_H

#include "fiber.h"

namespace xemmai
{

struct t_thread
{
	struct t_cache
	{
		static const size_t V_SIZE = 1 << 11;
		static const size_t V_MASK = V_SIZE - 1;

		static inline volatile size_t v_revisions[V_SIZE];

		static size_t f_index(const t_pvalue& a_object, t_object* a_key)
		{
			return (a_object.f_tag() ^ reinterpret_cast<size_t>(a_key)) / sizeof(t_object*) & V_MASK;
		}
		static size_t f_revise(size_t a_i);

		t_slot v_object;
		t_slot v_key;
		t_svalue v_value;
		volatile size_t v_revision = 0;
		volatile size_t v_key_revision = 0;
	};
	struct t_internal
	{
		t_internal* v_next;
		int v_done = -1;
		t_slot::t_increments v_increments;
		t_slot::t_decrements v_decrements;
		t_thread* v_thread;
		std::mutex v_mutex;
		t_fiber::t_internal* v_fibers = nullptr;
		t_fiber* v_active;
#ifdef __unix__
		pthread_t v_handle;
#endif
#ifdef _WIN32
		HANDLE v_handle = NULL;
#endif
		t_object* volatile* v_reviving = nullptr;
		t_structure::t_cache v_index_cache[t_structure::t_cache::V_SIZE];
		t_cache v_cache[t_cache::V_SIZE];
		size_t v_cache_hit = 0;
		size_t v_cache_missed = 0;

#ifdef _WIN32
		~t_internal()
		{
			if (v_handle != NULL) CloseHandle(v_handle);
		}
#endif
		void f_initialize();
		void f_initialize(t_thread* a_thread, void* a_bottom);
		void f_epoch_get()
		{
			v_active->v_internal->f_epoch_get();
			v_increments.v_epoch.store(v_increments.v_head, std::memory_order_release);
			v_decrements.v_epoch.store(v_decrements.v_head, std::memory_order_release);
		}
		void f_epoch_suspend();
		void f_epoch_resume();
		void f_epoch();
		void f_revive()
		{
			v_reviving = v_increments.v_head;
		}
	};

	static inline XEMMAI__PORTABLE__THREAD t_internal* v_current;
	static inline XEMMAI__PORTABLE__THREAD t_cache* v_cache;
	static inline XEMMAI__PORTABLE__THREAD size_t v_cache_hit;
	static inline XEMMAI__PORTABLE__THREAD size_t v_cache_missed;

	static t_object* f_current()
	{
		return t_object::f_of(v_current->v_thread);
	}
	static void f_cache_clear();
	static XEMMAI__PORTABLE__EXPORT void f_cache_acquire();
	static void f_cache_release()
	{
	}
	static t_object* f_instantiate(const t_pvalue& a_callable, size_t a_stack);

	t_internal* v_internal;
	t_slot v_fiber;

	t_thread(t_internal* a_internal, t_object* a_fiber) : v_internal(a_internal), v_fiber(a_fiber)
	{
	}
	void f_join();
};

template<>
struct t_type_of<t_thread> : t_underivable<t_fixed<t_holds<t_thread>>>
{
	void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_thread>().v_fiber);
	}
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
};

inline t_object* t_fiber::f_current()
{
	return t_object::f_of(t_thread::v_current->v_active);
}

}

#endif
