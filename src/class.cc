#include <xemmai/convert.h>

namespace xemmai
{

void t_class::f_do_scan(t_object* a_this, t_scan a_scan)
{
	auto& p = a_this->f_as<t_type>();
	if (!p.v_builtin || !f_engine()->f_module_global()) p.f_scan_type(a_scan);
}

t_pvalue t_class::f_do_get(t_object* a_this, t_object* a_key)
{
	auto& type = a_this->f_as<t_type>();
	auto index = type.f_index(a_key);
	if (index < type.v_instance_fields || index >= type.v_fields) f_throw(f_as<t_symbol&>(a_key).f_string());
	return type.f_fields()[index].second;
}

size_t t_class::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	auto& p = f_as<t_type&>(a_this);
	(p.*p.v_instantiate)(a_stack, a_n);
	return -1;
}

size_t t_class::f_do_add(t_object* a_this, t_pvalue* a_stack)
{
	auto& p = f_as<t_type&>(a_this);
	if (!p.v_derive) f_throw(L"underivable."sv);
	t_fields fields;
	t_builder::f_do(fields, [&](auto builder)
	{
		a_stack[1] = builder;
		a_stack[2].f_call(a_stack, 0);
	});
	a_stack[0] = (p.*p.v_derive)(fields);
	return -1;
}

t_pvalue t_type_of<t_builder>::f_do_get(t_object* a_this, t_object* a_key)
{
	auto& builder = a_this->f_as<t_builder>();
	builder.f_owned_or_throw([&]
	{
		if (!builder.v_fields) f_throw(L"already disposed."sv);
		builder.v_fields->v_instance.push_back(a_key);
	});
	return {};
}

void t_type_of<t_builder>::f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	auto& builder = a_this->f_as<t_builder>();
	builder.f_owned_or_throw([&]
	{
		if (!builder.v_fields) f_throw(L"already disposed."sv);
		builder.v_fields->v_class.emplace_back(a_key, a_value);
	});
}

}
