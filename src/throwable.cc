#include <xemmai/throwable.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_throwable::~t_throwable()
{
	while (v_backtrace) {
		t_backtrace* p = v_backtrace;
		v_backtrace = v_backtrace->v_next;
		delete p;
	}
}

t_scoped t_throwable::f_instantiate(const std::wstring& a_message)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_throwable>());
	object.f_pointer__(new t_throwable(a_message));
	return object;
}

void t_throwable::f_throw(const std::wstring& a_message)
{
	throw f_instantiate(a_message);
}

void t_throwable::f_dump() const
{
	if (v_backtrace) v_backtrace->f_dump();
}

void t_type_of<t_throwable>::f_define()
{
	t_define<t_throwable, t_object>(f_global(), L"Throwable")
		(t_construct<const std::wstring&>())
		(f_global()->f_symbol_string(), t_member<const std::wstring& (t_throwable::*)() const, &t_throwable::f_string>())
		(L"dump", t_member<void (t_throwable::*)() const, &t_throwable::f_dump>())
	;
}

t_type* t_type_of<t_throwable>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

void t_type_of<t_throwable>::f_scan(t_object* a_this, t_scan a_scan)
{
	for (t_backtrace* p = f_as<t_throwable&>(a_this).v_backtrace; p; p = p->v_next) p->f_scan(a_scan);
}

void t_type_of<t_throwable>::f_finalize(t_object* a_this)
{
	delete &f_as<t_throwable&>(a_this);
}

t_scoped t_type_of<t_throwable>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct<const std::wstring&>::t_bind<t_throwable>::f_do(a_class, a_stack, a_n);
}

}
