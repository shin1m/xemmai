#ifndef XEMMAI__SCOPE_H
#define XEMMAI__SCOPE_H

#include "object.h"

namespace xemmai
{

class t_scope
{
	friend class t_lambda_shared;
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_scope>;

	size_t v_size;
	t_slot v_outer;

	t_scope() : v_size(0)
	{
	}
	t_scope(size_t a_size, t_object* a_outer) : v_size(a_size), v_outer(a_outer)
	{
		std::uninitialized_default_construct_n(f_entries(), v_size);
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_outer);
		auto p = f_entries();
		auto q = p + v_size;
		for (; p < q; ++p) a_scan(*p);
	}

public:
	t_object* f_outer() const
	{
		return v_outer;
	}
	t_svalue* f_entries()
	{
		return reinterpret_cast<t_svalue*>(this + 1);
	}
};

template<>
struct t_type_of<t_scope> : t_uninstantiatable<t_finalizes<t_derives<t_scope>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_scope>().f_scan(a_scan);
	}
};

}

#endif
