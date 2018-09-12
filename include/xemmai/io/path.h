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

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		t_scoped object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>(), true);
		object.f_pointer__(new portable::t_path(std::forward<T>(a_value)));
		return object;
	}
	static t_scoped f_construct(t_type* a_class, const t_string& a_value)
	{
		t_scoped object = t_object::f_allocate(a_class, true);
		object.f_pointer__(new portable::t_path(a_value));
		return object;
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
