#include <xemmai/lambda.h>

#include <xemmai/method.h>
#include <xemmai/tuple.h>
#include <xemmai/global.h>

namespace xemmai
{

t_transfer t_lambda::f_instantiate(const t_transfer& a_scope, const t_transfer& a_code)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_lambda>());
	object.f_pointer__(new t_lambda(a_scope, a_code));
	return object;
}

t_type* t_type_of<t_lambda>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_lambda& p = f_as<t_lambda&>(a_this);
	a_scan(p.v_scope);
	a_scan(p.v_code);
}

void t_type_of<t_lambda>::f_finalize(t_object* a_this)
{
	delete &f_as<t_lambda&>(a_this);
}

void t_type_of<t_lambda>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_lambda>::f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	t_lambda& p = f_as<t_lambda&>(a_this);
	t_code& code = f_as<t_code&>(p.v_code);
	if (a_n != code.v_arguments) t_throwable::f_throw(L"invalid number of arguments.");
	t_fiber::t_context::f_push(p.v_code, p.v_scope, a_self, a_stack);
}

void t_type_of<t_lambda>::f_get_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(t_method::f_instantiate(a_this, a0));
	context.f_done();
}

t_transfer t_advanced_lambda::f_instantiate(const t_transfer& a_scope, const t_transfer& a_code, t_slot* a_stack)
{
	t_code& code = f_as<t_code&>(a_code);
	t_transfer defaults;
	size_t n = code.v_arguments - code.v_minimum;
	if (code.v_variadic) --n;
	if (n > 0) {
		defaults = t_tuple::f_instantiate(n);
		t_tuple& tuple = f_as<t_tuple&>(defaults);
		for (size_t i = 0; i < n; ++i) tuple[i].f_construct(a_stack[i].f_transfer());
	}
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_advanced_lambda>());
	object.f_pointer__(new t_advanced_lambda(a_scope, a_code, defaults));
	return object;
}

void t_type_of<t_advanced_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_type_of<t_lambda>::f_scan(a_this, a_scan);
	a_scan(f_as<t_advanced_lambda&>(a_this).v_defaults);
}

void t_type_of<t_advanced_lambda>::f_finalize(t_object* a_this)
{
	delete &f_as<t_advanced_lambda&>(a_this);
}

void t_type_of<t_advanced_lambda>::f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	t_advanced_lambda& p = f_as<t_advanced_lambda&>(a_this);
	t_code& code = f_as<t_code&>(p.v_code);
	if (a_n < code.v_minimum) t_throwable::f_throw(L"too few arguments.");
	size_t arguments = code.v_arguments;
	if (code.v_variadic)
		--arguments;
	else if (a_n > arguments)
		t_throwable::f_throw(L"too many arguments.");
	t_fiber::t_context::f_push(p.v_code, p.v_scope, a_self, a_stack);
	if (a_n < arguments) {
		const t_tuple& t0 = f_as<const t_tuple&>(p.v_defaults);
		t_slot* t1 = a_stack + code.v_minimum + 1;
		for (size_t i = a_n - code.v_minimum; i < t0.f_size(); ++i) t1[i].f_construct(t0[i]);
		if (code.v_variadic) a_stack[code.v_arguments].f_construct(t_tuple::f_instantiate(0));
	} else if (code.v_variadic) {
		size_t n = a_n - arguments;
		t_transfer x = t_tuple::f_instantiate(n);
		t_slot* t0 = a_stack + code.v_arguments;
		t_tuple& t1 = f_as<t_tuple&>(x);
		for (size_t i = 0; i < n; ++i) t1[i].f_construct(t0[i].f_transfer());
		t0[0].f_construct(x);
	}
}

}
