#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include <string>

#include "pointer.h"

namespace xemmai
{

class t_global;

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
struct t_fundamental<t_transfer>
{
	typedef t_object t_type;
};

struct t_stack
{
	t_slot* v_top;

	XEMMAI__PORTABLE__ALWAYS_INLINE void f_push(t_object* a_value)
	{
		(--v_top)->f_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_push(const t_transfer& a_value)
	{
		(--v_top)->f_construct(a_value);
	}
	XEMMAI__PORTABLE__ALWAYS_INLINE void f_push(const t_shared& a_value)
	{
		f_push(static_cast<t_object*>(a_value));
	}
	t_transfer f_pop()
	{
		t_transfer p = v_top->f_transfer();
		++v_top;
		return p;
	}
	t_slot& f_top()
	{
		return *v_top;
	}
	t_slot& f_at(size_t a_i)
	{
		return v_top[a_i];
	}
	void f_return(t_object* a_value)
	{
		v_top->f_construct(a_value);
	}
	void f_return(const t_transfer& a_value)
	{
		v_top->f_construct(a_value);
	}
	void f_return(const t_shared& a_value)
	{
		f_return(static_cast<t_object*>(a_value));
	}
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

template<typename T>
struct t_type_of
{
};

template<>
struct t_type_of<t_object>
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			return *static_cast<T0*>(a_object->v_pointer);
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0*, T1>
	{
		typedef T0* t_type;

		static T0* f_call(T1 a_object)
		{
			return static_cast<T0*>(a_object->v_pointer);
		}
	};
	template<typename T0, typename T1>
	struct t_as<const T0*, T1>
	{
		typedef const T0* t_type;

		static const T0* f_call(T1 a_object)
		{
			return static_cast<T0*>(a_object->v_pointer);
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0&, T1>
	{
		typedef T0& t_type;

		static T0& f_call(T1 a_object)
		{
			return *static_cast<T0*>(a_object->v_pointer);
		}
	};
	template<typename T0, typename T1>
	struct t_as<const T0&, T1>
	{
		typedef const T0& t_type;

		static const T0& f_call(T1 a_object)
		{
			return *static_cast<T0*>(a_object->v_pointer);
		}
	};
	template<typename T>
	struct t_as<t_object*, T>
	{
		typedef t_object* t_type;

		static t_object* f_call(T a_object)
		{
			return a_object;
		}
	};
	template<typename T>
	struct t_as<const t_transfer&, T>
	{
		typedef t_transfer t_type;

		static t_object* f_call(T a_object)
		{
			return a_object;
		}
	};
	template<typename T0, typename T1>
	struct t_is
	{
		static bool f_call(T1 a_object)
		{
			return dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(static_cast<t_type_of<t_object>*>(a_object->f_type()->v_pointer)) != 0;
		}
	};
	template<typename T>
	struct t_is<t_object*, T>
	{
		static bool f_call(T a_object)
		{
			return true;
		}
	};
	template<typename T>
	struct t_is<const t_transfer&, T>
	{
		static bool f_call(T a_object)
		{
			return true;
		}
	};
	typedef t_global t_extension;

	t_slot v_module;
	t_slot v_super;
	bool v_builtin;
	bool v_primitive;
	bool v_revive;

	XEMMAI__PORTABLE__EXPORT static bool f_derives(t_object* a_this, t_object* a_type);
	static void f_construct(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack);
	static void f_initialize(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT static std::wstring f_string(t_object* a_self)
	{
		wchar_t cs[13 + sizeof(t_object*) * 2];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", a_self);
		return cs;
	}
	XEMMAI__PORTABLE__EXPORT static int f_hash(t_object* a_self)
	{
		return reinterpret_cast<int>(a_self);
	}
	XEMMAI__PORTABLE__EXPORT static bool f_equals(t_object* a_self, t_object* a_other)
	{
		return a_self == a_other;
	}
	XEMMAI__PORTABLE__EXPORT static bool f_not_equals(t_object* a_self, t_object* a_other)
	{
		return a_self != a_other;
	}
	XEMMAI__PORTABLE__EXPORT static void f_define(t_object* a_class);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : v_module(a_module), v_super(a_super), v_builtin(false), v_primitive(false), v_revive(false)
	{
	}
	XEMMAI__PORTABLE__EXPORT virtual ~t_type_of();
	XEMMAI__PORTABLE__EXPORT virtual t_type_of* f_derive(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual t_transfer f_get(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_put(t_object* a_this, t_object* a_key, const t_transfer& a_value);
	XEMMAI__PORTABLE__EXPORT virtual bool f_has(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual t_transfer f_remove(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_hash(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_get_at(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_set_at(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_plus(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_minus(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_not(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_complement(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_multiply(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_divide(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_modulus(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_add(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_subtract(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_left_shift(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_right_shift(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_less(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_less_equal(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_greater(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_greater_equal(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_equals(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_not_equals(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_and(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_xor(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_or(t_object* a_this, t_stack& a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_send(t_object* a_this, t_stack& a_stack);
};

template<>
struct t_type_of<t_object>::t_as<const t_transfer&, const t_transfer&>
{
	typedef const t_transfer& t_type;

	static const t_transfer& f_call(const t_transfer& a_object)
	{
		return a_object;
	}
};

typedef t_type_of<t_object> t_type;

template<typename T>
inline typename t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, t_object*>::t_type f_as(t_object* a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, t_object*>::f_call(a_object);
}

template<typename T>
inline typename t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, const t_transfer&>::t_type f_as(const t_transfer& a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, const t_transfer&>::f_call(a_object);
}

template<typename T>
inline typename t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, const t_shared&>::t_type f_as(const t_shared& a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, const t_shared&>::f_call(a_object);
}

template<typename T>
inline bool f_is(t_object* a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_is<T, t_object*>::f_call(a_object);
}

}

#endif
