#include "container.h"

namespace xemmai
{

t_scoped t_type_of<t_pair>::f_instantiate(t_container* a_extension, t_scoped&& a_value)
{
	return a_extension->v_type_pair->f_new<t_pair>(false, std::move(a_value));
}

t_scoped t_type_of<t_pair>::f_define(t_container* a_extension)
{
	auto super = a_extension->f_type<t_object>();
	return t_object::f_of(super)->f_type()->f_new<t_type_of>(true, V_ids, super, a_extension->f_module());
}

void t_type_of<t_queue>::f_define(t_container* a_extension)
{
	t_define<t_queue, t_object>(a_extension, L"Queue"sv)
		(f_global()->f_symbol_string(), t_member<t_scoped(t_queue::*)() const, &t_queue::f_string>())
		(L"empty"sv, t_member<bool(t_queue::*)() const, &t_queue::f_empty>())
		(L"push"sv, t_member<void(t_queue::*)(t_container*, t_scoped&&), &t_queue::f_push>())
		(L"pop"sv, t_member<t_scoped(t_queue::*)(), &t_queue::f_pop>())
	;
}

t_scoped t_type_of<t_queue>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<false>,
		t_construct<false, std::wstring_view>
	>::t_bind<t_queue>::f_do(this, a_stack, a_n);
}

}

t_container::t_container(t_object* a_module) : t_extension(a_module)
{
	v_type_pair.f_construct(t_type_of<t_pair>::f_define(this));
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
