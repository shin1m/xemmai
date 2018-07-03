#include <xemmai/lambda.h>

#include <xemmai/tuple.h>
#include <xemmai/global.h>

namespace xemmai
{

t_scoped t_lambda::f_instantiate(t_scoped&& a_scope, t_scoped&& a_code)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_lambda>());
	object.f_pointer__(new t_lambda(std::move(a_scope), std::move(a_code)));
	return object;
}

constexpr decltype(t_type_of<t_lambda>::V_ids) t_type_of<t_lambda>::V_ids;

t_type* t_type_of<t_lambda>::f_derive()
{
	return nullptr;
}

void t_type_of<t_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	auto& p = f_as<t_lambda&>(a_this);
	a_scan(p.v_scope);
	a_scan(p.v_code);
}

void t_type_of<t_lambda>::f_finalize(t_object* a_this)
{
	delete &f_as<t_lambda&>(a_this);
}

void t_type_of<t_lambda>::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(a_stack, a_n, L"uninstantiatable.");
}

size_t t_type_of<t_lambda>::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	auto& p = f_as<t_lambda&>(a_this);
	if (a_n != p.v_arguments) t_throwable::f_throw(a_stack, a_n, L"invalid number of arguments.");
	if (p.v_shared) {
		t_context context(a_this, a_stack, p.v_size, p.v_shareds, p.v_scope);
		return t_code::f_loop(&context, p);
	} else {
		t_context context(a_this, a_stack, p.v_size);
		return t_code::f_loop(&context, p);
	}
}

size_t t_type_of<t_lambda>::f_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(t_method::f_instantiate(a_this, std::move(a0)));
	return -1;
}

t_scoped t_advanced_lambda::f_instantiate(t_scoped&& a_scope, t_scoped&& a_code, t_stacked* a_stack)
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
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_advanced_lambda>());
	object.f_pointer__(new t_advanced_lambda(std::move(a_scope), std::move(a_code), std::move(defaults)));
	return object;
}

constexpr decltype(t_type_of<t_advanced_lambda>::V_ids) t_type_of<t_advanced_lambda>::V_ids;

void t_type_of<t_advanced_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_type_of<t_lambda>::f_scan(a_this, a_scan);
	a_scan(f_as<t_advanced_lambda&>(a_this).v_defaults);
}

void t_type_of<t_advanced_lambda>::f_finalize(t_object* a_this)
{
	delete &f_as<t_advanced_lambda&>(a_this);
}

size_t t_type_of<t_advanced_lambda>::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	auto& p = f_as<t_advanced_lambda&>(a_this);
	if (a_n < p.v_minimum) t_throwable::f_throw(a_stack, a_n, L"too few arguments.");
	size_t arguments = p.v_arguments;
	if (p.v_variadic)
		--arguments;
	else if (a_n > arguments)
		t_throwable::f_throw(a_stack, a_n, L"too many arguments.");
	if (a_n < arguments) {
		auto& t0 = f_as<const t_tuple&>(p.v_defaults);
		t_stacked* t1 = a_stack + p.v_minimum + 2;
		for (size_t i = a_n - p.v_minimum; i < t0.f_size(); ++i) t1[i].f_construct(t0[i]);
		if (p.v_variadic) a_stack[p.v_arguments + 1].f_construct(t_tuple::f_instantiate(0));
	} else if (p.v_variadic) {
		size_t n = a_n - arguments;
		t_scoped x = t_tuple::f_instantiate(n);
		t_stacked* t0 = a_stack + arguments + 2;
		auto& t1 = f_as<t_tuple&>(x);
		for (size_t i = 0; i < n; ++i) t1[i].f_construct(std::move(t0[i]));
		t0[0].f_construct(std::move(x));
	}
	if (p.v_shared) {
		t_context context(a_this, a_stack, p.v_size, p.v_shareds, p.v_scope);
		return t_code::f_loop(&context, p);
	} else {
		t_context context(a_this, a_stack, p.v_size);
		return t_code::f_loop(&context, p);
	}
}

}
