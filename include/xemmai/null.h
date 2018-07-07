#ifndef XEMMAI__NULL_H
#define XEMMAI__NULL_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::nullptr_t> : t_uninstantiatable<t_underivable<t_with_ids<std::nullptr_t, t_type_immutable>>>
{
	static std::wstring f_string(const t_value& a_self)
	{
		return L"null";
	}
	static void f_define();

	using t_base::t_base;
};

}

#endif
