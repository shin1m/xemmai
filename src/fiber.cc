#include <xemmai/convert.h>
#include <xemmai/io/file.h>
#include <cstring>

namespace xemmai
{

void f_print_with_caret(std::FILE* a_out, std::wstring_view a_path, long a_position, size_t a_column)
{
	io::t_file file(a_path, "r");
	std::fseek(file, a_position, SEEK_SET);
	std::putc('\t', a_out);
	while (true) {
		int c = std::getc(file);
		if (c == EOF || c == '\n') break;
		std::putc(c, a_out);
	}
	std::putc('\n', a_out);
	std::fseek(file, a_position, SEEK_SET);
	std::putc('\t', a_out);
	for (size_t i = 1; i < a_column; ++i) {
		int c = std::getc(file);
		std::putc(std::isspace(c) ? c : ' ', a_out);
	}
	std::putc('^', a_out);
	std::putc('\n', a_out);
}

t_fiber::t_internal::t_internal(t_fiber* a_fiber, size_t a_stack, size_t a_n) : v_thread(t_thread::v_current->v_thread), v_fiber(a_fiber), v_estack(std::make_unique<t_pvalue[]>(a_stack)), v_estack_used(v_estack.get()), v_estack_buffer(std::make_unique<t_object*[]>(a_stack * 2)), v_estack_last_head(v_estack_buffer.get() + a_stack), v_estack_last_used(v_estack_last_head)
{
	v_next = v_thread->v_internal->v_fibers;
	v_thread->v_internal->v_fibers = this;
	auto limit = f_limit();
	v_stack_buffer = std::make_unique<char[]>(limit * a_n);
	auto p = v_stack_buffer.get() + limit;
	v_stack_last_top = v_stack_last_bottom = reinterpret_cast<t_object**>(p);
	v_stack_copy = reinterpret_cast<t_object**>(p + limit);
}

t_fiber::t_internal::t_internal(t_fiber* a_fiber, void* a_bottom) : t_internal(a_fiber, a_fiber->v_stack, 2)
{
	v_stack_bottom = reinterpret_cast<t_object**>(a_bottom);
#ifdef _WIN32
	v_handle = ConvertThreadToFiber(NULL);
#endif
}

#ifdef __unix__
t_fiber::t_internal::t_internal(t_fiber* a_fiber, void(*a_f)()) : t_internal(a_fiber, a_fiber->v_stack, 3)
{
	auto n = v_stack_copy - v_stack_last_bottom;
	v_stack_bottom = v_stack_copy + n;
	getcontext(&v_context);
	v_context.uc_link = nullptr;
	v_context.uc_stack.ss_sp = v_stack_copy;
	v_context.uc_stack.ss_size = n * sizeof(t_object*);
	makecontext(&v_context, a_f, 0);
}
#endif
#ifdef _WIN32
t_fiber::t_internal::t_internal(t_fiber* a_fiber, void(*a_f)()) : t_internal(a_fiber, a_fiber->v_stack, 2)
{
	v_handle = CreateFiber(0, f_start, a_f);
}
#endif

void t_fiber::t_internal::f_epoch_copy()
{
	if (!v_thread) {
		v_stack_top = v_stack_bottom;
		v_estack_used = v_estack.get();
	}
	auto n = v_stack_bottom - v_stack_top;
	std::memcpy(v_stack_copy - n, v_stack_top, n * sizeof(t_object*));
	auto m = v_estack_used - v_estack.get();
	for (size_t i = 0; i < m; ++i) v_estack_buffer[i] = v_estack[i];
}

void t_fiber::t_internal::f_epoch_scan()
{
	auto n = v_stack_bottom - v_stack_top;
	auto top0 = v_stack_copy - n;
	auto top1 = v_stack_last_bottom - n;
	auto top2 = v_stack_last_top;
	v_stack_last_top = top1;
	v_stack_decrements = v_stack_last_bottom;
	auto m = v_estack_used - v_estack.get();
	auto used0 = v_estack_buffer.get() + m;
	auto used1 = v_estack_last_head + m;
	auto used2 = v_estack_last_used;
	v_estack_last_used = used1;
	v_estack_decrements = v_estack_buffer.get();
	std::lock_guard lock(f_engine()->v_object__heap.f_mutex());
	if (top1 < top2)
		do {
			auto p = f_engine()->f_object__find(*top0++);
			if (p) p->f_increment();
			*top1++ = p;
		} while (top1 < top2);
	else
		for (; top2 < top1; ++top2) if (*top2) *v_stack_decrements++ = *top2;
	for (; top0 < v_stack_copy; ++top1) {
		auto p = *top0++;
		auto q = *top1;
		if (p == q) continue;
		p = f_engine()->f_object__find(p);
		if (p == q) continue;
		if (p) p->f_increment();
		if (q) *v_stack_decrements++ = q;
		*top1 = p;
	}
	auto p0 = v_estack_buffer.get();
	auto p1 = v_estack_last_head;
	for (auto used = std::min(used1, used2); p1 < used; ++p1) {
		auto p = *p0++;
		auto q = *p1;
		if (p == q) continue;
		p = f_engine()->f_object__find(p);
		if (p == q) continue;
		if (p) p->f_increment();
		if (q) *v_estack_decrements++ = q;
		*p1 = p;
	}
	if (used1 > used2)
		do {
			auto p = f_engine()->f_object__find(*p0++);
			if (p) p->f_increment();
			*p1++ = p;
		} while (p1 < used1);
	else
		for (; p1 < used2; ++p1) if (*p1) *v_estack_decrements++ = *p1;
}

void t_fiber::t_internal::f_epoch_decrement()
{
	for (auto p = v_stack_last_bottom; p != v_stack_decrements; ++p) (*p)->f_decrement();
	for (auto p = v_estack_buffer.get(); p != v_estack_decrements; ++p) (*p)->f_decrement();
}

t_object* t_fiber::f_instantiate(const t_pvalue& a_callable, size_t a_stack)
{
	return f_new<t_fiber>(f_global(), false, a_callable, a_stack);
}

template<typename T_context>
void t_fiber::f_run()
{
	t_thread::v_current->v_mutex.unlock();
	auto used = f_stack() - 1;
	auto x = *used;
	f_stack__(used);
	auto q = t_thread::v_current->v_active;
	auto b = false;
	{
		T_context context;
		try {
			x = q->v_callable(x);
		} catch (const t_rvalue& thrown) {
			q->f_caught(thrown, nullptr);
			b = true;
			x = thrown;
		} catch (...) {
			b = true;
			x = t_throwable::f_instantiate(L"<unexpected>."sv);
		}
		assert(f_stack() == q->v_internal->v_estack.get());
	}
	t_thread::v_current->v_mutex.lock();
	q->v_internal->v_thread = nullptr;
	q->v_internal = nullptr;
	auto& p = f_as<t_fiber&>(t_thread::v_current->v_thread->v_fiber);
	p.v_throw = b;
	*p.v_return = x;
	t_thread::v_current->v_active = &p;
#ifdef __unix__
	f_stack__(p.v_internal->v_estack_used);
	setcontext(&p.v_internal->v_context);
#endif
#ifdef _WIN32
	v_current = p.v_internal;
	SwitchToFiber(p.v_internal->v_handle);
#endif
}

void t_fiber::f_caught(const t_pvalue& a_value, t_object* a_lambda, void** a_pc)
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

void t_type_of<t_fiber>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	if (a_n != 1 && a_n != 2) f_throw(L"must be called with 1 or 2 argument(s)."sv);
	auto size = f_engine()->v_options.v_stack_size;
	if (a_n == 2) {
		f_check<size_t>(a_stack[3], L"argument1");
		size = f_as<size_t>(a_stack[3]);
	}
	a_stack[0] = t_fiber::f_instantiate(a_stack[2], size);
}

size_t t_type_of<t_fiber>::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(L"must be called with an argument."sv);
	t_thread::v_current->v_mutex.lock();
	auto& p = f_as<t_fiber&>(a_this);
	if (&p == t_thread::v_current->v_active) {
		t_thread::v_current->v_mutex.unlock();
		f_throw(L"already active."sv);
	}
	if (p.v_internal) {
		if (p.v_internal->v_thread != t_thread::v_current->v_thread) {
			t_thread::v_current->v_mutex.unlock();
			f_throw(L"can not yield to other thread."sv);
		}
		*p.v_return = a_stack[2];
	} else {
		p.v_internal = new t_fiber::t_internal(&p, f_engine()->v_debugger ? t_fiber::f_run<t_debug_context> : t_fiber::f_run<t_context>);
		p.v_internal->v_estack_used = p.v_internal->v_estack.get() + 1;
		p.v_internal->v_estack[0] = a_stack[2];
	}
	auto q = t_thread::v_current->v_active;
	q->v_internal->f_epoch_get();
	q->v_return = a_stack;
	t_thread::v_current->v_active = &p;
#ifdef __unix__
	f_stack__(p.v_internal->v_estack_used);
	swapcontext(&q->v_internal->v_context, &p.v_internal->v_context);
#endif
#ifdef _WIN32
	t_fiber::v_current = p.v_internal;
	SwitchToFiber(p.v_internal->v_handle);
#endif
	t_thread::v_current->v_mutex.unlock();
	if (!q->v_throw) return -1;
	q->v_throw = false;
	throw t_rvalue(*q->v_return);
}

void t_context::f_backtrace(const t_pvalue& a_value)
{
	if (f_is<t_throwable>(a_value)) {
		auto& p = f_as<t_fiber&>(t_fiber::f_current());
		t_backtrace::f_push(a_value, v_lambda, p.v_caught == nullptr ? v_pc : p.v_caught);
		p.v_caught = nullptr;
	}
	f_stack__(v_previous);
}

const t_pvalue* t_context::f_variable(std::wstring_view a_name) const
{
	auto& lambda = f_as<t_lambda&>(v_lambda);
	auto& code = f_as<t_code&>(lambda.v_code);
	auto i = code.v_variables.find(a_name);
	if (i == code.v_variables.end()) return nullptr;
	size_t outer = 0;
	for (auto i = a_name.begin(); i != a_name.end() && *i == L':'; ++i) ++outer;
	size_t index = i->second.v_index;
	if (outer <= 0) return (i->second.v_shared ? reinterpret_cast<const t_pvalue*>(v_scope) : v_base) + index;
	auto scope = lambda.v_scope_entries;
	for (size_t i = 1; i < outer; ++i) scope = t_scope::f_outer(scope);
	return reinterpret_cast<const t_pvalue*>(scope) + index;
}

#ifdef _WIN32
t_pvalue* f_stack()
{
	return t_fiber::v_current->v_estack_used;
}

void f_stack__(t_pvalue* a_p)
{
	std::atomic_signal_fence(std::memory_order_release);
	t_fiber::v_current->v_estack_used = a_p;
}
#endif

}
