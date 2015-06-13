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
	~t_bytes() = default;
	unsigned char* f_entries() const
	{
		return const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(this + 1));
	}
	void f_validate(intptr_t& a_index) const;
	void f_validate(intptr_t& a_index, size_t a_size) const;

public:
	static XEMMAI__PORTABLE__EXPORT t_scoped f_instantiate(size_t a_size);

	XEMMAI__PORTABLE__EXPORT std::wstring f_string() const;
	intptr_t f_hash() const
	{
		intptr_t n = 0;
		for (size_t i = 1; i < v_size; ++i) n += (*this)[i];
		return n;
	}
	XEMMAI__PORTABLE__EXPORT intptr_t f_get_at(intptr_t a_index) const;
	XEMMAI__PORTABLE__EXPORT intptr_t f_set_at(intptr_t a_index, intptr_t a_value);
	size_t f_size() const
	{
		return v_size;
	}
	const unsigned char& operator[](size_t a_index) const
	{
		return f_entries()[a_index];
	}
	unsigned char& operator[](size_t a_index)
	{
		return f_entries()[a_index];
	}
	XEMMAI__PORTABLE__EXPORT void f_copy(intptr_t a_index0, size_t a_size, t_bytes& a_other, intptr_t a_index1) const;
};

template<>
struct t_type_of<t_bytes> : t_type
{
	static void f__construct(t_object* a_module, t_scoped* a_stack, size_t a_n);
	static bool f_equals(const t_value& a_self, const t_value& a_other);
	static bool f_not_equals(const t_value& a_self, const t_value& a_other)
	{
		return !f_equals(a_self, a_other);
	}
	static void f_define();

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_get_at(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_set_at(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_equals(t_object* a_this, t_scoped* a_stack);
	virtual size_t f_not_equals(t_object* a_this, t_scoped* a_stack);
};

}

#endif
