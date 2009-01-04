#ifndef XEMMAI__POINTER_H
#define XEMMAI__POINTER_H

#include <cassert>
#include <cstddef>

#include "portable/define.h"
#include "portable/thread.h"

namespace xemmai
{

class t_engine;
class t_object;
class t_shared;
struct t_slot;
struct t_thread;
t_engine* f_engine();

class t_pointer
{
	friend class t_engine;
	friend struct t_thread;
	friend t_engine* f_engine();

public:
	class t_collector
	{
	protected:
		bool v_collector__running;
		bool v_collector__quitting;
		portable::t_mutex v_collector__mutex;
		portable::t_condition v_collector__wake;
		portable::t_condition v_collector__done;
		size_t v_collector__tick;
		size_t v_collector__wait;
		size_t v_collector__epoch;
		size_t v_collector__skip;
		size_t v_collector__collect;

		t_collector() :
		v_collector__running(true),
		v_collector__quitting(false),
		v_collector__tick(0),
		v_collector__wait(0),
		v_collector__epoch(0),
		v_collector__skip(0),
		v_collector__collect(0)
		{
			v_collector = this;
		}
		~t_collector()
		{
			v_collector = 0;
		}

	public:
		void f_tick()
		{
			if (v_collector__running) return;
			{
				portable::t_scoped_lock lock(v_collector__mutex);
				++v_collector__tick;
				if (v_collector__running) return;
				v_collector__running = true;
			}
			v_collector__wake.f_signal();
		}
		void f_wait()
		{
			{
				portable::t_scoped_lock lock(v_collector__mutex);
				++v_collector__wait;
				if (v_collector__running) {
					v_collector__done.f_wait(v_collector__mutex);
					return;
				}
				v_collector__running = true;
			}
			v_collector__wake.f_signal();
			{
				portable::t_scoped_lock lock(v_collector__mutex);
				if (v_collector__running) v_collector__done.f_wait(v_collector__mutex);
			}
		}
	};

protected:
	template<size_t A_SIZE>
	struct t_queue
	{
		static const size_t V_SIZE = A_SIZE;

		t_object* volatile v_objects[V_SIZE];
		t_object* volatile* volatile v_head;
		t_object* volatile* v_next;
		t_object* volatile* volatile v_tail;
		t_object* volatile* v_epoch;

		t_queue() :
		v_head(v_objects),
		v_next(v_objects + V_SIZE - 1),
		v_tail(v_objects + V_SIZE - 1),
		v_epoch(v_objects)
		{
		}
		void f_next(t_object* a_object);
		XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE void f_push(t_object* a_object)
		{
			t_object* volatile* head = v_head;
			if (head == v_next) {
				f_next(a_object);
			} else {
				*head = a_object;
				v_head = ++head;
			}
		}
		void f_epoch()
		{
			v_epoch = v_head;
		}
	};
	struct t_increments : t_queue<4096>
	{
		void f_flush();
	};
	struct t_decrements : t_queue<8192>
	{
		t_object* volatile* v_last;

		t_decrements() : v_last(v_objects)
		{
		}
		void f_flush();
	};
	struct t_queues
	{
		t_collector* v_collector;
		t_increments v_increments;
		t_decrements v_decrements;

		t_queues() : v_collector(t_pointer::v_collector)
		{
		}
	};
	class t_pass
	{
	};

	static XEMMAI__PORTABLE__THREAD t_collector* v_collector;
	static XEMMAI__PORTABLE__THREAD t_increments* v_increments;
	static XEMMAI__PORTABLE__THREAD t_decrements* v_decrements;

	mutable t_object* v_p;

	t_pointer() : v_p(0)
	{
	}
	t_pointer(t_object* a_p, const t_pass&) : v_p(a_p)
	{
	}
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	t_pointer(t_object* a_p) : v_p(a_p)
	{
		if (v_p) v_increments->f_push(v_p);
	}
	t_pointer(const t_pointer& a_p) : v_p(a_p.v_p)
	{
		if (v_p) v_increments->f_push(v_p);
	}
#else
	XEMMAI__PORTABLE__EXPORT t_pointer(t_object* a_p);
	XEMMAI__PORTABLE__EXPORT t_pointer(const t_pointer& a_p);
#endif

public:
	operator t_object*() const
	{
		return v_p;
	}
	t_object& operator*() const
	{
		return *v_p;
	}
	t_object* operator->() const
	{
		return v_p;
	}
};

class t_transfer : public t_pointer
{
	friend class t_object;
	friend class t_shared;
	friend struct t_slot;

	t_transfer(t_object* a_p, const t_pass&) : t_pointer(a_p, t_pass())
	{
	}

public:
	t_transfer()
	{
	}
	t_transfer(t_object* a_p) : t_pointer(a_p)
	{
	}
	t_transfer(const t_transfer& a_p) : t_pointer(a_p, t_pass())
	{
		a_p.v_p = 0;
	}
	t_transfer(const t_shared& a_p);
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	XEMMAI__PORTABLE__ALWAYS_INLINE ~t_transfer()
	{
		if (v_p) v_decrements->f_push(v_p);
	}
	t_transfer& operator=(t_object* a_p)
	{
		if (a_p) v_increments->f_push(a_p);
		t_object* p = v_p;
		v_p = a_p;
		if (p) v_decrements->f_push(p);
		return *this;
	}
	t_transfer& operator=(const t_transfer& a_p)
	{
		t_object* p = v_p;
		v_p = a_p;
		a_p.v_p = 0;
		if (p) v_decrements->f_push(p);
		return *this;
	}
#else
	XEMMAI__PORTABLE__EXPORT ~t_transfer();
	XEMMAI__PORTABLE__EXPORT t_transfer& operator=(t_object* a_p);
	XEMMAI__PORTABLE__EXPORT t_transfer& operator=(const t_transfer& a_p);
#endif
	t_transfer& operator=(const t_shared& a_p);
};

class t_shared : public t_pointer
{
protected:
	t_shared()
	{
	}
	t_shared(t_object* a_p) : t_pointer(a_p)
	{
	}
	t_shared(const t_transfer& a_p) : t_pointer(a_p, t_pass())
	{
		a_p.v_p = 0;
	}
	t_shared(const t_shared& a_p) : t_pointer(a_p)
	{
	}

public:
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	XEMMAI__PORTABLE__ALWAYS_INLINE t_shared& operator=(t_object* a_p)
	{
		if (a_p) v_increments->f_push(a_p);
		t_object* p = v_p;
		v_p = a_p;
		if (p) v_decrements->f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_shared& operator=(const t_transfer& a_p)
	{
		t_object* p = v_p;
		v_p = a_p;
		a_p.v_p = 0;
		if (p) v_decrements->f_push(p);
		return *this;
	}
#else
	XEMMAI__PORTABLE__EXPORT t_shared& operator=(t_object* a_p);
	XEMMAI__PORTABLE__EXPORT t_shared& operator=(const t_transfer& a_p);
#endif
	XEMMAI__PORTABLE__ALWAYS_INLINE t_shared& operator=(const t_shared& a_p)
	{
		return operator=(static_cast<t_object*>(a_p));
	}
	t_transfer f_transfer()
	{
		t_object* p = v_p;
		v_p = 0;
		return t_transfer(p, t_pass());
	}
};

struct t_scoped : t_shared
{
	t_scoped()
	{
	}
	t_scoped(t_object* a_p) : t_shared(a_p)
	{
	}
	t_scoped(const t_transfer& a_p) : t_shared(a_p)
	{
	}
	t_scoped(const t_shared& a_p) : t_shared(a_p)
	{
	}
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	~t_scoped()
	{
		if (v_p) v_decrements->f_push(v_p);
	}
#else
	XEMMAI__PORTABLE__EXPORT ~t_scoped();
#endif
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_object* a_p)
	{
		t_shared::operator=(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_transfer& a_p)
	{
		t_shared::operator=(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_shared& a_p)
	{
		return operator=(static_cast<t_object*>(a_p));
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_scoped& a_p)
	{
		return operator=(static_cast<t_object*>(a_p));
	}
};

struct t_slot : t_shared
{
	friend class t_object;

	t_slot()
	{
	}
	t_slot(t_object* a_p) : t_shared(a_p)
	{
	}
	t_slot(const t_transfer& a_p) : t_shared(a_p)
	{
	}
	t_slot(const t_shared& a_p) : t_shared(a_p)
	{
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(t_object* a_p)
	{
		t_shared::operator=(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_transfer& a_p)
	{
		t_shared::operator=(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_shared& a_p)
	{
		return operator=(static_cast<t_object*>(a_p));
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_slot& a_p)
	{
		return operator=(static_cast<t_object*>(a_p));
	}
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	void f_construct(t_object* a_p)
	{
		assert(!v_p);
		if (a_p) v_increments->f_push(a_p);
		v_p = a_p;
	}
#else
	XEMMAI__PORTABLE__EXPORT void f_construct(t_object* a_p);
#endif
	void f_construct(const t_transfer& a_p)
	{
		assert(!v_p);
		v_p = a_p;
		a_p.v_p = 0;
	}
};

typedef void (*t_scan)(t_slot&);

template<size_t A_SIZE>
void t_pointer::t_queue<A_SIZE>::f_next(t_object* a_object)
{
	v_collector->f_tick();
	while (v_head == v_tail) v_collector->f_wait();
	v_next = v_tail;
	t_object* volatile* head = v_head;
	if (head < v_objects + V_SIZE - 1) {
		if (v_next < head) v_next = v_objects + V_SIZE - 1;
		*head = a_object;
		v_head = ++head;
	} else {
		*head = a_object;
		v_head = v_objects;
	}
}

inline t_transfer::t_transfer(const t_shared& a_p) : t_pointer(a_p)
{
}

inline t_transfer& t_transfer::operator=(const t_shared& a_p)
{
	return operator=(static_cast<t_object*>(a_p));
}

}

#endif
