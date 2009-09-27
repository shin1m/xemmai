#ifndef XEMMAI__SYMBOL_H
#define XEMMAI__SYMBOL_H

#include <map>

#include "object.h"

namespace xemmai
{

class t_symbol
{
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_type>;
	friend struct t_type_of<t_symbol>;

	std::map<std::wstring, t_slot>::iterator v_entry;
	volatile size_t v_revision;

	t_symbol(std::map<std::wstring, t_slot>::iterator a_entry) : v_entry(a_entry), v_revision(0)
	{
	}
	~t_symbol()
	{
	}

public:
	XEMMAI__PORTABLE__EXPORT static t_transfer f_instantiate(const std::wstring& a_value);
	static void f_define(t_object* a_class);
	static void f_revise(t_object* a_this);

	const std::wstring& f_string() const
	{
		return v_entry->first;
	}
};

template<>
struct t_type_of<t_symbol> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
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
