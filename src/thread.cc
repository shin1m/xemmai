#include <xemmai/thread.h>

#include <xemmai/module.h>
#include <xemmai/convert.h>

namespace xemmai
{

volatile size_t t_thread::t_cache::v_revisions[V_SIZE];

size_t t_thread::t_cache::f_revise(size_t a_i)
{
	size_t revision = f_atomic_increment(v_revisions[a_i]);
	if (revision != 0) return revision;
	{
		std::lock_guard<std::mutex> lock(f_engine()->v_thread__mutex);
		t_internal* internals = f_engine()->v_thread__internals;
		t_internal* p = internals;
		do {
			p = p->v_next;
			p->v_cache[a_i].v_revision = 0;
		} while (p != internals);
	}
	return v_revisions[a_i] = 1;
}

XEMMAI__PORTABLE__THREAD t_object* t_thread::v_current;
XEMMAI__PORTABLE__THREAD t_thread::t_cache* t_thread::v_cache;
XEMMAI__PORTABLE__THREAD size_t t_thread::v_cache_hit;
XEMMAI__PORTABLE__THREAD size_t t_thread::v_cache_missed;

void t_thread::f_main(t_object* a_p)
{
	v_current = a_p;
	t_thread& p = f_as<t_thread&>(v_current);
	t_thread::t_internal* internal = p.v_internal;
	t_value::v_collector = internal->v_collector;
	internal->f_initialize();
	p.v_active = p.v_fiber;
	t_fiber::v_current = p.v_active;
	t_global::v_instance = f_extension<t_global>(f_engine()->f_module_global());
	t_fiber::f_main([]{
		f_as<t_fiber&>(t_fiber::v_current).v_callable();
	});
	f_cache_clear();
	p.v_active = 0;
	p.v_internal = 0;
	t_value::v_decrements->f_push(v_current);
	f_engine()->f_pools__return();
	{
		std::lock_guard<std::mutex> lock(f_engine()->v_thread__mutex);
		++internal->v_done;
		internal->v_cache_hit = v_cache_hit;
		internal->v_cache_missed = v_cache_missed;
		f_engine()->v_thread__condition.notify_all();
	}
}

void t_thread::f_cache_clear()
{
	for (size_t i = 0; i < t_structure::t_cache::V_SIZE; ++i) {
		t_structure::t_cache& cache = t_structure::v_cache[i];
		cache.v_structure = cache.v_key = 0;
	}
	for (size_t i = 0; i < t_cache::V_SIZE; ++i) {
		t_cache& cache = v_cache[i];
		cache.v_object = cache.v_key = cache.v_value = 0;
	}
}

void t_thread::f_cache_acquire()
{
	for (size_t i = 0; i < t_cache::V_SIZE; ++i) {
		t_cache& cache = v_cache[i];
		size_t revision = t_cache::v_revisions[i];
		if (cache.v_revision == revision) continue;
		cache.v_revision = revision;
		cache.v_object = cache.v_key = cache.v_value = 0;
	}
}

t_transfer t_thread::f_instantiate(const t_transfer& a_callable, size_t a_stack)
{
	t_transfer fiber = t_fiber::f_instantiate(a_callable, a_stack, true, true);
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_thread>());
	t_thread* p = new t_thread(fiber);
	object.f_pointer__(p);
	t_internal* internal = p->v_internal;
	{
		std::lock_guard<std::mutex> lock(f_engine()->v_thread__mutex);
		t_internal*& internals = f_engine()->v_thread__internals;
		internal->v_next = internals->v_next;
		internals = internals->v_next = internal;
	}
	t_value::v_increments->f_push(static_cast<t_object*>(object));
	f_cache_release();
	try {
		std::thread(f_main, static_cast<t_object*>(object)).detach();
	} catch (std::system_error&) {
		p->v_internal = 0;
		t_value::v_decrements->f_push(static_cast<t_object*>(object));
		std::lock_guard<std::mutex> lock(f_engine()->v_thread__mutex);
		++internal->v_done;
	}
	return object;
}

void t_thread::f_define(t_object* a_class)
{
	t_define<t_thread, t_object>(f_global(), L"Thread", a_class)
		(L"current", t_static<t_object* (*)(), f_current>())
		(L"join", t_member<void (t_thread::*)(), &t_thread::f_join>())
	;
}

void t_thread::f_join()
{
	if (this == &f_as<t_thread&>(v_current)) t_throwable::f_throw(L"current thread can not be joined.");
	if (this == &f_as<t_thread&>(f_engine()->v_thread)) t_throwable::f_throw(L"engine thread can not be joined.");
	{
		std::unique_lock<std::mutex> lock(f_engine()->v_thread__mutex);
		while (v_internal) f_engine()->v_thread__condition.wait(lock);
	}
	f_cache_acquire();
}

t_type* t_type_of<t_thread>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_thread>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_thread&>(a_this).v_fiber);
}

void t_type_of<t_thread>::f_finalize(t_object* a_this)
{
	delete &f_as<t_thread&>(a_this);
}

void t_type_of<t_thread>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	if (a_n != 1 && a_n != 2) t_throwable::f_throw(L"must be called with 1 or 2 argument(s).");
	size_t size = f_engine()->v_stack_size;
	t_transfer a0 = a_stack[1].f_transfer();
	if (a_n == 2) {
		t_transfer a1 = a_stack[2].f_transfer();
		f_check<size_t>(a1, L"argument1");
		size = f_as<size_t>(a1);
	}
	a_stack[0].f_construct(t_thread::f_instantiate(a0, size));
}

}
