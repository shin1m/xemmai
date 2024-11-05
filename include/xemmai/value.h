#ifndef XEMMAI__VALUE_H
#define XEMMAI__VALUE_H

#include "portable/define.h"
#include <atomic>
#include <string_view>
#include <cstddef>

namespace xemmai
{

using namespace std::literals;

template<typename> struct t_type_of;
class t_object;
class t_engine;
XEMMAI__PUBLIC t_engine* f_engine();

using t_type = t_type_of<t_object>;

enum t_tag
{
	c_tag__NULL,
	c_tag__FALSE,
	c_tag__TRUE,
	c_tag__INTEGER,
	c_tag__FLOAT,
	c_tag__OBJECT
};

class t_slot
{
	friend class t_object;
	friend struct t_owned;
	friend struct t_sharable;
	friend struct t_thread;
	friend class t_engine;

protected:
	template<size_t A_SIZE>
	struct t_queue
	{
		static constexpr size_t c_SIZE = A_SIZE;

		static inline XEMMAI__PORTABLE__THREAD t_queue* v_instance;
#ifdef _WIN32
		t_object* volatile* v_head;
#else
		static inline XEMMAI__PORTABLE__THREAD t_object* volatile* v_head;
#endif
		static inline XEMMAI__PORTABLE__THREAD t_object* volatile* v_next;

#ifdef _WIN32
		void f__push(t_object* a_object)
#else
		XEMMAI__PORTABLE__ALWAYS_INLINE static void f_push(t_object* a_object)
#endif
		{
			auto p = v_head;
			*p = a_object;
			if (p == v_next)
				v_instance->f_next();
			else
				[[likely]] v_head = p + 1;
		}

		t_object* volatile v_objects[c_SIZE];
		std::atomic<t_object* volatile*> v_epoch;
		t_object* volatile* v_tail{v_objects + c_SIZE - 1};

		void f_next() noexcept;
		void f__flush(t_object* volatile* a_epoch, auto a_do)
		{
			auto end = v_objects + c_SIZE - 1;
			if (a_epoch > v_objects)
				--a_epoch;
			else
				a_epoch = end;
			while (v_tail != a_epoch) {
				auto next = a_epoch;
				if (v_tail < end) {
					if (next < v_tail) next = end;
					++v_tail;
				} else {
					v_tail = v_objects;
				}
				while (true) {
					a_do(*v_tail);
					if (v_tail == next) break;
					++v_tail;
				}
			}
		}
	};
#ifdef NDEBUG
	struct t_increments : t_queue<16384>
#else
	struct t_increments : t_queue<128>
#endif
	{
#ifdef _WIN32
		XEMMAI__PUBLIC static void f_push(t_object* a_object);
#endif
		void f_flush()
		{
			this->f__flush(this->v_epoch.load(std::memory_order_acquire), [](auto x)
			{
				x->f_increment();
			});
		}
	};
#ifdef NDEBUG
	struct t_decrements : t_queue<32768>
#else
	struct t_decrements : t_queue<256>
#endif
	{
		t_object* volatile* v_last = this->v_objects;

#ifdef _WIN32
		XEMMAI__PUBLIC static void f_push(t_object* a_object);
#endif
		void f_flush()
		{
			this->f__flush(v_last, [](auto x)
			{
				x->f_decrement();
			});
			v_last = this->v_epoch.load(std::memory_order_acquire);
		}
	};

	std::atomic<t_object*> v_p;

public:
	t_slot(t_object* a_p = nullptr) : v_p(a_p)
	{
		if (reinterpret_cast<uintptr_t>(a_p) >= c_tag__OBJECT) t_increments::f_push(a_p);
	}
	t_slot(t_object& a_p) : v_p(&a_p)
	{
		t_increments::f_push(&a_p);
	}
	t_slot(const t_slot& a_value) : t_slot(static_cast<t_object*>(a_value))
	{
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(t_object* a_p)
	{
		if (reinterpret_cast<uintptr_t>(a_p) >= c_tag__OBJECT) t_increments::f_push(a_p);
		auto p = v_p.exchange(a_p, std::memory_order_relaxed);
		if (reinterpret_cast<uintptr_t>(p) >= c_tag__OBJECT) t_decrements::f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(t_object& a_p)
	{
		t_increments::f_push(&a_p);
		auto p = v_p.exchange(&a_p, std::memory_order_relaxed);
		if (reinterpret_cast<uintptr_t>(p) >= c_tag__OBJECT) t_decrements::f_push(p);
		return *this;
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE t_slot& operator=(const t_slot& a_value)
	{
		return *this = static_cast<t_object*>(a_value);
	}
	operator t_object*() const
	{
		return v_p.load(std::memory_order_relaxed);
	}
	t_object* operator->() const
	{
		return v_p.load(std::memory_order_relaxed);
	}
};

struct t_root : t_slot
{
	using t_slot::t_slot;
	t_root(const t_root& a_value) : t_slot(a_value)
	{
	}
	~t_root()
	{
		auto p = v_p.load(std::memory_order_relaxed);
		if (reinterpret_cast<uintptr_t>(p) >= c_tag__OBJECT) t_decrements::f_push(p);
	}
	using t_slot::operator=;
	t_root& operator=(const t_root& a_value)
	{
		static_cast<t_slot&>(*this) = a_value;
		return *this;
	}
};

class t_pointer
{
	template<typename> friend class t_value;
	friend struct t_code;

	t_object* v_p;

public:
	t_pointer() = default;
	t_pointer(t_object* a_p) : v_p(a_p)
	{
	}
	t_pointer(t_object& a_p) : v_p(&a_p)
	{
	}
	t_pointer& operator=(t_object* a_p)
	{
		v_p = a_p;
		return *this;
	}
	t_pointer& operator=(t_object& a_p)
	{
		v_p = &a_p;
		return *this;
	}
	operator t_object*() const
	{
		return v_p;
	}
	t_object* operator->() const
	{
		return v_p;
	}
};

void XEMMAI__PUBLIC f_throw [[noreturn]] (std::wstring_view a_message);

template<typename T_tag>
class t_value : public T_tag
{
	template<typename> friend class t_value;
	template<typename> friend struct t_type_of;
	friend struct t_code;

	union
	{
		intptr_t v_integer;
		double v_float;
	};

	void f_copy(const auto& a_value)
	{
		if (sizeof(double) > sizeof(intptr_t))
			v_float = a_value.v_float;
		else
			v_integer = a_value.v_integer;
	}

public:
	using T_tag::T_tag;
	t_value(bool a_value) : T_tag(reinterpret_cast<t_object*>(a_value ? c_tag__TRUE : c_tag__FALSE))
	{
	}
	template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	t_value(T a_value) : T_tag(reinterpret_cast<t_object*>(c_tag__INTEGER)), v_integer(a_value)
	{
	}
	t_value(double a_value) : T_tag(reinterpret_cast<t_object*>(c_tag__FLOAT)), v_float(a_value)
	{
	}
	t_value(const t_value& a_value) : T_tag(a_value)
	{
		f_copy(a_value);
	}
	template<typename T>
	t_value(const t_value<T>& a_value) : T_tag(a_value)
	{
		f_copy(a_value);
	}
	using T_tag::operator=;
	t_value& operator=(const t_value& a_value)
	{
		static_cast<T_tag&>(*this) = a_value;
		f_copy(a_value);
		return *this;
	}
	template<typename T>
	t_value& operator=(const t_value<T>& a_value)
	{
		static_cast<T_tag&>(*this) = a_value;
		f_copy(a_value);
		return *this;
	}
	bool operator==(const t_value<t_pointer>& a_value) const
	{
		auto p = f_tag();
		if (p != a_value.f_tag()) return false;
		switch (p) {
		case c_tag__INTEGER:
			return v_integer == a_value.v_integer;
		case c_tag__FLOAT:
			return v_float == a_value.v_float;
		default:
			return true;
		}
	}
	uintptr_t f_tag() const
	{
		return reinterpret_cast<uintptr_t>(static_cast<t_object*>(*this));
	}
	bool f_boolean() const
	{
		return f_tag() >= c_tag__TRUE;
	}
	intptr_t f_integer() const;
	double f_float() const;
	t_type_of<t_object>* f_type() const;
	bool f_is(t_type* a_class) const
	{
		return f_type()->f_derives(a_class);
	}
	t_value<t_pointer> f_get(t_object* a_key, size_t& a_index) const;
	void f_bind(t_object* a_key, size_t& a_index, t_value<t_pointer>* a_stack) const;
	t_object* f_object_or_throw() const
	{
		auto p = static_cast<t_object*>(*this);
		if (reinterpret_cast<uintptr_t>(p) < c_tag__OBJECT) f_throw(L"not supported."sv);
		return p;
	}
	void f_put(t_object* a_key, size_t& a_index, const t_value<t_pointer>& a_value) const
	{
		auto p = f_object_or_throw();
		p->f_type()->f_put(p, a_key, a_index, a_value);
	}
	bool f_has(t_object* a_key, size_t& a_index) const;
	XEMMAI__PORTABLE__ALWAYS_INLINE size_t f_call_without_loop(t_value<t_pointer>* a_stack, size_t a_n) const
	{
		return f_object_or_throw()->f_call_without_loop(a_stack, a_n);
	}
	void f_call(t_value<t_pointer>* a_stack, size_t a_n) const;
	void f_call(t_object* a_key, size_t& a_index, t_value<t_pointer>* a_stack, size_t a_n) const;
	t_value<t_pointer> operator()(auto&&... a_arguments) const;
	t_value<t_pointer> f_invoke(t_object* a_key, size_t& a_index, auto&&... a_arguments) const;
	t_value<t_pointer> f_string() const;
	t_value<t_pointer> f_hash() const;
	t_value<t_pointer> f_get_at(const t_value<t_pointer>& a_index) const;
	t_value<t_pointer> f_set_at(const t_value<t_pointer>& a_index, const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_plus() const;
	t_value<t_pointer> f_minus() const;
	t_value<t_pointer> f_not() const
	{
		return f_tag() < c_tag__TRUE;
	}
	t_value<t_pointer> f_complement() const;
	t_value<t_pointer> f_multiply(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_divide(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_modulus(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_add(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_subtract(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_left_shift(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_right_shift(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_less(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_less_equal(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_greater(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_greater_equal(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_equals(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_not_equals(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_and(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_xor(const t_value<t_pointer>& a_value) const;
	t_value<t_pointer> f_or(const t_value<t_pointer>& a_value) const;
};

using t_svalue = t_value<t_slot>;
using t_rvalue = t_value<t_root>;
using t_pvalue = t_value<t_pointer>;

inline void f_loop(t_pvalue* a_stack, size_t a_n)
{
	do a_n = a_stack[0].f_call_without_loop(a_stack, a_n); while (a_n != size_t(-1));
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value<T_tag>::f_call(t_pvalue* a_stack, size_t a_n) const
{
	size_t n = f_call_without_loop(a_stack, a_n);
	if (n != size_t(-1)) f_loop(a_stack, n);
}

}

#endif
