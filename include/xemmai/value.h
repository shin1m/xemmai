#ifndef XEMMAI__VALUE_H
#define XEMMAI__VALUE_H

#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <condition_variable>
#include <mutex>
#include <type_traits>

#include "portable/define.h"

namespace xemmai
{

template<typename T>
struct t_type_of;
class t_engine;
class t_object;
struct t_slot;
class t_scoped;
class t_stacked;
struct t_context;
struct t_backtrace;
struct t_fiber;
struct t_thread;
struct t_code;
t_engine* f_engine();
#ifdef XEMMAI_ENABLE_JIT
struct t_engine_jit;
#endif

class t_value
{
	template<typename T>
	friend struct t_type_of;
	friend class t_engine;
	friend class t_object;
	friend struct t_slot;
	friend class t_scoped;
	friend class t_stacked;
	friend struct t_context;
	friend struct t_backtrace;
	friend struct t_fiber;
	friend struct t_thread;
	friend struct t_code;
	friend t_engine* f_engine();
#ifdef XEMMAI_ENABLE_JIT
	friend struct t_engine_jit;
#endif

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
		size_t v_collector__release = 0;
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
			std::lock_guard<std::mutex> lock(v_collector__mutex);
			++v_collector__tick;
			if (v_collector__running) return;
			v_collector__running = true;
			v_collector__wake.notify_one();
		}
		void f_wait()
		{
			std::unique_lock<std::mutex> lock(v_collector__mutex);
			++v_collector__wait;
			if (!v_collector__running) {
				v_collector__running = true;
				v_collector__wake.notify_one();
			}
			v_collector__done.wait(lock);
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

	static XEMMAI__PORTABLE__THREAD t_collector* v_collector;
	static XEMMAI__PORTABLE__THREAD t_increments* v_increments;
	static XEMMAI__PORTABLE__THREAD t_decrements* v_decrements;

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
		intptr_t v_integer;
		double v_float;
		void* v_pointer;
	};

	XEMMAI__PORTABLE__ALWAYS_INLINE void f_copy(const t_value& a_value)
	{
		auto p = reinterpret_cast<const char*>(&a_value);
		std::copy(p, p + sizeof(t_value), reinterpret_cast<char*>(this));
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_copy_construct(const t_value& a_value)
	{
		if (reinterpret_cast<size_t>(a_value.v_p) >= e_tag__OBJECT) f_increments()->f_push(a_value.v_p);
		f_copy(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_move_construct(t_value& a_value)
	{
		f_copy(a_value);
		a_value.v_p = nullptr;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_value(t_object* a_p, const t_own&) : v_p(a_p)
	{
		if (v_p) f_increments()->f_push(v_p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_value(t_object& a_p) : v_p(&a_p)
	{
		f_increments()->f_push(v_p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_value(const t_value& a_value, const t_own&)
	{
		f_copy_construct(a_value);
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
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(t_object& a_p)
	{
		f_increments()->f_push(&a_p);
		t_object* p = v_p;
		v_p = &a_p;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(const t_value& a_value)
	{
		t_object* p = v_p;
		f_copy_construct(a_value);
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(t_value&& a_value)
	{
		t_object* p = v_p;
		f_move_construct(a_value);
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_assign(t_stacked&& a_value);

public:
	static const t_value v_null;
	static const t_value v_true;
	static const t_value v_false;

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
	t_value(const t_value& a_value)
	{
		f_copy(a_value);
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
	intptr_t f_integer() const;
	void f_integer__(intptr_t a_value);
	double f_float() const;
	void f_float__(double a_value);
	void f_pointer__(void* a_value);
	t_object* f_type() const;
	bool f_is(t_object* a_class) const;
	t_scoped f_get(t_object* a_key) const;
	void f_put(t_object* a_key, t_scoped&& a_value) const;
	bool f_has(t_object* a_key) const;
	t_scoped f_remove(t_object* a_key) const;
	size_t f_call_without_loop(t_stacked* a_stack, size_t a_n) const;
	static void f_loop(t_stacked* a_stack, size_t a_n);
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_call(t_stacked* a_stack, size_t a_n) const
	{
		size_t n = f_call_without_loop(a_stack, a_n);
		if (n != size_t(-1)) f_loop(a_stack, n);
	}
	void f_call(t_object* a_key, t_stacked* a_stack, size_t a_n) const;
	void f_get(t_object* a_key, t_stacked* a_stack) const;
	t_scoped f_call_with_same(t_stacked* a_stack, size_t a_n) const;
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
	void f_construct(t_object* a_p = nullptr)
	{
		assert(f_tag() < e_tag__OBJECT);
		if (a_p) f_increments()->f_push(a_p);
		v_p = a_p;
	}
	void f_construct_nonnull(t_object* a_p)
	{
		assert(f_tag() < e_tag__OBJECT);
		f_increments()->f_push(a_p);
		v_p = a_p;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_construct(const t_value& a_value)
	{
		assert(f_tag() < e_tag__OBJECT);
		f_copy_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_destruct()
	{
		if (f_tag() < e_tag__OBJECT) return;
		f_decrements()->f_push(v_p);
		v_p = nullptr;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_construct(t_value&& a_value)
	{
		f_construct(a_value);
		a_value.f_destruct();
	}
};

struct t_slot : t_value
{
	friend class t_object;

	using t_value::t_value;
	explicit t_slot(t_object* a_p = nullptr) : t_value(a_p, t_own())
	{
	}
	explicit t_slot(const t_value& a_value) : t_value(a_value, t_own())
	{
	}
	explicit t_slot(t_value&& a_value) : t_value(a_value, t_own())
	{
		a_value.f_destruct();
	}
	t_slot(const t_slot& a_value) : t_value(a_value, t_own())
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
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(t_value&& a_value)
	{
		f_assign(a_value);
		a_value.f_destruct();
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_slot& a_value)
	{
		f_assign(a_value);
		return *this;
	}
};

class t_scoped : public t_value
{
	friend class t_object;

	t_scoped(t_object* a_p, const t_pass&) : t_value(a_p)
	{
	}

public:
	using t_value::t_value;
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped(t_object* a_p = nullptr) : t_value(a_p, t_own())
	{
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped(t_object& a_p) : t_value(&a_p)
	{
		f_increments()->f_push(v_p);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped(const t_value& a_value) : t_value(a_value, t_own())
	{
	}
	t_scoped(t_value&& a_value) : t_value(t_pass())
	{
		f_move_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped(const t_scoped& a_value) : t_value(a_value, t_own())
	{
	}
	t_scoped(t_scoped&& a_value) : t_value(t_pass())
	{
		f_move_construct(a_value);
	}
	t_scoped(t_stacked&& a_value);
	XEMMAI__PORTABLE__ALWAYS_INLINE ~t_scoped()
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
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_value&& a_value)
	{
		f_assign(std::move(a_value));
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(const t_scoped& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_scoped&& a_value)
	{
		f_assign(std::move(a_value));
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_scoped& operator=(t_stacked&& a_value)
	{
		f_assign(std::move(a_value));
		return *this;
	}
	using t_value::f_construct;
	void f_construct(t_value&& a_value)
	{
		assert(f_tag() < e_tag__OBJECT);
		f_move_construct(a_value);
	}
	void f_construct(t_stacked&& a_value);
};

class t_stacked : public t_value
{
	using t_value::t_value;

public:
	~t_stacked() = delete;
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(bool a_value)
	{
		t_object* p = v_p;
		v_p = reinterpret_cast<t_object*>(e_tag__BOOLEAN);
		v_boolean = a_value;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(intptr_t a_value)
	{
		t_object* p = v_p;
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(double a_value)
	{
		t_object* p = v_p;
		v_p = reinterpret_cast<t_object*>(e_tag__FLOAT);
		v_float = a_value;
		if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(t_object* a_p)
	{
		f_assign(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(t_object& a_p)
	{
		f_assign(a_p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(const t_value& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(t_value&& a_value)
	{
		f_assign(std::move(a_value));
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(const t_stacked& a_value)
	{
		f_assign(a_value);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_stacked& operator=(t_stacked&& a_value)
	{
		f_assign(std::move(a_value));
		return *this;
	}
	void f_construct(t_object* a_p = nullptr)
	{
		if (a_p) f_increments()->f_push(a_p);
		v_p = a_p;
	}
	void f_construct_nonnull(t_object* a_p)
	{
		f_increments()->f_push(a_p);
		v_p = a_p;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_construct(const t_value& a_value)
	{
		f_copy_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_destruct()
	{
		if (f_tag() >= e_tag__OBJECT) f_decrements()->f_push(v_p);
	}
	void f_construct(bool a_value)
	{
		v_p = reinterpret_cast<t_object*>(e_tag__BOOLEAN);
		v_boolean = a_value;
	}
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value>::type f_construct(T a_value)
	{
		v_p = reinterpret_cast<t_object*>(e_tag__INTEGER);
		v_integer = a_value;
	}
	void f_construct(double a_value)
	{
		v_p = reinterpret_cast<t_object*>(e_tag__FLOAT);
		v_float = a_value;
	}
	void f_construct(t_value&& a_value)
	{
		f_move_construct(a_value);
	}
	void f_construct(t_stacked&& a_value)
	{
		f_copy(a_value);
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

inline XEMMAI__PORTABLE__ALWAYS_INLINE void t_value::f_assign(t_stacked&& a_value)
{
	t_object* p = v_p;
	f_copy(a_value);
	if (reinterpret_cast<size_t>(p) >= e_tag__OBJECT) f_decrements()->f_push(p);
}

inline t_scoped::t_scoped(t_stacked&& a_value) : t_value(t_pass())
{
	f_copy(a_value);
}

inline void t_scoped::f_construct(t_stacked&& a_value)
{
	assert(f_tag() < e_tag__OBJECT);
	f_copy(a_value);
}

typedef void (*t_scan)(t_slot&);

struct t_stack
{
	static XEMMAI__PORTABLE__THREAD t_stack* v_instance;

	size_t v_size;
	char* v_head;
	t_stacked* v_used;

	t_stack(size_t a_size) : v_size(a_size), v_head(new char[sizeof(t_value) * v_size]), v_used(f_head())
	{
	}
	~t_stack()
	{
		delete[] v_head;
	}
	t_stacked* f_head() const
	{
		return reinterpret_cast<t_stacked*>(v_head);
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
	t_stacked* v_p;

public:
	t_scoped_stack(size_t a_n)
	{
		t_stack* stack = f_stack();
		v_p = stack->v_used;
		stack->v_used = v_p + a_n;
	}
	template<typename T_x, typename... T>
	t_scoped_stack(size_t a_n, T_x&& a_x, T&&... a_xs) : t_scoped_stack(a_n, std::forward<T>(a_xs)...)
	{
		v_p[a_n - sizeof...(a_xs) - 1].f_construct(std::forward<T_x>(a_x));
	}
	~t_scoped_stack()
	{
		f_stack()->v_used = v_p;
	}
	operator t_stacked*() const
	{
		return v_p;
	}
	t_scoped f_return()
	{
		return std::move(v_p[0]);
	}
};

template<size_t A_N = -1>
struct t_destruct
{
	t_stacked* v_p;

	t_destruct(t_stacked* a_stack) : v_p(a_stack)
	{
	}
	~t_destruct()
	{
		for (size_t i = 1; i < A_N + 2; ++i) v_p[i].f_destruct();
	}
};

template<>
struct t_destruct<-1>
{
	t_stacked& v_p;

	t_destruct(t_stacked& a_p) : v_p(a_p)
	{
	}
	~t_destruct()
	{
		v_p.f_destruct();
	}
};

struct t_destruct_n
{
	t_stacked* v_p;
	size_t v_n;

	t_destruct_n(t_stacked* a_stack, size_t a_n) : v_p(a_stack), v_n(a_n)
	{
	}
	~t_destruct_n()
	{
		(++v_p)->f_destruct();
		for (size_t i = 0; i < v_n; ++i) (++v_p)->f_destruct();
	}
};

}

#endif
