#ifndef XEMMAI__CONVERT_H
#define XEMMAI__CONVERT_H

#include "global.h"

namespace xemmai
{

template<wchar_t... A_cs>
struct t_cs
{
	static inline constexpr wchar_t v[sizeof...(A_cs) + 1] = {A_cs..., L'\0'};

	constexpr operator const wchar_t*() const
	{
		return v;
	}
	template<wchar_t... A_ys>
	constexpr t_cs<A_cs..., A_ys...> operator+(const t_cs<A_ys...>&) const
	{
		return t_cs<A_cs..., A_ys...>();
	}
};

template<size_t A_n>
struct t__n2s
{
	using t = decltype(typename t__n2s<A_n / 10>::t() + t_cs<L'0' + (A_n % 10)>());
};

template<>
struct t__n2s<0>
{
	using t = t_cs<>;
};

template<size_t A_n>
struct t_n2s
{
	using t = typename std::conditional<A_n <= 0, t_cs<L'0'>, typename t__n2s<A_n>::t>::type;
};

template<typename... T_an>
struct t_signature
{
	static void f_check(size_t a_n)
	{
		if (a_n != sizeof...(T_an)) [[unlikely]] f_throw((t_cs<L'm', L'u', L's', L't', L' ', L'b', L'e', L' ', L'c', L'a', L'l', L'l', L'e', L'd', L' ', L'w', L'i', L't', L'h', L' '>() + typename t_n2s<sizeof...(T_an)>::t() + t_cs<L' ', L'a', L'r', L'g', L'u', L'm', L'e', L'n', L't', L'(', L's', L')', L'.'>()).v);
	}
	template<size_t... A_i>
	static void f_check(t_pvalue* a_stack, std::index_sequence<A_i...>)
	{
		(xemmai::f_check<T_an>(a_stack[A_i], t_cs<L'a', L'r', L'g', L'u', L'm', L'e', L'n', L't'>() + typename t_n2s<A_i>::t()), ...);
	}
	static void f_check(t_pvalue* a_stack)
	{
		f_check(a_stack + 2, std::index_sequence_for<T_an...>());
	}
	template<t_pvalue(*A_function)(t_library*, const t_pvalue&, T_an&&...), size_t... A_i>
	static t_pvalue f_call(t_library* a_library, const t_pvalue& a_self, t_pvalue* a_stack, std::index_sequence<A_i...>)
	{
		return A_function(a_library, a_self, f_as<T_an>(a_stack[A_i])...);
	}
	template<t_pvalue(*A_function)(t_library*, const t_pvalue&, T_an&&...)>
	static void f_call(t_library* a_library, const t_pvalue& a_self, t_pvalue* a_stack)
	{
		a_stack[0] = f_call<A_function>(a_library, a_self, a_stack + 2, std::index_sequence_for<T_an...>());
	}
	template<size_t... A_i>
	static bool f_match(t_pvalue* a_stack, std::index_sequence<A_i...>)
	{
		return (f_is<T_an>(a_stack[A_i]) && ...);
	}
	static bool f_match(t_pvalue* a_stack, size_t a_n)
	{
		return a_n == sizeof...(T_an) && f_match(a_stack + 2, std::index_sequence_for<T_an...>());
	}
};

template<typename T_function, T_function A_function>
struct t_construct_with;

template<typename T_r, typename... T_an, T_r(*A_function)(t_type*, T_an...)>
struct t_construct_with<T_r(*)(t_type*, T_an...), A_function>
{
	template<size_t... A_i>
	static T_r f_do(t_type* a_class, t_pvalue* a_stack, std::index_sequence<A_i...>)
	{
		return A_function(a_class, f_as<T_an>(a_stack[A_i])...);
	}
	static t_pvalue f_do(t_type* a_class, t_pvalue* a_stack, size_t a_n)
	{
		t_signature<T_an...>::f_check(a_n);
		t_signature<T_an...>::f_check(a_stack);
		return f_do(a_class, a_stack + 2, std::index_sequence_for<T_an...>());
	}
	static t_pvalue f_do(t_type* a_class, t_pvalue* a_stack)
	{
		return f_do(a_class, a_stack + 2, std::index_sequence_for<T_an...>());
	}
	static bool f_match(t_pvalue* a_stack, size_t a_n)
	{
		return t_signature<T_an...>::f_match(a_stack, a_n);
	}

	template<typename> using t_bind = t_construct_with;
};

template<typename... T_an>
struct t_construct
{
	template<typename T_self>
	using t_bind = t_construct_with<t_object*(*)(t_type*, T_an&&...), [](t_type* a_class, T_an&&... a_an)
	{
		return a_class->template f_new<T_self>(std::forward<T_an>(a_an)...);
	}>;
};

template<typename T_library, typename T_r, typename... T_an>
t_pvalue f_call_as(T_r a_do(T_an...), T_library* a_library, auto&&... a_xs)
{
	return a_library->f_as(a_do(a_library, std::forward<decltype(a_xs)>(a_xs)...));
}

template<typename T_library, typename... T_an>
t_pvalue f_call_as(void a_do(T_an...), T_library* a_library, auto&&... a_xs)
{
	a_do(a_library, std::forward<decltype(a_xs)>(a_xs)...);
	return {};
}

template<typename T_self, typename... T_an>
struct t_call_member_base
{
	using t_self = T_self;
	using t_signature = xemmai::t_signature<T_an...>;
};

template<typename T_library, typename T_function>
struct t_call_member;

template<typename T_library, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_library, T_r(T_self::*)(T_an...) const> : t_call_member_base<T_self, T_an...>
{
	template<T_r(T_self::*A_function)(T_an...) const>
	static T_r f_call(T_library* a_library, const t_pvalue& a_self, T_an&&... a_n)
	{
		return (f_as<const T_self&>(a_self).*A_function)(std::forward<T_an>(a_n)...);
	}
};

template<typename T_library, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_library, T_r(T_self::*)(T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r(T_self::*A_function)(T_an...)>
	static T_r f_call(T_library* a_library, const t_pvalue& a_self, T_an&&... a_n)
	{
		return (f_as<T_self&>(a_self).*A_function)(std::forward<T_an>(a_n)...);
	}
};

template<typename T_library, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_library, T_r(T_self::*)(T_library*, T_an...) const> : t_call_member_base<T_self, T_an...>
{
	template<T_r(T_self::*A_function)(T_library*, T_an...) const>
	static T_r f_call(T_library* a_library, const t_pvalue& a_self, T_an&&... a_n)
	{
		return (f_as<const T_self&>(a_self).*A_function)(a_library, std::forward<T_an>(a_n)...);
	}
};

template<typename T_library, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_library, T_r(T_self::*)(T_library*, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r(T_self::*A_function)(T_library*, T_an...)>
	static T_r f_call(T_library* a_library, const t_pvalue& a_self, T_an&&... a_n)
	{
		return (f_as<T_self&>(a_self).*A_function)(a_library, std::forward<T_an>(a_n)...);
	}
};

template<typename T_library, typename T_r, typename T_self, typename... T_an>
struct t_call_member<T_library, T_r(*)(T_self, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r(*A_function)(T_self, T_an...)>
	static T_r f_call(T_library* a_library, const t_pvalue& a_self, T_an&&... a_n)
	{
		return A_function(f_as<T_self>(a_self), std::forward<T_an>(a_n)...);
	}
};

template<typename T_library, typename T_r, typename T_self, typename... T_an>
struct t_call_member<T_library, T_r(*)(T_library*, T_self, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r(*A_function)(T_library*, T_self, T_an...)>
	static T_r f_call(T_library* a_library, const t_pvalue& a_self, T_an&&... a_n)
	{
		return A_function(a_library, f_as<T_self>(a_self), std::forward<T_an>(a_n)...);
	}
};

template<typename T_function, T_function A_function>
struct t_member
{
	template<typename T_library>
	struct t_bind
	{
		using t_self = typename t_call_member<T_library, T_function>::t_self;
		using t_signature = typename t_call_member<T_library, T_function>::t_signature;

		static t_pvalue f_function(t_library* a_library, const t_pvalue& a_self, auto&&... a_xs)
		{
			return f_call_as<T_library>(t_call_member<T_library, T_function>::template f_call<A_function>, static_cast<T_library*>(a_library), a_self, std::forward<decltype(a_xs)>(a_xs)...);
		}
		static void f_call(t_library* a_library, t_pvalue* a_stack, size_t a_n)
		{
			t_signature::f_check(a_n);
			f_check<t_self>(a_stack[1], L"this");
			t_signature::f_check(a_stack);
			t_signature::template f_call<f_function>(a_library, a_stack[1], a_stack);
		}
		static void f_call(t_library* a_library, t_pvalue* a_stack)
		{
			t_signature::template f_call<f_function>(a_library, a_stack[1], a_stack);
		}
		static bool f_match(t_pvalue* a_stack, size_t a_n)
		{
			return f_is<t_self>(a_stack[1]) && t_signature::f_match(a_stack, a_n);
		}
	};
};

template<typename T_library, typename T_function>
struct t_call_static;

template<typename T_library, typename T_r, typename... T_an>
struct t_call_static<T_library, T_r(*)(T_an...)>
{
	using t_signature = xemmai::t_signature<T_an...>;

	template<T_r(*A_function)(T_an...)>
	static T_r f_call(T_library* a_library, T_an&&... a_n)
	{
		return A_function(std::forward<T_an>(a_n)...);
	}
};

template<typename T_library, typename T_r, typename... T_an>
struct t_call_static<T_library, T_r(*)(T_library*, T_an...)>
{
	using t_signature = xemmai::t_signature<T_an...>;

	template<T_r(*A_function)(T_library*, T_an...)>
	static T_r f_call(T_library* a_library, T_an&&... a_n)
	{
		return A_function(a_library, std::forward<T_an>(a_n)...);
	}
};

template<typename T_function, T_function A_function>
struct t_static
{
	template<typename T_library>
	struct t_bind
	{
		using t_signature = typename t_call_static<T_library, T_function>::t_signature;

		static t_pvalue f_function(t_library* a_library, const t_pvalue& a_self, auto&&... a_xs)
		{
			return f_call_as<T_library>(t_call_static<T_library, T_function>::template f_call<A_function>, static_cast<T_library*>(a_library), std::forward<decltype(a_xs)>(a_xs)...);
		}
		static void f_call(t_library* a_library, t_pvalue* a_stack, size_t a_n)
		{
			t_signature::f_check(a_n);
			t_signature::f_check(a_stack);
			t_signature::template f_call<f_function>(a_library, a_stack[1], a_stack);
		}
		static void f_call(t_library* a_library, t_pvalue* a_stack)
		{
			t_signature::template f_call<f_function>(a_library, a_stack[1], a_stack);
		}
		static bool f_match(t_pvalue* a_stack, size_t a_n)
		{
			return t_signature::f_match(a_stack, a_n);
		}
	};
};

template<typename...> struct t_overload;

template<>
struct t_overload<>
{
	template<typename T>
	struct t_bind
	{
		static void f_call(t_library* a_library, t_pvalue* a_stack, size_t a_n)
		{
			f_throw(L"no method matching signature is found."sv);
		}
		static t_pvalue f_do(t_type* a_class, t_pvalue* a_stack, size_t a_n)
		{
			f_throw(L"no method matching signature is found."sv);
		}
	};
};

template<typename T, typename... T_next>
struct t_overload<T, T_next...>
{
	template<typename T_self>
	struct t_bind
	{
		using t_bound = typename T::template t_bind<T_self>;

		static t_pvalue f_do(t_type* a_class, t_pvalue* a_stack, size_t a_n)
		{
			return t_bound::f_match(a_stack, a_n) ? t_bound::f_do(a_class, a_stack) : t_overload<T_next...>::template t_bind<T_self>::f_do(a_class, a_stack, a_n);
		}
	};
};

template<typename T_function, T_function A_function, typename... T_next>
struct t_overload<t_member<T_function, A_function>, T_next...>
{
	template<typename T_library>
	struct t_bind
	{
		using t_bound = typename t_member<T_function, A_function>::template t_bind<T_library>;

		static void f_call(t_library* a_library, t_pvalue* a_stack, size_t a_n)
		{
			if (t_bound::f_match(a_stack, a_n))
				t_bound::f_call(a_library, a_stack);
			else
				t_overload<T_next...>::template t_bind<T_library>::f_call(a_library, a_stack, a_n);
		}
	};
};

template<typename T_function, T_function A_function, typename... T_next>
struct t_overload<t_static<T_function, A_function>, T_next...>
{
	template<typename T_library>
	struct t_bind
	{
		using t_bound = typename t_static<T_function, A_function>::template t_bind<T_library>;

		static void f_call(t_library* a_library, t_pvalue* a_stack, size_t a_n)
		{
			if (t_bound::f_match(a_stack, a_n))
				t_bound::f_call(a_library, a_stack);
			else
				t_overload<T_next...>::template t_bind<T_library>::f_call(a_library, a_stack, a_n);
		}
	};
};

template<typename T_library>
class t_define
{
	T_library* v_library;
	t_fields v_fields;

public:
	t_define(T_library* a_library) : v_library(a_library)
	{
	}
	template<typename T, typename T_super>
	void f_derive()
	{
		v_library->template f_type_slot<T>().f_construct(v_library->template f_type<T_super>()->template f_derive<t_type_of<T>>(t_object::f_of(v_library), v_fields, &t_type_of<T>::f_do_construct != &t_type_of<T_super>::f_do_construct));
	}
	template<typename T_super>
	void f_derive(t_object* a_type)
	{
		auto [fields, key2index] = v_library->template f_type<T_super>()->f_merge(v_fields);
		auto& type = a_type->f_as<t_type>();
		type.v_instance_fields = v_fields.v_instance.size();
		type.v_fields = fields.size();
		std::copy(fields.begin(), fields.end(), type.f_fields());
		std::copy(key2index.begin(), key2index.end(), type.f_key2index());
	}
	operator const t_fields&() const
	{
		return v_fields;
	}
	operator std::vector<std::pair<t_root, t_rvalue>>()
	{
		return std::move(v_fields.v_class);
	}
	t_define& operator()(t_object* a_name)
	{
		v_fields.v_instance.push_back(a_name);
		return *this;
	}
	t_define& operator()(t_object* a_name, auto a_value)
	{
		v_fields.v_class.emplace_back(a_name, v_library->f_as(a_value));
		return *this;
	}
	t_define& operator()(t_object* a_name, t_library::t_function a_function)
	{
		v_fields.v_class.emplace_back(a_name, f_new<t_native>(f_global(), a_function, t_object::f_of(v_library)));
		return *this;
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(t_object* a_name, const t_member<T_function, A_function>&)
	{
		return (*this)(a_name, t_member<T_function, A_function>::template t_bind<T_library>::f_call);
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(t_object* a_name, const t_static<T_function, A_function>&)
	{
		return (*this)(a_name, t_static<T_function, A_function>::template t_bind<T_library>::f_call);
	}
	t_define& operator()(t_object* a_name, auto&& a_overload0, auto&& a_overload1, auto&&... a_overloadn)
	{
		return (*this)(a_name, t_overload<std::decay_t<decltype(a_overload0)>, std::decay_t<decltype(a_overload1)>, std::decay_t<decltype(a_overloadn)>...>::template t_bind<T_library>::f_call);
	}
	t_define& operator()(std::wstring_view a_name, auto&&... a_xs)
	{
		return (*this)(t_symbol::f_instantiate(a_name), std::forward<decltype(a_xs)>(a_xs)...);
	}
};

template<typename T, typename T_library>
struct t_enum_of : t_derivable<t_bears<T, t_type_of<intptr_t>>>
{
	using t_library = T_library;
	using t_base = t_enum_of;

	static t_pvalue f_transfer(const T_library* a_library, T a_value)
	{
		return a_library->template f_type<typename t_fundamental<T>::t_type>()->template f_new<intptr_t>(static_cast<intptr_t>(a_value));
	}
	static t_object* f_define(t_library* a_library, auto a_fields)
	{
		t_define{a_library}.template f_derive<T, intptr_t>();
		t_define fields(a_library);
		a_fields(std::ref(fields));
		return static_cast<t_type_of<T>*>(a_library->template f_type<T>())->f_do_derive(fields);
	}

	using t_derivable<t_bears<T, t_type_of<intptr_t>>>::t_derivable;
};

}

#endif
