#ifndef XEMMAI__NULL_H
#define XEMMAI__NULL_H

#include "string.h"

namespace xemmai
{

template<>
struct t_type_of<std::nullptr_t> : t_uninstantiatable<t_underivable<t_bears<std::nullptr_t, t_type_immutable>>>
{
	static t_scoped f_string(const t_value& a_self)
	{
		return t_string::f_instantiate(L"null"sv);
	}
	static void f_define();

	using t_base::t_base;
};

}

#endif
