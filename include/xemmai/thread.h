#ifndef XEMMAI__THREAD_H
#define XEMMAI__THREAD_H

#include "object.h"

namespace xemmai
{

struct t_thread
{
	struct t_cache
	{
		static const size_t V_SIZE = 1 << 11;
		static const size_t V_MASK = V_SIZE - 1;

		static volatile size_t v_revisions[V_SIZE];

		static size_t f_index(const t_value& a_object, t_object* a_key)
		{
			return (a_object.f_tag() ^ reinterpret_cast<size_t>(a_key)) / sizeof(t_object*) & V_MASK;
		}
		static size_t f_revise(size_t a_i);

		t_slot v_object;
		t_slot v_key;
		t_slot v_value;
		volatile size_t v_revision = 0;
		volatile size_t v_key_revision = 0;
	};
	struct t_internal
	{
		t_internal* v_next = nullptr;
		size_t v_done = 0;
		t_value::t_collector* v_collector;
		t_value::t_increments v_increments;
		t_value::t_decrements v_decrements;
		t_object* volatile* v_reviving = nullptr;
		t_structure::t_cache v_index_cache[t_structure::t_cache::V_SIZE];
		t_cache v_cache[t_cache::V_SIZE];
		size_t v_cache_hit = 0;
		size_t v_cache_missed = 0;
		t_object* v_thread;

		t_internal() : v_collector(t_value::v_collector)
		{
		}
		void f_initialize()
		{
			t_value::v_increments = &v_increments;
			t_value::v_decrements = &v_decrements;
			t_structure::v_cache = v_index_cache;
			t_thread::v_cache = v_cache;
		}
		void f_revive()
		{
			v_reviving = v_increments.v_head;
		}
	};

	static XEMMAI__PORTABLE__THREAD t_object* v_current;
	static XEMMAI__PORTABLE__THREAD t_cache* v_cache;
	static XEMMAI__PORTABLE__THREAD size_t v_cache_hit;
	static XEMMAI__PORTABLE__THREAD size_t v_cache_missed;

	static void f_main(t_object* a_p);
	static t_object* f_current()
	{
		return v_current;
	}
	static void f_cache_clear();
	static XEMMAI__PORTABLE__EXPORT void f_cache_acquire();
	static void f_cache_release()
	{
	}
	static t_scoped f_instantiate(t_scoped&& a_callable, size_t a_stack);
	static void f_define(t_object* a_class);

	t_internal* v_internal;
	t_slot v_fiber;
	t_scoped v_active;

	t_thread(t_scoped&& a_fiber) : v_internal(new t_internal()), v_fiber(std::move(a_fiber))
	{
	}
	void f_join();
};

template<>
struct t_type_of<t_thread> : t_type
{
	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_fixed = v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
