#include <xemmai/convert.h>

using namespace xemmai;

enum t_number
{
	e_number__ZERO, e_number__ONE
};

struct t_enum : t_extension
{
	t_slot_of<t_type> v_type_number;

	t_enum(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_enum* t_enum::f_extension<t_enum>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_enum::f_type_slot<t_number>()
{
	return v_type_number;
}

namespace xemmai
{

template<>
struct t_type_of<t_number> : t_enum_of<t_number, t_enum>
{
	static void f_define(t_extension* a_extension)
	{
		t_define<t_number, intptr_t>(a_extension, L"Number")
			(L"ZERO", e_number__ZERO)
			(L"ONE", e_number__ONE)
		;
	}

	using t_base::t_base;
};

}

t_enum::t_enum(t_object* a_module) : t_extension(a_module)
{
	t_type_of<t_number>::f_define(this);
}

void t_enum::f_scan(t_scan a_scan)
{
	a_scan(v_type_number);
}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new t_enum(a_module);
}
