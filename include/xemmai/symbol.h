#ifndef XEMMAI__SYMBOL_H
#define XEMMAI__SYMBOL_H

#include "object.h"

namespace xemmai
{

class t_symbol
{
	friend class t_object;
	friend struct t_finalizes<t_bears<t_symbol>>;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_type>;
	friend struct t_type_of<t_symbol>;

	std::map<std::wstring, t_slot, std::less<>>::iterator v_entry;

	t_symbol(std::map<std::wstring, t_slot, std::less<>>::iterator a_entry) : v_entry(a_entry)
	{
		v_entry->second = t_object::f_of(this);
	}
	XEMMAI__LOCAL ~t_symbol();

public:
	XEMMAI__PUBLIC static t_object* f_instantiate(std::wstring_view a_value);

	const std::wstring& f_string() const
	{
		return v_entry->first;
	}
};

template<>
struct t_type_of<t_symbol> : t_holds<t_symbol>
{
	XEMMAI__LOCAL void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_symbol>().v_entry->second);
	}
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
};

}

#endif
