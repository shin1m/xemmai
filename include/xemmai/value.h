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
class t_slot;
class t_scoped;
struct t_fiber;
struct t_thread;
struct t_code;
t_engine* f_engine();

class t_value
{
	template<typename T>
	friend struct t_type_of;
	friend class t_engine;
	friend class t_object;
	friend class t_slot;
	friend class t_scoped;
	friend struct t_fiber;
	friend struct t_thread;
	friend struct t_code;
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
			v_collector = nullptr;
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

	static thread_local t_collector* v_collector;
	static thread_local t_increments* v_increments;
	static thread_local t_decrements* v_decrements;

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	static t_increments* f_increments()
	{
		return v_increments;
	}
	static t_decrements* f_decrements()
	{
		return v_decrements;
	}
#else
	static XEMMAI__PORTABLE__EXPORT t_increments* f_increments();
	static XEMMAI__PORTABLE__EXPORT t_decrements* f_decrements();
#endif

	t_object* v_p;
	union
	{
		bool v_boolean;
		ptrdiff_t v_integer;
		double v_float;
		void* v_pointer;
	};

	XEMMAI__PORTABLE__ALWAYS_INLINE void f_copy(const t_value& a_value)
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
			f_increments()->f_push(a_value.v_p);
		}
	}
	void f_copy_union(const t_value& a_value)
	{
		switch (a_value.f_tag()) {
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
	t_value(t_object* a_p, const t_own&) : v_p(a_p)
	{
		if (v_p) f_increments()->f_push(v_p);
	}
	t_value(const t_value& a_value, const t_own&) : v_p(a_value.v_p)
	{
		f_copy(a_value);
	}
	t_value(const t_pass&)
	{
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(t_object* a_p)
	{
		if (a_p) f_increments()->f_push(a_p);
		t_object* p = v_p;
		v_p = a_p;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(const t_value& a_value)
	{
		f_copy(a_value);
		t_object* p = v_p;
		v_p = a_value.v_p;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
	}

public:
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
	t_value(t_object* a_p = nullptr) : v_p(a_p)
	{
	}
	t_value(const t_value& a_value) : v_p(a_value.v_p)
	{
		f_copy_union(a_value);
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
	bool f_boolean() const
	{
		return v_boolean;
	}
	ptrdiff_t f_integer() const;
	void f_integer__(ptrdiff_t a_value);
	double f_float() const;
	void f_float__(double a_value);
	void f_pointer__(void* a_value);
	t_object* f_type() const;
	bool f_is(t_object* a_class) const;
	t_scoped f_get(t_object* a_key) const;
	void f_put(t_object* a_key, t_scoped&& a_value) const;
	bool f_has(t_object* a_key) const;
	t_scoped f_remove(t_object* a_key) const;
	void f_call(t_slot* a_stack, size_t a_n) const;
	void f_call_and_return(t_slot* a_stack, size_t a_n) const;
	t_scoped f_call_with_same(t_slot* a_stack, size_t a_n) const;
	t_scoped f_hash() const;
	template<typename... T>
	t_scoped operator()(T&&... a_arguments) const;
	t_scoped f_get_at(const t_value& a_index) const;
	t_scoped f_set_at(const t_value& a_index, const t_value& a_value) const;
	t_scoped f_plus() const;
	t_scoped f_minus() const;
	t_scoped f_not() const;
	t_scoped f_complement() const;
	t_scoped f_multiply(const t_value& a_value) const;
	t_scoped f_divide(const t_value& a_value) const;
	t_scoped f_modulus(const t_value& a_value) const;
	t_scoped f_add(const t_value& a_value) const;
	t_scoped f_subtract(const t_value& a_value) const;
	t_scoped f_left_shift(const t_value& a_value) const;
	t_scoped f_right_shift(const t_value& a_value) const;
	t_scoped f_less(const t_value& a_value) const;
	t_scoped f_less_equal(const t_value& a_value) const;
	t_scoped f_greater(const t_value& a_value) const;
	t_scoped f_greater_equal(const t_value& a_value) const;
	t_scoped f_equals(const t_value& a_value) const;
	t_scoped f_not_equals(const t_value& a_value) const;
	t_scoped f_and(const t_value& a_value) const;
	t_scoped f_xor(const t_value& a_value) const;
	t_scoped f_or(const t_value& a_value) const;
	t_scoped f_send(const t_value& a_value) const;
};

class t_slot : public t_value
{
	friend class t_object;

	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& f__assign(ptrdiff_t a_value)
	{
		t_object* p = v_p;
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
	}
	void f__construct(ptrdiff_t a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_move(t_slot&& a_value)
	{
		t_object* p = a_value.v_p;
		a_value.v_p = nullptr;
		v_p = p;
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
			f_increments()->f_push(v_p);
			f_decrements()->f_push(p);
		}
	}

public:
	using t_value::t_value;
	t_slot(t_object* a_p = nullptr) : t_value(a_p, t_own())
	{
	}
	t_slot(const t_value& a_value) : t_value(a_value, t_own())
	{
	}
	t_slot(const t_slot& a_value) : t_value(a_value, t_own())
	{
	}
	t_slot(t_slot&& a_value) : t_value(t_pass())
	{
		f_move(std::move(a_value));
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(bool a_value)
	{
		t_object* p = v_p;
		v_p = reinterpret_cast<t_object*>(e_tag__BOOLEAN);
		v_boolean = a_value;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(short a_value)
	{
		return f__assign(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(unsigned short a_value)
	{
		return f__assign(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(int a_value)
	{
		return f__assign(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(unsigned a_value)
	{
		return f__assign(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(long a_value)
	{
		return f__assign(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(unsigned long a_value)
	{
		return f__assign(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(double a_value)
	{
		t_object* p = v_p;
		v_p = reinterpret_cast<t_object*>(e_tag__FLOAT);
		v_float = a_value;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
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
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_slot& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(t_slot&& a_value)
	{
		t_object* p = v_p;
		f_move(std::move(a_value));
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
	}
	void f_construct(bool a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__BOOLEAN);
		v_boolean = a_value;
	}
	void f_construct(short a_value)
	{
		f__construct(a_value);
	}
	void f_construct(unsigned short a_value)
	{
		f__construct(a_value);
	}
	void f_construct(int a_value)
	{
		f__construct(a_value);
	}
	void f_construct(unsigned a_value)
	{
		f__construct(a_value);
	}
	void f_construct(long a_value)
	{
		f__construct(a_value);
	}
	void f_construct(unsigned long a_value)
	{
		f__construct(a_value);
	}
	void f_construct(double a_value)
	{
		assert(!v_p);
		v_p = reinterpret_cast<t_object*>(e_tag__FLOAT);
		v_float = a_value;
	}
	void f_construct(t_object* a_p = nullptr)
	{
		assert(!v_p);
		if (!a_p) return;
		f_increments()->f_push(a_p);
		v_p = a_p;
	}
	void f_construct(const t_value& a_value)
	{
		assert(!v_p);
		f_copy(a_value);
		v_p = a_value.v_p;
	}
	void f_construct(t_slot&& a_value)
	{
		assert(!v_p);
		f_move(std::move(a_value));
	}
};

class t_scoped : public t_value
{
	friend class t_object;

	t_scoped(t_object* a_p, const t_pass&) : t_value(a_p)
	{
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_move(t_value&& a_value)
	{
		f_copy_union(a_value);
		t_object* p = v_p;
		v_p = a_value.v_p;
		a_value.v_p = nullptr;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
	}

public:
	using t_value::t_value;
	t_scoped(t_object* a_p = nullptr) : t_value(a_p, t_own())
	{
	}
	t_scoped(const t_value& a_value) : t_value(a_value, t_own())
	{
	}
	t_scoped(const t_scoped& a_value) : t_value(a_value, t_own())
	{
	}
	t_scoped(t_scoped&& a_value) : t_value(a_value)
	{
		a_value.v_p = nullptr;
	}
	t_scoped(t_slot&& a_value) : t_value(a_value)
	{
		a_value.v_p = nullptr;
	}
	~t_scoped()
	{
		if (f_tag() >= e_tag__OBJECT) f_decrements()->f_push(v_p);
	}
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
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_scoped& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_scoped&& a_value)
	{
		f_move(std::move(a_value));
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_slot&& a_value)
	{
		f_move(std::move(a_value));
		return *this;
	}
};

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

typedef void (*t_scan)(t_slot&);

struct t_stack
{
	static thread_local t_stack* v_instance;

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
		while (a_p < v_used) *a_p++ = nullptr;
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
	bool v_done = false;

public:
	t_scoped_stack(size_t a_n)
	{
		t_stack* stack = f_stack();
		v_p = stack->v_used;
		t_slot* used = v_p + a_n;
		stack->f_allocate(used);
		stack->v_used = used;
	}
	t_scoped_stack(std::initializer_list<t_scoped> a_xs) : t_scoped_stack(a_xs.size() + 1)
	{
		t_slot* p = v_p;
		for (auto& x : a_xs) (++p)->f_construct(std::move(x));
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
	t_scoped f_return()
	{
		v_done = true;
		return std::move(v_p[0]);
	}
};

}

#endif
