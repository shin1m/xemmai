#include "container.h"

namespace xemmai
{

t_transfer t_type_of<t_pair>::f_instantiate(t_container* a_extension, const t_transfer& a_value)
{
	t_transfer object = t_object::f_allocate(a_extension->v_type_pair);
	object.f_pointer__(new t_pair(a_value));
	return object;
}

t_transfer t_type_of<t_pair>::f_define(t_container* a_extension)
{
	return t_class::f_instantiate(new t_type_of(a_extension->f_module(), a_extension->f_type<t_object>()));
}

t_type* t_type_of<t_pair>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_pair>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_pair&>(a_this).f_scan(a_scan);
}

void t_type_of<t_pair>::f_finalize(t_object* a_this)
{
	delete &f_as<t_pair&>(a_this);
}

void t_type_of<t_pair>::f_instantiate(t_object* a_class, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<t_queue>::f_define(t_container* a_extension)
{
	t_define<t_queue, t_object>(a_extension, L"Queue")
		(f_global()->f_symbol_string(), t_member<std::wstring (t_queue::*)() const, &t_queue::f_string>())
		(L"empty", t_member<bool (t_queue::*)() const, &t_queue::f_empty>())
		(L"push", t_member<void (t_queue::*)(t_container*, const t_transfer&), &t_queue::f_push>())
		(L"pop", t_member<t_transfer (t_queue::*)(), &t_queue::f_pop>())
	;
}

t_type* t_type_of<t_queue>::f_derive(t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

void t_type_of<t_queue>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_queue&>(a_this).f_scan(a_scan);
}

void t_type_of<t_queue>::f_finalize(t_object* a_this)
{
	delete &f_as<t_queue&>(a_this);
}

void t_type_of<t_queue>::f_construct(t_object* a_class, size_t a_n)
{
	t_overload<t_construct<t_queue>,
	t_overload<t_construct<t_queue, const std::wstring&>
	> >::f_call(a_class, a_n);
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

#ifdef _MSC_VER
extern "C" __declspec(dllexport) t_extension* f_factory(t_object* a_module)
#else
extern "C" t_extension* f_factory(t_object* a_module)
#endif
{
	return new t_container(a_module);
}
