#ifndef XEMMAI__SCOPE_H
#define XEMMAI__SCOPE_H

#include "object.h"

namespace xemmai
{

class t_scope
{
	friend class t_lambda;
	friend class t_module;
	friend class t_type_of<t_scope>;

	static t_scoped f_instantiate(t_scope* a_scope);

	size_t v_size;
	t_slot v_outer;
	t_slot* v_outer_entries;
	t_object* v_this;

	t_scope() : v_size(0)
	{
	}
	t_scope(size_t a_size, t_slot* a_outer) : v_size(a_size), v_outer(f_this(a_outer)), v_outer_entries(a_outer)
	{
		auto p = f_entries();
		auto q = p + v_size;
		for (; p < q; ++p) new(p) t_slot();
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_outer);
		auto p = f_entries();
		auto q = p + v_size;
		for (; p < q; ++p) if (*p) a_scan(*p);
	}

public:
	static t_slot* f_outer(t_slot* a_entries)
	{
		return reinterpret_cast<t_scope*>(a_entries)[-1].v_outer_entries;
	}
	static t_object* f_this(t_slot* a_entries)
	{
		return reinterpret_cast<t_scope*>(a_entries)[-1].v_this;
	}

	void* operator new(size_t a_size, size_t a_n)
	{
		return new char[a_size + sizeof(t_slot) * a_n];
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_slot* f_entries()
	{
		return reinterpret_cast<t_slot*>(this + 1);
	}
};

template<>
struct t_type_of<t_scope> : t_uninstantiatable<t_underivable<t_with_traits<t_finalizes<t_scope>, false, true>>>
{
	using t_base::t_base;
	virtual void f_scan(t_object* a_this, t_scan a_scan);
};

}

#endif
