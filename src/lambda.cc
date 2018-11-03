#include <xemmai/lambda.h>

#include <xemmai/global.h>

namespace xemmai
{

t_scoped t_lambda::f_instantiate(t_slot* a_scope, t_scoped&& a_code)
{
	if (f_as<t_code&>(a_code).v_shared)
		return f_global()->f_type<t_lambda_shared>()->f_new<t_lambda_shared>(true, a_scope, std::move(a_code));
	else
		return f_global()->f_type<t_lambda>()->f_new<t_lambda>(true, a_scope, std::move(a_code));
}

t_scoped t_lambda::f_instantiate(t_slot* a_scope, t_scoped&& a_code, t_stacked* a_stack)
{
	auto& code = f_as<t_code&>(a_code);
	t_scoped defaults;
	size_t n = code.v_arguments - code.v_minimum;
	if (code.v_variadic) --n;
	if (n > 0) {
		defaults = t_tuple::f_instantiate(n);
		auto& tuple = f_as<t_tuple&>(defaults);
		for (size_t i = 0; i < n; ++i) tuple[i].f_construct(std::move(a_stack[i]));
	}
	if (code.v_shared)
		return f_global()->f_type<t_advanced_lambda<t_lambda_shared>>()->f_new<t_advanced_lambda<t_lambda_shared>>(true, a_scope, std::move(a_code), std::move(defaults));
	else
		return f_global()->f_type<t_advanced_lambda<t_lambda>>()->f_new<t_advanced_lambda<t_lambda>>(true, a_scope, std::move(a_code), std::move(defaults));
}

size_t t_type_of<t_lambda>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(t_method::f_instantiate(f_global()->f_type<t_method>(), a_this, std::move(a0)));
	return -1;
}

}
