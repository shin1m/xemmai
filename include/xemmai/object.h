#ifndef XEMMAI__OBJECT_H
#define XEMMAI__OBJECT_H

#include "atomic.h"
#include "heap.h"
#include "type.h"
#include <map>
#include <cassert>

namespace xemmai
{

enum t_color
{
	e_color__BLACK,
	e_color__PURPLE,
	e_color__GRAY,
	e_color__WHITING,
	e_color__WHITE,
	e_color__ORANGE,
	e_color__RED
};

class t_object
{
	template<typename> friend class t_heap;
	friend class t_slot;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_type>;
	friend struct t_fiber;
	friend struct t_code;
	friend class t_engine;
	friend class t_with_lock_for_read;
	friend class t_with_lock_for_write;

	static inline XEMMAI__PORTABLE__THREAD struct
	{
		t_object* v_next;
		t_object* v_previous;
	} v_roots;
	static inline XEMMAI__PORTABLE__THREAD t_object* v_scan_stack;
	static inline XEMMAI__PORTABLE__THREAD t_object* v_cycle;

	XEMMAI__PORTABLE__FORCE_INLINE static void f_append(t_object* a_p)
	{
		a_p->v_next = reinterpret_cast<t_object*>(&v_roots);
		a_p->v_previous = v_roots.v_previous;
		a_p->v_previous->v_next = v_roots.v_previous = a_p;
	}
	static void f_push(t_object* a_p)
	{
		a_p->v_scan = v_scan_stack;
		v_scan_stack = a_p;
	}
	template<void (t_object::*A_push)()>
	static void f_push(t_slot& a_slot)
	{
		auto p = a_slot.v_p.load(std::memory_order_relaxed);
		if (reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT) (p->*A_push)();
	}
	template<void (t_object::*A_push)()>
	static void f_push_and_clear(t_slot& a_slot)
	{
		auto p = a_slot.v_p.load(std::memory_order_relaxed);
		if (reinterpret_cast<uintptr_t>(p) < e_tag__OBJECT) return;
		(p->*A_push)();
		a_slot.v_p.store(nullptr, std::memory_order_relaxed);
	}

	t_object* v_next;
	t_object* v_previous;
	t_object* v_scan;
	t_color v_color;
	size_t v_count = 1;
	size_t v_cyclic;
	size_t v_rank;
	t_object* v_next_cycle;
	t_type* v_type;
	char v_data[sizeof(void*) * 7];

	void f_scan_fields(t_scan a_scan)
	{
		auto p = f_fields();
		auto n = v_type->v_instance_fields;
		for (size_t i = 0; i < n; ++i) a_scan(p[i]);
	}
	template<void (t_object::*A_push)()>
	void f_step()
	{
		f_scan_fields(f_push<A_push>);
		v_type->f_scan(this, f_push<A_push>);
		(t_object::f_of(v_type)->*A_push)();
	}
	template<void (t_object::*A_step)()>
	void f_loop()
	{
		auto p = this;
		while (true) {
			(p->*A_step)();
			p = v_scan_stack;
			if (!p) break;
			v_scan_stack = p->v_scan;
		}
	}
	XEMMAI__PORTABLE__FORCE_INLINE void f_increment()
	{
		++v_count;
		v_color = e_color__BLACK;
	}
	void f_decrement_push()
	{
		assert(v_count > 0);
		if (--v_count > 0) {
			v_color = e_color__PURPLE;
			if (!v_next) f_append(this);
		} else {
			f_push(this);
		}
	}
	void f_decrement_step();
	XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE void f_decrement()
	{
		assert(v_count > 0);
		if (--v_count > 0) {
			v_color = e_color__PURPLE;
			if (!v_next) f_append(this);
		} else {
			f_loop<&t_object::f_decrement_step>();
		}
	}
	void f_mark_gray_push()
	{
		if (v_color != e_color__GRAY) {
			v_color = e_color__GRAY;
			v_cyclic = v_count;
			f_push(this);
		}
		--v_cyclic;
	}
	void f_mark_gray()
	{
		v_color = e_color__GRAY;
		v_cyclic = v_count;
		f_loop<&t_object::f_step<&t_object::f_mark_gray_push>>();
	}
	void f_scan_black_push()
	{
		if (v_color == e_color__BLACK) return;
		v_color = e_color__BLACK;
		f_push(this);
	}
	void f_scan_gray_scan_black_push()
	{
		if (v_color == e_color__BLACK) return;
		if (v_color != e_color__WHITING) f_push(this);
		v_color = e_color__BLACK;
	}
	void f_scan_gray_push()
	{
		if (v_color != e_color__GRAY) return;
		v_color = v_cyclic > 0 ? e_color__BLACK : e_color__WHITING;
		f_push(this);
	}
	void f_scan_gray_step()
	{
		if (v_color == e_color__BLACK) {
			f_step<&t_object::f_scan_gray_scan_black_push>();
		} else {
			v_color = e_color__WHITE;
			f_step<&t_object::f_scan_gray_push>();
		}
	}
	void f_scan_gray()
	{
		if (v_color != e_color__GRAY) return;
		if (v_cyclic > 0) {
			v_color = e_color__BLACK;
			f_loop<&t_object::f_step<&t_object::f_scan_black_push>>();
		} else {
			f_loop<&t_object::f_scan_gray_step>();
		}
	}
	void f_collect_white_push()
	{
		if (v_color != e_color__WHITE) return;
		v_color = e_color__RED;
		v_cyclic = v_count;
		v_next = v_cycle->v_next;
		v_cycle->v_next = this;
		v_previous = nullptr;
		f_push(this);
	}
	void f_collect_white()
	{
		v_color = e_color__RED;
		v_cyclic = v_count;
		v_cycle = v_next = this;
		v_previous = nullptr;
		f_loop<&t_object::f_step<&t_object::f_collect_white_push>>();
	}
	void f_scan_red()
	{
		if (v_color == e_color__RED && v_cyclic > 0) --v_cyclic;
	}
	void f_cyclic_decrement_push()
	{
		if (v_color == e_color__RED) return;
		if (v_color != e_color__ORANGE)
			f_decrement();
		else if (--v_count > 0)
			--v_cyclic;
		else
			f_loop<&t_object::f_decrement_step>();
	}
	void f_cyclic_decrement()
	{
		f_scan_fields(f_push_and_clear<&t_object::f_cyclic_decrement_push>);
		v_type->f_scan(this, f_push_and_clear<&t_object::f_cyclic_decrement_push>);
		v_type->f_finalize(this);
		if (v_type->v_this) v_type->v_this->f_cyclic_decrement_push();
		v_type = nullptr;
	}

public:
	static t_object* f_of(void* a_data)
	{
		return reinterpret_cast<t_object*>(reinterpret_cast<char*>(a_data) - offsetof(t_object, v_data));
	}
	static size_t f_align_for_fields(size_t a_native)
	{
		return (a_native + sizeof(t_object*) - 1) / sizeof(t_object*) * sizeof(t_object*);
	}
	static size_t f_fields_offset(size_t a_native)
	{
		return offsetof(t_object, v_data) + f_align_for_fields(a_native);
	}

	XEMMAI__PORTABLE__ALWAYS_INLINE void f_be(t_type* a_type)
	{
		t_slot::t_increments::f_push(f_of(a_type));
		std::atomic_signal_fence(std::memory_order_release);
		v_type = a_type;
		t_slot::t_decrements::f_push(this);
	}
	t_type* f_type() const
	{
		return v_type;
	}
	bool f_is(t_type* a_class) const
	{
		return v_type->f_derives(a_class);
	}
	void* f_data()
	{
		return v_data;
	}
	template<typename T>
	T& f_as()
	{
		return *reinterpret_cast<T*>(v_data);
	}
	t_svalue* f_fields(size_t a_native)
	{
		return reinterpret_cast<t_svalue*>(reinterpret_cast<char*>(this) + f_fields_offset(a_native));
	}
	t_svalue* f_fields()
	{
		return reinterpret_cast<t_svalue*>(reinterpret_cast<char*>(this) + v_type->v_fields_offset);
	}
	t_pvalue f_get(t_object* a_key)
	{
		return v_type->f_get(this, a_key);
	}
	void f_get(t_object* a_key, t_pvalue* a_stack)
	{
		v_type->f_get(this, a_key, a_stack);
	}
	void f_put(t_object* a_key, const t_pvalue& a_value)
	{
		v_type->f_put(this, a_key, a_value);
	}
	bool f_has(t_object* a_key)
	{
		return v_type->f_has(this, a_key);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE size_t f_call_without_loop(t_pvalue* a_stack, size_t a_n)
	{
		return v_type->f_call(this, a_stack, a_n);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_call(t_object* a_key, t_pvalue* a_stack, size_t a_n)
	{
		v_type->f_invoke(this, a_key, a_stack, a_n);
	}
	template<typename... T>
	t_pvalue f_invoke(t_object* a_key, T&&... a_arguments);
};

struct t_owned
{
	decltype(t_slot::t_increments::v_instance) v_owner = t_slot::t_increments::v_instance;

	bool f_owned() const
	{
		return v_owner == t_slot::t_increments::v_instance;
	}
	template<typename T>
	auto f_owned_or_throw(T a_do) -> decltype(a_do())
	{
		if (!f_owned()) f_throw(L"not owned."sv);
		return a_do();
	}
};

struct t_sharable : t_owned
{
	t_lock v_lock;

	bool f_shared() const
	{
		return !v_owner;
	}
	void f_own();
	void f_share();
	template<typename T_lock, typename T>
	auto f_owned_or_shared(T a_do) -> decltype(a_do())
	{
		if (f_owned()) return a_do();
		if (!f_shared()) f_throw(L"owned by another thread."sv);
		T_lock lock(v_lock);
		return a_do();
	}
};

template<typename T_tag>
inline intptr_t t_value<T_tag>::f_integer() const
{
	auto p = static_cast<t_object*>(*this);
	return reinterpret_cast<uintptr_t>(p) < e_tag__OBJECT ? v_integer : p->f_as<intptr_t>();
}

template<typename T_tag>
inline double t_value<T_tag>::f_float() const
{
	auto p = static_cast<t_object*>(*this);
	return reinterpret_cast<uintptr_t>(p) < e_tag__OBJECT ? v_float : p->f_as<double>();
}

template<typename T_tag>
inline bool t_value<T_tag>::f_has(t_object* a_key) const
{
	auto p = static_cast<t_object*>(*this);
	return reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT && p->f_has(a_key);
}

inline t_type::t_type_of() : v_this(t_object::f_of(this)), v_depth(V_ids.size() - 1), v_ids(V_ids.data()), v_fields_offset(t_object::f_fields_offset(0)), v_instance_fields(0), v_fields(0)
{
}

template<size_t A_n>
inline t_type::t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module, size_t a_native, size_t a_instance_fields, const std::vector<std::pair<t_root, t_rvalue>>& a_fields, const std::map<t_object*, size_t>& a_key2index) : v_this(t_object::f_of(this)), v_depth(A_n - 1), v_ids(a_ids.data()), v_module(a_module), v_fields_offset(t_object::f_fields_offset(a_native)), v_instance_fields(a_instance_fields), v_fields(a_fields.size())
{
	v_super.f_construct(t_object::f_of(a_super));
	std::uninitialized_copy(a_fields.begin(), a_fields.end(), f_fields());
	std::copy(a_key2index.begin(), a_key2index.end(), f_key2index());
}

template<typename T_base>
void t_finalizes<T_base>::f_do_finalize(t_object* a_this)
{
	using t = typename T_base::t_what;
	a_this->f_as<t>().~t();
}

}

#endif
