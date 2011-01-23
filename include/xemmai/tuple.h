#ifndef XEMMAI__TUPLE_H
#define XEMMAI__TUPLE_H

#include "object.h"

namespace xemmai
{

class t_tuple
{
	friend struct t_type_of<t_tuple>;

	size_t v_size;

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

	t_tuple()
	{
		t_slot* p = f_entries();
		for (size_t i = 0; i < v_size; ++i) new(p + i) t_slot();
	}
	~t_tuple()
	{
	}
	t_slot* f_entries() const
	{
		return const_cast<t_slot*>(reinterpret_cast<const t_slot*>(this + 1));
	}

public:
	static t_transfer f_instantiate(size_t a_size);

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
struct t_type_of<t_tuple> : t_type
{
	static std::wstring f_string(const t_value& a_self);
	static int f_hash(const t_value& a_self);
	static bool f_less(const t_value& a_self, const t_value& a_other);
	static bool f_less_equal(const t_value& a_self, const t_value& a_other);
	static bool f_greater(const t_value& a_self, const t_value& a_other);
	static bool f_greater_equal(const t_value& a_self, const t_value& a_other);
	static bool f_equals(const t_value& a_self, const t_value& a_other);
	static bool f_not_equals(const t_value& a_self, const t_value& a_other)
	{
		return !f_equals(a_self, a_other);
	}
	static void f_each(const t_value& a_self, const t_value& a_callable);
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_slot* a_stack);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
	virtual void f_less(t_object* a_this, t_slot* a_stack);
	virtual void f_less_equal(t_object* a_this, t_slot* a_stack);
	virtual void f_greater(t_object* a_this, t_slot* a_stack);
	virtual void f_greater_equal(t_object* a_this, t_slot* a_stack);
	virtual void f_equals(t_object* a_this, t_slot* a_stack);
	virtual void f_not_equals(t_object* a_this, t_slot* a_stack);
};

}

#endif
