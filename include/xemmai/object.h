#ifndef XEMMAI__OBJECT_H
#define XEMMAI__OBJECT_H

#include "atomic.h"
#include "heap.h"
#include "type.h"
#include <map>
#include <cassert>

namespace xemmai
{

class t_structure
{
	friend class t_object;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_structure>;
	friend struct t_thread;
	friend struct t_code;
	friend class t_engine;
	friend class t_global;

	struct t_entry
	{
		t_object* v_key;
		size_t v_index;
	};
	struct t_cache
	{
		static const size_t V_SIZE = 1 << 11;
		static const size_t V_MASK = V_SIZE - 1;

		static size_t f_index(t_object* a_structure, t_object* a_key)
		{
			return (reinterpret_cast<size_t>(a_structure) ^ reinterpret_cast<size_t>(a_key)) / sizeof(t_object*) & V_MASK;
		}

		t_slot v_structure;
		t_slot v_key;
		intptr_t v_index;
	};
	struct t_fields
	{
		size_t v_size;

		void* operator new(size_t a_size, size_t a_n)
		{
			return new char[a_size + sizeof(t_svalue) * a_n];
		}
		void operator delete(void* a_p)
		{
			delete[] static_cast<char*>(a_p);
		}
		void operator delete(void* a_p, size_t)
		{
			delete[] static_cast<char*>(a_p);
		}

		t_fields(size_t a_size) : v_size(a_size)
		{
		}
		operator t_svalue*() const
		{
			return const_cast<t_svalue*>(reinterpret_cast<const t_svalue*>(this + 1));
		}
		void f_scan(t_scan a_scan)
		{
			t_svalue* p = *this;
			for (size_t i = 0; i < v_size; ++i) a_scan(p[i]);
		}
	};
	friend struct t_type_of<std::unique_ptr<t_fields>>;

	static inline XEMMAI__PORTABLE__THREAD t_cache* v_cache;

	size_t v_size;
	std::map<t_object*, t_object*>::iterator v_iterator;
	t_slot v_this;
	t_slot v_parent;
	std::mutex v_mutex;
	std::map<t_object*, t_object*> v_children;

	t_structure();
	t_structure(size_t a_size, std::map<t_object*, t_object*>::iterator a_iterator, t_structure* a_parent);
	~t_structure() = default;
	t_svalue* f_fields() const
	{
		return const_cast<t_svalue*>(reinterpret_cast<const t_svalue*>(this + 1));
	}
	t_entry* f_entries() const
	{
		return const_cast<t_entry*>(reinterpret_cast<const t_entry*>(f_fields() + v_size));
	}

public:
	void f_scan(t_scan a_scan)
	{
		a_scan(v_this);
		a_scan(v_parent);
		auto p = f_fields();
		for (size_t i = 0; i < v_size; ++i) a_scan(p[i]);
	}
	size_t f_size() const
	{
		return v_size;
	}
	intptr_t f_index(t_object* a_key, t_cache& a_cache) const;
	intptr_t f_index(t_object* a_key) const;
	t_object* f_append(t_object* a_key);
	t_object* f_remove(size_t a_index);
};

template<typename T>
void f_get_of_type(T& a_this, t_object* a_key, t_pvalue* a_stack);
template<typename T>
void f_call_of_type(T& a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n);

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
	decltype(t_slot::t_increments::v_instance) v_owner;
	t_structure* v_structure;
	t_structure::t_fields* v_fields;
	t_lock v_lock;
	char v_data[sizeof(void*) * 3];

	template<void (t_object::*A_push)()>
	void f_step()
	{
		(t_object::f_of(v_structure)->*A_push)();
		if (v_fields) v_fields->f_scan(f_push<A_push>);
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
		if (v_structure->v_this) v_structure->v_this->f_cyclic_decrement_push();
		if (v_fields) {
			v_fields->f_scan(f_push_and_clear<&t_object::f_cyclic_decrement_push>);
			delete v_fields;
			v_fields = nullptr;
		}
		v_type->f_scan(this, f_push_and_clear<&t_object::f_cyclic_decrement_push>);
		v_type->f_finalize(this);
		if (v_type->v_this) v_type->v_this->f_cyclic_decrement_push();
		v_type = nullptr;
	}
	void f_field_add(t_object* a_structure, const t_pvalue& a_value);

public:
	static t_object* f_of(void* a_data)
	{
		return reinterpret_cast<t_object*>(reinterpret_cast<char*>(a_data) - offsetof(t_object, v_data));
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
	bool f_owned() const
	{
		return v_owner == t_slot::t_increments::v_instance;
	}
	bool f_shared() const
	{
		return !v_owner;
	}
	void f_own();
	void f_share();
	size_t f_field_size() const
	{
		return v_structure->f_size();
	}
	intptr_t f_field_index(t_object* a_key) const
	{
		return v_structure->f_index(a_key);
	}
	t_object* f_field_key(size_t a_index) const
	{
		return v_structure->f_fields()[a_index];
	}
	t_svalue& f_field_get(size_t a_index) const
	{
		return (*v_fields)[a_index];
	}
	void f_field_put(t_object* a_key, const t_pvalue& a_value);
	void f_field_remove(size_t a_index);
	t_pvalue f_get(t_object* a_key)
	{
		return v_type->f_get(this, a_key);
	}
	void f_get_owned(t_object* a_key, t_pvalue* a_stack)
	{
		intptr_t index = f_field_index(a_key);
		if (index < 0) {
			f_get_of_type(*this, a_key, a_stack);
		} else {
			a_stack[0] = f_field_get(index);
			a_stack[1] = nullptr;
		}
	}
	void f_get(t_object* a_key, t_pvalue* a_stack)
	{
		if (f_owned())
			f_get_owned(a_key, a_stack);
		else
			v_type->f_get_nonowned(this, a_key, a_stack);
	}
	void f_put(t_object* a_key, const t_pvalue& a_value)
	{
		v_type->f_put(this, a_key, a_value);
	}
	bool f_has(t_object* a_key)
	{
		return v_type->f_has(this, a_key);
	}
	t_pvalue f_remove(t_object* a_key)
	{
		return v_type->f_remove(this, a_key);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE size_t f_call_without_loop(t_pvalue* a_stack, size_t a_n)
	{
		return v_type->f_call(this, a_stack, a_n);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_call(t_object* a_key, t_pvalue* a_stack, size_t a_n)
	{
		if (f_owned()) {
			intptr_t index = f_field_index(a_key);
			if (index < 0)
				f_call_of_type(*this, a_key, a_stack, a_n);
			else
				f_field_get(index).f_call(a_stack, a_n);
		} else {
			v_type->f_call_nonowned(this, a_key, a_stack, a_n);
		}
	}
	template<typename... T>
	t_pvalue f_invoke(t_object* a_key, T&&... a_arguments);
	t_pvalue f_call_preserved(t_object* a_key, t_pvalue* a_stack, size_t a_n);
	void* f_data()
	{
		return v_data;
	}
	template<typename T>
	T& f_as()
	{
		return *reinterpret_cast<T*>(v_data);
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

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_remove(t_object* a_key) const
{
	auto p = f_object_or_throw();
	return p->f_type()->f_remove(p, a_key);
}

template<typename T_lock, typename T>
inline static auto f_owned_or_shared(t_object* a_self, T a_do) -> decltype(a_do())
{
	if (a_self->f_owned()) {
		return a_do();
	} else if (a_self->f_shared()) {
		T_lock lock(a_self);
		return a_do();
	} else {
		f_throw(L"owned by another thread."sv);
	}
}

template<size_t A_n>
inline t_type::t_type_of(const std::array<t_type_id, A_n>& a_ids) : v_this(t_object::f_of(this)), v_depth(A_n - 1), v_ids(a_ids.data())
{
}

template<size_t A_n>
inline t_type::t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module) : v_this(t_object::f_of(this)), v_depth(A_n - 1), v_ids(a_ids.data()), v_module(a_module)
{
	v_super.f_construct(t_object::f_of(a_super));
}

template<typename T>
t_object* t_type::f_derive()
{
	return t_object::f_of(this)->v_type->f_new<T>(true, T::V_ids, this, v_module);
}

template<typename T_base>
inline void t_finalizes<T_base>::f_do_finalize(t_object* a_this)
{
	using t = typename T_base::t_what;
	a_this->f_as<t>().~t();
}

inline t_structure::t_structure() : v_size(0), v_this(t_object::f_of(this))
{
}

inline intptr_t t_structure::f_index(t_object* a_key) const
{
	auto p = t_object::f_of(const_cast<t_structure*>(this));
	auto& cache = v_cache[t_cache::f_index(p, a_key)];
	return static_cast<t_object*>(cache.v_structure) == p && static_cast<t_object*>(cache.v_key) == a_key ? cache.v_index : f_index(a_key, cache);
}

}

#endif
