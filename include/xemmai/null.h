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

	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
