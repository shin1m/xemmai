#ifndef XEMMAI__BYTES_H
#define XEMMAI__BYTES_H

#include "object.h"

namespace xemmai
{

class t_bytes
{
	friend struct t_type_of<t_bytes>;

	size_t v_size;

	void* operator new(size_t a_size, size_t a_n)
	{
		char* p = new char[a_size + a_n];
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

	t_bytes()
	{
	}
	~t_bytes()
	{
	}
	unsigned char* f_entries() const
	{
		return const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(this + 1));
	}

public:
	static t_transfer f_instantiate(size_t a_size);

	std::wstring f_string() const;
	int f_hash() const
	{
		int n = 0;
		for (size_t i = 1; i < v_size; ++i) n += (*this)[i];
		return n;
	}
	int f_get_at(int a_index) const;
	int f_set_at(int a_index, int a_value);
	size_t f_size() const
	{
		return v_size;
	}
	unsigned char operator[](size_t a_index) const
	{
		return f_entries()[a_index];
	}
	unsigned char& operator[](size_t a_index)
	{
		return f_entries()[a_index];
	}
};

template<>
struct t_type_of<t_bytes> : t_type
{
	static bool f_equals(t_object* a_self, t_object* a_other);
	static bool f_not_equals(t_object* a_self, t_object* a_other)
	{
		return !f_equals(a_self, a_other);
	}
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
	virtual void f_hash(t_object* a_this, t_stack& a_stack);
	virtual void f_get_at(t_object* a_this, t_stack& a_stack);
	virtual void f_set_at(t_object* a_this, t_stack& a_stack);
	virtual void f_equals(t_object* a_this, t_stack& a_stack);
	virtual void f_not_equals(t_object* a_this, t_stack& a_stack);
};

}

#endif
