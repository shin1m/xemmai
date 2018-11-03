#ifndef XEMMAI__NATIVE_H
#define XEMMAI__NATIVE_H

#include "module.h"

namespace xemmai
{

class t_native
{
	friend struct t_finalizes<t_bears<t_native>>;
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
struct t_type_of<t_native> : t_uninstantiatable<t_underivable<t_holds<t_native>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_native>().v_module);
	}
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
};

}

#endif
