#ifndef XEMMAI__BOOLEAN_H
#define XEMMAI__BOOLEAN_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<bool> : t_uninstantiatable<t_bears<bool>>
{
	template<typename T>
	struct t_as
	{
		static bool f_call(auto&& a_object)
		{
			return a_object.f_boolean();
		}
	};
	template<typename T>
	struct t_is
	{
		static bool f_call(auto&& a_object)
		{
			return reinterpret_cast<uintptr_t>(f_object(std::forward<decltype(a_object)>(a_object))) == e_tag__BOOLEAN;
		}
	};

	XEMMAI__PORTABLE__EXPORT static t_object* f__string(bool a_self);
	static intptr_t f__hash(bool a_self)
	{
		return a_self ? 1 : 0;
	}
	static void f_define();

	using t_base::t_base;
};

}

#endif
