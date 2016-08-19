#include "container.h"

namespace xemmai
{

t_scoped t_type_of<t_pair>::f_instantiate(t_container* a_extension, t_scoped&& a_value)
{
	t_scoped object = t_object::f_allocate(a_extension->v_type_pair);
	object.f_pointer__(new t_pair(std::move(a_value)));
	return object;
}

t_scoped t_type_of<t_pair>::f_define(t_container* a_extension)
{
	return t_class::f_instantiate(new t_type_of(a_extension->f_module(), a_extension->f_type<t_object>()));
}

t_type* t_type_of<t_pair>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_pair>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_pair&>(a_this).f_scan(a_scan);
}

void t_type_of<t_pair>::f_finalize(t_object* a_this)
{
	delete &f_as<t_pair&>(a_this);
}

void t_type_of<t_pair>::f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_queue>::f_define(t_container* a_extension)
{
	t_define<t_queue, t_object>(a_extension, L"Queue")
		(f_global()->f_symbol_string(), t_member<std::wstring (t_queue::*)() const, &t_queue::f_string>())
		(L"empty", t_member<bool (t_queue::*)() const, &t_queue::f_empty>())
		(L"push", t_member<void (t_queue::*)(t_container*, t_scoped&&), &t_queue::f_push>())
		(L"pop", t_member<t_scoped (t_queue::*)(), &t_queue::f_pop>())
	;
}

t_type* t_type_of<t_queue>::f_derive(t_object* a_this)
{
	return new t_type_of(t_scoped(v_module), a_this);
}

void t_type_of<t_queue>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_queue&>(a_this).f_scan(a_scan);
}

void t_type_of<t_queue>::f_finalize(t_object* a_this)
{
	delete &f_as<t_queue&>(a_this);
}

t_scoped t_type_of<t_queue>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<>,
		t_construct<const std::wstring&>
	>::t_bind<t_queue>::f_do(a_class, a_stack, a_n);
}

}

t_container::t_container(t_object* a_module) : t_extension(a_module)
{
	v_type_pair = t_type_of<t_pair>::f_define(this);
	t_type_of<t_queue>::f_define(this);
}

void t_container::f_scan(t_scan a_scan)
{
	a_scan(v_type_pair);
	a_scan(v_type_queue);
}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new t_container(a_module);
}
