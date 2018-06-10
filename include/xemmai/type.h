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

template<typename T>
class t_slot_of
{
	T* v_p = nullptr;
	t_slot v_slot;

public:
	void f_construct(t_object* a_value);
	operator T*() const
	{
		return v_p;
	}
	T* operator->() const
	{
		return v_p;
	}
	operator t_slot&()
	{
		return v_slot;
	}
};

template<>
struct t_type_of<t_object>
{
	template<typename T>
	static t_object* f_object(T&& a_value)
	{
		return a_value;
	}
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *static_cast<typename t_fundamental<T0>::t_type*>(f_object(std::forward<T1>(a_object))->v_pointer);
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		static_assert(!std::is_same<T0, t_object>::value);

		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<size_t>(p) == t_value::e_tag__NULL ? nullptr : static_cast<T0*>(p->v_pointer);
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			if (std::is_same<typename t_fundamental<T0>::t_type, t_object>::value) return true;
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(p->f_type()) != nullptr;
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		static_assert(!std::is_same<T0, t_object>::value);

		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(p->f_type()) != nullptr;
			}
		}
	};
	typedef t_global t_extension;

	t_slot v_this;
	t_slot v_module;
	t_slot_of<t_type> v_super;
	bool v_builtin = false;
	bool v_primitive = false;
	bool v_revive = false;
	bool v_fixed = false;
	bool v_shared = false;
	bool v_immutable = false;

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		return t_scoped(std::forward<T>(a_value));
	}
	static void f_initialize(t_object* a_module, t_stacked* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT static std::wstring f_string(const t_value& a_self)
	{
		wchar_t cs[13 + sizeof(t_object*) * 2];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
		return cs;
	}
	XEMMAI__PORTABLE__EXPORT static intptr_t f_hash(const t_value& a_self)
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
	XEMMAI__PORTABLE__EXPORT void f_define();

	t_type_of();
	t_type_of(t_type* a_super);
	t_type_of(t_scoped&& a_module, t_type* a_super);
	XEMMAI__PORTABLE__EXPORT virtual ~t_type_of() = default;
	XEMMAI__PORTABLE__EXPORT virtual t_type* f_derive();
	XEMMAI__PORTABLE__EXPORT bool f_derives(t_type* a_type);
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_get(const t_value& a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	XEMMAI__PORTABLE__EXPORT virtual bool f_has(const t_value& a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_remove(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_hash(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_get_at(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_set_at(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_plus(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_minus(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_not(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_complement(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_multiply(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_divide(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_modulus(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_add(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_subtract(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_left_shift(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_right_shift(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_less(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_less_equal(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_greater(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_greater_equal(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_equals(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_not_equals(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_and(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_xor(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_or(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual size_t f_send(t_object* a_this, t_stacked* a_stack);
};

template<>
struct t_type_of<t_object>::t_as<const t_value&>
{
	template<typename T>
	static const t_value& f_call(T&& a_object)
	{
		return a_object;
	}
};

template<>
struct t_type_of<t_object>::t_as<t_scoped&&>
{
	template<typename T>
	static t_scoped f_call(T&& a_object)
	{
		return std::forward<T>(a_object);
	}
	static t_scoped&& f_call(t_scoped&& a_object)
	{
		return std::move(a_object);
	}
	static t_scoped f_call(t_stacked&& a_object)
	{
		return static_cast<t_value&&>(a_object);
	}
};

template<typename T0, typename T1>
inline decltype(auto) f_as(T1&& a_object)
{
	return t_type_of<typename t_fundamental<T0>::t_type>::template t_as<T0>::f_call(std::forward<T1>(a_object));
}

template<typename T0, typename T1>
inline bool f_is(T1&& a_object)
{
	return t_type_of<typename t_fundamental<T0>::t_type>::template t_is<T0>::f_call(std::forward<T1>(a_object));
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

template<typename T>
inline void t_slot_of<T>::f_construct(t_object* a_value)
{
	v_p = &f_as<T&>(a_value);
	v_slot.f_construct(a_value);
}

}

#endif
