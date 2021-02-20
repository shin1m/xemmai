#ifndef XEMMAI__TUPLE_H
#define XEMMAI__TUPLE_H

#include "object.h"

namespace xemmai
{

class t_tuple
{
	friend struct t_finalizes<t_bears<t_tuple, t_type_immutable>>;
	friend struct t_type_of<t_object>;

	size_t v_size;

	t_tuple(size_t a_size) : v_size(a_size)
	{
		auto p = f_entries();
		for (size_t i = 0; i < v_size; ++i) new(p + i) t_svalue();
	}
	~t_tuple() = default;
	t_svalue* f_entries() const
	{
		return const_cast<t_svalue*>(reinterpret_cast<const t_svalue*>(this + 1));
	}

public:
	static XEMMAI__PORTABLE__EXPORT t_object* f_instantiate(size_t a_size);

	void f_scan(t_scan a_scan)
	{
		auto p = f_entries();
		for (size_t i = 0; i < v_size; ++i) a_scan(p[i]);
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_svalue& operator[](size_t a_index) const
	{
		return f_entries()[a_index];
	}
	t_svalue& operator[](size_t a_index)
	{
		return f_entries()[a_index];
	}
	const t_svalue& f_get_at(size_t a_index) const
	{
		if (a_index >= v_size) f_throw(L"out of range."sv);
		return (*this)[a_index];
	}
	t_object* f_string() const;
	intptr_t f_hash() const;
	bool f_less(const t_tuple& a_other) const;
	bool f_less_equal(const t_tuple& a_other) const;
	bool f_greater(const t_tuple& a_other) const;
	bool f_greater_equal(const t_tuple& a_other) const;
	bool f_equals(const t_pvalue& a_other) const;
	bool f_not_equals(const t_pvalue& a_other) const
	{
		return !f_equals(a_other);
	}
	void f_each(const t_pvalue& a_callable) const;
};

template<>
struct t_type_of<t_tuple> : t_derivable<t_holds<t_tuple, t_type_immutable>>
{
	static void f__construct(xemmai::t_extension* a_extension, t_pvalue* a_stack, size_t a_n);
	void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_tuple>().f_scan(a_scan);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	static void f_do_hash(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_less(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_less_equal(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_greater(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_greater_equal(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_equals(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_not_equals(t_object* a_this, t_pvalue* a_stack);
};

template<size_t A_i>
inline void f_tuple(t_tuple& a_tuple)
{
}

template<size_t A_i, typename T_x, typename... T_xs>
inline void f_tuple(t_tuple& a_tuple, T_x&& a_x, T_xs&&... a_xs)
{
	a_tuple[A_i] = std::forward<T_x>(a_x);
	f_tuple<A_i + 1>(a_tuple, a_xs...);
}

template<typename... T_xs>
inline t_object* f_tuple(T_xs&&... a_xs)
{
	auto object = t_tuple::f_instantiate(sizeof...(T_xs));
	f_tuple<0>(object->f_as<t_tuple>(), a_xs...);
	return object;
}

}

#endif
