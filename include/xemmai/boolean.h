#ifndef XEMMAI__BOOLEAN_H
#define XEMMAI__BOOLEAN_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<bool> : t_type_immutable
{
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			return a_object.f_boolean();
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			return reinterpret_cast<size_t>(f_object(std::forward<T1>(a_object))) == t_value::e_tag__BOOLEAN;
		}
	};

	static constexpr auto V_ids = f_ids<bool, t_object>();

	static std::wstring f_string(bool a_self)
	{
		return a_self ? L"true" : L"false";
	}
	static intptr_t f_hash(bool a_self)
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

	using t_type_immutable::t_type_immutable;
	virtual t_type* f_derive();
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
};

}

#endif
