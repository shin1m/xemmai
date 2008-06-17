#ifndef XEMMAI__POINTER_H
#define XEMMAI__POINTER_H

#include <cassert>
#include <cstddef>

#include "portable/define.h"

namespace xemmai
{

class t_engine;
class t_object;
class t_shared;
struct t_slot;
struct t_thread;

class t_pointer
{
	friend class t_engine;
	friend struct t_thread;

protected:
	struct t_queue
	{
		static const size_t V_SIZE = 4096;

		t_object* volatile v_objects[V_SIZE];
		t_object* volatile* volatile v_head;
		t_object* volatile* volatile v_tail;
		t_object* volatile* v_next;
		t_object* volatile* v_epoch;

		t_queue() : v_head(v_objects), v_tail(v_objects + V_SIZE - 1), v_next(v_objects + V_SIZE - 1), v_epoch(v_objects)
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
		void f_flush();
	};
	struct t_decrements : t_queue
	{
		t_object* volatile* v_last;

		t_decrements() : v_last(v_objects)
		{
		}
		void f_flush();
	};
	struct t_queues
	{
		t_queue v_increments;
		t_decrements v_decrements;
	};
	class t_pass
	{
	};

	static XEMMAI__PORTABLE__THREAD t_queue* v_increments;
	static XEMMAI__PORTABLE__THREAD t_queue* v_decrements;

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

inline t_transfer::t_transfer(const t_shared& a_p) : t_pointer(a_p)
{
}

inline t_transfer& t_transfer::operator=(const t_shared& a_p)
{
	return operator=(static_cast<t_object*>(a_p));
}

}

#endif
