#include <xemmai/convert.h>

using namespace xemmai;

enum t_number
{
	c_number__ZERO, c_number__ONE
};

struct t_enum : t_library
{
	t_slot_of<t_type> v_type_number;

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_enum, t_global, f_global())
XEMMAI__LIBRARY__TYPE(t_enum, number)

namespace xemmai
{

template<>
struct t_type_of<t_number> : t_enum_of<t_number, t_enum>
{
	static t_object* f_define(t_library* a_library)
	{
		return t_base::f_define(a_library, [](auto a_fields)
		{
			a_fields
			(L"ZERO"sv, c_number__ZERO)
			(L"ONE"sv, c_number__ONE)
			;
		});
	}

	using t_base::t_base;
};

}

void t_enum::f_scan(t_scan a_scan)
{
	a_scan(v_type_number);
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
