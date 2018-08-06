#ifndef XEMMAI__IO__PATH_H
#define XEMMAI__IO__PATH_H

#include "../portable/path.h"
#include "../object.h"

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
		t_scoped object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new portable::t_path(std::forward<T>(a_value)));
		return object;
	}
	static void f_define(t_io* a_extension);

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : t_base(a_ids, a_super, std::move(a_module))
	{
		v_construct = static_cast<t_scoped (t_type::*)(t_stacked*, size_t)>(&t_type_of::f_do_construct);
		f_divide = f_do_divide;
	}
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static size_t f_do_divide(t_object* a_this, t_stacked* a_stack);
};

}

#endif
