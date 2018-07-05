#ifndef XEMMAI__IO__PATH_H
#define XEMMAI__IO__PATH_H

#include "../portable/path.h"
#include "../object.h"

namespace xemmai
{

class t_io;

template<>
struct t_type_of<portable::t_path> : t_with_ids<portable::t_path, t_type_immutable>
{
	typedef t_io t_extension;

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		t_scoped object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new portable::t_path(std::forward<T>(a_value)));
		return object;
	}
	static void f_define(t_io* a_extension);

	using t_with_ids<portable::t_path, t_type_immutable>::t_with_ids;
	virtual t_type* f_derive();
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
	virtual size_t f_divide(t_object* a_this, t_stacked* a_stack);
};

}

#endif
