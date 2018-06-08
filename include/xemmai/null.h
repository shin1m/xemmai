#ifndef XEMMAI__NULL_H
#define XEMMAI__NULL_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::nullptr_t> : t_type
{
	static std::wstring f_string(const t_value& a_self)
	{
		return L"null";
	}
	static void f_define();

	t_type_of(t_scoped&& a_module, t_type* a_super) : t_type(std::move(a_module), a_super)
	{
		v_shared = v_immutable = true;
	}
	virtual t_type* f_derive();
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
};

}

#endif
