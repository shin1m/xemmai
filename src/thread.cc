#include <xemmai/convert.h>

namespace xemmai
{

size_t t_thread::t_cache::f_revise(size_t a_i)
{
	size_t revision = f_atomic_increment(v_revisions[a_i]);
	if (revision != 0) return revision;
	{
		std::lock_guard lock(f_engine()->v_thread__mutex);
		for (auto p = f_engine()->v_thread__internals; p; p = p->v_next) p->v_cache[a_i].v_revision = 0;
	}
	return v_revisions[a_i] = 1;
}

void t_thread::t_internal::f_initialize()
{
	t_slot::t_increments::v_instance = &v_increments;
	v_increments.v_head = v_increments.v_objects;
	t_slot::t_increments::v_next = v_increments.v_objects + t_slot::t_increments::V_SIZE / 8;
	t_slot::t_decrements::v_instance = &v_decrements;
	v_decrements.v_head = v_decrements.v_objects;
	t_slot::t_decrements::v_next = v_decrements.v_objects + t_slot::t_decrements::V_SIZE / 8;
}

void t_thread::t_internal::f_initialize(t_thread* a_thread, void* a_bottom)
{
	v_thread = a_thread;
	v_current = v_thread->v_internal;
	v_active = &f_as<t_fiber&>(v_thread->v_fiber);
	v_active->v_internal = new t_fiber::t_internal(v_active, a_bottom);
#ifdef __unix__
	f_stack__(v_active->v_internal->v_estack_used);
	v_handle = pthread_self();
#endif
#ifdef _WIN32
	t_fiber::v_current = v_active->v_internal;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &v_handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
#endif
	t_structure::v_cache = v_index_cache;
	t_thread::v_cache = v_cache;
	v_done = 0;
}

void t_thread::t_internal::f_epoch()
{
	std::lock_guard lock(v_mutex);
	if (v_done > 0) {
		++v_done;
		for (auto p = v_fibers; p; p = p->v_next) p->f_epoch_copy();
	} else {
		f_epoch_suspend();
		for (auto p = v_fibers; p; p = p->v_next) p->f_epoch_copy();
		f_epoch_resume();
	}
	for (auto p = v_fibers; p; p = p->v_next) p->f_epoch_scan();
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
	v_decrements.f_flush();
}

void t_thread::f_cache_clear()
{
	for (size_t i = 0; i < t_structure::t_cache::V_SIZE; ++i) {
		auto& cache = t_structure::v_cache[i];
		cache.v_structure = cache.v_key = nullptr;
	}
	for (size_t i = 0; i < t_cache::V_SIZE; ++i) {
		auto& cache = v_cache[i];
		cache.v_object = cache.v_key = cache.v_value = nullptr;
	}
}

void t_thread::f_cache_acquire()
{
	for (size_t i = 0; i < t_cache::V_SIZE; ++i) {
		auto& cache = v_cache[i];
		size_t revision = t_cache::v_revisions[i];
		if (cache.v_revision == revision) continue;
		cache.v_revision = revision;
		cache.v_object = cache.v_key = cache.v_value = nullptr;
	}
}

t_object* t_thread::f_instantiate(const t_pvalue& a_callable, size_t a_stack)
{
	return f_engine()->f_fork(a_callable, a_stack);
}

void t_thread::f_join()
{
	if (v_internal == v_current) f_throw(L"current thread can not be joined."sv);
	if (this == &f_as<t_thread&>(f_engine()->v_thread)) f_throw(L"engine thread can not be joined."sv);
	{
		t_safe_region region;
		std::unique_lock lock(f_engine()->v_thread__mutex);
		while (v_internal) f_engine()->v_thread__condition.wait(lock);
	}
	f_cache_acquire();
}

void t_type_of<t_thread>::f_define()
{
	v_builtin = true;
	t_define<t_thread, t_object>(f_global(), L"Thread"sv, t_object::f_of(this))
		(L"current"sv, t_static<t_object*(*)(), t_thread::f_current>())
		(L"join"sv, t_member<void(t_thread::*)(), &t_thread::f_join>())
	;
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
