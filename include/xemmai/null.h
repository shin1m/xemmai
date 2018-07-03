#ifndef XEMMAI__NULL_H
#define XEMMAI__NULL_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::nullptr_t> : t_type_immutable
{
	static constexpr auto V_ids = f_ids<std::nullptr_t, t_object>();

	static std::wstring f_string(const t_value& a_self)
	{
		return L"null";
	}
	static void f_define();

	using t_type_immutable::t_type_immutable;
	virtual t_type* f_derive();
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
};

}

#endif
