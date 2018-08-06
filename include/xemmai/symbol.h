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
struct t_type_of<t_symbol> : t_underivable<t_bears<t_symbol, t_type_immutable>>
{
	void f_define();

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : t_base(a_ids, a_super, std::move(a_module))
	{
		v_revive = true;
		f_scan = f_do_scan;
		f_finalize = f_do_finalize;
		v_instantiate = static_cast<void (t_type::*)(t_stacked*, size_t)>(&t_type_of::f_do_instantiate);
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	static void f_do_finalize(t_object* a_this);
	void f_do_instantiate(t_stacked* a_stack, size_t a_n);
};

}

#endif
