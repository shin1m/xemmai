#ifndef XEMMAI__BOOLEAN_H
#define XEMMAI__BOOLEAN_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<bool> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			return static_cast<T0>(a_object->v_boolean);
		}
	};

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		return a_value ? a_extension->f_true() : a_extension->f_false();
	}
	static std::wstring f_string(bool a_self)
	{
		return a_self ? L"true" : L"false";
	}
	static bool f_not(bool a_self)
	{
		return !a_self;
	}
	static bool f_and(bool a_self, bool a_value)
	{
		return a_self & a_value;
	}
	static bool f_xor(bool a_self, bool a_value)
	{
		return a_self ^ a_value;
	}
	static bool f_or(bool a_self, bool a_value)
	{
		return a_self | a_value;
	}
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_not(t_object* a_this, t_stack& a_stack);
	virtual void f_and(t_object* a_this, t_stack& a_stack);
	virtual void f_xor(t_object* a_this, t_stack& a_stack);
	virtual void f_or(t_object* a_this, t_stack& a_stack);
};

}

#endif
