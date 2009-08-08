#ifndef XEMMAI__CONVERT_H
#define XEMMAI__CONVERT_H

#include <typeinfo>

#include "portable/convert.h"
#include "class.h"
#include "symbol.h"
#include "throwable.h"
#include "global.h"
#include "macro.h"

#define XEMMAI__MACRO__OVERLOADS_LIMIT 8

namespace xemmai
{

template<typename T>
struct t_is_void
{
	static const bool v_value = false;
};

template<>
struct t_is_void<void>
{
	static const bool v_value = true;
};

XEMMAI__PORTABLE__EXPORT void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name);

template<typename T>
void f_throw_type_error(const wchar_t* a_name)
{
	f_throw_type_error(typeid(typename t_fundamental<T>::t_type), a_name);
}

template<typename T>
struct t_allow_null
{
	static const bool v_value = false;
};

template<typename T>
struct t_allow_null<T*>
{
	static const bool v_value = true;
};

template<>
struct t_allow_null<t_object*>
{
	static const bool v_value = false;
};

template<typename T>
inline void f_check(t_object* a_object, const wchar_t* a_name)
{
	if (t_allow_null<T>::v_value && a_object == f_global()->f_null()) return;
	if (!f_is<T>(a_object)) f_throw_type_error<T>(a_name);
}

template<typename T_function>
struct t_call_construct
{
};

template<typename T_extension, typename T_function>
struct t_call_member
{
};

template<typename T_extension, typename T_function>
struct t_call_static
{
};

struct t_unspecified
{
	static void f_call(t_object* a_self, size_t a_n, t_stack& a_stack)
	{
		t_throwable::f_throw(L"no method matching signature is found.");
	}

	template<typename T_extension>
	struct t_bind
	{
		static void f_call(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
		{
			t_throwable::f_throw(L"no method matching signature is found.");
		}
	};
};

#define XEMMAI__MACRO__TYPENAME_UNSPECIFIED(n) typename T_a##n = t_unspecified
template<typename T_self, XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TYPENAME_UNSPECIFIED, XEMMAI__MACRO__ARGUMENTS_LIMIT), typename T_an = t_unspecified>
struct t_construct
{
};

#define XEMMAI__MACRO__TYPENAME_T_AN(n) typename T_a##n
#define XEMMAI__MACRO__T_AN(n) T_a##n
#define XEMMAI__MACRO__T_AN_A_N(n) T_a##n a_##n
#define XEMMAI__MACRO__TRANSFER_A_N(n) const t_transfer& a_##n
#define XEMMAI__MACRO__A_N(n) a_##n
#define XEMMAI__MACRO__AS_A_N(n) f_as<T_a##n>(a_##n)
#define XEMMAI__MACRO__AN(n) a##n
#define XEMMAI__MACRO__AS_AN(n) f_as<T_a##n>(a##n)
#define XEMMAI__MACRO__POP(n) t_transfer a##n = a_stack.f_pop();
#define XEMMAI__MACRO__CHECK(n) f_check<T_a##n>(a##n, L"argument" XEMMAI__MACRO__L(n));
#define XEMMAI__MACRO__UNSPECIFIED(n) t_unspecified
#define XEMMAI__MACRO__ITERATE "convert_call.h"
#define XEMMAI__MACRO__N XEMMAI__MACRO__ARGUMENTS_LIMIT
#include "macro.h"

template<typename T_function, T_function A_function>
struct t_construct_with
{
	typedef t_call_construct<T_function> t_type;

	static void f_call(t_object* a_class, size_t a_n, t_stack& a_stack)
	{
		t_type::f_call(A_function, a_class, a_n, a_stack);
	}
	static void f_call(t_object* a_class, t_stack& a_stack)
	{
		t_type::f_call(A_function, a_class, a_stack);
	}
};

template<typename T_function, T_function A_function>
struct t_member
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_call_member<T_extension, T_function>::t_type t_type;
		typedef typename t_call_member<T_extension, T_function>::t_call t_call;

		static void f_call(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
		{
			t_call::f_call(A_function, a_module, a_self, a_n, a_stack);
		}
		static void f_call(t_object* a_module, t_object* a_self, t_stack& a_stack)
		{
			t_call::f_call(A_function, a_module, a_self, a_stack);
		}
	};
};

template<typename T_function, T_function A_function>
struct t_static
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_call_static<T_extension, T_function>::t_type t_type;
		typedef typename t_call_static<T_extension, T_function>::t_call t_call;

		static void f_call(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
		{
			t_call::f_call(A_function, a_module, a_n, a_stack);
		}
		static void f_call(t_object* a_module, t_stack& a_stack)
		{
			t_call::f_call(A_function, a_module, a_stack);
		}
	};
};

template<typename T, typename T_next = t_unspecified>
struct t_overload
{
	static void f_call(t_object* a_class, size_t a_n, t_stack& a_stack)
	{
		if (T::t_type::f_match(a_n, a_stack))
			T::f_call(a_class, a_stack);
		else
			T_next::f_call(a_class, a_n, a_stack);
	}

	template<typename T_extension>
	struct t_bind
	{
		typedef typename T::template t_bind<T_extension> t_bound;

		static void f_call(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
		{
			if (t_bound::t_type::f_match(a_self, a_n, a_stack))
				t_bound::f_call(a_module, a_self, a_stack);
			else
				T_next::template t_bind<T_extension>::f_call(a_module, a_self, a_n, a_stack);
		}
	};
};

template<typename T_function, T_function A_function, typename T_next>
struct t_overload<t_static<T_function, A_function>, T_next>
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_static<T_function, A_function>::template t_bind<T_extension> t_bound;

		static void f_call(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
		{
			if (t_bound::t_type::f_match(a_n, a_stack))
				t_bound::f_call(a_module, a_stack);
			else
				T_next::template t_bind<T_extension>::f_call(a_module, a_self, a_n, a_stack);
		}
	};
};

template<typename T, typename T_super>
class t_define
{
	typename t_type_of<T>::t_extension* v_extension;
	t_transfer v_type;

public:
	t_define(typename t_type_of<T>::t_extension* a_extension, const std::wstring& a_name) :
	v_extension(a_extension), v_type(t_class::f_instantiate(new t_type_of<T>(v_extension->f_module(), a_extension->template f_type<T_super>())))
	{
		v_extension->template f_type__<T>(static_cast<t_object*>(v_type));
		v_extension->f_module()->f_put(t_symbol::f_instantiate(a_name), static_cast<t_object*>(v_type));
	}
	t_define(typename t_type_of<T>::t_extension* a_extension, const std::wstring& a_name, const t_transfer& a_type) : v_extension(a_extension), v_type(a_type)
	{
		v_extension->f_module()->f_put(t_symbol::f_instantiate(a_name), static_cast<t_object*>(v_type));
	}
	operator t_transfer() const
	{
		return v_type;
	}
	template<typename T_value>
	t_define& operator()(const t_transfer& a_name, T_value a_value)
	{
		v_type->f_put(a_name, v_extension->f_as(a_value));
		return *this;
	}
	template<typename T_value>
	t_define& operator()(const std::wstring& a_name, T_value a_value)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_value);
	}
	t_define& operator()(const t_transfer& a_name, t_native::t_function a_function)
	{
		v_type->f_put(a_name, v_extension->f_function(a_function));
		return *this;
	}
	t_define& operator()(const std::wstring& a_name, t_native::t_function a_function)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_function);
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(const t_transfer& a_name, const t_member<T_function, A_function>&)
	{
		v_type->f_put(a_name, v_extension->f_function(t_member<T_function, A_function>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(const std::wstring& a_name, const t_member<T_function, A_function>& a_member0)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_member0);
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(const t_transfer& a_name, const t_static<T_function, A_function>&)
	{
		v_type->f_put(a_name, v_extension->f_function(t_static<T_function, A_function>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(const std::wstring& a_name, const t_static<T_function, A_function>& a_member0)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_member0);
	}
#define XEMMAI__MACRO__TYPENAME_T_OVERLOADN(n) typename T_overload##n
#define XEMMAI__MACRO__CONST_T_OVERLOADN(n) const T_overload##n&
#define XEMMAI__MACRO__CONST_T_OVERLOADN_A_OVERLOADN(n) const T_overload##n& a_overload##n
#define XEMMAI__MACRO__A_OVERLOADN(n) a_overload##n
#define XEMMAI__MACRO__T_OVERLOADN_OPEN(n) , t_overload<T_overload##n
#define XEMMAI__MACRO__T_OVERLOADN_CLOSE(n)  >
#define XEMMAI__MACRO__ITERATE "convert_override.h"
#define XEMMAI__MACRO__N XEMMAI__MACRO__OVERLOADS_LIMIT
#include "macro.h"
};

template<typename T_function, T_function A_function, typename T_extension>
inline void f_define(T_extension* a_extension, const t_transfer& a_name)
{
	a_extension->f_module()->f_put(a_name, a_extension->f_function(t_static<T_function, A_function>::template t_bind<T_extension>::f_call));
}

template<typename T_function, T_function A_function, typename T_extension>
inline void f_define(T_extension* a_extension, const std::wstring& a_name)
{
	f_define<T_function, A_function, T_extension>(a_extension, t_symbol::f_instantiate(a_name));
}

}

#endif
