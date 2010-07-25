#include <xemmai/fiber.h>

#include <cctype>
#include <xemmai/engine.h>
#include <xemmai/convert.h>
#include <xemmai/io/file.h>

namespace xemmai
{

void f_print_with_caret(const std::wstring& a_path, long a_position, size_t a_column)
{
	io::t_file file(a_path, "r");
	std::fseek(file, a_position, SEEK_SET);
	std::putc('\t', stderr);
	while (true) {
		int c = std::getc(file);
		if (c == EOF) break;
		std::putc(c, stderr);
		if (c == '\n') break;
	}
	std::fseek(file, a_position, SEEK_SET);
	std::putc('\t', stderr);
	for (size_t i = 1; i < a_column; ++i) {
		int c = std::getc(file);
		std::putc(std::isspace(c) ? c : ' ', stderr);
	}
	std::putc('^', stderr);
	std::putc('\n', stderr);
}

XEMMAI__PORTABLE__THREAD t_fiber::t_context* t_fiber::t_context::v_instance;

t_fiber::t_context* t_fiber::t_context::f_allocate()
{
	return f_engine()->v_fiber__context__pool.f_allocate(t_engine::V_POOL__ALLOCATION__UNIT);
}

void t_fiber::t_context::f_backtrace()
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_context* p = v_instance;
	v_instance = fiber.v_context = p->v_next;
	if (v_instance->v_native > 0) --fiber.v_native;
	p->v_next = fiber.v_backtrace;
	fiber.v_backtrace = p;
	p->v_native = fiber.v_undone;
	fiber.v_undone = 0;
}

void t_fiber::t_context::f_dump() const
{
	if (v_next) {
		v_next->f_dump();
		std::fputs("from ", stderr);
	} else {
		std::fputs("at ", stderr);
	}
	if (v_native > 0) {
		std::fputs("<native code>\n", stderr);
		std::fputs("from ", stderr);
	}
	if (&*v_code) {
		t_code& code = f_as<t_code&>(v_code);
		std::fprintf(stderr, "%ls", code.v_path.c_str());
		const t_code::t_at* at = code.f_at(v_pc);
		if (at) {
			std::fprintf(stderr, ":%d:%d\n", at->f_line(), at->f_column());
			f_print_with_caret(code.v_path.c_str(), at->f_position(), at->f_column());
		} else {
			std::fputc('\n', stderr);
		}
	} else {
		std::fputs("<fiber>\n", stderr);
	}
}

t_fiber::t_try* t_fiber::t_try::f_allocate()
{
	return f_engine()->v_fiber__try__pool.f_allocate(t_engine::V_POOL__ALLOCATION__UNIT);
}

XEMMAI__PORTABLE__THREAD t_object* t_fiber::v_current;

void t_fiber::f_throw(const t_scoped& a_value)
{
	t_fiber& p = f_as<t_fiber&>(v_current);
	while (true) {
		t_try* q = p.v_try;
		if (!q) {
			while (f_context()->v_native <= 0) t_context::f_backtrace();
			throw a_value;
		}
		while (true) {
			if (f_context()->v_native > 0) throw a_value;
			if (t_context::v_instance == q->v_context) break;
			t_context::f_backtrace();
		}
		t_scope* stack = f_context()->v_stack;
		while (stack->v_top < q->v_stack) stack->f_pop();
		if (q->v_state == t_try::e_state__TRY) {
			stack->f_push(a_value);
			q->v_state = t_try::e_state__CATCH;
			p.v_caught = f_context()->v_pc;
			f_context()->v_pc = q->v_catch;
			break;
		} else if (q->v_state == t_try::e_state__CATCH) {
			stack->f_push(a_value);
			q->v_state = t_try::e_state__THROW;
			p.v_caught = f_context()->v_pc;
			f_context()->v_pc = q->v_finally;
			break;
		} else {
			t_try::f_pop();
		}
	}
}

t_transfer t_fiber::f_instantiate(const t_transfer& a_callable, bool a_main, bool a_active)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_fiber>());
	object.f_pointer__(new t_fiber(a_callable, a_main, a_active));
	return object;
}

void t_fiber::f_define(t_object* a_class)
{
	t_define<t_fiber, t_object>(f_global(), L"Fiber", a_class)
		(L"current", t_static<t_object* (*)(), f_current>())
	;
}

t_fiber::~t_fiber()
{
	t_context::f_finalize(v_backtrace);
	while (v_try) {
		t_try* p = v_try;
		v_try = p->v_next;
		t_local_pool<t_try>::f_free(p);
	}
	t_context::f_finalize(v_context);
}

void t_fiber::f_caught(const t_value& a_object)
{
	if (f_is<t_throwable>(a_object)) {
		portable::t_scoped_lock_for_write lock((*a_object).v_lock);
		t_throwable& p = f_as<t_throwable&>(a_object);
		t_context::f_finalize(p.v_context);
		p.v_context = t_context::f_instantiate(v_backtrace, false, t_transfer(), 0, v_context->v_code, v_caught);
		p.v_context->v_native = v_undone;
	} else {
		t_context::f_finalize(v_backtrace);
	}
	v_backtrace = 0;
	v_undone = 0;
}

t_type* t_type_of<t_fiber>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_fiber>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_fiber& p = f_as<t_fiber&>(a_this);
	a_scan(p.v_callable);
	if (p.v_main || p.v_active) return;
	if (p.v_active) return;
	for (t_fiber::t_context* q = p.v_context; q; q = q->v_next) {
		q->f_scan(a_scan);
		if (p.v_active) return;
	}
}

void t_type_of<t_fiber>::f_finalize(t_object* a_this)
{
	delete &f_as<t_fiber&>(a_this);
}

void t_type_of<t_fiber>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_transfer x = a_stack.f_pop();
	a_stack.f_return(t_fiber::f_instantiate(x));
}

void t_type_of<t_fiber>::f_call(t_object* a_this, const t_value& a_self, size_t a_n, t_stack& a_stack)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_fiber& p = f_as<t_fiber&>(a_this);
	t_fiber& q = f_as<t_fiber&>(t_fiber::v_current);
	t_thread& thread = f_as<t_thread&>(t_thread::v_current);
	if (p.v_main) {
		if (a_this != &*thread.v_fiber) t_throwable::f_throw(L"can not yield to other thread.");
	} else {
		if (t_fiber::v_current != &*thread.v_fiber && (q.v_native > 0 || q.v_context->v_native > 0)) t_throwable::f_throw(L"can not yield beyond native context.");
	}
	{
		portable::t_scoped_lock_for_write lock(a_this->v_lock);
		if (p.v_active) t_throwable::f_throw(L"already active.");
		p.v_active = true;
	}
	t_transfer x = a_stack.f_pop();
	q.v_active = false;
	thread.v_active = a_this;
	t_fiber::v_current = a_this;
	if (p.v_context) {
		t_fiber::t_context::v_instance = p.v_context;
		t_stack* stack = f_context()->v_native > 0 ? &a_stack : f_context()->v_stack;
		stack->f_return(x);
	} else {
		t_transfer scope = t_scope::f_instantiate(0, 0, 0);
		f_as<t_scope&>(scope).f_push(p.v_callable);
		f_as<t_scope&>(scope).f_push(x);
		t_fiber::t_context::f_initiate(scope, f_engine()->v_fiber__instructions);
	}
}

}
