#ifndef XEMMAI__THREAD_H
#define XEMMAI__THREAD_H

#include "object.h"

namespace xemmai
{

struct t_thread
{
	struct t_queues : t_pointer::t_queues
	{
		t_queues* v_next;
		size_t v_done;
		t_object* volatile* v_reviving;

		t_queues() : v_next(0), v_done(0), v_reviving(0)
		{
		}
		void f_revive()
		{
			v_reviving = v_increments.v_head;
		}
	};

	static XEMMAI__PORTABLE__THREAD t_object* v_current;

	static void* f_main(void* a_p);
	static t_object* f_current()
	{
		return v_current;
	}
	static t_transfer f_instantiate(const t_transfer& a_callable);
	static void f_define(t_object* a_class);

	t_queues* v_queues;
	t_slot v_fiber;
	t_scoped v_active;

	t_thread(const t_transfer& a_fiber) : v_queues(new t_queues()), v_fiber(a_fiber)
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
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

#endif
