#ifndef XEMMAI__IO__PATH_H
#define XEMMAI__IO__PATH_H

#include "../portable/path.h"
#include "../string.h"

namespace xemmai
{

class t_io;

template<>
struct t_type_of<portable::t_path> : t_derivable<t_holds<portable::t_path, t_type_immutable>>
{
	typedef t_io t_extension;

	template<typename T>
	static t_scoped f_transfer(const t_io* a_extension, T&& a_value);
	static t_scoped f_construct(t_type* a_class, const t_string& a_value)
	{
		return a_class->f_new<portable::t_path>(true, a_value);
	}
	static portable::t_path f__divide(const portable::t_path& a_self, const t_string& a_value)
	{
		return a_self / a_value;
	}
	static void f_define(t_io* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static size_t f_do_divide(t_object* a_this, t_stacked* a_stack);
};

}

#endif
