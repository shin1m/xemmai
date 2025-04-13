#ifndef XEMMAI__BYTES_H
#define XEMMAI__BYTES_H

#include "object.h"

namespace xemmai
{

class t_bytes
{
	friend struct t_finalizes<t_bears<t_bytes>>;
	friend struct t_type_of<t_bytes>;

	size_t v_size;

	t_bytes(size_t a_size) : v_size(a_size)
	{
	}
	~t_bytes() = default;
	unsigned char* f_entries() const
	{
		return const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(this + 1));
	}
	void f_validate(intptr_t& a_index) const
	{
		if (a_index < 0) {
			a_index += v_size;
			if (a_index < 0) f_throw(L"out of range."sv);
		} else {
			if (a_index >= static_cast<intptr_t>(v_size)) f_throw(L"out of range."sv);
		}
	}
	void f_validate(intptr_t& a_index, size_t a_size) const
	{
		if (a_index < 0) {
			a_index += v_size;
			if (a_index < 0) f_throw(L"out of range."sv);
		}
		if (a_index + a_size > v_size) f_throw(L"out of range."sv);
	}

public:
	XEMMAI__PUBLIC static t_object* f_instantiate(size_t a_size);

	t_object* f_string() const;
	intptr_t f_get_at(intptr_t a_index) const
	{
		f_validate(a_index);
		return (*this)[a_index];
	}
	intptr_t f_set_at(intptr_t a_index, intptr_t a_value)
	{
		f_validate(a_index);
		return (*this)[a_index] = a_value;
	}
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
	void f_copy(intptr_t a_index0, size_t a_size, t_bytes& a_other, intptr_t a_index1) const
	{
		f_validate(a_index0, a_size);
		a_other.f_validate(a_index1, a_size);
		unsigned char* p = f_entries() + a_index0;
		std::copy(p, p + a_size, a_other.f_entries() + a_index1);
	}
};

template<>
struct t_type_of<t_bytes> : t_holds<t_bytes>
{
	static t_object* f__construct(t_type* a_class, size_t a_size);
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
};

}

#endif
