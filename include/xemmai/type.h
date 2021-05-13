#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include "value.h"
#include <array>
#include <typeinfo>
#include <typeindex>
#include <vector>

namespace xemmai
{

class t_library;
class t_global;

template<typename T, typename = void>
struct t_fundamental
{
	using t_type = T;
};

template<typename T>
struct t_fundamental<T, std::enable_if_t<std::disjunction_v<std::is_const<T>, std::is_pointer<T>, std::is_reference<T>>>>
{
	using t_type = typename t_fundamental<std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T>>>>::t_type;
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

#ifdef _WIN32
using t_type_id = std::type_index;
template<typename T>
inline t_type_id f_type_id()
{
	return typeid(T);
}
#define XEMMAI__TYPE__IDS_MODIFIER inline const
#else
using t_type_id = void(*)();
template<typename T>
void f__type_id()
{
}
template<typename T>
constexpr t_type_id f_type_id()
{
	return f__type_id<T>;
}
#define XEMMAI__TYPE__IDS_MODIFIER constexpr
#endif

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

struct t_fields
{
	std::vector<t_root> v_instance;
	std::vector<std::pair<t_root, t_rvalue>> v_class;
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
	using t_library = t_global;

	static XEMMAI__TYPE__IDS_MODIFIER std::array<t_type_id, 1> V_ids{f_type_id<t_object>()};
	static constexpr size_t V_native = 0;

	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, T&& a_value)
	{
		return std::forward<T>(a_value);
	}
	static void f_initialize(xemmai::t_library* a_library, t_pvalue* a_stack, size_t a_n);
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
	void f_define();

	t_slot v_this;
	size_t v_depth;
	const t_type_id* v_ids;
	t_slot_of<t_type> v_super;
	t_slot v_module;
	size_t v_fields_offset;
	size_t v_instance_fields;
	size_t v_class_fields;
	bool v_builtin = false;
	bool v_primitive = false;
	bool v_revive = false;

	t_type_of();
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module, size_t a_native, size_t a_instance_fields, const std::vector<std::pair<t_root, t_rvalue>>& a_fields, const std::map<t_object*, size_t>& a_key2index);
	std::pair<t_slot, t_svalue>* f_fields()
	{
		return reinterpret_cast<std::pair<t_slot, t_svalue>*>(this + 1);
	}
	std::pair<t_object*, size_t>* f_key2index()
	{
		return reinterpret_cast<std::pair<t_object*, size_t>*>(f_fields() + v_class_fields);
	}
	size_t f_index(t_object* a_key);
	void f_scan_type(t_scan a_scan)
	{
		a_scan(v_this);
		a_scan(v_super);
		a_scan(v_module);
		auto p = f_fields();
		for (size_t i = 0; i < v_class_fields; ++i) {
			a_scan(p[i].first);
			a_scan(p[i].second);
		}
	}
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
	t_object* f_new(T_an&&... a_an);
	template<typename T>
	bool f_derives() const
	{
		size_t i = t_type_of<T>::V_ids.size() - 1;
		return i <= v_depth && v_ids[i] == f_type_id<T>();
	}
	std::pair<std::vector<std::pair<t_root, t_rvalue>>, std::map<t_object*, size_t>> f_merge(const t_fields& a_fields);
	template<typename T>
	t_object* f_derive(t_object* a_module, const t_fields& a_fields);
	XEMMAI__PORTABLE__EXPORT t_object* f_do_derive(const t_fields& a_fields);
	t_object* (t_type::*v_derive)(const t_fields&) = nullptr;
	bool f_derives(t_type* a_type);
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
	}
	void (*f_scan)(t_object*, t_scan) = f_do_scan;
	static void f_do_finalize(t_object* a_this)
	{
	}
	void (*f_finalize)(t_object*) = f_do_finalize;
	XEMMAI__PORTABLE__EXPORT t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	t_pvalue (t_type::*v_construct)(t_pvalue*, size_t) = &t_type::f_do_construct;
	t_pvalue f_construct(t_pvalue* a_stack, size_t a_n)
	{
		return (this->*v_construct)(a_stack, a_n);
	}
	XEMMAI__PORTABLE__EXPORT void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	void (t_type::*v_instantiate)(t_pvalue*, size_t) = &t_type::f_do_instantiate;
	XEMMAI__PORTABLE__EXPORT t_pvalue f_do_get(t_object* a_this, t_object* a_key);
	t_pvalue (t_type::*v_get)(t_object*, t_object*) = &t_type::f_do_get;
	t_pvalue f_get(const t_pvalue& a_this, t_object* a_key);
	void f_get(const t_pvalue& a_this, t_object* a_key, t_pvalue* a_stack);
	XEMMAI__PORTABLE__EXPORT static void f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value);
	void (*v_put)(t_object*, t_object*, const t_pvalue&) = f_do_put;
	void f_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value);
	XEMMAI__PORTABLE__EXPORT bool f_do_has(t_object* a_this, t_object* a_key);
	bool (t_type::*v_has)(t_object*, t_object*) = &t_type::f_do_has;
	bool f_has(t_object* a_this, t_object* a_key)
	{
		return (this->*v_has)(a_this, a_key);
	}
	XEMMAI__PORTABLE__EXPORT static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
	size_t (*f_call)(t_object*, t_pvalue*, size_t) = f_do_call;
	void f_invoke(const t_pvalue& a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n);
	XEMMAI__PORTABLE__EXPORT static void f_do_hash(t_object* a_this, t_pvalue* a_stack);
	void (*f_hash)(t_object*, t_pvalue*) = f_do_hash;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_get_at)(t_object*, t_pvalue*) = f_do_get_at;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_set_at)(t_object*, t_pvalue*) = f_do_set_at;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_plus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_plus)(t_object*, t_pvalue*) = f_do_plus;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_minus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_minus)(t_object*, t_pvalue*) = f_do_minus;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_not(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_not)(t_object*, t_pvalue*) = f_do_not;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_complement(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_complement)(t_object*, t_pvalue*) = f_do_complement;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_multiply(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_multiply)(t_object*, t_pvalue*) = f_do_multiply;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_divide(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_divide)(t_object*, t_pvalue*) = f_do_divide;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_modulus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_modulus)(t_object*, t_pvalue*) = f_do_modulus;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_add(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_add)(t_object*, t_pvalue*) = f_do_add;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_subtract(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_subtract)(t_object*, t_pvalue*) = f_do_subtract;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_left_shift(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_left_shift)(t_object*, t_pvalue*) = f_do_left_shift;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_right_shift(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_right_shift)(t_object*, t_pvalue*) = f_do_right_shift;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_less(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_less)(t_object*, t_pvalue*) = f_do_less;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_less_equal(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_less_equal)(t_object*, t_pvalue*) = f_do_less_equal;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_greater(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_greater)(t_object*, t_pvalue*) = f_do_greater;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_greater_equal(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_greater_equal)(t_object*, t_pvalue*) = f_do_greater_equal;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_equals(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_equals)(t_object*, t_pvalue*) = f_do_equals;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_not_equals(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_not_equals)(t_object*, t_pvalue*) = f_do_not_equals;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_and(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_and)(t_object*, t_pvalue*) = f_do_and;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_xor(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_xor)(t_object*, t_pvalue*) = f_do_xor;
	XEMMAI__PORTABLE__EXPORT static size_t f_do_or(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_or)(t_object*, t_pvalue*) = f_do_or;
	template<typename T, typename U>
	void f_override()
	{
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

	static constexpr size_t V_native = sizeof(T);

	template<typename... T_an>
	t_derives(T_an&&... a_an) : T_base(std::forward<T_an>(a_an)...)
	{
		this->template f_override<t_type_of<T>, T_base>();
	}
};

template<typename T, typename T_base = t_type>
struct t_bears : t_derives<T, T_base>
{
	using t_base = t_bears;

	static XEMMAI__TYPE__IDS_MODIFIER std::array<t_type_id, T_base::V_ids.size() + 1> V_ids = f_append(T_base::V_ids, std::make_index_sequence<T_base::V_ids.size()>(), f_type_id<T>());

	using t_derives<T, T_base>::t_derives;
};

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
struct t_uninstantiatable : T_base
{
	using t_base = t_uninstantiatable;

	using T_base::T_base;
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n)
	{
		f_throw(L"uninstantiatable."sv);
	}
};

template<typename T>
struct t_derived : T
{
	template<typename... T_an>
	t_derived(T_an&&... a_an) : T(std::forward<T_an>(a_an)...)
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
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<typename T_base>
struct t_derivable : T_base
{
	using t_base = t_derivable;

	template<typename... T_an>
	t_derivable(T_an&&... a_an) : T_base(std::forward<T_an>(a_an)...)
	{
		this->v_derive = static_cast<t_object*(t_type::*)(const t_fields&)>(&t_derivable::f_do_derive);
	}
	t_object* f_do_derive(const t_fields& a_fields)
	{
		return this->template f_derive<t_derived<t_type_of<typename T_base::t_what>>>(this->v_module, a_fields);
	}
};

}

#endif
