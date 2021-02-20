#ifndef XEMMAI__SCOPE_H
#define XEMMAI__SCOPE_H

#include "object.h"

namespace xemmai
{

class t_scope
{
	friend class t_lambda_shared;
	friend class t_type_of<t_object>;
	friend class t_type_of<t_scope>;

	size_t v_size;
	t_slot v_outer;
	t_svalue* v_outer_entries;

	t_scope() : v_size(0)
	{
	}
	t_scope(size_t a_size, t_svalue* a_outer) : v_size(a_size), v_outer(f_this(a_outer)), v_outer_entries(a_outer)
	{
		auto p = f_entries();
		auto q = p + v_size;
		for (; p < q; ++p) new(p) t_svalue();
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_outer);
		auto p = f_entries();
		auto q = p + v_size;
		for (; p < q; ++p) a_scan(*p);
	}

public:
	static t_svalue* f_outer(t_svalue* a_entries)
	{
		return reinterpret_cast<t_scope*>(a_entries)[-1].v_outer_entries;
	}
	static t_object* f_this(t_svalue* a_entries)
	{
		return t_object::f_of(reinterpret_cast<t_scope*>(a_entries) - 1);
	}

	t_svalue* f_entries()
	{
		return reinterpret_cast<t_svalue*>(this + 1);
	}
};

template<>
struct t_type_of<t_scope> : t_uninstantiatable<t_underivable<t_finalizes<t_derives<t_scope>>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_scope>().f_scan(a_scan);
	}
};

}

#endif
