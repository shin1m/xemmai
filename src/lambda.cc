#include <xemmai/global.h>

namespace xemmai
{

t_object* t_lambda::f_instantiate(t_svalue* a_scope, t_object* a_code)
{
	if (a_code->f_as<t_code>().v_shared)
		return f_new<t_lambda_shared>(f_global(), a_scope, a_code);
	else
		return f_new<t_lambda>(f_global(), a_scope, a_code);
}

t_object* t_lambda::f_instantiate(t_svalue* a_scope, t_object* a_code, t_pvalue* a_stack)
{
	auto& code = a_code->f_as<t_code>();
	t_object* defaults = nullptr;
	size_t n = code.v_arguments - code.v_minimum;
	if (code.v_variadic) --n;
	if (n > 0) defaults = t_tuple::f_instantiate(n, [&](auto& tuple)
	{
		std::uninitialized_copy_n(a_stack, n, &tuple[0]);
	});
	if (code.v_shared)
		return f_new<t_advanced_lambda<t_lambda_shared>>(f_global(), a_scope, a_code, defaults);
	else
		return f_new<t_advanced_lambda<t_lambda>>(f_global(), a_scope, a_code, defaults);
}

size_t t_type_of<t_lambda>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = xemmai::f_new<t_method>(f_global(), a_this, a_stack[2]);
	return -1;
}

}
