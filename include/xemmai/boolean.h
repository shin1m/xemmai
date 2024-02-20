#ifndef XEMMAI__BOOLEAN_H
#define XEMMAI__BOOLEAN_H

#include "string.h"

namespace xemmai
{

template<>
struct t_type_of<bool> : t_uninstantiatable<t_bears<bool>>
{
	template<typename T>
	struct t_cast
	{
		static bool f_as(auto&& a_object)
		{
			return a_object && (a_object.f_tag() != e_tag__BOOLEAN || a_object.f_boolean());
		}
		static bool f_is(t_object* a_object)
		{
			return true;
		}
	};

	static t_object* f__string(bool a_self)
	{
		return t_string::f_instantiate(a_self ? L"true"sv : L"false"sv);
	}
	static intptr_t f__hash(bool a_self)
	{
		return a_self ? 1 : 0;
	}
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
};

}

#endif
