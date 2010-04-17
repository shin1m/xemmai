#include <xemmai/lambda.h>

#include <xemmai/code.h>
#include <xemmai/method.h>
#include <xemmai/global.h>

namespace xemmai
{

t_transfer t_lambda::f_instantiate(const t_transfer& a_scope, const t_transfer& a_code)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_lambda>());
	object->v_pointer = new t_lambda(a_scope, a_code);
	return object;
}

t_type* t_type_of<t_lambda>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_lambda>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_lambda* p = f_as<t_lambda*>(a_this);
	a_scan(p->v_scope);
	a_scan(p->v_code);
}

void t_type_of<t_lambda>::f_finalize(t_object* a_this)
{
	delete f_as<t_lambda*>(a_this);
}

void t_type_of<t_lambda>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_lambda>::f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	t_lambda* p = f_as<t_lambda*>(a_this);
	t_code* code = f_as<t_code*>(p->v_code);
	if (a_n != code->v_arguments) t_throwable::f_throw(L"invalid number of arguments.");
	if (code->v_simple) {
		t_scope& s = *t_fixed_scope::f_instantiate(p->v_scope, a_self);
		while (a_n > 0) s[--a_n].f_construct(a_stack.f_pop());
		t_fiber::t_context::f_push(&s, p->v_code, &code->v_instructions[0]);
	} else {
		t_transfer scope = t_scope::f_instantiate(code->v_size, p->v_scope, a_self);
		t_scope& s = f_as<t_scope&>(scope);
		while (a_n > 0) s[--a_n].f_construct(a_stack.f_pop());
		t_fiber::t_context::f_push(scope, p->v_code, &code->v_instructions[0]);
	}
}

void t_type_of<t_lambda>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(t_method::f_instantiate(a_this, a0));
	context.f_done();
}

}
