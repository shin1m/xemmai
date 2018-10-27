#include <xemmai/throwable.h>

#include <xemmai/convert.h>

namespace xemmai
{

void t_backtrace::f_push(const t_value& a_throwable, const t_scoped& a_lambda, void** a_pc)
{
	t_with_lock_for_write lock(a_throwable);
	auto& p = f_as<t_throwable&>(a_throwable);
	p.v_backtrace = new t_backtrace(p.v_backtrace, a_lambda, a_pc);
}

void t_backtrace::f_dump() const
{
	if (v_next) {
		v_next->f_dump();
		std::fputs("from ", stderr);
	} else {
		std::fputs("at ", stderr);
	}
	f_engine()->f_context_print(stderr, v_lambda ? f_as<t_lambda*>(v_lambda) : nullptr, f_pc());
}

void f_throw(std::wstring_view a_message)
{
	throw t_throwable::f_instantiate(a_message);
}

t_throwable::~t_throwable()
{
	while (v_backtrace) {
		t_backtrace* p = v_backtrace;
		v_backtrace = v_backtrace->v_next;
		delete p;
	}
}

t_scoped t_throwable::f_instantiate(std::wstring_view a_message)
{
	return f_global()->f_type<t_throwable>()->f_new<t_throwable>(false, a_message);
}

void t_throwable::f_dump() const
{
	if (v_backtrace) v_backtrace->f_dump();
}

void t_type_of<t_throwable>::f_define()
{
	t_define<t_throwable, t_object>(f_global(), L"Throwable"sv)
		(t_construct<false, std::wstring_view>())
		(f_global()->f_symbol_string(), t_member<const std::wstring&(t_throwable::*)() const, &t_throwable::f_string>())
		(f_global()->f_symbol_dump(), t_member<void(t_throwable::*)() const, &t_throwable::f_dump>())
	;
}

void t_type_of<t_throwable>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	for (t_backtrace* p = f_as<t_throwable&>(a_this).v_backtrace; p; p = p->v_next) a_scan(p->v_lambda);
}

t_scoped t_type_of<t_throwable>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<false, std::wstring_view>::t_bind<t_throwable>::f_do(this, a_stack, a_n);
}

}
