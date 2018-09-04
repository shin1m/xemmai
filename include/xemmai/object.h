#ifndef XEMMAI__OBJECT_H
#define XEMMAI__OBJECT_H

#include <map>

#include "atomic.h"
#include "pool.h"
#include "type.h"

namespace xemmai
{

class t_with_lock_for_read;
class t_with_lock_for_write;

class t_structure
{
	friend class t_engine;
	friend class t_object;
	friend struct t_type_of<t_structure>;
	friend struct t_thread;
	friend struct t_code;

	struct t_entry
	{
		t_object* v_key;
		size_t v_index;

		t_entry(t_object* a_key, size_t a_index) : v_key(a_key), v_index(a_index)
		{
		}
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

	static XEMMAI__PORTABLE__THREAD t_cache* v_cache;

	size_t v_size;
	std::map<t_object*, t_object*>::iterator v_iterator;
	t_slot v_this;
	t_slot v_parent0;
	t_structure* v_parent1 = nullptr;
	std::mutex v_mutex;
	std::map<t_object*, t_object*> v_children;

	void* operator new(size_t a_size, size_t a_n)
	{
		return new char[a_size + (sizeof(t_slot) + sizeof(t_entry)) * a_n];
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_structure(t_scoped&& a_this) : v_size(0), v_this(std::move(a_this))
	{
	}
	t_structure(size_t a_size, std::map<t_object*, t_object*>::iterator a_iterator, t_scoped&& a_this, t_structure* a_parent) : v_size(a_size), v_iterator(a_iterator), v_this(std::move(a_this)), v_parent0(a_parent->v_this), v_parent1(a_parent)
	{
		t_object* key = v_iterator->first;
		size_t n = a_parent->v_size;
		{
			t_slot* p0 = a_parent->f_fields();
			t_slot* p1 = f_fields();
			for (size_t i = 0; i < n; ++i) new(p1++) t_slot(*p0++);
			new(p1++) t_slot(key);
		}
		t_entry* p0 = a_parent->f_entries();
		t_entry* p1 = f_entries();
		size_t i = 0;
		while (i < n && p0->v_key < key) {
			new(p1) t_entry(p0->v_key, p0->v_index);
			++p0;
			++p1;
			++i;
		}
		new(p1) t_entry(key, n);
		++p1;
		while (i < n) {
			new(p1) t_entry(p0->v_key, p0->v_index);
			++p0;
			++p1;
			++i;
		}
	}
	~t_structure() = default;
	t_slot* f_fields() const
	{
		return const_cast<t_slot*>(reinterpret_cast<const t_slot*>(this + 1));
	}
	t_entry* f_entries() const
	{
		return const_cast<t_entry*>(reinterpret_cast<const t_entry*>(f_fields() + v_size));
	}

public:
	void f_scan(t_scan a_scan)
	{
		a_scan(v_this);
		a_scan(v_parent0);
		t_slot* p = f_fields();
		for (size_t i = 0; i < v_size; ++i) a_scan(p[i]);
	}
	size_t f_size() const
	{
		return v_size;
	}
	intptr_t f_index(t_object* a_key, t_cache& a_cache) const;
	intptr_t f_index(t_object* a_key) const
	{
		auto& cache = v_cache[t_cache::f_index(v_this, a_key)];
		return static_cast<t_object*>(cache.v_structure) == v_this && static_cast<t_object*>(cache.v_key) == a_key ? cache.v_index : f_index(a_key, cache);
	}
	t_scoped f_append(t_object* a_key);
	t_scoped f_remove(size_t a_index);
};

class t_tuple
{
	friend class t_object;
	friend struct t_finalizes<t_bears<t_tuple, t_type_immutable>>;
	friend struct t_type_of<t_tuple>;

	size_t v_size;

	void* operator new(size_t a_size, size_t a_n)
	{
		return new char[a_size + sizeof(t_slot) * a_n];
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_tuple(size_t a_size) : v_size(a_size)
	{
		t_slot* p = f_entries();
		for (size_t i = 0; i < v_size; ++i) new(p + i) t_slot();
	}
	~t_tuple() = default;
	t_slot* f_entries() const
	{
		return const_cast<t_slot*>(reinterpret_cast<const t_slot*>(this + 1));
	}

public:
	static XEMMAI__PORTABLE__EXPORT t_scoped f_instantiate(size_t a_size);

	template<typename T_scan>
	void f_scan(T_scan a_scan)
	{
		t_slot* p = f_entries();
		for (size_t i = 0; i < v_size; ++i) a_scan(p[i]);
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_slot& operator[](size_t a_index) const
	{
		return f_entries()[a_index];
	}
	t_slot& operator[](size_t a_index)
	{
		return f_entries()[a_index];
	}
	const t_value& f_get_at(size_t a_index) const
	{
		if (a_index >= v_size) f_throw(L"out of range.");
		return (*this)[a_index];
	}
	t_scoped f_string() const;
	intptr_t f_hash() const;
	bool f_less(const t_tuple& a_other) const;
	bool f_less_equal(const t_tuple& a_other) const;
	bool f_greater(const t_tuple& a_other) const;
	bool f_greater_equal(const t_tuple& a_other) const;
	bool f_equals(const t_value& a_other) const;
	bool f_not_equals(const t_value& a_other) const
	{
		return !f_equals(a_other);
	}
	void f_each(const t_value& a_callable) const;
};

template<typename T>
void f_get_of_type(T& a_this, t_object* a_key, t_stacked* a_stack);
template<typename T>
void f_call_of_type(T& a_this, t_object* a_key, t_stacked* a_stack, size_t a_n);

class t_object
{
	friend class t_value;
	template<typename T, size_t A_size> friend class t_shared_pool;
	friend class t_local_pool<t_object>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_type>;
	friend struct t_type_of<t_structure>;
	friend struct t_code;
	friend class t_engine;
	friend class t_with_lock_for_read;
	friend class t_with_lock_for_write;

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

	static XEMMAI__PORTABLE__THREAD t_object* v_roots;
	static XEMMAI__PORTABLE__THREAD t_object* v_scan_stack;

	XEMMAI__PORTABLE__FORCE_INLINE static void f_append(t_object*& a_list, t_object* a_p)
	{
		if (a_list) {
			a_p->v_next = a_list->v_next;
			a_list->v_next = a_p;
		} else {
			a_p->v_next = a_p;
		}
		a_list = a_p;
	}
	static void f_push(t_object* a_p)
	{
		a_p->v_scan = v_scan_stack;
		v_scan_stack = a_p;
	}
	template<void (t_object::*A_push)()>
	static void f_push(t_slot& a_slot)
	{
		t_object* p = a_slot.v_p;
		if (reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT) (p->*A_push)();
	}
	template<void (t_object::*A_push)()>
	static void f_push_and_clear(t_slot& a_slot)
	{
		t_object* p = a_slot.v_p;
		if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
		(p->*A_push)();
		a_slot.v_p = nullptr;
	}
	static void f_collect();
	static t_object* f_pool__allocate();
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	static t_object* f_local_pool__allocate()
	{
		return t_local_pool<t_object>::f_allocate(f_pool__allocate);
	}
#else
	static XEMMAI__PORTABLE__EXPORT t_object* f_local_pool__allocate();
#endif

	t_type* v_type;
	union
	{
		intptr_t v_integer;
		double v_float;
		void* v_pointer;
	};
	t_value::t_increments* v_owner;
	t_structure* v_structure;
	t_tuple* v_fields = nullptr;
	t_lock v_lock;
	t_object* v_next;
	t_object* v_scan;
	t_color v_color;
	size_t v_count;
	size_t v_cyclic;

	template<void (t_object::*A_push)()>
	void f_step()
	{
		(static_cast<t_object*>(v_structure->v_this)->*A_push)();
		if (v_fields) v_fields->f_scan(f_push<A_push>);
		v_type->f_scan(this, f_push<A_push>);
		(static_cast<t_object*>(v_type->v_this)->*A_push)();
	}
	template<void (t_object::*A_step)()>
	void f_loop()
	{
		t_object* p = this;
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
#ifdef XEMMAI__OBJECT__CALL_SCAN_BLACK
		f_scan_black();
#else
		v_color = e_color__BLACK;
#endif
	}
	void f_decrement_push()
	{
		if (--v_count > 0) {
#ifdef XEMMAI__OBJECT__CALL_SCAN_BLACK
			t_object* stack = v_scan_stack;
			v_scan_stack = nullptr;
			f_scan_black();
			v_scan_stack = stack;
#endif
			v_color = e_color__PURPLE;
			if (!v_next) f_append(v_roots, this);
		} else {
			f_push(this);
		}
	}
	void f_decrement_step();
	XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE void f_decrement()
	{
		assert(v_count > 0);
		if (--v_count > 0) {
#ifdef XEMMAI__OBJECT__CALL_SCAN_BLACK
			f_scan_black();
#endif
			v_color = e_color__PURPLE;
			if (!v_next) f_append(v_roots, this);
		} else {
			f_loop<&t_object::f_decrement_step>();
		}
	}
	void f_scan_black_push()
	{
		if (v_color == e_color__BLACK) return;
		v_color = e_color__BLACK;
		f_push(this);
	}
	void f_scan_black()
	{
		if (v_color == e_color__BLACK) return;
		v_color = e_color__BLACK;
		f_loop<&t_object::f_step<&t_object::f_scan_black_push>>();
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
	void f_scan_gray_scan_black_push()
	{
		switch (v_color) {
		case e_color__BLACK:
			break;
		case e_color__WHITING:
			v_color = e_color__BLACK;
			break;
		default:
			v_color = e_color__BLACK;
			f_push(this);
		}
	}
	void f_scan_gray_push()
	{
		if (v_color == e_color__GRAY && v_cyclic <= 0) {
			v_color = e_color__WHITING;
			f_push(this);
		} else if (v_color != e_color__WHITING && v_color != e_color__WHITE) {
			f_scan_black_push();
		}
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
		if (v_color == e_color__GRAY && v_cyclic <= 0) {
			v_color = e_color__WHITING;
			f_loop<&t_object::f_scan_gray_step>();
			if (v_color == e_color__WHITING) v_color = e_color__WHITE;
		} else if (v_color != e_color__WHITE) {
			f_scan_black();
		}
	}
	void f_collect_white_push();
	void f_collect_white();
	void f_scan_red()
	{
		if (v_color == e_color__RED && v_cyclic > 0) --v_cyclic;
	}
	void f_cyclic_decrement_push()
	{
		if (v_color == e_color__RED) return;
		if (v_color == e_color__ORANGE) {
			--v_count;
			--v_cyclic;
		} else {
			f_decrement();
		}
	}
	void f_cyclic_decrement();
	void f_field_add(t_scoped&& a_structure, t_scoped&& a_value);

public:
	static t_scoped f_allocate_on_boot();
	static t_scoped f_allocate_uninitialized(t_type* a_type);
	static t_scoped f_allocate(t_type* a_type);

	t_type* f_type() const
	{
		return v_type;
	}
	intptr_t f_integer() const
	{
		return v_integer;
	}
	double f_float() const
	{
		return v_float;
	}
	void* f_pointer() const
	{
		return v_pointer;
	}
	bool f_is(t_type* a_class) const
	{
		return v_type->f_derives(a_class);
	}
	bool f_owned() const
	{
		return v_owner == t_value::f_increments();
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
	t_slot& f_field_get(size_t a_index) const
	{
		return (*v_fields)[a_index];
	}
	void f_field_put(t_object* a_key, t_scoped&& a_value);
	void f_field_remove(size_t a_index);
	t_scoped f_get(t_object* a_key)
	{
		return v_type->f_get(this, a_key);
	}
	void f_get_owned(t_object* a_key, t_stacked* a_stack)
	{
		intptr_t index = f_field_index(a_key);
		if (index < 0) {
			f_get_of_type(*this, a_key, a_stack);
		} else {
			a_stack[0].f_construct(f_field_get(index));
			a_stack[1].f_construct();
		}
	}
	void f_get(t_object* a_key, t_stacked* a_stack)
	{
		if (f_owned())
			f_get_owned(a_key, a_stack);
		else
			v_type->f_get_nonowned(this, a_key, a_stack);
	}
	void f_put(t_object* a_key, t_scoped&& a_value)
	{
		v_type->f_put(this, a_key, std::move(a_value));
	}
	bool f_has(t_object* a_key)
	{
		return v_type->f_has(this, a_key);
	}
	t_scoped f_remove(t_object* a_key)
	{
		return v_type->f_remove(this, a_key);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE size_t f_call_without_loop(t_stacked* a_stack, size_t a_n)
	{
		return v_type->f_call(this, a_stack, a_n);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_call(t_object* a_key, t_stacked* a_stack, size_t a_n)
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
	t_scoped f_invoke(t_object* a_key, T&&... a_arguments)
	{
		t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
		stack[1].f_construct();
		f_call(a_key, stack, sizeof...(a_arguments));
		return stack.f_return();
	}
	t_scoped f_call_preserved(t_object* a_key, t_stacked* a_stack, size_t a_n)
	{
		size_t n = a_n + 2;
		t_scoped_stack stack(n);
		stack[1].f_construct();
		for (size_t i = 2; i < n; ++i) stack[i].f_construct(a_stack[i]);
		f_call(a_key, stack, a_n);
		return stack.f_return();
	}
};

inline intptr_t t_value::f_integer() const
{
	return f_tag() < e_tag__OBJECT ? v_integer : v_p->v_integer;
}

inline void t_value::f_integer__(intptr_t a_value)
{
	v_p->v_integer = a_value;
}

inline double t_value::f_float() const
{
	return f_tag() < e_tag__OBJECT ? v_float : v_p->v_float;
}

inline void t_value::f_float__(double a_value)
{
	v_p->v_float = a_value;
}

inline void t_value::f_pointer__(void* a_value)
{
	v_p->v_pointer = a_value;
}

inline bool t_value::f_is(t_type* a_class) const
{
	return f_type()->f_derives(a_class);
}

inline void t_value::f_put(t_object* a_key, t_scoped&& a_value) const
{
	if (f_tag() < e_tag__OBJECT) f_throw(L"not supported.");
	v_p->f_type()->f_put(v_p, a_key, std::move(a_value));
}

inline bool t_value::f_has(t_object* a_key) const
{
	return f_tag() >= e_tag__OBJECT && v_p->f_has(a_key);
}

inline t_scoped t_value::f_remove(t_object* a_key) const
{
	if (f_tag() < e_tag__OBJECT) f_throw(L"not supported.");
	return v_p->f_type()->f_remove(v_p, a_key);
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline size_t t_value::f_call_without_loop(t_stacked* a_stack, size_t a_n) const
{
	if (f_tag() < e_tag__OBJECT) {
		t_destruct_n(a_stack, a_n);
		f_throw(L"not supported.");
	}
	return v_p->f_call_without_loop(a_stack, a_n);
}

inline void t_value::f_loop(t_stacked* a_stack, size_t a_n)
{
	do {
		t_scoped x = std::move(a_stack[0]);
		a_n = x.f_call_without_loop(a_stack, a_n);
	} while (a_n != size_t(-1));
}

#define XEMMAI__VALUE__UNARY(a_method)\
		{\
			t_scoped_stack stack(2);\
			size_t n = v_p->f_type()->a_method(v_p, stack);\
			if (n != size_t(-1)) f_loop(stack, n);\
			return stack.f_return();\
		}
#define XEMMAI__VALUE__BINARY(a_method)\
		{\
			t_scoped_stack stack(3);\
			stack[2].f_construct(a_value);\
			size_t n = v_p->f_type()->a_method(v_p, stack);\
			if (n != size_t(-1)) f_loop(stack, n);\
			return stack.f_return();\
		}

template<typename... T>
inline t_scoped t_value::operator()(T&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
	stack[1].f_construct();
	f_call(stack, sizeof...(a_arguments));
	return stack.f_return();
}

template<typename... T>
inline t_scoped t_value::f_invoke(t_object* a_key, T&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
	stack[1].f_construct();
	f_call(a_key, stack, sizeof...(a_arguments));
	return stack.f_return();
}

inline t_scoped t_value::f_get_at(const t_value& a_index) const
{
	if (f_tag() < e_tag__OBJECT) f_throw(L"not supported.");
	t_scoped_stack stack(3);
	stack[2].f_construct(a_index);
	size_t n = v_p->f_type()->f_get_at(v_p, stack);
	if (n != size_t(-1)) f_loop(stack, n);
	return stack.f_return();
}

inline t_scoped t_value::f_set_at(const t_value& a_index, const t_value& a_value) const
{
	if (f_tag() < e_tag__OBJECT) f_throw(L"not supported.");
	t_scoped_stack stack(4);
	stack[2].f_construct(a_index);
	stack[3].f_construct(a_value);
	size_t n = v_p->f_type()->f_set_at(v_p, stack);
	if (n != size_t(-1)) f_loop(stack, n);
	return stack.f_return();
}

inline t_scoped t_value::f_plus() const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_scoped(v_integer);
	case e_tag__FLOAT:
		return t_scoped(v_float);
	default:
		XEMMAI__VALUE__UNARY(f_plus)
	}
}

inline t_scoped t_value::f_minus() const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_scoped(-v_integer);
	case e_tag__FLOAT:
		return t_scoped(-v_float);
	default:
		XEMMAI__VALUE__UNARY(f_minus)
	}
}

inline t_scoped t_value::f_not() const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		return t_scoped(!v_boolean);
	case e_tag__NULL:
	case e_tag__INTEGER:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__UNARY(f_not)
	}
}

inline t_scoped t_value::f_complement() const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		return t_scoped(~v_integer);
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__UNARY(f_complement)
	}
}

inline t_scoped t_value::f_send(const t_value& a_value) const
{
	if (f_tag() < e_tag__OBJECT) f_throw(L"not supported.");
	XEMMAI__VALUE__BINARY(f_send)
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
		f_throw(L"owned by another thread.");
	}
}

}

#endif
