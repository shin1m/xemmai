#ifndef XEMMAI__NATIVE_H
#define XEMMAI__NATIVE_H

#include "module.h"

namespace xemmai
{

class t_native
{
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_native>;

	t_extension::t_function v_function;
	t_extension* v_extension;
	t_slot v_module;

	t_native(t_extension::t_function a_function, t_extension* a_extension) : v_function(a_function), v_extension(a_extension), v_module(a_extension->f_module())
	{
	}
	~t_native() = default;

public:
	XEMMAI__PORTABLE__EXPORT static t_scoped f_instantiate(t_extension::t_function a_function, t_extension* a_extension);
};

template<>
struct t_type_of<t_native> : t_uninstantiatable<t_underivable<t_with_traits<t_holds<t_native>, false, true>>>
{
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : t_base(a_ids, a_super, std::move(a_module))
	{
		f_scan = f_do_scan;
		f_call = f_do_call;
		f_get_at = f_do_get_at;
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
};

}

#endif
