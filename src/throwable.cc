#include <xemmai/convert.h>

namespace xemmai
{

void t_backtrace::f_push(t_object* a_throwable, t_object* a_lambda, void** a_pc)
{
	auto& p = f_as<t_throwable&>(a_throwable);
	t_scoped_lock_for_write lock(p.v_lock);
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
	f_engine()->f_context_print(stderr, f_as<t_lambda*>(v_lambda), v_pc);
}

void f_throw(std::wstring_view a_message)
{
	throw t_rvalue(t_throwable::f_instantiate(a_message));
}

t_throwable::~t_throwable()
{
	while (v_backtrace) {
		auto p = v_backtrace;
		v_backtrace = p->v_next;
		delete p;
	}
}

t_object* t_throwable::f_instantiate(std::wstring_view a_message)
{
	return f_new<t_throwable>(f_global(), a_message);
}

void t_throwable::f_dump() const
{
	if (v_backtrace) v_backtrace->f_dump();
}

void t_type_of<t_throwable>::f_define()
{
	t_define<t_throwable, t_object>{f_global()}
		(f_global()->f_symbol_string(), t_member<const std::wstring&(t_throwable::*)() const, &t_throwable::f_string>())
		(f_global()->f_symbol_dump(), t_member<void(t_throwable::*)() const, &t_throwable::f_dump>())
	.f_derive();
}

t_pvalue t_type_of<t_throwable>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<std::wstring_view>::t_bind<t_throwable>::f_do(this, a_stack, a_n);
}

}
