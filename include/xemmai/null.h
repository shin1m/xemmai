#ifndef XEMMAI__NULL_H
#define XEMMAI__NULL_H

#include "object.h"

namespace xemmai
{

class t_null
{
};

template<>
struct t_type_of<t_null> : t_type
{
	static std::wstring f_string(t_object* a_self)
	{
		return L"null";
	}
	static t_transfer f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

#endif
