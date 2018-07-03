#include <xemmai/native.h>

#include <xemmai/global.h>

namespace xemmai
{

t_scoped t_native::f_instantiate(t_extension::t_function a_function, t_extension* a_extension)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_native>());
	object.f_pointer__(new t_native(a_function, a_extension));
	return object;
}

constexpr decltype(t_type_of<t_native>::V_ids) t_type_of<t_native>::V_ids;

t_type* t_type_of<t_native>::f_derive()
{
	return nullptr;
}

void t_type_of<t_native>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_native&>(a_this).v_module);
}

void t_type_of<t_native>::f_finalize(t_object* a_this)
{
	delete &f_as<t_native&>(a_this);
}

void t_type_of<t_native>::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(a_stack, a_n, L"uninstantiatable.");
}

size_t t_type_of<t_native>::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	auto& p = f_as<t_native&>(a_this);
	p.v_function(p.v_extension, a_stack, a_n);
	return -1;
}

size_t t_type_of<t_native>::f_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(t_method::f_instantiate(a_this, std::move(a0)));
	return -1;
}

}
