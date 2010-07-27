#ifndef XEMMAI__OBJECT_H
#define XEMMAI__OBJECT_H

#include "portable/atomic.h"
#include "hash.h"
#include "type.h"

namespace xemmai
{

class t_engine;

class t_object
{
	friend class t_value;
	template<typename T_base> friend class t_shared_pool;
	template<typename T, size_t A_size> friend class t_fixed_pool;
	friend class t_local_pool<t_object>;
	friend struct t_type_of<t_object>;
	friend class t_engine;

	enum t_color
	{
		e_color__BLACK,
		e_color__PURPLE,
		e_color__GRAY,
		e_color__WHITE,
		e_color__ORANGE,
		e_color__RED
	};

	static XEMMAI__PORTABLE__THREAD t_object* v_roots;
	static XEMMAI__PORTABLE__THREAD size_t v_release;
	static XEMMAI__PORTABLE__THREAD size_t v_collect;

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
	static void f_decrement(t_slot& a_slot);
	static void f_mark_gray(t_slot& a_slot);
	static void f_scan_gray(t_slot& a_slot);
	static void f_scan_black(t_slot& a_slot);
	static void f_collect_white(t_slot& a_slot);
	static void f_scan_red(t_slot& a_slot);
	static void f_cyclic_decrement(t_slot& a_slot);
	static void f_collect();
	static XEMMAI__PORTABLE__EXPORT t_object* f_pool__allocate();

	t_object* v_next;
	t_color v_color;
	size_t v_count;
	size_t v_cyclic;
	t_slot v_type;

	t_type* f_type_as_type() const
	{
		return static_cast<t_type*>(f_type()->f_pointer());
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
	XEMMAI__PORTABLE__ALWAYS_INLINE XEMMAI__PORTABLE__FORCE_INLINE void f_decrement()
	{
#ifdef _DEBUG
		if (v_count <= 0) {
			std::fprintf(stderr, "%p: dangling\n", this);
			assert(false);
		}
#endif
		if (--v_count > 0) {
#ifdef XEMMAI__OBJECT__CALL_SCAN_BLACK
			f_scan_black();
#endif
			v_color = e_color__PURPLE;
			if (!v_next) f_append(v_roots, this);
		} else {
			t_type* type = f_type_as_type();
			v_fields.f_scan(f_decrement);
			if (!type->v_primitive) {
				type->f_scan(this, f_decrement);
				type->f_finalize(this);
			}
			v_fields.f_finalize();
#ifdef XEMMAI__OBJECT__CALL_DECREMENT_TYPE
			f_decrement(v_type);
#else
			t_object* p = f_type();
			if (--p->v_count > 0) {
				p->v_color = e_color__PURPLE;
				if (!p->v_next) f_append(v_roots, p);
			} else {
				t_type* type = p->f_type_as_type();
				p->v_fields.f_scan(f_decrement);
				type->f_scan(p, f_decrement);
				p->v_fields.f_finalize();
				type->f_finalize(p);
				f_decrement(p->v_type);
				p->v_color = e_color__BLACK;
				if (!p->v_next) {
					++v_release;
					t_local_pool<t_object>::f_free(p);
				}
			}
			v_type.v_p = 0;
#endif
			v_color = e_color__BLACK;
			if (!v_next) {
				++v_release;
				t_local_pool<t_object>::f_free(this);
			}
		}
	}
	void f_mark_gray();
	void f_scan_gray();
	XEMMAI__PORTABLE__FORCE_INLINE void f_scan_black()
	{
		if (v_color == e_color__BLACK) return;
		v_color = e_color__BLACK;
		v_fields.f_scan(f_scan_black);
		f_type_as_type()->f_scan(this, f_scan_black);
		f_scan_black(v_type);
	}
	void f_collect_white();
	void f_cyclic_decrement();

public:
	struct t_hash_traits
	{
		static size_t f_hash(const t_value& a_key)
		{
			return a_key.f_tag();
		}
		static bool f_equals(const t_value& a_x, const t_value& a_y)
		{
			return a_x.f_object() == a_y.f_object();
		}
	};
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	static t_transfer f_allocate_uninitialized(t_object* a_type)
	{
		t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
		p->v_next = 0;
		p->v_count = 1;
		p->v_type.f_construct(a_type);
		return t_transfer(p, t_transfer::t_pass());
	}
	static t_transfer f_allocate(t_object* a_type)
	{
		t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
		p->v_next = 0;
		p->v_count = 1;
		p->v_type.f_construct(a_type);
		p->v_type.v_pointer = 0;
		return t_transfer(p, t_transfer::t_pass());
	}
#else
	static XEMMAI__PORTABLE__EXPORT t_transfer f_allocate_uninitialized(t_object* a_type);
	static XEMMAI__PORTABLE__EXPORT t_transfer f_allocate(t_object* a_type);
#endif

	portable::t_lock v_lock;
	t_hash v_fields;

	t_object* f_type() const
	{
		return v_type.f_object();
	}
	bool f_boolean() const
	{
		return v_type.v_boolean;
	}
	int f_integer() const
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
	t_transfer f_get(t_object* a_key)
	{
		return f_type_as_type()->f_get(this, a_key);
	}
	void f_put(t_object* a_key, const t_transfer& a_value)
	{
		f_type_as_type()->f_put(this, a_key, a_value);
	}
	bool f_has(t_object* a_key)
	{
		return f_type_as_type()->f_has(this, a_key);
	}
	t_transfer f_remove(t_object* a_key)
	{
		return f_type_as_type()->f_remove(this, a_key);
	}
	void f_call(const t_value& a_self, size_t a_n, t_stack& a_stack)
	{
		f_type_as_type()->f_call(this, a_self, a_n, a_stack);
	}
	XEMMAI__PORTABLE__EXPORT void f_call_and_return(const t_value& a_self, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT t_transfer f_call(size_t a_n, t_slot* a_slots);
#define XEMMAI__MACRO__A_N_COMMA(n) a_##n,
#define XEMMAI__MACRO__ITERATE "object_call.h"
#define XEMMAI__MACRO__N XEMMAI__MACRO__ARGUMENTS_LIMIT
#include "macro.h"
};

}

#endif
