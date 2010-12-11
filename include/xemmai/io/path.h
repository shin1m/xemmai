#ifndef XEMMAI__IO__PATH_H
#define XEMMAI__IO__PATH_H

#include "../portable/path.h"
#include "../object.h"

namespace xemmai
{

class t_io;

template<>
struct t_type_of<portable::t_path> : t_type
{
	typedef t_io t_extension;

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		t_transfer object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new portable::t_path(a_value));
		return object;
	}
	static void f_define(t_io* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_divide(t_object* a_this, t_slot* a_stack);
};

}

#endif
