#include <xemmai/global.h>

namespace xemmai
{

size_t t_type_of<t_native>::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	auto& p = a_this->f_as<t_native>();
	p.v_function(&p.v_library->f_as<t_library>(), a_stack, a_n);
	return -1;
}

size_t t_type_of<t_native>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = xemmai::f_new<t_method>(f_global(), a_this, a_stack[2]);
	return -1;
}

}
