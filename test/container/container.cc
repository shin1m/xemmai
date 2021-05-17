#include "container.h"

namespace xemmai
{

t_object* t_type_of<t_pair>::f_instantiate(t_container* a_library, const t_pvalue& a_value)
{
	return a_library->v_type_pair->f_new<t_pair>(a_value);
}

t_object* t_type_of<t_pair>::f_define(t_container* a_library)
{
	return a_library->f_type<t_object>()->f_derive<t_type_of>(t_object::f_of(a_library), {});
}

void t_type_of<t_queue>::f_define(t_container* a_library)
{
	t_define<t_queue, t_object>{a_library}
		(f_global()->f_symbol_string(), t_member<t_object*(t_queue::*)() const, &t_queue::f_string>())
		(L"empty"sv, t_member<bool(t_queue::*)() const, &t_queue::f_empty>())
		(L"push"sv, t_member<void(t_queue::*)(t_container*, const t_pvalue&), &t_queue::f_push>())
		(L"pop"sv, t_member<t_pvalue(t_queue::*)(), &t_queue::f_pop>())
	.f_derive();
}

t_pvalue t_type_of<t_queue>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<>,
		t_construct<std::wstring_view>
	>::t_bind<t_queue>::f_do(this, a_stack, a_n);
}

}

void t_container::f_scan(t_scan a_scan)
{
	a_scan(v_type_pair);
	a_scan(v_type_queue);
}

std::vector<std::pair<t_root, t_rvalue>> t_container::f_define()
{
	v_type_pair.f_construct(t_type_of<t_pair>::f_define(this));
	t_type_of<t_queue>::f_define(this);
	return t_export(this)
		(L"Queue"sv, t_object::f_of(v_type_queue))
	;
}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<t_container>(a_handle);
}
