#include <xemmai/convert.h>

using namespace xemmai;

enum t_number
{
	e_number__ZERO, e_number__ONE
};

struct t_enum : t_extension
{
	t_slot v_type_number;

	template<typename T>
	void f_type__(const t_transfer& a_type);

	t_enum(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return f_global();
	}
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), a_value);
	}
};

template<>
inline void t_enum::f_type__<t_number>(const t_transfer& a_type)
{
	v_type_number = a_type;
}

template<>
inline const t_enum* t_enum::f_extension<t_enum>() const
{
	return this;
}

template<>
inline t_object* t_enum::f_type<t_number>() const
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
		t_define<t_number, ptrdiff_t>(a_extension, L"Number")
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
