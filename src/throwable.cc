#include <xemmai/convert.h>

namespace xemmai
{

void t_backtrace::f_push(t_object* a_throwable, t_object* a_lambda, void** a_pc)
{
	auto& head = f_as<t_throwable&>(a_throwable).v_backtrace;
	auto p = new t_backtrace(head.load(std::memory_order_relaxed), a_lambda, a_pc);
	while (!head.compare_exchange_weak(p->v_next, p, std::memory_order_release, std::memory_order_relaxed));
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
	for (auto p = v_backtrace.load(std::memory_order_acquire); p;) {
		auto q = p;
		p = q->v_next;
		delete q;
	}
}

t_object* t_throwable::f_instantiate(std::wstring_view a_message)
{
	return f_new<t_throwable>(f_global(), a_message);
}

void t_throwable::f_dump() const
{
	if (auto p = v_backtrace.load(std::memory_order_acquire)) p->f_dump();
}

void t_type_of<t_throwable>::f_define()
{
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<const std::wstring&(t_throwable::*)() const, &t_throwable::f_string>())
		(L"dump"sv, t_member<void(t_throwable::*)() const, &t_throwable::f_dump>())
	.f_derive<t_throwable, t_object>();
}

t_pvalue t_type_of<t_throwable>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<std::wstring_view>::t_bind<t_throwable>::f_do(this, a_stack, a_n);
}

}
