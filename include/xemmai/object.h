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
		char* p = new char[a_size + (sizeof(t_slot) + sizeof(t_entry)) * a_n];
		*reinterpret_cast<size_t*>(p) = a_n;
		return p;
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_structure(t_scoped&& a_this) : v_this(std::move(a_this))
	{
	}
	t_structure(std::map<t_object*, t_object*>::iterator a_iterator, t_scoped&& a_this, t_structure* a_parent) : v_iterator(a_iterator), v_this(std::move(a_this)), v_parent0(a_parent->v_this), v_parent1(a_parent)
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
	intptr_t f_index(t_object* a_key) const
	{
		t_cache& cache = v_cache[t_cache::f_index(v_this, a_key)];
		if (static_cast<t_object*>(cache.v_structure) == v_this && static_cast<t_object*>(cache.v_key) == a_key) return cache.v_index;
		cache.v_structure = v_this;
		cache.v_key = a_key;
		const t_entry* p = f_entries();
		size_t i = 0;
		size_t j = v_size;
		while (i < j) {
			size_t k = (i + j) / 2;
			const t_entry& entry = p[k];
			if (entry.v_key == a_key) return cache.v_index = entry.v_index;
			if (entry.v_key < a_key)
				i = k + 1;
			else
				j = k;
		}
		return cache.v_index = -1;
	}
	t_scoped f_append(t_object* a_key);
	t_scoped f_remove(size_t a_index);
};

class t_tuple
{
	friend class t_object;
	friend struct t_type_of<t_tuple>;

	size_t v_size;

	void* operator new(size_t a_size, size_t a_n)
	{
		char* p = new char[a_size + sizeof(t_slot) * a_n];
		*reinterpret_cast<size_t*>(p) = a_n;
		return p;
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_tuple()
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
	const t_value& f_get_at(size_t a_index) const;
	std::wstring f_string() const;
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

class t_object
{
	friend class t_value;
	template<typename T, size_t A_size> friend class t_shared_pool;
	friend class t_local_pool<t_object>;
	friend struct t_type_of<t_object>;
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

	t_object* v_next;
	t_object* v_scan;
	t_color v_color;
	size_t v_count;
	size_t v_cyclic;
	t_slot v_type;
	t_structure* v_structure;
	t_tuple* v_fields = nullptr;
	t_lock v_lock;
	t_value::t_increments* v_owner;

	template<void (t_object::*A_push)()>
	void f_step()
	{
		(static_cast<t_object*>(v_structure->v_this)->*A_push)();
		if (v_fields) v_fields->f_scan(f_push<A_push>);
		f_type_as_type()->f_scan(this, f_push<A_push>);
		f_push<A_push>(v_type);
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
	static t_scoped f_allocate_on_boot(t_object* a_type);
	static t_scoped f_allocate_uninitialized(t_object* a_type);
	static t_scoped f_allocate(t_object* a_type);

	t_object* f_type() const
	{
		return v_type;
	}
	t_type* f_type_as_type() const
	{
		return static_cast<t_type*>(f_type()->f_pointer());
	}
	intptr_t f_integer() const
	{
		return v_type.v_integer;
	}
	double f_float() const
	{
		return v_type.v_float;
	}
	void* f_pointer() const
	{
		return v_type.v_pointer;
	}
	bool f_is(t_object* a_class) const
	{
		return t_type::f_derives(f_type(), a_class);
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
		return f_type_as_type()->f_get(this, a_key);
	}
	void f_put(t_object* a_key, t_scoped&& a_value)
	{
		f_type_as_type()->f_put(this, a_key, std::move(a_value));
	}
	bool f_has(t_object* a_key)
	{
		return f_type_as_type()->f_has(this, a_key);
	}
	t_scoped f_remove(t_object* a_key)
	{
		return f_type_as_type()->f_remove(this, a_key);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE size_t f_call_without_loop(t_scoped* a_stack, size_t a_n)
	{
		return f_type_as_type()->f_call(this, a_stack, a_n);
	}
};

}

#endif
