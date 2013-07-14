#ifndef XEMMAI__VALUE_H
#define XEMMAI__VALUE_H

#include <cassert>
#include <cstddef>
#include <condition_variable>
#include <mutex>
#include <new>
#include <thread>

#include "portable/define.h"

namespace xemmai
{

template<typename T>
struct t_type_of;
class t_engine;
class t_object;
class t_transfer;
class t_shared;
struct t_slot;
struct t_thread;
struct t_code;
class t_parser;
t_engine* f_engine();

class t_value
{
	template<typename T>
	friend struct t_type_of;
	friend class t_engine;
	friend class t_object;
	friend struct t_slot;
	friend struct t_thread;
	friend struct t_code;
	friend class t_parser;
	friend t_engine* f_engine();

public:
	class t_collector
	{
	protected:
		bool v_collector__running = true;
		bool v_collector__quitting = false;
		std::mutex v_collector__mutex;
		std::condition_variable v_collector__wake;
		std::condition_variable v_collector__done;
		size_t v_collector__tick = 0;
		size_t v_collector__wait = 0;
		size_t v_collector__epoch = 0;
		size_t v_collector__skip = 0;
		size_t v_collector__collect = 0;

		t_collector()
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
				std::lock_guard<std::mutex> lock(v_collector__mutex);
				++v_collector__tick;
				if (v_collector__running) return;
				v_collector__running = true;
			}
			v_collector__wake.notify_one();
		}
		void f_wait()
		{
			{
				std::unique_lock<std::mutex> lock(v_collector__mutex);
				++v_collector__wait;
				if (v_collector__running) {
					v_collector__done.wait(lock);
					return;
				}
				v_collector__running = true;
			}
			v_collector__wake.notify_one();
			{
				std::unique_lock<std::mutex> lock(v_collector__mutex);
				if (v_collector__running) v_collector__done.wait(lock);
			}
		}
	};
	enum t_tag
	{
		e_tag__NULL,
		e_tag__BOOLEAN,
		e_tag__INTEGER,
		e_tag__FLOAT,
		e_tag__OBJECT
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
		void f_next(t_object* a_object) noexcept;
		XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE void f_push(t_object* a_object)
//		XEMMAI__PORTABLE__FORCE_INLINE void f_push(t_object* a_object)
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
	struct t_increments : t_queue<16384>
	{
		void f_flush();
	};
	struct t_decrements : t_queue<32768>
	{
		t_object* volatile* v_last;

		t_decrements() : v_last(v_objects)
		{
		}
		void f_flush();
	};
	class t_own
	{
	};
	class t_pass
	{
	};

	static XEMMAI__PORTABLE__THREAD t_collector* v_collector;
	static XEMMAI__PORTABLE__THREAD t_increments* v_increments;
	static XEMMAI__PORTABLE__THREAD t_decrements* v_decrements;

	mutable t_object* v_p;
	union
	{
		bool v_boolean;
		ptrdiff_t v_integer;
		double v_float;
		void* v_pointer;
	};

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	t_value(t_object* a_p, const t_own&) : v_p(a_p)
	{
		if (v_p) v_increments->f_push(v_p);
	}
	t_value(const t_value& a_value, const t_own&) : v_p(a_value.v_p)
	{
		switch (f_tag()) {
		case e_tag__NULL:
			break;
		case e_tag__BOOLEAN:
			v_boolean = a_value.v_boolean;
			break;
		case e_tag__INTEGER:
			v_integer = a_value.v_integer;
			break;
		case e_tag__FLOAT:
			v_float = a_value.v_float;
			break;
		default:
			v_increments->f_push(v_p);
		}
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(t_object* a_p)
	{
		if (a_p) v_increments->f_push(a_p);
		t_object* p = v_p;
		v_p = a_p;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) v_decrements->f_push(p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(const t_value& a_value)
	{
		switch (a_value.f_tag()) {
		case e_tag__NULL:
			break;
		case e_tag__BOOLEAN:
			v_boolean = a_value.v_boolean;
			break;
		case e_tag__INTEGER:
			v_integer = a_value.v_integer;
			break;
		case e_tag__FLOAT:
			v_float = a_value.v_float;
			break;
		default:
			v_increments->f_push(a_value.v_p);
		}
		t_object* p = v_p;
		v_p = a_value.v_p;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) v_decrements->f_push(p);
	}
	void f_assign(const t_transfer& a_value);
#else
	XEMMAI__PORTABLE__EXPORT t_value(t_object* a_p, const t_own&);
	XEMMAI__PORTABLE__EXPORT t_value(const t_value& a_value, const t_own&);
	XEMMAI__PORTABLE__EXPORT void f_assign(t_object* a_p);
	XEMMAI__PORTABLE__EXPORT void f_assign(const t_value& a_value);
	XEMMAI__PORTABLE__EXPORT void f_assign(const t_transfer& a_value);
#endif

public:
	t_value(t_object* a_p = 0) : v_p(a_p)
	{
	}
	explicit t_value(bool a_value) : v_p(reinterpret_cast<t_object*>(e_tag__BOOLEAN)), v_boolean(a_value)
	{
	}
	explicit t_value(short a_value) : v_p(reinterpret_cast<t_object*>(e_tag__INTEGER)), v_integer(a_value)
	{
	}
	explicit t_value(unsigned short a_value) : v_p(reinterpret_cast<t_object*>(e_tag__INTEGER)), v_integer(a_value)
	{
	}
	explicit t_value(int a_value) : v_p(reinterpret_cast<t_object*>(e_tag__INTEGER)), v_integer(a_value)
	{
	}
	explicit t_value(unsigned a_value) : v_p(reinterpret_cast<t_object*>(e_tag__INTEGER)), v_integer(a_value)
	{
	}
	explicit t_value(long a_value) : v_p(reinterpret_cast<t_object*>(e_tag__INTEGER)), v_integer(a_value)
	{
	}
	explicit t_value(unsigned long a_value) : v_p(reinterpret_cast<t_object*>(e_tag__INTEGER)), v_integer(a_value)
	{
	}
	explicit t_value(double a_value) : v_p(reinterpret_cast<t_object*>(e_tag__FLOAT)), v_float(a_value)
	{
	}
	t_value(const t_value& a_value) : v_p(a_value.v_p)
	{
		switch (f_tag()) {
		case e_tag__BOOLEAN:
			v_boolean = a_value.v_boolean;
			break;
		case e_tag__INTEGER:
			v_integer = a_value.v_integer;
			break;
		case e_tag__FLOAT:
			v_float = a_value.v_float;
			break;
		}
	}
	bool operator==(const t_value& a_value) const
	{
		if (v_p != a_value.v_p) return false;
		switch (f_tag()) {
		case e_tag__BOOLEAN:
			return v_boolean == a_value.v_boolean;
		case e_tag__INTEGER:
			return v_integer == a_value.v_integer;
		case e_tag__FLOAT:
			return v_float == a_value.v_float;
		default:
			return true;
		}
	}
	bool operator!=(const t_value& a_value) const
	{
		return !operator==(a_value);
	}
	operator t_object*() const
	{
		return v_p;
	}
	size_t f_tag() const
	{
		return reinterpret_cast<size_t>(v_p);
	}
	bool f_boolean() const;
	void f_boolean__(bool a_value);
	ptrdiff_t f_integer() const;
	void f_integer__(ptrdiff_t a_value);
	double f_float() const;
	void f_float__(double a_value);
	void f_pointer__(void* a_value);
	t_object* f_type() const;
	bool f_is(t_object* a_class) const;
	t_transfer f_get(t_object* a_key) const;
	void f_put(t_object* a_key, const t_transfer& a_value) const;
	bool f_has(t_object* a_key) const;
	t_transfer f_remove(t_object* a_key) const;
	void f_call(const t_value& a_self, t_slot* a_stack, size_t a_n) const;
	void f_call_and_return(const t_value& a_self, t_slot* a_stack, size_t a_n) const;
	t_transfer f_call_with_same(t_slot* a_stack, size_t a_n) const;
	t_transfer f_hash() const;
	template<typename... T>
	t_transfer operator()(const T&... a_arguments) const;
	t_transfer f_get_at(const t_value& a_index) const;
	t_transfer f_set_at(const t_value& a_index, const t_value& a_value) const;
	t_transfer f_plus() const;
	t_transfer f_minus() const;
	t_transfer f_not() const;
	t_transfer f_complement() const;
	t_transfer f_multiply(const t_value& a_value) const;
	t_transfer f_divide(const t_value& a_value) const;
	t_transfer f_modulus(const t_value& a_value) const;
	t_transfer f_add(const t_value& a_value) const;
	t_transfer f_subtract(const t_value& a_value) const;
	t_transfer f_left_shift(const t_value& a_value) const;
	t_transfer f_right_shift(const t_value& a_value) const;
	t_transfer f_less(const t_value& a_value) const;
	t_transfer f_less_equal(const t_value& a_value) const;
	t_transfer f_greater(const t_value& a_value) const;
	t_transfer f_greater_equal(const t_value& a_value) const;
	t_transfer f_equals(const t_value& a_value) const;
	t_transfer f_not_equals(const t_value& a_value) const;
	t_transfer f_and(const t_value& a_value) const;
	t_transfer f_xor(const t_value& a_value) const;
	t_transfer f_or(const t_value& a_value) const;
	t_transfer f_send(const t_value& a_value) const;
};

class t_transfer : public t_value
{
	friend class t_object;
	friend class t_shared;
	friend struct t_slot;

	t_transfer(t_object* a_p, const t_pass&) : t_value(a_p)
	{
	}
	t_transfer(const t_value& a_value, const t_pass&) : t_value(a_value)
	{
	}

public:
	t_transfer(t_object* a_p = 0) : t_value(a_p, t_own())
	{
	}
	explicit t_transfer(bool a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(short a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(unsigned short a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(int a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(unsigned a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(long a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(unsigned long a_value) : t_value(a_value)
	{
	}
	explicit t_transfer(double a_value) : t_value(a_value)
	{
	}
	t_transfer(const t_value& a_value) : t_value(a_value, t_own())
	{
	}
	t_transfer(const t_transfer& a_value) : t_value(a_value)
	{
		a_value.v_p = 0;
	}
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	XEMMAI__PORTABLE__ALWAYS_INLINE ~t_transfer()
	{
		if (f_tag() >= e_tag__OBJECT) v_decrements->f_push(v_p);
	}
#else
	XEMMAI__PORTABLE__EXPORT ~t_transfer();
#endif
	t_transfer& operator=(t_object* a_p)
	{
		f_assign(a_p);
		return *this;
	}
	t_transfer& operator=(const t_value& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	t_transfer& operator=(const t_transfer& a_value)
	{
		f_assign(a_value);
		return *this;
	}
};

class t_shared : public t_value
{
protected:
	t_shared(t_object* a_p) : t_value(a_p, t_own())
	{
	}
	t_shared(bool a_value) : t_value(a_value)
	{
	}
	t_shared(short a_value) : t_value(a_value)
	{
	}
	t_shared(unsigned short a_value) : t_value(a_value)
	{
	}
	t_shared(int a_value) : t_value(a_value)
	{
	}
	t_shared(unsigned a_value) : t_value(a_value)
	{
	}
	t_shared(long a_value) : t_value(a_value)
	{
	}
	t_shared(unsigned long a_value) : t_value(a_value)
	{
	}
	t_shared(double a_value) : t_value(a_value)
	{
	}
	t_shared(const t_value& a_value) : t_value(a_value, t_own())
	{
	}
	t_shared(const t_transfer& a_value) : t_value(a_value)
	{
		a_value.v_p = 0;
	}
	t_shared(const t_shared& a_value) : t_value(a_value, t_own())
	{
	}

public:
	XEMMAI__PORTABLE__ALWAYS_INLINE t_transfer f_transfer()
	{
		t_value p = *this;
		v_p = 0;
		return t_transfer(p, t_pass());
	}
};

struct t_scoped : t_shared
{
	t_scoped(t_object* a_p = 0) : t_shared(a_p)
	{
	}
	explicit t_scoped(bool a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(short a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(unsigned short a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(int a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(unsigned a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(long a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(unsigned long a_value) : t_shared(a_value)
	{
	}
	explicit t_scoped(double a_value) : t_shared(a_value)
	{
	}
	t_scoped(const t_value& a_value) : t_shared(a_value)
	{
	}
	t_scoped(const t_transfer& a_value) : t_shared(a_value)
	{
	}
	t_scoped(const t_scoped& a_value) : t_shared(a_value)
	{
	}
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	~t_scoped()
	{
		if (f_tag() >= e_tag__OBJECT) v_decrements->f_push(v_p);
	}
#else
	XEMMAI__PORTABLE__EXPORT ~t_scoped();
#endif
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_object* a_p)
	{
		f_assign(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_value& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_transfer& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_scoped& a_value)
	{
		f_assign(a_value);
		return *this;
	}
};

struct t_slot : t_shared
{
	friend class t_object;

	t_slot(t_object* a_p = 0) : t_shared(a_p)
	{
	}
	explicit t_slot(bool a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(short a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(unsigned short a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(int a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(unsigned a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(long a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(unsigned long a_value) : t_shared(a_value)
	{
	}
	explicit t_slot(double a_value) : t_shared(a_value)
	{
	}
	t_slot(const t_value& a_value) : t_shared(a_value)
	{
	}
	t_slot(const t_transfer& a_value) : t_shared(a_value)
	{
	}
	t_slot(const t_slot& a_value) : t_shared(a_value)
	{
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(t_object* a_p)
	{
		f_assign(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_value& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_transfer& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_slot& a_value)
	{
		f_assign(a_value);
		return *this;
	}
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	void f_construct(t_object* a_p = 0)
	{
		assert(!v_p);
		if (a_p) v_increments->f_push(a_p);
		v_p = a_p;
	}
	void f_construct(const t_value& a_value)
	{
		assert(!v_p);
		switch (a_value.f_tag()) {
		case e_tag__NULL:
			break;
		case e_tag__BOOLEAN:
			v_boolean = a_value.v_boolean;
			break;
		case e_tag__INTEGER:
			v_integer = a_value.v_integer;
			break;
		case e_tag__FLOAT:
			v_float = a_value.v_float;
			break;
		default:
			v_increments->f_push(a_value.v_p);
		}
		v_p = a_value.v_p;
	}
#else
	XEMMAI__PORTABLE__EXPORT void f_construct(t_object* a_p = 0);
	XEMMAI__PORTABLE__EXPORT void f_construct(const t_value& a_value);
#endif
	void f_construct(bool a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__BOOLEAN);
		v_boolean = a_value;
	}
	void f_construct(short a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(unsigned short a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(int a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(unsigned a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(long a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(unsigned long a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(double a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__FLOAT);
		v_float = a_value;
	}
	void f_construct(const t_transfer& a_value)
	{
		assert(!v_p);
		v_p = a_value.v_p;
		switch (f_tag()) {
		case e_tag__BOOLEAN:
			v_boolean = a_value.v_boolean;
			break;
		case e_tag__INTEGER:
			v_integer = a_value.v_integer;
			break;
		case e_tag__FLOAT:
			v_float = a_value.v_float;
			break;
		}
		a_value.v_p = 0;
	}
};

typedef void (*t_scan)(t_slot&);

template<size_t A_SIZE>
void t_value::t_queue<A_SIZE>::f_next(t_object* a_object) noexcept
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

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline void t_value::f_assign(const t_transfer& a_value)
{
	t_object* p = v_p;
	v_p = a_value.v_p;
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		v_boolean = a_value.v_boolean;
		break;
	case e_tag__INTEGER:
		v_integer = a_value.v_integer;
		break;
	case e_tag__FLOAT:
		v_float = a_value.v_float;
		break;
	}
	a_value.v_p = 0;
	if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) v_decrements->f_push(p);
}
#endif

struct t_stack
{
	static XEMMAI__PORTABLE__THREAD t_stack* v_instance;

	size_t v_size;
	char* v_head;
	t_slot* v_tail;
	t_slot* v_used;

	t_stack(size_t a_size) : v_size(a_size), v_head(new char[sizeof(t_value) * v_size]), v_tail(f_head()), v_used(v_tail)
	{
	}
	~t_stack()
	{
		delete[] v_head;
	}
	t_slot* f_head() const
	{
		return reinterpret_cast<t_slot*>(v_head);
	}
	XEMMAI__PORTABLE__EXPORT void f_expand(t_slot* a_p);
	void f_allocate(t_slot* a_p)
	{
		if (a_p > v_tail) f_expand(a_p);
	}
	void f_clear(t_slot* a_p)
	{
		while (a_p < v_used) *a_p++ = 0;
	}
};

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_stack* f_stack()
{
	return t_stack::v_instance;
}
#else
XEMMAI__PORTABLE__EXPORT t_stack* f_stack();
#endif

class t_scoped_stack
{
	t_slot* v_p;
	bool v_done;

public:
	t_scoped_stack(size_t a_n) : v_done(false)
	{
		t_stack* stack = f_stack();
		v_p = stack->v_used;
		t_slot* used = v_p + a_n;
		stack->f_allocate(used);
		stack->v_used = used;
	}
	t_scoped_stack(std::initializer_list<t_transfer> a_xs) : t_scoped_stack(a_xs.size() + 1)
	{
		t_slot* p = v_p;
		for (auto& x : a_xs) (++p)->f_construct(x);
	}
	~t_scoped_stack()
	{
		t_stack* stack = f_stack();
		if (!v_done) stack->f_clear(v_p);
		stack->v_used = v_p;
	}
	operator t_slot*() const
	{
		return v_p;
	}
	t_transfer f_return()
	{
		v_done = true;
		return v_p[0].f_transfer();
	}
};

}

#endif
