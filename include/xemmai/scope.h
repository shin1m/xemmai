#ifndef XEMMAI__SCOPE_H
#define XEMMAI__SCOPE_H

#include "object.h"

namespace xemmai
{

class t_scope
{
	size_t v_size;

	t_slot* f_entries() const
	{
		return const_cast<t_slot*>(reinterpret_cast<const t_slot*>(this + 1));
	}
	void f_initialize()
	{
		t_slot* p = f_entries();
		t_slot* q = p + v_size;
		while (p < q) {
			new(p) t_slot();
			++p;
		}
	}

	t_scope(size_t a_size) : v_size(a_size)
	{
		f_initialize();
	}

public:
	static t_scoped f_instantiate(size_t a_size, t_scoped&& a_outer);

	t_slot v_outer;

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

	t_scope(size_t a_size, t_scoped&& a_outer) : v_size(a_size), v_outer(std::move(a_outer))
	{
		f_initialize();
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_outer);
		t_slot* p = f_entries();
		t_slot* q = p + v_size;
		while (p < q) {
			if (*p) a_scan(*p);
			++p;
		}
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_slot& operator[](size_t a_index) const
	{
		return f_entries()[a_index];
	}
	t_slot& operator[](size_t a_index)
	{
		return f_entries()[a_index];
	}
};

template<>
struct t_type_of<t_scope> : t_type
{
	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
