#include <xemmai/global.h>

namespace xemmai
{

t_object* t_lambda::f_instantiate(t_svalue* a_scope, t_object* a_code)
{
	if (f_as<t_code&>(a_code).v_shared)
		return f_new<t_lambda_shared>(f_global(), true, a_scope, a_code);
	else
		return f_new<t_lambda>(f_global(), true, a_scope, a_code);
}

t_object* t_lambda::f_instantiate(t_svalue* a_scope, t_object* a_code, t_pvalue* a_stack)
{
	auto& code = f_as<t_code&>(a_code);
	t_object* defaults = nullptr;
	size_t n = code.v_arguments - code.v_minimum;
	if (code.v_variadic) --n;
	if (n > 0) defaults = t_tuple::f_instantiate(n, [&](auto& tuple)
	{
		for (size_t i = 0; i < n; ++i) new(&tuple[i]) t_svalue(a_stack[i]);
	});
	if (code.v_shared)
		return f_new<t_advanced_lambda<t_lambda_shared>>(f_global(), true, a_scope, a_code, defaults);
	else
		return f_new<t_advanced_lambda<t_lambda>>(f_global(), true, a_scope, a_code, defaults);
}

size_t t_type_of<t_lambda>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = t_method::f_instantiate(f_global()->f_type<t_method>(), a_this, a_stack[2]);
	return -1;
}

}
