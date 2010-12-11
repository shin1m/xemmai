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
	static t_transfer f_instantiate(size_t a_size, const t_transfer& a_outer);

	t_slot v_outer;

	void* operator new(size_t a_size, size_t a_n)
	{
		char* p = new char[a_size + sizeof(t_slot) * a_n];
		*reinterpret_cast<size_t*>(p) = a_n;
		return p;
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_scope(const t_transfer& a_outer) : v_outer(a_outer)
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
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
