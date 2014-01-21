#include <xemmai/fiber.h>

#include <cctype>
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

thread_local t_fiber::t_context* t_fiber::t_context::v_instance;

void t_fiber::t_context::f_pop()
{
	t_context* p = v_instance;
	v_instance = p->f_next();
	size_t n = f_as<t_code&>(f_as<t_lambda&>(p->v_lambda).v_code).v_privates;
	t_slot* base = p->f_base();
	for (size_t i = 0; i < n; ++i) base[i] = nullptr;
	f_stack()->v_used = p->f_previous();
	if (v_instance->f_native() > 0) --f_as<t_fiber&>(v_current).v_native;
}

void t_fiber::t_context::f_pop(t_slot* a_stack, size_t a_n)
{
	++a_stack;
	t_context* p = v_instance;
	v_instance = p->f_next();
	t_slot* base = p->f_base();
	f_stack()->v_used = std::max(p->f_previous(), base + a_n);
	size_t n = f_as<t_code&>(f_as<t_lambda&>(p->v_lambda).v_code).v_privates;
	base[-1] = nullptr;
	size_t i = 0;
	for (; i < a_n; ++i) base[i] = std::move(a_stack[i]);
	for (; i < n; ++i) base[i] = nullptr;
	if (v_instance->f_native() > 0) --f_as<t_fiber&>(v_current).v_native;
}

void t_fiber::t_context::f_backtrace(const t_value& a_value)
{
	t_fiber& fiber = f_as<t_fiber&>(v_current);
	t_context* p = v_instance;
	v_instance = p->f_next();
	if (v_instance->f_native() > 0) --fiber.v_native;
	if (f_is<t_throwable>(a_value)) t_backtrace::f_push(a_value, fiber.v_undone, p->v_lambda, p->f_pc());
	fiber.v_undone = 0;
}

void t_fiber::t_backtrace::f_push(const t_value& a_throwable, size_t a_native, const t_slot& a_lambda, void** a_pc)
{
	t_with_lock_for_write lock(a_throwable);
	t_throwable& p = f_as<t_throwable&>(a_throwable);
	p.v_backtrace = new t_backtrace(p.v_backtrace, a_native, a_lambda, a_pc);
}

void t_fiber::t_backtrace::f_dump() const
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
	if (!v_lambda || v_lambda == f_engine()->v_lambda_fiber) {
		std::fputs("<fiber>\n", stderr);
	} else {
		t_code& code = f_as<t_code&>(f_as<t_lambda&>(v_lambda).v_code);
		std::fprintf(stderr, "%ls", code.v_path.c_str());
		const t_at* at = code.f_at(f_pc());
		if (at) {
			std::fprintf(stderr, ":%" XEMMAI__PORTABLE__FORMAT_SIZE_T "d:%" XEMMAI__PORTABLE__FORMAT_SIZE_T "d\n", at->f_line(), at->f_column());
			f_print_with_caret(code.v_path.c_str(), at->f_position(), at->f_column());
		} else {
			std::fputc('\n', stderr);
		}
	}
}

thread_local t_object* t_fiber::v_current;

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_object* t_fiber::f_current()
{
	return v_current;
}
#endif

void t_fiber::f_throw(const t_scoped& a_value)
{
	t_fiber& p = f_as<t_fiber&>(v_current);
	while (true) {
		t_try* q = p.v_try;
		if (!q) {
			while (f_context()->f_native() <= 0) t_context::f_backtrace(a_value);
			throw a_value;
		}
		while (true) {
			if (f_context()->f_native() > 0) throw a_value;
			if (t_context::v_instance == q->v_context) break;
			t_context::f_backtrace(a_value);
		}
		if (q->v_state == t_try::e_state__TRY) {
			p.v_stack.f_clear(q->v_stack);
			t_code& code = f_as<t_code&>(f_as<t_lambda&>(f_context()->v_lambda).v_code);
			p.v_stack.v_used = f_context()->f_base() + code.v_size;
			q->v_stack->f_construct(a_value);
			q->v_state = t_try::e_state__CATCH;
			p.v_caught = f_context()->f_pc();
			f_context()->f_pc() = q->v_catch;
			break;
		} else if (q->v_state == t_try::e_state__CATCH) {
			p.v_stack.f_clear(q->v_stack);
			t_code& code = f_as<t_code&>(f_as<t_lambda&>(f_context()->v_lambda).v_code);
			p.v_stack.v_used = f_context()->f_base() + code.v_size;
			q->v_stack->f_construct(a_value);
			q->v_state = t_try::e_state__THROW;
			p.v_caught = f_context()->f_pc();
			f_context()->f_pc() = q->v_finally;
			break;
		} else {
			t_try::f_pop();
		}
	}
}

t_scoped t_fiber::f_instantiate(t_scoped&& a_callable, size_t a_stack, bool a_main, bool a_active)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_fiber>());
	object.f_pointer__(new t_fiber(std::move(a_callable), a_stack, a_main, a_active));
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
	while (v_try) {
		t_try* p = v_try;
		v_try = p->v_next;
		f_engine()->f_free(p);
	}
}

void t_fiber::f_caught(const t_value& a_value)
{
	if (f_is<t_throwable>(a_value)) t_backtrace::f_push(a_value, v_undone, t_context::v_instance->v_lambda, v_caught);
	v_undone = 0;
}

t_type* t_type_of<t_fiber>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_fiber>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_fiber& p = f_as<t_fiber&>(a_this);
	a_scan(p.v_callable);
	if (p.v_main) return;
	t_slot* used = p.v_used;
	{
		t_with_lock_for_read lock(a_this);
		if (p.v_active) return;
	}
	for (t_slot* q = p.v_stack.f_head(); q < used; ++q) a_scan(*q);
}

void t_type_of<t_fiber>::f_finalize(t_object* a_this)
{
	delete &f_as<t_fiber&>(a_this);
}

void t_type_of<t_fiber>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	if (a_n != 1 && a_n != 2) t_throwable::f_throw(L"must be called with 1 or 2 argument(s).");
	t_scoped a0 = std::move(a_stack[1]);
	size_t size = f_engine()->v_stack_size;
	if (a_n == 2) {
		t_scoped a1 = std::move(a_stack[2]);
		f_check<size_t>(a1, L"argument1");
		size = f_as<size_t>(a1);
	}
	a_stack[0].f_construct(t_fiber::f_instantiate(std::move(a0), size));
}

void t_type_of<t_fiber>::f_call(t_object* a_this, t_slot* a_stack, size_t a_n)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_fiber& p = f_as<t_fiber&>(a_this);
	t_fiber& q = f_as<t_fiber&>(t_fiber::v_current);
	t_thread& thread = f_as<t_thread&>(t_thread::v_current);
	if (p.v_main) {
		if (a_this != static_cast<t_object*>(thread.v_fiber)) t_throwable::f_throw(L"can not yield to other thread.");
	} else {
		if (t_fiber::v_current != static_cast<t_object*>(thread.v_fiber) && (q.v_native > 0 || t_fiber::t_context::v_instance->f_native() > 0)) t_throwable::f_throw(L"can not yield beyond native context.");
	}
	{
		t_with_lock_for_write lock(a_this);
		if (p.v_active) t_throwable::f_throw(L"already active.");
		p.v_active = true;
		q.v_active = false;
	}
	t_scoped x = std::move(a_stack[1]);
	q.v_context = t_fiber::t_context::v_instance;
	q.v_used = q.v_stack.v_used;
	q.v_return = a_stack;
	thread.v_active = a_this;
	t_fiber::v_current = a_this;
	if (p.v_context) {
		t_stack::v_instance = &p.v_stack;
		t_fiber::t_context::v_instance = p.v_context;
		p.v_return->f_construct(std::move(x));
	} else {
		t_fiber::t_context::f_initiate(f_engine()->v_lambda_fiber, p.v_callable, std::move(x));
	}
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_fiber::t_context* f_context()
{
	return t_fiber::t_context::v_instance;
}
#endif

}
