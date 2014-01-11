#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include <string>
#include <typeinfo>

#include "value.h"

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

template<typename T>
struct t_fundamental<T&&>
{
	typedef typename t_fundamental<T>::t_type t_type;
};

template<>
struct t_fundamental<t_value>
{
	typedef t_object t_type;
};

template<>
struct t_fundamental<t_scoped>
{
	typedef t_object t_type;
};

template<>
struct t_fundamental<t_slot>
{
	typedef t_object t_type;
};

template<typename T>
struct t_type_of;

template<>
struct t_type_of<t_object>
{
	static t_object* f_object(t_object* a_p)
	{
		return a_p;
	}
	static t_object* f_object(const t_value& a_value)
	{
		return a_value;
	}
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			return t_as<T0&, T1>::f_call(f_object(a_object));
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0*, T1>
	{
		typedef T0* t_type;

		static T0* f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) == t_value::e_tag__NULL ? nullptr : static_cast<T0*>(f_object(a_object)->f_pointer());
		}
	};
	template<typename T0, typename T1>
	struct t_as<const T0*, T1>
	{
		typedef const T0* t_type;

		static const T0* f_call(T1 a_object)
		{
			return t_as<T0*, T1>::f_call(f_object(a_object));
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0&, T1>
	{
		typedef T0& t_type;

		static T0& f_call(T1 a_object)
		{
			return *static_cast<T0*>(f_object(a_object)->f_pointer());
		}
	};
	template<typename T0, typename T1>
	struct t_as<const T0&, T1>
	{
		typedef const T0& t_type;

		static const T0& f_call(T1 a_object)
		{
			return t_as<T0&, T1>::f_call(f_object(a_object));
		}
	};
	template<typename T>
	struct t_as<t_object*, T>;
	template<typename T>
	struct t_as<const t_value&, T>
	{
		typedef t_scoped t_type;

		static t_scoped f_call(T a_object)
		{
			return a_object;
		}
	};
	template<typename T>
	struct t_as<t_scoped&&, T>
	{
		typedef t_scoped t_type;

		static t_scoped f_call(T a_object)
		{
			return a_object;
		}
	};
	template<typename T0, typename T1>
	struct t_is
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(f_object(a_object)->f_type_as_type()) != nullptr;
		}
	};
	template<typename T0, typename T1>
	struct t_is<T0*, T1>
	{
		static bool f_call(T1 a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(f_object(a_object)->f_type_as_type()) != nullptr;
			}
		}
	};
	template<typename T0, typename T1>
	struct t_is<const T0*, T1>
	{
		static bool f_call(T1 a_object)
		{
			return t_is<T0*, T1>::f_call(f_object(a_object));
		}
	};
	template<typename T>
	struct t_is<t_object*, T>;
	template<typename T>
	struct t_is<const t_value&, T>
	{
		static bool f_call(T a_object)
		{
			return true;
		}
	};
	template<typename T>
	struct t_is<t_scoped&&, T>
	{
		static bool f_call(T a_object)
		{
			return true;
		}
	};
	typedef t_global t_extension;

	t_slot v_module;
	t_slot v_super;
	bool v_builtin = false;
	bool v_primitive = false;
	bool v_revive = false;
	bool v_fixed = false;
	bool v_shared = false;
	bool v_immutable = false;

	static t_scoped f_transfer(const t_global* a_extension, t_object* a_value)
	{
		return a_value;
	}
	static t_scoped f_transfer(const t_global* a_extension, const t_value& a_value)
	{
		return a_value;
	}
	static t_scoped f_transfer(const t_global* a_extension, t_scoped&& a_value)
	{
		return a_value;
	}
	XEMMAI__PORTABLE__EXPORT static bool f_derives(t_object* a_this, t_object* a_type);
	static void f_initialize(t_object* a_module, t_slot* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT static std::wstring f_string(const t_value& a_self)
	{
		wchar_t cs[13 + sizeof(t_object*) * 2];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
		return cs;
	}
	XEMMAI__PORTABLE__EXPORT static ptrdiff_t f_hash(const t_value& a_self)
	{
		return a_self.f_tag();
	}
	XEMMAI__PORTABLE__EXPORT static bool f_equals(const t_value& a_self, const t_value& a_other)
	{
		return a_self == a_other;
	}
	XEMMAI__PORTABLE__EXPORT static bool f_not_equals(const t_value& a_self, const t_value& a_other)
	{
		return a_self != a_other;
	}
	XEMMAI__PORTABLE__EXPORT static void f_own(const t_value& a_self);
	XEMMAI__PORTABLE__EXPORT static void f_share(const t_value& a_self);
	XEMMAI__PORTABLE__EXPORT static void f_define(t_object* a_class);

	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : v_module(std::move(a_module)), v_super(std::move(a_super))
	{
	}
	XEMMAI__PORTABLE__EXPORT virtual ~t_type_of() = default;
	XEMMAI__PORTABLE__EXPORT virtual t_type_of* f_derive(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_get(const t_value& a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	XEMMAI__PORTABLE__EXPORT virtual bool f_has(const t_value& a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_remove(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_hash(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_call(t_object* a_this, t_slot* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual void f_get_at(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_set_at(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_plus(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_minus(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_not(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_complement(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_multiply(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_divide(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_modulus(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_add(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_subtract(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_left_shift(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_right_shift(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_less(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_less_equal(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_greater(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_greater_equal(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_equals(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_not_equals(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_and(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_xor(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_or(t_object* a_this, t_slot* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_send(t_object* a_this, t_slot* a_stack);
};

template<>
struct t_type_of<t_object>::t_as<const t_value&, const t_value&>
{
	typedef const t_value& t_type;

	static const t_value& f_call(const t_value& a_object)
	{
		return a_object;
	}
};

template<>
struct t_type_of<t_object>::t_as<t_scoped&&, t_scoped&&>
{
	typedef t_scoped&& t_type;

	static t_scoped&& f_call(t_scoped&& a_object)
	{
		return std::move(a_object);
	}
};

typedef t_type_of<t_object> t_type;

template<typename T>
inline typename t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, t_object*>::t_type f_as(t_object* a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, t_object*>::f_call(a_object);
}

template<typename T>
inline typename t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, const t_value&>::t_type f_as(const t_value& a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, const t_value&>::f_call(a_object);
}

template<typename T>
inline typename t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, t_scoped&&>::t_type f_as(t_scoped&& a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_as<T, t_scoped&&>::f_call(std::move(a_object));
}

template<typename T>
inline bool f_is(t_object* a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_is<T, t_object*>::f_call(a_object);
}

template<typename T>
inline bool f_is(const t_value& a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_is<T, const t_value&>::f_call(a_object);
}

XEMMAI__PORTABLE__EXPORT void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name);

template<typename T>
void f_throw_type_error(const wchar_t* a_name)
{
	f_throw_type_error(typeid(typename t_fundamental<T>::t_type), a_name);
}

template<typename T>
inline void f_check(const t_value& a_object, const wchar_t* a_name)
{
	if (!f_is<T>(a_object)) f_throw_type_error<T>(a_name);
}

}

#endif
