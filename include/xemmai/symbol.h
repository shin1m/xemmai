#ifndef XEMMAI__SYMBOL_H
#define XEMMAI__SYMBOL_H

#include <map>

#include "object.h"

namespace xemmai
{

struct t_symbol : t_type
{
	XEMMAI__PORTABLE__EXPORT static t_transfer f_instantiate(const std::wstring& a_value);
	static std::wstring f_string(t_object* a_self)
	{
		return f_as<const std::map<std::wstring, t_slot>::iterator&>(a_self)->first;
	}
	static void f_define(t_object* a_class);

	t_symbol(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_revive = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

#endif
