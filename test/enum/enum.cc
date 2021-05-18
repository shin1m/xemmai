#include <xemmai/convert.h>

using namespace xemmai;

enum t_number
{
	e_number__ZERO, e_number__ONE
};

struct t_enum : t_library
{
	t_slot_of<t_type> v_type_number;

	using t_library::t_library;
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_type_number);
	}
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_enum*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		using t = t_type_of<typename t_fundamental<T>::t_type>;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_enum* t_enum::f_library<t_enum>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE(t_enum, number)

namespace xemmai
{

template<>
struct t_type_of<t_number> : t_enum_of<t_number, t_enum>
{
	static t_object* f_define(t_library* a_library)
	{
		t_define{a_library}.f_derive<t_number, intptr_t>();
		return a_library->f_type<t_number>()->f_do_derive({{}, t_define(a_library)
			(L"ZERO"sv, e_number__ZERO)
			(L"ONE"sv, e_number__ONE)
		});
	}

	using t_base::t_base;
};

}

std::vector<std::pair<t_root, t_rvalue>> t_enum::f_define()
{
	return t_define(this)
		(L"Number"sv, t_type_of<t_number>::f_define(this))
	;
}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<t_enum>(a_handle);
}
