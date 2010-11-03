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
		bool v_modified;
		volatile size_t v_revision;
		volatile size_t v_key_revision;

		t_cache() : v_modified(false), v_revision(0), v_key_revision(0)
		{
		}
	};
	struct t_internal
	{
		t_internal* v_next;
		size_t v_done;
		t_value::t_collector* v_collector;
		t_value::t_increments v_increments;
		t_value::t_decrements v_decrements;
		t_object* volatile* v_reviving;
		t_cache v_cache[t_cache::V_SIZE];

		t_internal() : v_next(0), v_done(0), v_collector(t_value::v_collector), v_reviving(0)
		{
		}
		void f_initialize()
		{
			t_value::v_increments = &v_increments;
			t_value::v_decrements = &v_decrements;
			t_thread::v_cache = v_cache;
		}
		void f_revive()
		{
			v_reviving = v_increments.v_head;
		}
	};

	static XEMMAI__PORTABLE__THREAD t_object* v_current;
	static XEMMAI__PORTABLE__THREAD t_cache* v_cache;

	static void* f_main(void* a_p);
	static t_object* f_current()
	{
		return v_current;
	}
	static void f_cache_clear();
	static void f_cache_acquire();
	static void f_cache_release();
	static t_transfer f_instantiate(const t_transfer& a_callable, size_t a_stack);
	static void f_define(t_object* a_class);

	t_internal* v_internal;
	t_slot v_fiber;
	t_scoped v_active;

	t_thread(const t_transfer& a_fiber) : v_internal(new t_internal()), v_fiber(a_fiber)
	{
	}
	void f_join();
};

template<>
struct t_type_of<t_thread> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n);
};

}

#endif
