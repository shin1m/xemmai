#ifndef XEMMAI__TYPE_H
#define XEMMAI__TYPE_H

#include "value.h"
#include <array>
#include <map>
#ifdef _WIN32
#include <typeindex>
#endif
#include <typeinfo>
#include <vector>

namespace xemmai
{

struct t_library;
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

template<typename T, size_t... A_i>
static constexpr std::array<T, sizeof...(A_i) + 1> f_append(const std::array<T, sizeof...(A_i)>& a_xs, std::index_sequence<A_i...>, T a_x)
{
	return {a_xs[A_i]..., a_x};
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
using t_type_id = const void*;
template<typename> const void* v__type_id;
template<typename T>
constexpr t_type_id f_type_id()
{
	return &v__type_id<T>;
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
	operator t_object*() const
	{
		return v_slot;
	}
};

using t_scan = void(*)(t_object*);

struct t_fields
{
	std::vector<t_root> v_instance;
	std::vector<std::pair<t_root, t_rvalue>> v_class;
};

template<>
struct t_type_of<t_object>
{
	template<typename> struct t_cast;
	template<typename T_tag>
	struct t_cast<const t_value<T_tag>&>
	{
		static const t_value<T_tag>& f_as(auto&& a_object)
		{
			return a_object;
		}
		static bool f_is(t_object* a_object)
		{
			return true;
		}
	};
	using t_library = t_global;

	static XEMMAI__TYPE__IDS_MODIFIER std::array<t_type_id, 1> c_IDS{f_type_id<t_object>()};
	static constexpr size_t c_NATIVE = 0;
	static constexpr size_t c_FIELDS = 25;

	static t_pvalue f_transfer(auto* a_library, auto&& a_value)
	{
		return std::forward<decltype(a_value)>(a_value);
	}
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
	XEMMAI__LOCAL void f_define();

	t_slot v_this;
	size_t v_depth;
	const t_type_id* v_ids;
	t_slot_of<t_type> v_super;
	t_slot v_module;
	size_t v_fields_offset;
	size_t v_instance_fields;
	size_t v_fields;
	bool v_builtin = false;
	bool v_revive = false;
	bool v_bindable = false;

	t_type_of();
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_object* a_module, size_t a_native, size_t a_instance_fields, const std::vector<std::pair<t_root, t_rvalue>>& a_fields, const std::map<t_object*, size_t>& a_key2index);
	std::pair<t_slot, t_svalue>* f_fields()
	{
		return reinterpret_cast<std::pair<t_slot, t_svalue>*>(this + 1);
	}
	std::pair<t_object*, size_t>* f_key2index()
	{
		return reinterpret_cast<std::pair<t_object*, size_t>*>(f_fields() + v_fields);
	}
	XEMMAI__LOCAL size_t f_index(t_object* a_key);
	void f_scan_type(t_scan a_scan)
	{
		a_scan(v_this);
		a_scan(v_super);
		a_scan(v_module);
		auto p = f_fields();
		for (size_t i = 0; i < v_fields; ++i) {
			a_scan(p[i].first);
			a_scan(p[i].second);
		}
	}
	template<typename T>
	t_object* f_new(auto&&... a_xs);
	template<typename T>
	bool f_derives() const
	{
		size_t i = t_type_of<T>::c_IDS.size() - 1;
		return i <= v_depth && v_ids[i] == f_type_id<T>();
	}
	XEMMAI__PUBLIC std::pair<std::vector<std::pair<t_root, t_rvalue>>, std::map<t_object*, size_t>> f_merge(const t_fields& a_fields);
	template<typename T>
	t_object* f_derive(t_object* a_module, const t_fields& a_fields, bool a_overridden_construct = false);
	XEMMAI__PUBLIC t_object* f_do_derive(const t_fields& a_fields);
	t_object* (t_type::*v_derive)(const t_fields&) = nullptr;
	t_object* f_derive(const t_fields& a_fields)
	{
		return (this->*v_derive)(a_fields);
	}
	bool f_derives(t_type* a_type);
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
	}
	void (*f_scan)(t_object*, t_scan) = f_do_scan;
	void (*f_finalize)(t_object*, t_scan) = nullptr;
	XEMMAI__PUBLIC t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	t_pvalue (t_type::*v_construct)(t_pvalue*, size_t) = &t_type::f_do_construct;
	t_pvalue f_construct(t_pvalue* a_stack, size_t a_n)
	{
		return (this->*v_construct)(a_stack, a_n);
	}
	XEMMAI__PUBLIC void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	void (t_type::*v_instantiate)(t_pvalue*, size_t) = &t_type::f_do_instantiate;
	static void f_throw_undefined_field [[noreturn]] (t_object* a_key);
	XEMMAI__PUBLIC t_pvalue f_do_get(t_object* a_this, t_object* a_key, size_t& a_index);
	t_pvalue (t_type::*v_get)(t_object*, t_object*, size_t&) = &t_type::f_do_get;
	t_pvalue f__get(const t_pvalue& a_this, t_object* a_key, size_t& a_index);
	t_pvalue f_get(const t_pvalue& a_this, t_object* a_key, size_t& a_index);
	void f_bind_class(auto&& a_this, size_t a_index, t_pvalue* a_stack);
	void f__bind(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack);
	void f_bind(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack);
	XEMMAI__PUBLIC static void f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value);
	void (*v_put)(t_object*, t_object*, const t_pvalue&) = f_do_put;
	void f__put(t_object* a_this, t_object* a_key, size_t& a_index, const t_pvalue& a_value);
	void f_put(t_object* a_this, t_object* a_key, size_t& a_index, const t_pvalue& a_value);
	XEMMAI__PUBLIC bool f_do_has(t_object* a_this, t_object* a_key);
	bool (t_type::*v_has)(t_object*, t_object*) = &t_type::f_do_has;
	bool f_has(t_object* a_this, t_object* a_key, size_t& a_index);
	XEMMAI__PUBLIC static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
	size_t (*f_call)(t_object*, t_pvalue*, size_t) = f_do_call;
	void f_invoke_class(auto&& a_this, size_t a_index, t_pvalue* a_stack, size_t a_n);
	XEMMAI__PUBLIC void f__invoke(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack, size_t a_n);
	void f_invoke(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack, size_t a_n);
	XEMMAI__PUBLIC static void f_do_string(t_object* a_this, t_pvalue* a_stack);
	void (*f_string)(t_object*, t_pvalue*) = f_do_string;
	XEMMAI__PUBLIC static void f_do_hash(t_object* a_this, t_pvalue* a_stack);
	void (*f_hash)(t_object*, t_pvalue*) = f_do_hash;
	XEMMAI__PUBLIC static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_get_at)(t_object*, t_pvalue*) = f_do_get_at;
	XEMMAI__PUBLIC static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_set_at)(t_object*, t_pvalue*) = f_do_set_at;
	XEMMAI__PUBLIC static size_t f_do_plus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_plus)(t_object*, t_pvalue*) = f_do_plus;
	XEMMAI__PUBLIC static size_t f_do_minus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_minus)(t_object*, t_pvalue*) = f_do_minus;
	XEMMAI__PUBLIC static size_t f_do_complement(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_complement)(t_object*, t_pvalue*) = f_do_complement;
	XEMMAI__PUBLIC static size_t f_do_multiply(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_multiply)(t_object*, t_pvalue*) = f_do_multiply;
	XEMMAI__PUBLIC static size_t f_do_divide(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_divide)(t_object*, t_pvalue*) = f_do_divide;
	XEMMAI__PUBLIC static size_t f_do_modulus(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_modulus)(t_object*, t_pvalue*) = f_do_modulus;
	XEMMAI__PUBLIC static size_t f_do_add(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_add)(t_object*, t_pvalue*) = f_do_add;
	XEMMAI__PUBLIC static size_t f_do_subtract(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_subtract)(t_object*, t_pvalue*) = f_do_subtract;
	XEMMAI__PUBLIC static size_t f_do_left_shift(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_left_shift)(t_object*, t_pvalue*) = f_do_left_shift;
	XEMMAI__PUBLIC static size_t f_do_right_shift(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_right_shift)(t_object*, t_pvalue*) = f_do_right_shift;
	XEMMAI__PUBLIC static size_t f_do_less(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_less)(t_object*, t_pvalue*) = f_do_less;
	XEMMAI__PUBLIC static size_t f_do_less_equal(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_less_equal)(t_object*, t_pvalue*) = f_do_less_equal;
	XEMMAI__PUBLIC static size_t f_do_greater(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_greater)(t_object*, t_pvalue*) = f_do_greater;
	XEMMAI__PUBLIC static size_t f_do_greater_equal(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_greater_equal)(t_object*, t_pvalue*) = f_do_greater_equal;
	XEMMAI__PUBLIC static size_t f_do_equals(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_equals)(t_object*, t_pvalue*) = f_do_equals;
	XEMMAI__PUBLIC static size_t f_do_not_equals(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_not_equals)(t_object*, t_pvalue*) = f_do_not_equals;
	XEMMAI__PUBLIC static size_t f_do_and(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_and)(t_object*, t_pvalue*) = f_do_and;
	XEMMAI__PUBLIC static size_t f_do_xor(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_xor)(t_object*, t_pvalue*) = f_do_xor;
	XEMMAI__PUBLIC static size_t f_do_or(t_object* a_this, t_pvalue* a_stack);
	size_t (*f_or)(t_object*, t_pvalue*) = f_do_or;
	template<typename T, typename U>
	void f_override()
	{
		if (T::f_do_scan != U::f_do_scan) f_scan = T::f_do_scan;
		if (&T::f_do_construct != &U::f_do_construct) v_construct = static_cast<t_pvalue(t_type::*)(t_pvalue*, size_t)>(&T::f_do_construct);
		if (&T::f_do_instantiate != &U::f_do_instantiate) v_instantiate = static_cast<void(t_type::*)(t_pvalue*, size_t)>(&T::f_do_instantiate);
		if (T::f_do_call != U::f_do_call) f_call = T::f_do_call;
		if (T::f_do_string != U::f_do_string) f_string = T::f_do_string;
		if (T::f_do_hash != U::f_do_hash) f_hash = T::f_do_hash;
		if (T::f_do_get_at != U::f_do_get_at) f_get_at = T::f_do_get_at;
		if (T::f_do_set_at != U::f_do_set_at) f_set_at = T::f_do_set_at;
		if (T::f_do_plus != U::f_do_plus) f_plus = T::f_do_plus;
		if (T::f_do_minus != U::f_do_minus) f_minus = T::f_do_minus;
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

template<>
inline bool t_type_of<t_object>::f_derives<t_object>() const
{
	return true;
}

template<typename T>
inline decltype(auto) f_as(auto&& a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_cast<T>::f_as(std::forward<decltype(a_object)>(a_object));
}

template<typename T>
inline bool f_is(t_object* a_object)
{
	return t_type_of<typename t_fundamental<T>::t_type>::template t_cast<T>::f_is(a_object);
}

XEMMAI__PUBLIC void f_throw_type_error [[noreturn]] (const std::type_info& a_type, const wchar_t* a_name);

template<typename T>
void f_throw_type_error [[noreturn]] (const wchar_t* a_name)
{
	f_throw_type_error(typeid(typename t_fundamental<T>::t_type), a_name);
}

template<typename T>
inline void f_check(t_object* a_object, const wchar_t* a_name)
{
	if (!f_is<T>(a_object)) [[unlikely]] f_throw_type_error<T>(a_name);
}

template<typename T, typename T_base = t_type>
struct t_derives : T_base
{
	using t_what = T;
	using t_base = t_derives;

	static constexpr size_t c_NATIVE = sizeof(T);

	t_derives(auto&&... a_xs) : T_base(std::forward<decltype(a_xs)>(a_xs)...)
	{
		this->template f_override<t_type_of<T>, T_base>();
	}
};

template<typename T, typename T_base = t_type>
struct t_bears : t_derives<T, T_base>
{
	using t_base = t_bears;

	static XEMMAI__TYPE__IDS_MODIFIER std::array<t_type_id, T_base::c_IDS.size() + 1> c_IDS = f_append(T_base::c_IDS, std::make_index_sequence<T_base::c_IDS.size()>(), f_type_id<T>());

	using t_derives<T, T_base>::t_derives;
};

template<typename T_base>
struct t_finalizes : T_base
{
	using t_base = t_finalizes;

	t_finalizes(auto&&... a_xs) : T_base(std::forward<decltype(a_xs)>(a_xs)...)
	{
		this->f_finalize = f_do_finalize;
	}
	static void f_do_finalize(t_object* a_this, t_scan a_scan);
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
	t_derived(auto&&... a_xs) : T(std::forward<decltype(a_xs)>(a_xs)...)
	{
		this->f_call = t_type::f_do_call;
		this->f_string = t_type::f_do_string;
		this->f_hash = t_type::f_do_hash;
		this->f_get_at = t_type::f_do_get_at;
		this->f_set_at = t_type::f_do_set_at;
		this->f_plus = t_type::f_do_plus;
		this->f_minus = t_type::f_do_minus;
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
};

template<typename T_base, typename T_derived = t_derived<t_type_of<typename T_base::t_what>>>
struct t_derivable : T_base
{
	using t_base = t_derivable;

	t_derivable(auto&&... a_xs) : T_base(std::forward<decltype(a_xs)>(a_xs)...)
	{
		this->v_derive = static_cast<t_object*(t_type::*)(const t_fields&)>(&t_derivable::f_do_derive);
	}
	t_object* f_do_derive(const t_fields& a_fields)
	{
		return this->template f_derive<T_derived>(this->v_module, a_fields);
	}
};

template<typename T>
struct t_derived_primitive : t_derived<t_type_of<T>>
{
	t_derived_primitive(auto&&... a_xs) : t_derived<t_type_of<T>>(std::forward<decltype(a_xs)>(a_xs)...)
	{
		this->v_construct = static_cast<t_pvalue(t_type::*)(t_pvalue*, size_t)>(&t_derived_primitive::f_do_construct);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n)
	{
		if (a_n < 1) f_throw(L"must be called with at least an argument."sv);
		f_check<T>(a_stack[2], L"argument0");
		return this->template f_new<T>(f_as<T>(a_stack[2]));
	}
};

}

#endif
