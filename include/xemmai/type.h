#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include <string>
#include <typeinfo>

#include "value.h"

namespace xemmai
{

class t_extension;
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

template<typename T, size_t... A_Is>
static constexpr std::array<T, sizeof...(A_Is) + 1> f_append(const std::array<T, sizeof...(A_Is)>& a_xs, std::index_sequence<A_Is...>, T a_x)
{
	return {a_xs[A_Is]..., a_x};
}

using t_type_id = void(*)();

template<typename T>
void f_type_id()
{
}

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
			return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
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
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};
	typedef t_global t_extension;

	static constexpr std::array<t_type_id, 1> V_ids{f_type_id<t_object>};

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		return t_scoped(std::forward<T>(a_value));
	}
	static void f_initialize(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n);
	static std::wstring f_string(const t_value& a_self)
	{
		wchar_t cs[13 + sizeof(t_object*) * 2];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
		return cs;
	}
	static intptr_t f_hash(const t_value& a_self)
	{
		return a_self.f_tag();
	}
	static bool f_equals(const t_value& a_self, const t_value& a_other)
	{
		return a_self == a_other;
	}
	static bool f_not_equals(const t_value& a_self, const t_value& a_other)
	{
		return a_self != a_other;
	}
	static void f_own(const t_value& a_self);
	static void f_share(const t_value& a_self);
	void f_define();

	t_slot v_this;
	size_t v_depth;
	const t_type_id* v_ids;
	t_slot_of<t_type> v_super;
	t_slot v_module;
	bool v_builtin = false;
	bool v_primitive = false;
	bool v_revive = false;
	bool v_fixed = false;
	bool v_shared = false;

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids);
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_type_of(a_ids)
	{
		v_super.f_construct(a_super->v_this);
	}
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module);
	XEMMAI__PORTABLE__EXPORT virtual ~t_type_of() = default;
	template<typename T>
	bool f_derives() const
	{
		size_t i = t_type_of<T>::V_ids.size() - 1;
		return i <= v_depth && v_ids[i] == static_cast<t_type_id>(f_type_id<T>);
	}
	XEMMAI__PORTABLE__EXPORT virtual t_type* f_derive();
	XEMMAI__PORTABLE__EXPORT bool f_derives(t_type* a_type);
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
	t_scoped f_get_nonowned(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_get(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	XEMMAI__PORTABLE__EXPORT virtual bool f_has(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_remove(t_object* a_this, t_object* a_key);
	XEMMAI__PORTABLE__EXPORT virtual void f_hash(t_object* a_this, t_stacked* a_stack);
	XEMMAI__PORTABLE__EXPORT virtual void f_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n);
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

template<>
inline bool t_type_of<t_object>::f_derives<t_object>() const
{
	return true;
}

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

template<typename T, typename T_base = t_type>
struct t_bears : T_base
{
	typedef T t_what;
	typedef t_bears t_base;

	static constexpr std::array<t_type_id, T_base::V_ids.size() + 1> V_ids = f_append(T_base::V_ids, std::make_index_sequence<T_base::V_ids.size()>(), static_cast<t_type_id>(f_type_id<T>));

	using T_base::T_base;
};

template<typename T, typename T_base>
constexpr std::array<t_type_id, T_base::V_ids.size() + 1> t_bears<T, T_base>::V_ids;

template<typename T, typename T_base = t_type>
struct t_finalizes : T_base
{
	typedef t_finalizes t_base;

	using T_base::T_base;
	virtual void f_finalize(t_object* a_this)
	{
		delete &f_as<T&>(a_this);
	}
};

template<typename T, typename T_base = t_type>
struct t_holds : t_finalizes<T, t_bears<T, T_base>>
{
	typedef t_holds t_base;

	using t_finalizes<T, t_bears<T, T_base>>::t_finalizes;
};

template<typename T_base, bool A_fixed, bool A_shared>
struct t_with_traits : T_base
{
	typedef t_with_traits t_base;

	template<size_t A_n>
	t_with_traits(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : T_base(a_ids, a_super, std::move(a_module))
	{
		T_base::v_fixed = A_fixed;
		T_base::v_shared = A_shared;
	}
};

template<typename T_base>
struct t_derivable : T_base
{
	typedef t_derivable t_base;

	using T_base::T_base;
	virtual t_type* f_derive();
};

template<typename T_base>
struct t_underivable : T_base
{
	typedef t_underivable t_base;

	using T_base::T_base;
	virtual t_type* f_derive()
	{
		return nullptr;
	}
};

template<typename T_base>
struct t_uninstantiatable : T_base
{
	typedef t_uninstantiatable t_base;

	using T_base::T_base;
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
};

struct t_type_immutable : t_with_traits<t_type, true, true>
{
	using t_with_traits<t_type, true, true>::t_with_traits;
	virtual void f_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack);
	virtual t_scoped f_get(t_object* a_this, t_object* a_key);
	virtual void f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	virtual bool f_has(t_object* a_this, t_object* a_key);
	virtual t_scoped f_remove(t_object* a_this, t_object* a_key);
	virtual void f_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n);
};

}

#endif
