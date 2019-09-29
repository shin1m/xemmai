#include <xemmai/thread.h>

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
		for (auto p = f_engine()->v_thread__internals; p; p = p->v_next) p->v_cache[a_i].v_revision = 0;
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
	auto& p = f_as<t_thread&>(v_current);
	auto internal = p.v_internal;
	t_value::v_collector = internal->v_collector;
	internal->f_initialize();
	if (f_engine()->v_debugger) {
		std::unique_lock<std::mutex> lock(f_engine()->v_thread__mutex);
		f_engine()->f_debug_safe_point(lock);
		internal->v_thread = v_current;
	}
	p.v_active = p.v_fiber;
	t_global::v_instance = f_extension<t_global>(f_engine()->f_module_global());
	auto main = []
	{
		f_as<t_fiber&>(t_fiber::f_current()).v_callable();
	};
	if (f_engine()->v_debugger)
		t_fiber::f_main<t_debug_context>(main);
	else
		t_fiber::f_main<t_context>(main);
	f_cache_clear();
	p.v_active = nullptr;
	f_engine()->f_pools__return();
	{
		std::unique_lock<std::mutex> lock(f_engine()->v_thread__mutex);
		if (f_engine()->v_debugger) {
			if (f_engine()->v_debug__stepping == v_current) f_engine()->v_debug__stepping = nullptr;
			f_engine()->f_debug_safe_point(lock);
		}
		p.v_internal = nullptr;
	}
	t_value::v_decrements->f_push(v_current);
	std::unique_lock<std::mutex> lock(f_engine()->v_thread__mutex);
	++internal->v_done;
	internal->v_cache_hit = v_cache_hit;
	internal->v_cache_missed = v_cache_missed;
	f_engine()->v_thread__condition.notify_all();
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

t_scoped t_thread::f_instantiate(t_scoped&& a_callable, size_t a_stack)
{
	auto fiber = t_fiber::f_instantiate(std::move(a_callable), a_stack, true, true);
	auto internal = new t_internal();
	internal->v_thread = nullptr;
	auto object = f_new<t_thread>(f_global(), true, internal, std::move(fiber));
	{
		std::lock_guard<std::mutex> lock(f_engine()->v_thread__mutex);
		internal->v_next = f_engine()->v_thread__internals;
		f_engine()->v_thread__internals = internal;
	}
	t_value::v_increments->f_push(object);
	f_cache_release();
	try {
		std::thread(f_main, static_cast<t_object*>(object)).detach();
	} catch (std::system_error&) {
		t_value::v_decrements->f_push(object);
		{
			std::lock_guard<std::mutex> lock(f_engine()->v_thread__mutex);
			++internal->v_done;
		}
		f_throw(L"failed to create thread."sv);
	}
	return object;
}

void t_thread::f_join()
{
	if (this == &f_as<t_thread&>(v_current)) f_throw(L"current thread can not be joined."sv);
	if (this == &f_as<t_thread&>(f_engine()->v_thread)) f_throw(L"engine thread can not be joined."sv);
	{
		t_safe_region region;
		std::unique_lock<std::mutex> lock(f_engine()->v_thread__mutex);
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

void t_type_of<t_thread>::f_do_instantiate(t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1 && a_n != 2) f_throw(a_stack, a_n, L"must be called with 1 or 2 argument(s)."sv);
	size_t size = f_engine()->v_options.v_stack_size;
	t_scoped a0 = std::move(a_stack[2]);
	if (a_n == 2) {
		t_destruct<> a1(a_stack[3]);
		f_check<size_t>(a1.v_p, L"argument1");
		size = f_as<size_t>(a1.v_p);
	}
	a_stack[0].f_construct(t_thread::f_instantiate(std::move(a0), size));
}

}
