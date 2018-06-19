#ifndef XEMMAI__SYMBOL_H
#define XEMMAI__SYMBOL_H

#include "object.h"

namespace xemmai
{

class t_symbol
{
	friend class t_value;
	friend class t_object;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_type>;
	friend struct t_type_of<t_symbol>;

	std::map<std::wstring, t_slot>::iterator v_entry;
	volatile size_t v_revision = 0;

	t_symbol(std::map<std::wstring, t_slot>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_symbol() = default;

public:
	XEMMAI__PORTABLE__EXPORT static t_scoped f_instantiate(const std::wstring& a_value);
	static void f_revise(t_object* a_this);

	const std::wstring& f_string() const
	{
		return v_entry->first;
	}
};

template<>
struct t_type_of<t_symbol> : t_type_immutable
{
	void f_define();

	t_type_of(t_scoped&& a_module, t_type* a_super) : t_type_immutable(std::move(a_module), a_super)
	{
		v_revive = v_fixed = v_shared = true;
	}
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
};

}

#endif
