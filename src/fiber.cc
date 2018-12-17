#include <xemmai/fiber.h>

#include <xemmai/convert.h>
#include <xemmai/io/file.h>

namespace xemmai
{

void f_print_with_caret(std::FILE* a_out, std::wstring_view a_path, long a_position, size_t a_column)
{
	io::t_file file(a_path, "r");
	std::fseek(file, a_position, SEEK_SET);
	std::putc('\t', a_out);
	while (true) {
		int c = std::getc(file);
		if (c == EOF) break;
		std::putc(c, a_out);
		if (c == '\n') break;
	}
	std::fseek(file, a_position, SEEK_SET);
	std::putc('\t', a_out);
	for (size_t i = 1; i < a_column; ++i) {
		int c = std::getc(file);
		std::putc(std::isspace(c) ? c : ' ', a_out);
	}
	std::putc('^', a_out);
	std::putc('\n', a_out);
}

t_scoped t_fiber::f_instantiate(t_scoped&& a_callable, size_t a_stack, bool a_main, bool a_active)
{
	return f_new<t_fiber>(f_global(), false, std::move(a_callable), a_stack, a_main, a_active);
}

template<typename T_context>
void t_fiber::f_run()
{
	std::list<t_object*>::iterator i;
	{
		std::unique_lock<std::mutex> lock(f_engine()->v_fiber__mutex);
		i = f_engine()->v_fiber__runnings.insert(f_engine()->v_fiber__runnings.end(), f_current());
	}
	auto& q = f_as<t_fiber&>(f_current());
	t_scoped x = std::move(*--q.v_stack.v_used);
	bool b = false;
	{
		T_context context;
		try {
			x = q.v_callable(std::move(x));
		} catch (const t_scoped& thrown) {
			q.f_caught(thrown, nullptr);
			b = true;
			x = thrown;
		} catch (...) {
			b = true;
			x = t_throwable::f_instantiate(L"<unexpected>."sv);
		}
		assert(q.v_stack.v_used == q.v_stack.f_head());
	}
	{
		std::unique_lock<std::mutex> lock(f_engine()->v_fiber__mutex);
		f_engine()->v_fiber__runnings.erase(i);
	}
	auto& thread = f_as<t_thread&>(t_thread::v_current);
	auto& p = f_as<t_fiber&>(thread.v_fiber);
	p.v_active = true;
	q.v_active = false;
	thread.v_active = thread.v_fiber;
	t_stack::v_instance = &p.v_stack;
	p.v_throw = b;
	p.v_return->f_construct(std::move(x));
	p.v_fiber.f_set();
}

void t_fiber::f_caught(const t_value& a_value, t_object* a_lambda, void** a_pc)
{
	if (f_is<t_throwable>(a_value)) t_backtrace::f_push(a_value, a_lambda, a_pc);
}

void t_type_of<t_fiber>::f_define()
{
	v_builtin = true;
	t_define<t_fiber, t_object>(f_global(), L"Fiber"sv, t_object::f_of(this))
		(L"current"sv, t_static<t_object*(*)(), t_fiber::f_current>())
	;
}

void t_type_of<t_fiber>::f_do_instantiate(t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1 && a_n != 2) f_throw(a_stack, a_n, L"must be called with 1 or 2 argument(s)."sv);
	t_scoped a0 = std::move(a_stack[2]);
	size_t size = f_engine()->v_options.v_stack_size;
	if (a_n == 2) {
		t_destruct<> a1(a_stack[3]);
		f_check<size_t>(a1.v_p, L"argument1");
		size = f_as<size_t>(a1.v_p);
	}
	a_stack[0].f_construct(t_fiber::f_instantiate(std::move(a0), size));
}

size_t t_type_of<t_fiber>::f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(a_stack, a_n, L"must be called with an argument."sv);
	t_scoped x = std::move(a_stack[2]);
	auto& p = f_as<t_fiber&>(a_this);
	auto& thread = f_as<t_thread&>(t_thread::v_current);
	auto& q = f_as<t_fiber&>(thread.v_active);
	if (p.v_main && a_this != static_cast<t_object*>(thread.v_fiber)) f_throw(L"can not yield to other thread."sv);
	{
		t_with_lock_for_write lock(a_this);
		if (p.v_active) f_throw(L"already active."sv);
		p.v_active = true;
	}
	q.v_return = a_stack;
	thread.v_active = a_this;
	t_stacked* head = p.v_stack.f_head();
	if (p.v_stack.v_used == head) {
		p.v_stack.v_used = head + 1;
		head->f_construct(std::move(x));
		if (f_engine()->v_debugger)
			p.v_fiber.f_make(t_fiber::f_run<t_debug_context>);
		else
			p.v_fiber.f_make(t_fiber::f_run<t_context>);
	} else {
		t_stack::v_instance = &p.v_stack;
		p.v_return->f_construct(std::move(x));
	}
	q.v_active = false;
	p.v_fiber.f_swap(q.v_fiber);
	if (!q.v_throw) return -1;
	q.v_throw = false;
	throw t_scoped(std::move(*q.v_return));
}

void t_context::f_backtrace(const t_value& a_value)
{
	f_as<t_code&>(v_lambda->v_code).f_stack_clear(v_pc, v_base);
	if (f_is<t_throwable>(a_value)) {
		auto& p = f_as<t_fiber&>(t_fiber::f_current());
		t_backtrace::f_push(a_value, t_object::f_of(v_lambda), p.v_caught == nullptr ? v_pc : p.v_caught);
		p.v_caught = nullptr;
	}
	f_stack()->v_used = v_previous;
}

const t_value* t_context::f_variable(std::wstring_view a_name) const
{
	auto& code = f_as<t_code&>(v_lambda->v_code);
	auto i = code.v_variables.find(a_name);
	if (i == code.v_variables.end()) return nullptr;
	size_t outer = 0;
	for (auto i = a_name.begin(); i != a_name.end() && *i == L':'; ++i) ++outer;
	size_t index = i->second.v_index;
	if (outer <= 0) return (i->second.v_shared ? v_scope : static_cast<const t_value*>(v_base)) + index;
	auto scope = v_lambda->v_scope_entries;
	for (size_t i = 1; i < outer; ++i) scope = t_scope::f_outer(scope);
	return scope + index;
}

}
