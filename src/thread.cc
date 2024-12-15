#include <xemmai/convert.h>

namespace xemmai
{

void t_thread::t_internal::f_initialize(size_t a_stack, void* a_bottom)
{
	v_current = this;
	t_slot::t_increments::v_instance = &v_increments;
	t_slot::t_decrements::v_instance = &v_decrements;
	v_active = new t_fiber::t_internal(a_stack, a_bottom);
#ifdef __unix__
	f_stack__(v_active->v_estack_used);
	v_handle = pthread_self();
#endif
#ifdef _WIN32
	t_fiber::v_current = v_active;
	if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &v_handle, 0, FALSE, DUPLICATE_SAME_ACCESS)) throw std::system_error(GetLastError(), std::system_category());
#endif
	v_done = 0;
}

void t_thread::t_internal::f_initialize(t_thread* a_thread)
{
	v_thread = a_thread;
	v_active->v_fiber = &v_thread->v_fiber->f_as<t_fiber>();
	v_active->v_fiber->v_internal = v_active;
}

void t_thread::t_internal::f_epoch()
{
	std::lock_guard lock(v_mutex);
	if (v_done > 0) {
		++v_done;
		for (auto p = v_fibers; p; p = p->v_next) p->f_epoch_copy();
#ifdef _WIN32
		v_context = {};
#endif
	} else {
		f_epoch_suspend();
		for (auto p = v_fibers; p; p = p->v_next) p->f_epoch_copy();
		f_epoch_resume();
	}
	for (auto p = v_fibers; p; p = p->v_next) p->f_epoch_scan();
#ifdef _WIN32
	auto decrements = reinterpret_cast<t_object**>(&v_context);
	{
		auto engine = f_engine();
		std::lock_guard lock(engine->v_object__heap.f_mutex());
		auto p0 = decrements;
		auto p1 = reinterpret_cast<t_object**>(&v_context_last);
		engine->f_epoch_increment(p0, p1, reinterpret_cast<t_object**>(&v_context_last + 1), decrements);
	}
#endif
	v_increments.f_flush();
	for (auto p = &v_fibers; *p;) {
		auto q = *p;
		q->f_epoch_decrement();
		if (q->v_thread) {
			p = &q->v_next;
		} else {
			*p = q->v_next;
			delete q;
		}
	}
#ifdef _WIN32
	for (auto p = reinterpret_cast<t_object**>(&v_context); p != decrements; ++p) (*p)->f_decrement();
#endif
	v_decrements.f_flush();
}

t_object* t_thread::f_instantiate(const t_pvalue& a_callable, size_t a_stack)
{
	return f_engine()->f_fork(a_callable, a_stack);
}

void t_thread::f_join()
{
	if (v_internal == v_current) f_throw(L"current thread can not be joined."sv);
	auto engine = f_engine();
	if (this == &engine->v_thread->f_as<t_thread>()) f_throw(L"engine thread can not be joined."sv);
	t_safe_region region;
	std::unique_lock lock(engine->v_thread__mutex);
	while (v_internal) engine->v_thread__condition.wait(lock);
}

void t_type_of<t_thread>::f_define()
{
	t_define{f_global()}
		(L"current"sv, t_static<t_object*(*)(), t_thread::f_current>())
		(L"join"sv, t_member<void(t_thread::*)(), &t_thread::f_join>())
	.f_derive<t_thread, t_object>();
}

void t_type_of<t_thread>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	if (a_n != 1 && a_n != 2) f_throw(L"must be called with 1 or 2 argument(s)."sv);
	auto size = f_engine()->v_options.v_stack_size;
	if (a_n == 2) {
		f_check<size_t>(a_stack[3], L"argument1");
		size = f_as<size_t>(a_stack[3]);
	}
	a_stack[0] = t_thread::f_instantiate(a_stack[2], size);
}

}
