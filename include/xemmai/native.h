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

	t_library::t_function v_function;
	t_slot v_library;

	t_native(t_library::t_function a_function, t_object* a_library) : v_function(a_function), v_library(a_library)
	{
	}
	~t_native() = default;
};

template<>
struct t_type_of<t_native> : t_uninstantiatable<t_holds<t_native>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_native>().v_library);
	}
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
};

}

#endif
