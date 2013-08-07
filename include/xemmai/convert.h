#ifndef XEMMAI__CONVERT_H
#define XEMMAI__CONVERT_H

#include "global.h"

namespace xemmai
{

template<wchar_t... A_cs>
struct t_cs
{
	static constexpr wchar_t v[sizeof...(A_cs) + 1] = {A_cs..., L'\0'};

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

template<wchar_t... A_cs>
constexpr wchar_t t_cs<A_cs...>::v[sizeof...(A_cs) + 1];

template<size_t A_n>
struct t__n2s
{
	typedef decltype(typename t__n2s<A_n / 10>::t() + t_cs<L'0' + (A_n % 10)>()) t;
};

template<>
struct t__n2s<0>
{
	typedef t_cs<> t;
};

template<size_t A_n>
struct t_n2s
{
	typedef typename std::conditional<A_n <= 0, t_cs<L'0'>, typename t__n2s<A_n>::t>::type t;
};

template<typename... T_an>
struct t_signature
{
	template<size_t A_i>
	static void f_check__(t_slot* a_stack)
	{
	}
	template<size_t A_i, typename T_a0, typename... T_am>
	static void f_check__(t_slot* a_stack)
	{
		xemmai::f_check<T_a0>(*++a_stack, t_cs<L'a', L'r', L'g', L'u', L'm', L'e', L'n', L't'>() + typename t_n2s<A_i>::t());
		f_check__<A_i + 1, T_am...>(a_stack);
	}
	static void f_check(t_slot* a_stack, size_t a_n)
	{
		if (a_n != sizeof...(T_an)) t_throwable::f_throw((t_cs<L'm', L'u', L's', L't', L' ', L'b', L'e', L' ', L'c', L'a', L'l', L'l', L'e', L'd', L' ', L'w', L'i', L't', L'h', L' '>() + typename t_n2s<sizeof...(T_an)>::t() + t_cs<L' ', L'a', L'r', L'g', L'u', L'm', L'e', L'n', L't', L'(', L's', L')', L'.'>()).v);
		f_check__<0, T_an...>(a_stack);
	}
	template<t_scoped (*A_function)(t_object*, const t_value&, T_an&&...)>
	static t_scoped f__call(t_object* a_module, const t_value& a_self, t_slot* a_stack, T_an&&... a_n)
	{
		return A_function(a_module, a_self, std::forward<T_an>(a_n)...);
	}
	template<t_scoped (*A_function)(t_object*, const t_value&, T_an&&...), typename T_a0, typename... T_am, typename... T_ak>
	static t_scoped f__call(t_object* a_module, const t_value& a_self, t_slot* a_stack, T_ak&&... a_k)
	{
		t_scoped a0 = std::move(*++a_stack);
		return f__call<A_function, T_am...>(a_module, a_self, a_stack, std::forward<T_ak>(a_k)..., f_as<T_a0>(a0));
	}
	template<t_scoped (*A_function)(t_object*, const t_value&, T_an&&...)>
	static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack)
	{
		a_stack[0].f_construct(f__call<A_function, T_an...>(a_module, a_self, a_stack));
	}
	template<bool>
	static bool f__match(t_slot* a_stack)
	{
		return true;
	}
	template<bool, typename T_a0, typename... T_am>
	static bool f__match(t_slot* a_stack)
	{
		return f_is<T_a0>(*++a_stack) && f__match<false, T_am...>(a_stack);
	}
	static bool f_match(t_slot* a_stack, size_t a_n)
	{
		return a_n == sizeof...(T_an) && f__match<false, T_an...>(a_stack);
	}
};

struct t_with_none
{
	t_with_none(const t_value& a_self)
	{
	}
};

class t_with_lock_for_read
{
	t_scoped_lock_for_read v_lock;

public:
	t_with_lock_for_read(const t_value& a_self) : v_lock(static_cast<t_object*>(a_self)->v_lock)
	{
	}
};

class t_with_lock_for_write
{
	t_scoped_lock_for_write v_lock;

public:
	t_with_lock_for_write(const t_value& a_self) : v_lock(static_cast<t_object*>(a_self)->v_lock)
	{
	}
};

template<typename T_function, T_function A_function>
struct t_call_construct;

template<typename... T_an, t_scoped (*A_function)(t_object*, T_an...)>
struct t_call_construct<t_scoped (*)(t_object*, T_an...), A_function>
{
	static t_scoped f_function(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		return A_function(a_self, std::forward<T_an>(a_n)...);
	}
	static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
	{
		if (a_self.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
		t_signature<T_an...>::f_check(a_stack, a_n);
		t_signature<T_an...>::template f_call<f_function>(a_module, a_self, a_stack);
	}
	static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack)
	{
		t_signature<T_an...>::template f_call<f_function>(a_module, a_self, a_stack);
	}
	template<typename...>
	static t_scoped f__do(t_object* a_class, t_slot* a_stack, T_an&&... a_n)
	{
		return A_function(a_class, std::forward<T_an>(a_n)...);
	}
	template<typename T_a0, typename... T_am, typename... T_ak>
	static t_scoped f__do(t_object* a_class, t_slot* a_stack, T_ak&&... a_k)
	{
		++a_stack;
		return f__do<T_am...>(a_class, a_stack, std::forward<T_ak>(a_k)..., f_as<T_a0>(*a_stack));
	}
	static t_scoped f_do(t_object* a_class, t_slot* a_stack, size_t a_n)
	{
		t_signature<T_an...>::f_check(a_stack, a_n);
		return f__do<T_an...>(a_class, a_stack);
	}
	static t_scoped f_do(t_object* a_class, t_slot* a_stack)
	{
		return f__do<T_an...>(a_class, a_stack);
	}
	static bool f_match(t_slot* a_stack, size_t a_n)
	{
		return t_signature<T_an...>::f_match(a_stack, a_n);
	}
	static bool f_match(const t_value& a_self, t_slot* a_stack, size_t a_n)
	{
		return a_self.f_type() == f_global()->f_type<t_class>() && f_match(a_stack, a_n);
	}
};

template<typename... T_an>
struct t_construct
{
	template<typename T_self>
	static t_scoped f_default(t_object* a_class, T_an&&... a_an)
	{
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new T_self(std::forward<T_an>(a_an)...));
		return object;
	}

	template<typename T_self>
	using t_bind = t_call_construct<t_scoped (*)(t_object*, T_an&&...), f_default<T_self>>;
};

template<typename T_function, T_function A_function>
struct t_construct_with
{
	template<typename T_self>
	using t_bind = t_call_construct<T_function, A_function>;
};

template<typename T_self, typename... T_an>
struct t_call_member_base
{
	typedef T_self t_self;
	typedef xemmai::t_signature<T_an...> t_signature;
};

template<typename T_extension, typename T_function>
struct t_call_member;

template<typename T_extension, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_extension, T_r (T_self::*)(T_an...) const> : t_call_member_base<T_self, T_an...>
{
	template<T_r (T_self::*A_function)(T_an...) const>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		return f_extension<T_extension>(a_module)->f_as((f_as<const T_self&>(a_self).*A_function)(std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename T_self, typename... T_an>
struct t_call_member<T_extension, void (T_self::*)(T_an...) const> : t_call_member_base<T_self, T_an...>
{
	template<void (T_self::*A_function)(T_an...) const>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		(f_as<const T_self&>(a_self).*A_function)(std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_extension, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_extension, T_r (T_self::*)(T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r (T_self::*A_function)(T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		return f_extension<T_extension>(a_module)->f_as((f_as<T_self&>(a_self).*A_function)(std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename T_self, typename... T_an>
struct t_call_member<T_extension, void (T_self::*)(T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<void (T_self::*A_function)(T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		(f_as<T_self&>(a_self).*A_function)(std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_extension, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_extension, T_r (T_self::*)(T_extension*, T_an...) const> : t_call_member_base<T_self, T_an...>
{
	template<T_r (T_self::*A_function)(T_extension*, T_an...) const>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		T_extension* extension = f_extension<T_extension>(a_module);
		return extension->f_as((f_as<const T_self&>(a_self).*A_function)(extension, std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename T_self, typename... T_an>
struct t_call_member<T_extension, void (T_self::*)(T_extension*, T_an...) const> : t_call_member_base<T_self, T_an...>
{
	template<void (T_self::*A_function)(T_extension*, T_an...) const>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		(f_as<const T_self&>(a_self).*A_function)(f_extension<T_extension>(a_module), std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_extension, typename T_self, typename T_r, typename... T_an>
struct t_call_member<T_extension, T_r (T_self::*)(T_extension*, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r (T_self::*A_function)(T_extension*, T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		T_extension* extension = f_extension<T_extension>(a_module);
		return extension->f_as((f_as<T_self&>(a_self).*A_function)(extension, std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename T_self, typename... T_an>
struct t_call_member<T_extension, void (T_self::*)(T_extension*, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<void (T_self::*A_function)(T_extension*, T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		(f_as<T_self&>(a_self).*A_function)(f_extension<T_extension>(a_module), std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_extension, typename T_r, typename T_self, typename... T_an>
struct t_call_member<T_extension, T_r (*)(T_self, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r (*A_function)(T_self, T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		return f_extension<T_extension>(a_module)->f_as(A_function(f_as<T_self>(a_self), std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename T_self, typename... T_an>
struct t_call_member<T_extension, void (*)(T_self, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<void (*A_function)(T_self, T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		A_function(f_as<T_self>(a_self), std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_extension, typename T_r, typename T_self, typename... T_an>
struct t_call_member<T_extension, T_r (*)(T_extension*, T_self, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<T_r (*A_function)(T_extension*, T_self, T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		T_extension* extension = f_extension<T_extension>(a_module);
		return extension->f_as(A_function(extension, f_as<T_self>(a_self), std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename T_self, typename... T_an>
struct t_call_member<T_extension, void (*)(T_extension*, T_self, T_an...)> : t_call_member_base<T_self, T_an...>
{
	template<void (*A_function)(T_extension*, T_self, T_an...)>
	static t_scoped f_call(t_object* a_module, const t_value& a_self, T_an&&... a_n)
	{
		A_function(f_extension<T_extension>(a_module), f_as<T_self>(a_self), std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_function, T_function A_function, typename T_with = t_with_none>
struct t_member
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_call_member<T_extension, T_function>::t_self t_self;
		typedef typename t_call_member<T_extension, T_function>::t_signature t_signature;

		template<typename... T_an>
		static t_scoped f_function(t_object* a_module, const t_value& a_self, T_an&&... a_n)
		{
			T_with with(a_self);
			return t_call_member<T_extension, T_function>::template f_call<A_function>(a_module, a_self, std::forward<T_an>(a_n)...);
		}
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			f_check<t_self>(a_self, L"this");
			t_signature::f_check(a_stack, a_n);
			t_signature::template f_call<f_function>(a_module, a_self, a_stack);
		}
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack)
		{
			t_signature::template f_call<f_function>(a_module, a_self, a_stack);
		}
		static bool f_match(const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			return f_is<t_self>(a_self) && t_signature::f_match(a_stack, a_n);
		}
	};
};

template<typename T_extension, typename T_function>
struct t_call_static;

template<typename T_extension, typename T_r, typename... T_an>
struct t_call_static<T_extension, T_r (*)(T_an...)>
{
	typedef xemmai::t_signature<T_an...> t_signature;

	template<T_r (*A_function)(T_an...)>
	static t_scoped f_call(T_extension* a_extension, T_an&&... a_n)
	{
		return a_extension->f_as(A_function(std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename... T_an>
struct t_call_static<T_extension, void (*)(T_an...)>
{
	typedef xemmai::t_signature<T_an...> t_signature;

	template<void (*A_function)(T_an...)>
	static t_scoped f_call(T_extension* a_extension, T_an&&... a_n)
	{
		A_function(std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_extension, typename T_r, typename... T_an>
struct t_call_static<T_extension, T_r (*)(T_extension*, T_an...)>
{
	typedef xemmai::t_signature<T_an...> t_signature;

	template<T_r (*A_function)(T_extension*, T_an...)>
	static t_scoped f_call(T_extension* a_extension, T_an&&... a_n)
	{
		return a_extension->f_as(A_function(a_extension, std::forward<T_an>(a_n)...));
	}
};

template<typename T_extension, typename... T_an>
struct t_call_static<T_extension, void (*)(T_extension*, T_an...)>
{
	typedef xemmai::t_signature<T_an...> t_signature;

	template<void (*A_function)(T_extension*, T_an...)>
	static t_scoped f_call(T_extension* a_extension, T_an&&... a_n)
	{
		A_function(a_extension, std::forward<T_an>(a_n)...);
		return t_value();
	}
};

template<typename T_function, T_function A_function>
struct t_static
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_call_static<T_extension, T_function>::t_signature t_signature;

		template<typename... T_an>
		static t_scoped f_function(t_object* a_module, const t_value& a_self, T_an&&... a_n)
		{
			return t_call_static<T_extension, T_function>::template f_call<A_function>(f_extension<T_extension>(a_module), std::forward<T_an>(a_n)...);
		}
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			t_signature::f_check(a_stack, a_n);
			t_signature::template f_call<f_function>(a_module, a_self, a_stack);
		}
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack)
		{
			t_signature::template f_call<f_function>(a_module, a_self, a_stack);
		}
		static bool f_match(const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			return t_signature::f_match(a_stack, a_n);
		}
	};
};

template<typename... T>
struct t_overload;

template<>
struct t_overload<>
{
	template<typename T>
	struct t_bind
	{
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			t_throwable::f_throw(L"no method matching signature is found.");
		}
		static t_scoped f_do(t_object* a_class, t_slot* a_stack, size_t a_n)
		{
			t_throwable::f_throw(L"no method matching signature is found.");
		}
	};
};

template<typename T, typename... T_next>
struct t_overload<T, T_next...>
{
	template<typename T_self>
	struct t_bind
	{
		typedef typename T::template t_bind<T_self> t_bound;

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			if (t_bound::f_match(a_self, a_stack, a_n))
				t_bound::f_call(a_module, a_self, a_stack);
			else
				t_overload<T_next...>::template t_bind<T_self>::f_call(a_module, a_self, a_stack, a_n);
		}
		static t_scoped f_do(t_object* a_class, t_slot* a_stack, size_t a_n)
		{
			return t_bound::f_match(a_stack, a_n) ? t_bound::f_do(a_class, a_stack) : t_overload<T_next...>::template t_bind<T_self>::f_do(a_class, a_stack, a_n);
		}
	};
};

template<typename T_function, T_function A_function, typename T_with, typename... T_next>
struct t_overload<t_member<T_function, A_function, T_with>, T_next...>
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_member<T_function, A_function, T_with>::template t_bind<T_extension> t_bound;

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			if (t_bound::f_match(a_self, a_stack, a_n))
				t_bound::f_call(a_module, a_self, a_stack);
			else
				t_overload<T_next...>::template t_bind<T_extension>::f_call(a_module, a_self, a_stack, a_n);
		}
	};
};

template<typename T_function, T_function A_function, typename... T_next>
struct t_overload<t_static<T_function, A_function>, T_next...>
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_static<T_function, A_function>::template t_bind<T_extension> t_bound;

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			if (t_bound::f_match(a_stack, a_n))
				t_bound::f_call(a_module, a_stack);
			else
				t_overload<T_next...>::template t_bind<T_extension>::f_call(a_module, a_self, a_stack, a_n);
		}
	};
};

template<typename T, typename T_super>
class t_define
{
	typename t_type_of<T>::t_extension* v_extension;
	t_scoped v_type;

public:
	t_define(typename t_type_of<T>::t_extension* a_extension, const std::wstring& a_name) :
	v_extension(a_extension), v_type(t_class::f_instantiate(new t_type_of<T>(v_extension->f_module(), a_extension->template f_type<T_super>())))
	{
		v_extension->template f_type__<T>(static_cast<t_object*>(v_type));
		v_extension->f_module()->f_put(t_symbol::f_instantiate(a_name), static_cast<t_object*>(v_type));
	}
	t_define(typename t_type_of<T>::t_extension* a_extension, const std::wstring& a_name, t_object* a_type) : v_extension(a_extension), v_type(a_type)
	{
		v_extension->f_module()->f_put(t_symbol::f_instantiate(a_name), static_cast<t_object*>(v_type));
	}
	operator t_scoped() const
	{
		return std::move(v_type);
	}
	template<typename T_value>
	t_define& operator()(t_object* a_name, T_value a_value)
	{
		v_type.f_put(a_name, v_extension->f_as(a_value));
		return *this;
	}
	t_define& operator()(t_object* a_name, t_native::t_function a_function)
	{
		v_type.f_put(a_name, v_extension->f_function(a_function));
		return *this;
	}
	template<typename... T_an>
	t_define& operator()(const t_construct<T_an...>&)
	{
		v_type.f_put(f_global()->f_symbol_construct(), v_extension->f_function(t_construct<T_an...>::template t_bind<T>::f_call));
		return *this;
	}
	template<typename... T_an, typename T_overload0, typename... T_overloadn>
	t_define& operator()(const t_construct<T_an...>&, const T_overload0&, const T_overloadn&...)
	{
		v_type.f_put(f_global()->f_symbol_construct(), v_extension->f_function(t_overload<t_construct<T_an...>, T_overload0, T_overloadn...>::template t_bind<T>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(const t_construct_with<T_function, A_function>&)
	{
		v_type.f_put(f_global()->f_symbol_construct(), v_extension->f_function(t_construct_with<T_function, A_function>::template t_bind<T>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, typename T_overload0, typename... T_overloadn>
	t_define& operator()(const t_construct_with<T_function, A_function>&, const T_overload0&, const T_overloadn&...)
	{
		v_type.f_put(f_global()->f_symbol_construct(), v_extension->f_function(t_overload<t_construct_with<T_function, A_function>, T_overload0, T_overloadn...>::template t_bind<T>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, typename T_with>
	t_define& operator()(t_object* a_name, const t_member<T_function, A_function, T_with>&)
	{
		v_type.f_put(a_name, v_extension->f_function(t_member<T_function, A_function, T_with>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, typename T_with, typename T_overload0, typename... T_overloadn>
	t_define& operator()(t_object* a_name, const t_member<T_function, A_function, T_with>&, const T_overload0&, const T_overloadn&...)
	{
		v_type.f_put(a_name, v_extension->f_function(t_overload<t_member<T_function, A_function, T_with>, T_overload0, T_overloadn...>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(t_object* a_name, const t_static<T_function, A_function>&)
	{
		v_type.f_put(a_name, v_extension->f_function(t_static<T_function, A_function>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, typename T_overload0, typename... T_overloadn>
	t_define& operator()(t_object* a_name, const t_static<T_function, A_function>&, const T_overload0&, const T_overloadn&...)
	{
		v_type.f_put(a_name, v_extension->f_function(t_overload<t_static<T_function, A_function>, T_overload0, T_overloadn...>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename... T_an>
	t_define& operator()(const std::wstring& a_name, T_an&&... a_n)
	{
		return (*this)(t_symbol::f_instantiate(a_name), std::forward<T_an>(a_n)...);
	}
};

template<typename T_function, T_function A_function, typename T_extension>
inline void f_define(T_extension* a_extension, t_object* a_name)
{
	a_extension->f_module()->f_put(a_name, a_extension->f_function(t_static<T_function, A_function>::template t_bind<T_extension>::f_call));
}

template<typename T_function, T_function A_function, typename T_extension>
inline void f_define(T_extension* a_extension, const std::wstring& a_name)
{
	f_define<T_function, A_function, T_extension>(a_extension, t_symbol::f_instantiate(a_name));
}

template<typename T, typename T_extension>
struct t_enum_of : t_type_of<ptrdiff_t>
{
	typedef T_extension t_extension;
	typedef t_enum_of t_base;

	static t_scoped f_transfer(const T_extension* a_extension, T a_value)
	{
		return f_construct_derived(a_extension->template f_type<typename t_fundamental<T>::t_type>(), a_value);
	}

	using t_type_of<ptrdiff_t>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
};

template<typename T, typename T_extension>
t_type* t_enum_of<T, T_extension>::f_derive(t_object* a_this)
{
	return new t_type_of<T>(t_scoped(v_module), a_this);
}

}

#endif
