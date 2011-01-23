#ifndef XEMMAI__CONVERT_H
#define XEMMAI__CONVERT_H

#include "portable/convert.h"
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

template<typename T_function>
struct t_call_construct
{
};

template<typename T_extension, typename T_function, typename T_with>
struct t_call_member
{
};

template<typename T_extension, typename T_function>
struct t_call_static
{
};

struct t_unspecified
{
	static void f_call(t_object* a_class, t_slot* a_stack, size_t a_n)
	{
		t_throwable::f_throw(L"no method matching signature is found.");
	}

	template<typename T_extension>
	struct t_bind
	{
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			t_throwable::f_throw(L"no method matching signature is found.");
		}
	};

	t_unspecified(const t_value& a_self)
	{
	}
};

class t_with_lock_for_read
{
	portable::t_scoped_lock_for_read v_lock;

public:
	t_with_lock_for_read(const t_value& a_self) : v_lock(static_cast<t_object*>(a_self)->v_lock)
	{
	}
};

class t_with_lock_for_write
{
	portable::t_scoped_lock_for_write v_lock;

public:
	t_with_lock_for_write(const t_value& a_self) : v_lock(static_cast<t_object*>(a_self)->v_lock)
	{
	}
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
#define XEMMAI__MACRO__A_N_TRANSFER(n) t_transfer a##n = a_stack[n + 1].f_transfer();
#define XEMMAI__MACRO__CHECK(n) f_check<T_a##n>(a##n, L"argument" XEMMAI__MACRO__L(n));
#define XEMMAI__MACRO__UNSPECIFIED(n) t_unspecified
#define XEMMAI__MACRO__ITERATE "convert_call.h"
#define XEMMAI__MACRO__N XEMMAI__MACRO__ARGUMENTS_LIMIT
#include "macro.h"

template<typename T_function, T_function A_function>
struct t_construct_with
{
	typedef t_call_construct<T_function> t_type;

	static void f_call(t_object* a_class, t_slot* a_stack, size_t a_n)
	{
		t_type::f_call(A_function, a_class, a_stack, a_n);
	}
	static void f_call(t_object* a_class, t_slot* a_stack)
	{
		t_type::f_call(A_function, a_class, a_stack);
	}
};

template<typename T_function, T_function A_function, typename T_with = t_unspecified>
struct t_member
{
	template<typename T_extension>
	struct t_bind
	{
		typedef typename t_call_member<T_extension, T_function, T_with>::t_type t_type;
		typedef typename t_call_member<T_extension, T_function, T_with>::t_call t_call;

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			t_call::f_call(A_function, a_module, a_self, a_stack, a_n);
		}
		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack)
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

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			t_call::f_call(A_function, a_module, a_stack, a_n);
		}
		static void f_call(t_object* a_module, t_slot* a_stack)
		{
			t_call::f_call(A_function, a_module, a_stack);
		}
	};
};

template<typename T, typename T_next = t_unspecified>
struct t_overload
{
	static void f_call(t_object* a_class, t_slot* a_stack, size_t a_n)
	{
		if (T::t_type::f_match(a_stack, a_n))
			T::f_call(a_class, a_stack);
		else
			T_next::f_call(a_class, a_stack, a_n);
	}

	template<typename T_extension>
	struct t_bind
	{
		typedef typename T::template t_bind<T_extension> t_bound;

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			if (t_bound::t_type::f_match(a_self, a_stack, a_n))
				t_bound::f_call(a_module, a_self, a_stack);
			else
				T_next::template t_bind<T_extension>::f_call(a_module, a_self, a_stack, a_n);
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

		static void f_call(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
		{
			if (t_bound::t_type::f_match(a_stack, a_n))
				t_bound::f_call(a_module, a_stack);
			else
				T_next::template t_bind<T_extension>::f_call(a_module, a_self, a_stack, a_n);
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
		v_extension->f_module()->f_put(t_symbol::f_instantiate(a_name), t_value(v_type));
	}
	operator t_transfer() const
	{
		return v_type;
	}
	template<typename T_value>
	t_define& operator()(t_object* a_name, T_value a_value)
	{
		v_type.f_put(a_name, v_extension->f_as(a_value));
		return *this;
	}
	template<typename T_value>
	t_define& operator()(const std::wstring& a_name, T_value a_value)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_value);
	}
	t_define& operator()(t_object* a_name, t_native::t_function a_function)
	{
		v_type.f_put(a_name, v_extension->f_function(a_function));
		return *this;
	}
	t_define& operator()(const std::wstring& a_name, t_native::t_function a_function)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_function);
	}
	template<typename T_function, T_function A_function, typename T_with>
	t_define& operator()(t_object* a_name, const t_member<T_function, A_function, T_with>&)
	{
		v_type.f_put(a_name, v_extension->f_function(t_member<T_function, A_function, T_with>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
		return *this;
	}
	template<typename T_function, T_function A_function, typename T_with>
	t_define& operator()(const std::wstring& a_name, const t_member<T_function, A_function, T_with>& a_member0)
	{
		return (*this)(t_symbol::f_instantiate(a_name), a_member0);
	}
	template<typename T_function, T_function A_function>
	t_define& operator()(t_object* a_name, const t_static<T_function, A_function>&)
	{
		v_type.f_put(a_name, v_extension->f_function(t_static<T_function, A_function>::template t_bind<typename t_type_of<T>::t_extension>::f_call));
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
struct t_enum_of : t_type_of<int>
{
	typedef T_extension t_extension;
	typedef t_enum_of t_base;

	static t_transfer f_transfer(const T_extension* a_extension, T a_value)
	{
		return f_construct(a_extension->template f_type<typename t_fundamental<T>::t_type>(), static_cast<int>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, int a_value)
	{
		t_transfer object = t_object::f_allocate_uninitialized(a_class);
		object.f_integer__(a_value);
		return object;
	}

	t_enum_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<int>(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<typename T, typename T_extension>
t_type* t_enum_of<T, T_extension>::f_derive(t_object* a_this)
{
	return new t_type_of<T>(v_module, a_this);
}

template<typename T, typename T_extension>
void t_enum_of<T, T_extension>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_construct_with<t_transfer (*)(t_object*, int), t_enum_of<T, T_extension>::f_construct>::f_call(a_class, a_stack, a_n);
}

}

#endif
