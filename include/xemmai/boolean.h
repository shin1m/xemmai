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
			return a_object.f_boolean();
		}
	};
	template<typename T>
	struct t_as<T, t_object*>
	{
		typedef T t_type;

		static T f_call(t_object* a_object)
		{
			return a_object->f_boolean();
		}
	};
	template<typename T0, typename T1>
	struct t_of
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
		}
	};
	template<typename T>
	struct t_of<bool, T>
	{
		static bool f_call(T a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__BOOLEAN:
				return true;
			case t_value::e_tag__NULL:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return dynamic_cast<t_type_of<bool>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
			}
		}
	};
	template<typename T0, typename T1>
	struct t_is
	{
		static bool f_call(T1 a_object)
		{
			return t_of<typename t_fundamental<T0>::t_type, T1>::f_call(a_object);
		}
	};

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		return t_transfer(static_cast<bool>(a_value));
	}
	static std::wstring f_string(bool a_self)
	{
		return a_self ? L"true" : L"false";
	}
	static ptrdiff_t f_hash(bool a_self)
	{
		return a_self ? 1 : 0;
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
		v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
