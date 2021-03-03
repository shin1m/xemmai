#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include "value.h"
#include <array>
#include <typeinfo>

namespace xemmai
{

class t_extension;
class t_global;

template<typename T>
struct t_fundamental
{
	using t_type = T;
};

template<typename T>
struct t_fundamental<const T>
{
	using t_type = typename t_fundamental<T>::t_type;
};

template<typename T>
struct t_fundamental<T*>
{
	using t_type = typename t_fundamental<T>::t_type;
};

template<typename T>
struct t_fundamental<T&>
{
	using t_type = typename t_fundamental<T>::t_type;
};

template<typename T>
struct t_fundamental<T&&>
{
	using t_type = typename t_fundamental<T>::t_type;
};

template<typename T_tag>
struct t_fundamental<t_value<T_tag>>
{
	using t_type = t_object;
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

using t_scan = void(*)(t_slot&);

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
			return f_object(std::forward<T1>(a_object))->template f_as<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		static_assert(!std::is_same_v<T0, t_object>);

		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return p ? &p->template f_as<T0>() : nullptr;
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			if (std::is_same_v<typename t_fundamental<T0>::t_type, t_object>) return true;
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		static_assert(!std::is_same_v<T0, t_object>);

		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<uintptr_t>(p)) {
			case e_tag__NULL:
				return true;
			case e_tag__BOOLEAN:
			case e_tag__INTEGER:
			case e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};
	using t_extension = t_global;

	static constexpr std::array<t_type_id, 1> V_ids{f_type_id<t_object>};

	template<typename T_extension, typename T>
	static t_pvalue f_transfer(T_extension* a_extension, T&& a_value)
	{
		return std::forward<T>(a_value);
	}
	static void f_initialize(xemmai::t_extension* a_extension, t_pvalue* a_stack, size_t a_n);
	static t_object* f_string(const t_pvalue& a_self);
	static intptr_t f__hash(const t_pvalue& a_self)
	{
		return a_self.f_tag();
	}
	static bool f__equals(const t_pvalue& a_self, const t_pvalue& a_other)
	{
		return a_self == a_other;
	}
	static bool f__not_equals(const t_pvalue& a_self, const t_pvalue& a_other)
	{
		return a_self != a_other;
	}
	static void f_own(const t_pvalue& a_self);
	static void f_share(const t_pvalue& a_self);
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

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids);
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module);
	template<typename T>
	void f_do_destruct()
	{
		static_cast<T*>(this)->~T();
	}
	void (t_type::*v_destruct)() = &t_type::f_do_destruct<t_type>;
	void f_destruct()
	{
		(this->*v_destruct)();
	}
	template<typename T, typename... T_an>
	t_object* f_new(bool a_shared, T_an&&... a_an)
	{
		return f_new_sized<T>(a_shared, 0, std::forward<T_an>(a_an)...);
	}
	template<typename T, typename... T_an>
	t_object* f_new_sized(bool a_shared, size_t a_data, T_an&&... a_an);
	template<typename T>
	bool f_derives() const
	{
		size_t i = t_type_of<T>::V_ids.size() - 1;
		return i <= v_depth && v_ids[i] == static_cast<t_type_id>(f_type_id<T>);
	}
	template<typename T>
	t_object* f_derive();
	t_object* f_do_derive();
	t_object* (t_type::*v_derive)() = &t_type::f_do_derive;
	bool f_derives(t_type* a_type);
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
	}
	void (*f_scan)(t_object*, t_scan) = f_do_scan;
	static void f_do_finalize(t_object* a_this)
	{
	}
	void (*f_finalize)(t_object*) = f_do_finalize;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	t_pvalue (t_type::*v_construct)(t_pvalue*, size_t) = &t_type::f_do_construct;
	t_pvalue f_construct(t_pvalue* a_stack, size_t a_n)
	{
		return (this->*v_construct)(a_stack, a_n);
	}
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	void (t_type::*v_instantiate)(t_pvalue*, size_t) = &t_type::f_do_instantiate;
	t_pvalue f_get_nonowned(t_object* a_this, t_object* a_key);
	void f_do_get_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack);
	void (t_type::*v_get_nonowned)(t_object*, t_object*, t_pvalue*) = &t_type::f_do_get_nonowned;
	void f_get_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack)
	{
		(this->*v_get_nonowned)(a_this, a_key, a_stack);
	}
	t_pvalue f_do_get(t_object* a_this, t_object* a_key);
	t_pvalue (t_type::*v_get)(t_object*, t_object*) = &t_type::f_do_get;
	t_pvalue f_get(t_object* a_this, t_object* a_key)
	{
		return (this->*v_get)(a_this, a_key);
	}
	t_pvalue f_get_of_type(t_object* a_key);
	static void f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value);
	void (*f_put)(t_object*, t_object*, const t_pvalue&) = f_do_put;
	bool f_do_has(t_object* a_this, t_object* a_key);
	bool (t_type::*v_has)(t_object*, t_object*) = &t_type::f_do_has;
	bool f_has(t_object* a_this, t_object* a_key)
	{
		return (this->*v_has)(a_this, a_key);
	}
	static t_pvalue f_do_remove(t_object* a_this, t_object* a_key);
	t_pvalue (*f_remove)(t_object*, t_object*) = f_do_remove;
	static void f_do_call_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n);
	void (*f_call_nonowned)(t_object*, t_object*, t_pvalue*, size_t) = f_do_call_nonowned;
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
	size_t (*f_call)(t_object*, t_pvalue*, size_t) = f_do_call;
	static void f_do_hash(t_object* a_this, t_pvalue* a_stack);
	void (*f_hash)(t_object*, t_pvalue*) = f_do_hash;
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_get_at)(t_object*, t_pvalue*) = f_do_get_at;
	static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_set_at)(t_object*, t_pvalue*) = f_do_set_at;
	static size_t f_do_plus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_plus)(t_object*, t_pvalue*) = f_do_plus;
	static size_t f_do_minus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_minus)(t_object*, t_pvalue*) = f_do_minus;
	static size_t f_do_not(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_not)(t_object*, t_pvalue*) = f_do_not;
	static size_t f_do_complement(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_complement)(t_object*, t_pvalue*) = f_do_complement;
	static size_t f_do_multiply(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_multiply)(t_object*, t_pvalue*) = f_do_multiply;
	static size_t f_do_divide(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_divide)(t_object*, t_pvalue*) = f_do_divide;
	static size_t f_do_modulus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_modulus)(t_object*, t_pvalue*) = f_do_modulus;
	static size_t f_do_add(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_add)(t_object*, t_pvalue*) = f_do_add;
	static size_t f_do_subtract(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_subtract)(t_object*, t_pvalue*) = f_do_subtract;
	static size_t f_do_left_shift(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_left_shift)(t_object*, t_pvalue*) = f_do_left_shift;
	static size_t f_do_right_shift(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_right_shift)(t_object*, t_pvalue*) = f_do_right_shift;
	static size_t f_do_less(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_less)(t_object*, t_pvalue*) = f_do_less;
	static size_t f_do_less_equal(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_less_equal)(t_object*, t_pvalue*) = f_do_less_equal;
	static size_t f_do_greater(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_greater)(t_object*, t_pvalue*) = f_do_greater;
	static size_t f_do_greater_equal(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_greater_equal)(t_object*, t_pvalue*) = f_do_greater_equal;
	static size_t f_do_equals(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_equals)(t_object*, t_pvalue*) = f_do_equals;
	static size_t f_do_not_equals(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_not_equals)(t_object*, t_pvalue*) = f_do_not_equals;
	static size_t f_do_and(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_and)(t_object*, t_pvalue*) = f_do_and;
	static size_t f_do_xor(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_xor)(t_object*, t_pvalue*) = f_do_xor;
	static size_t f_do_or(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_or)(t_object*, t_pvalue*) = f_do_or;
	static size_t f_do_send(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_send)(t_object*, t_pvalue*) = f_do_send;
	template<typename T, typename U>
	void f_override()
	{
		if (&T::f_do_derive != &U::f_do_derive) v_derive = static_cast<t_object*(t_type::*)()>(&T::f_do_derive);
		if (T::f_do_scan != U::f_do_scan) f_scan = T::f_do_scan;
		if (T::f_do_finalize != U::f_do_finalize) f_finalize = T::f_do_finalize;
		if (&T::f_do_construct != &U::f_do_construct) v_construct = static_cast<t_pvalue(t_type::*)(t_pvalue*, size_t)>(&T::f_do_construct);
		if (&T::f_do_instantiate != &U::f_do_instantiate) v_instantiate = static_cast<void(t_type::*)(t_pvalue*, size_t)>(&T::f_do_instantiate);
		if (T::f_do_call != U::f_do_call) f_call = T::f_do_call;
		if (T::f_do_hash != U::f_do_hash) f_hash = T::f_do_hash;
		if (T::f_do_get_at != U::f_do_get_at) f_get_at = T::f_do_get_at;
		if (T::f_do_set_at != U::f_do_set_at) f_set_at = T::f_do_set_at;
		if (T::f_do_plus != U::f_do_plus) f_plus = T::f_do_plus;
		if (T::f_do_minus != U::f_do_minus) f_minus = T::f_do_minus;
		if (T::f_do_not != U::f_do_not) f_not = T::f_do_not;
		if (T::f_do_complement != U::f_do_complement) f_complement = T::f_do_complement;
		if (T::f_do_multiply != U::f_do_multiply) f_multiply = T::f_do_multiply;
		if (T::f_do_divide != U::f_do_divide) f_divide = T::f_do_divide;
		if (T::f_do_modulus != U::f_do_modulus) f_modulus = T::f_do_modulus;
		if (T::f_do_add != U::f_do_add) f_add = T::f_do_add;
		if (T::f_do_subtract != U::f_do_subtract) f_subtract = T::f_do_subtract;
		if (T::f_do_left_shift != U::f_do_left_shift) f_left_shift = T::f_do_left_shift;
		if (T::f_do_right_shift != U::f_do_right_shift) f_right_shift = T::f_do_right_shift;
		if (T::f_do_less != U::f_do_less) f_less = T::f_do_less;
		if (T::f_do_less_equal != U::f_do_less_equal) f_less_equal = T::f_do_less_equal;
		if (T::f_do_greater != U::f_do_greater) f_greater = T::f_do_greater;
		if (T::f_do_greater_equal != U::f_do_greater_equal) f_greater_equal = T::f_do_greater_equal;
		if (T::f_do_equals != U::f_do_equals) f_equals = T::f_do_equals;
		if (T::f_do_not_equals != U::f_do_not_equals) f_not_equals = T::f_do_not_equals;
		if (T::f_do_and != U::f_do_and) f_and = T::f_do_and;
		if (T::f_do_xor != U::f_do_xor) f_xor = T::f_do_xor;
		if (T::f_do_or != U::f_do_or) f_or = T::f_do_or;
		if (T::f_do_send != U::f_do_send) f_send = T::f_do_send;
	}
};

template<typename T_tag>
struct t_type_of<t_object>::t_as<const t_value<T_tag>&>
{
	template<typename T>
	static const t_value<T_tag>& f_call(T&& a_object)
	{
		return a_object;
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

template<typename T, typename T_tag>
inline void f_check(const t_value<T_tag>& a_object, const wchar_t* a_name)
{
	if (!f_is<T>(a_object)) f_throw_type_error<T>(a_name);
}

template<typename T>
inline void t_slot_of<T>::f_construct(t_object* a_value)
{
	v_p = &f_as<T&>(a_value);
	v_slot = a_value;
}

template<typename T, typename T_base = t_type>
struct t_derives : T_base
{
	using t_what = T;
	using t_base = t_derives;

	template<size_t A_n>
	t_derives(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module) : T_base(a_ids, a_super, a_module)
	{
		this->template f_override<t_type_of<T>, T_base>();
	}
};

template<typename T, typename T_base = t_type>
struct t_bears : t_derives<T, T_base>
{
	using t_base = t_bears;

	static constexpr std::array<t_type_id, T_base::V_ids.size() + 1> V_ids = f_append(T_base::V_ids, std::make_index_sequence<T_base::V_ids.size()>(), static_cast<t_type_id>(f_type_id<T>));

	using t_derives<T, T_base>::t_derives;
};

template<typename T, typename T_base>
constexpr std::array<t_type_id, T_base::V_ids.size() + 1> t_bears<T, T_base>::V_ids;

template<typename T_base>
struct t_finalizes : T_base
{
	using t_base = t_finalizes;

	using T_base::T_base;
	static void f_do_finalize(t_object* a_this);
};

template<typename T, typename T_base = t_type>
struct t_holds : t_finalizes<t_bears<T, T_base>>
{
	using t_base = t_holds;

	using t_finalizes<t_bears<T, T_base>>::t_finalizes;
};

template<typename T_base>
struct t_fixed : T_base
{
	using t_base = t_fixed;

	template<size_t A_n>
	t_fixed(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module) : T_base(a_ids, a_super, a_module)
	{
		T_base::v_fixed = true;
	}
};

template<typename T_base>
struct t_uninstantiatable : T_base
{
	using t_base = t_uninstantiatable;

	using T_base::T_base;
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n)
	{
		f_throw(L"uninstantiatable."sv);
	}
};

struct t_type_immutable : t_fixed<t_type>
{
	template<size_t A_n>
	t_type_immutable(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module) : t_base(a_ids, a_super, a_module)
	{
		v_get_nonowned = static_cast<void(t_type::*)(t_object*, t_object*, t_pvalue*)>(&t_type_immutable::f_do_get_nonowned);
		v_get = static_cast<t_pvalue(t_type::*)(t_object*, t_object*)>(&t_type_immutable::f_do_get);
		f_put = f_do_put;
		v_has = static_cast<bool(t_type::*)(t_object*, t_object*)>(&t_type_immutable::f_do_has);
		f_remove = f_do_remove;
		f_call_nonowned = f_do_call_nonowned;
	}
	void f_do_get_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack);
	t_pvalue f_do_get(t_object* a_this, t_object* a_key);
	static void f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value);
	bool f_do_has(t_object* a_this, t_object* a_key);
	static t_pvalue f_do_remove(t_object* a_this, t_object* a_key);
	static void f_do_call_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n);
};

template<typename T>
struct t_derived : T
{
	template<size_t A_n>
	t_derived(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module) : T(a_ids, a_super, a_module)
	{
		this->v_construct = static_cast<t_pvalue(t_type::*)(t_pvalue*, size_t)>(&t_derived::f_do_construct);
		this->f_call = t_type::f_do_call;
		this->f_hash = t_type::f_do_hash;
		this->f_get_at = t_type::f_do_get_at;
		this->f_set_at = t_type::f_do_set_at;
		this->f_plus = t_type::f_do_plus;
		this->f_minus = t_type::f_do_minus;
		this->f_not = t_type::f_do_not;
		this->f_complement = t_type::f_do_complement;
		this->f_multiply = t_type::f_do_multiply;
		this->f_divide = t_type::f_do_divide;
		this->f_modulus = t_type::f_do_modulus;
		this->f_add = t_type::f_do_add;
		this->f_subtract = t_type::f_do_subtract;
		this->f_left_shift = t_type::f_do_left_shift;
		this->f_right_shift = t_type::f_do_right_shift;
		this->f_less = t_type::f_do_less;
		this->f_less_equal = t_type::f_do_less_equal;
		this->f_greater = t_type::f_do_greater;
		this->f_greater_equal = t_type::f_do_greater_equal;
		this->f_equals = t_type::f_do_equals;
		this->f_not_equals = t_type::f_do_not_equals;
		this->f_and = t_type::f_do_and;
		this->f_xor = t_type::f_do_xor;
		this->f_or = t_type::f_do_or;
		this->f_send = t_type::f_do_send;
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<typename T_base>
struct t_derivable : T_base
{
	using t_base = t_derivable;

	using T_base::T_base;
	t_object* f_do_derive()
	{
		return this->template f_derive<t_derived<t_type_of<typename T_base::t_what>>>();
	}
};

template<typename T_base>
struct t_underivable : T_base
{
	using t_base = t_underivable;

	using T_base::T_base;
	t_object* f_do_derive()
	{
		return nullptr;
	}
};

}

#endif
