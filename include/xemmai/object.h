#ifndef XEMMAI__OBJECT_H
#define XEMMAI__OBJECT_H

#include "portable/atomic.h"
#include "hash.h"
#include "type.h"

#define XEMMAI__MACRO__ARGUMENTS_LIMIT 16

namespace xemmai
{

class t_engine;

class t_object
{
	friend class t_pointer;
	friend struct t_pointer::t_queues;
	template<typename T_base> friend class t_shared_pool;
	template<typename T, size_t A_size> friend class t_fixed_pool;
	friend class t_local_pool<t_object>;
	friend struct t_type;
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
	struct t_scoped_stack : t_stack
	{
		t_slot* v_base;

		t_scoped_stack(t_slot* a_top, t_slot* a_base) : v_base(a_base)
		{
			v_top = a_top;
		}
		~t_scoped_stack()
		{
			while (v_top < v_base) f_pop();
		}
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
			std::fwprintf(stderr, L"%p: dangling\n", this);
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
			t_type* type = static_cast<t_type*>(v_type->v_pointer);
			v_fields.f_scan(f_decrement);
			if (!type->v_primitive) {
				type->f_scan(this, f_decrement);
				type->f_finalize(this);
			}
			v_fields.f_finalize();
#ifdef XEMMAI__OBJECT__CALL_DECREMENT_TYPE
			f_decrement(v_type);
#else
			if (--v_type->v_count > 0) {
				v_type->v_color = e_color__PURPLE;
				if (!v_type->v_next) f_append(v_roots, v_type);
			} else {
				t_type* type = static_cast<t_type*>(v_type->v_type->v_pointer);
				v_type->v_fields.f_scan(f_decrement);
				type->f_scan(v_type, f_decrement);
				v_type->v_fields.f_finalize();
				type->f_finalize(v_type);
				f_decrement(v_type->v_type);
				v_type->v_color = e_color__BLACK;
				if (!v_type->v_next) {
					++v_release;
					t_local_pool<t_object>::f_free(v_type);
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
		static_cast<t_type*>(v_type->v_pointer)->f_scan(this, f_scan_black);
		f_scan_black(v_type);
	}
	void f_collect_white();
	void f_cyclic_decrement();

public:
	struct t_hash_traits
	{
		static size_t f_hash(t_object* a_key)
		{
			return reinterpret_cast<size_t>(a_key);
		}
		static size_t f_equals(t_object* a_x, t_object* a_y)
		{
			return a_x == a_y;
		}
	};
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	static t_transfer f_allocate_uninitialized(t_object* a_type)
	{
		t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
		p->v_next = 0;
		p->v_count = 1;
		p->v_type = a_type;
		return t_transfer(p, t_transfer::t_pass());
	}
	static t_transfer f_allocate(t_object* a_type)
	{
		t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
		p->v_next = 0;
		p->v_count = 1;
		p->v_type = a_type;
		p->v_pointer = 0;
		return t_transfer(p, t_transfer::t_pass());
	}
#else
	static XEMMAI__PORTABLE__EXPORT t_transfer f_allocate_uninitialized(t_object* a_type);
	static XEMMAI__PORTABLE__EXPORT t_transfer f_allocate(t_object* a_type);
#endif

	portable::t_lock v_lock;
	t_hash v_fields;
	union
	{
		bool v_boolean;
		int v_integer;
		double v_float;
		void* v_pointer;
	};

	t_object* f_type() const
	{
		return v_type;
	}
	bool f_is(t_object* a_class) const
	{
		return t_type::f_derives(v_type, a_class);
	}
	t_transfer f_get(t_object* a_key)
	{
		return static_cast<t_type*>(v_type->v_pointer)->f_get(this, a_key);
	}
	void f_put(t_object* a_key, const t_transfer& a_value)
	{
		static_cast<t_type*>(v_type->v_pointer)->f_put(this, a_key, a_value);
	}
	void f_remove(t_object* a_key)
	{
		static_cast<t_type*>(v_type->v_pointer)->f_remove(this, a_key);
	}
	void f_call(t_object* a_self, size_t a_n, t_stack& a_stack)
	{
		static_cast<t_type*>(v_type->v_pointer)->f_call(this, a_self, a_n, a_stack);
	}
	XEMMAI__PORTABLE__EXPORT void f_call(size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT t_transfer f_call(size_t a_n, t_slot* a_slots);
#define XEMMAI__MACRO__TRANSFER_A_N(n) const t_transfer& a_##n
#define XEMMAI__MACRO__A_N(n) a_##n
#define XEMMAI__MACRO__ITERATE "object_call.h"
#define XEMMAI__MACRO__N XEMMAI__MACRO__ARGUMENTS_LIMIT
#include "macro.h"
};

template<typename T>
struct t_argument
{
	typedef T t_type;
};

template<>
struct t_argument<const t_transfer&>
{
	typedef t_transfer t_type;
};

template<typename T>
struct t_as
{
	static T f_call(t_object* a_object)
	{
		return *static_cast<T*>(a_object->v_pointer);
	}
};

template<typename T>
struct t_as<T*>
{
	static T* f_call(t_object* a_object)
	{
		return static_cast<T*>(a_object->v_pointer);
	}
};

template<typename T>
struct t_as<const T*>
{
	static const T* f_call(t_object* a_object)
	{
		return static_cast<T*>(a_object->v_pointer);
	}
};

template<typename T>
struct t_as<T&>
{
	static T& f_call(t_object* a_object)
	{
		return *static_cast<T*>(a_object->v_pointer);
	}
};

template<typename T>
struct t_as<const T&>
{
	static const T& f_call(t_object* a_object)
	{
		return *static_cast<T*>(a_object->v_pointer);
	}
};

template<typename T>
inline typename t_argument<T>::t_type f_as(t_object* a_object)
{
	return t_as<T>::f_call(a_object);
}

template<typename T>
inline T f_as(const t_transfer& a_object)
{
	return f_as<T>(static_cast<t_object*>(a_object));
}

template<typename T>
inline T f_as(const t_shared& a_object)
{
	return f_as<T>(static_cast<t_object*>(a_object));
}

template<>
inline t_object* f_as<t_object*>(t_object* a_object)
{
	return a_object;
}

template<>
inline t_transfer f_as<const t_transfer&>(t_object* a_object)
{
	return a_object;
}

template<>
inline const t_transfer& f_as<const t_transfer&>(const t_transfer& a_object)
{
	return a_object;
}

template<typename T>
struct t_fundamental
{
	typedef T t_type;
};

template<typename T>
struct t_fundamental<const T>
{
	typedef typename t_fundamental<T>::t_type t_type;
};

template<typename T>
struct t_fundamental<T*>
{
	typedef typename t_fundamental<T>::t_type t_type;
};

template<typename T>
struct t_fundamental<T&>
{
	typedef typename t_fundamental<T>::t_type t_type;
};

template<>
struct t_fundamental<size_t>
{
	typedef int t_type;
};

template<typename T>
inline bool f_is(t_object* a_object)
{
	return dynamic_cast<t_type_of<typename t_fundamental<T>::t_type>*>(f_as<t_type*>(a_object->f_type())) != 0;
}

template<>
inline bool f_is<t_object*>(t_object* a_object)
{
	return true;
}

template<>
inline bool f_is<const t_transfer&>(t_object* a_object)
{
	return true;
}

}

#endif
