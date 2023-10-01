#ifndef XEMMAI__ENGINE_H
#define XEMMAI__ENGINE_H

#include "module.h"
#include "thread.h"
#include <condition_variable>
#include <shared_mutex>
#include <csignal>
#ifdef __unix__
#include <unistd.h>
#include <semaphore.h>
#endif

namespace xemmai
{

struct t_debugger
{
	virtual void f_stopped(t_thread* a_thread) = 0;
	virtual void f_loaded(t_debug_script& a_thread) = 0;
};

class t_engine
{
	friend class t_object;
	friend struct t_module;
	friend struct t_library;
	friend struct t_fiber;
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend struct t_type_of<t_thread>;
	friend class t_symbol;
	friend struct t_code;
	friend struct t_safe_region;
	friend XEMMAI__PUBLIC t_engine* f_engine();

public:
	struct t_options
	{
#ifdef NDEBUG
		size_t v_collector__threshold = 1024 * 16;
#else
		size_t v_collector__threshold = 64;
#endif
		size_t v_stack_size = 1 << 10;
		bool v_verbose = false;
	};

private:
	static inline XEMMAI__PORTABLE__THREAD t_engine* v_instance;

	std::atomic_flag v_collector__running;
	bool v_collector__quitting = false;
	std::atomic_size_t v_collector__tick;
	std::atomic_size_t v_collector__wait;
	size_t v_collector__epoch = 0;
	size_t v_collector__collect = 0;
	t_object* v_cycles = nullptr;
	t_heap<t_object> v_object__heap;
	size_t v_object__lower = 0;
	bool v_object__reviving = false;
	std::mutex v_object__reviving__mutex;
	size_t v_object__release = 0;
	size_t v_object__collect = 0;
#ifdef __unix__
	sem_t v_epoch__received;
	sigset_t v_epoch__not_signal_resume;
	struct sigaction v_epoch__old_signal_suspend;
	struct sigaction v_epoch__old_signal_resume;
#endif
	t_thread::t_internal* v_thread__internals = new t_thread::t_internal{nullptr};
	std::mutex v_thread__mutex;
	std::condition_variable v_thread__condition;
	t_type* v_type_type;
	std::map<std::wstring, t_slot, std::less<>> v_module__instances;
	std::mutex v_module__mutex;
	std::recursive_mutex v_module__instantiate__mutex;
	t_library::t_handle* v_library__handle__finalizing = nullptr;
	std::map<std::wstring, t_slot, std::less<>> v_symbol__instances;
	std::mutex v_symbol__instantiate__mutex;
	t_root v_module_global;
	t_root v_module_system;
	t_root v_module_io;
	t_root v_fiber_exit;
	t_root v_thread;
	t_options v_options;
	t_debugger* v_debugger = nullptr;
	bool v_debug__stopping = false;
	size_t v_debug__safe = 0;
	t_thread* v_debug__stepping = nullptr;

	void f_free(t_object* a_p)
	{
		a_p->v_count = 1;
		v_object__heap.f_free(a_p);
	}
	void f_free_as_release(t_object* a_p)
	{
		++v_object__release;
		f_free(a_p);
	}
	void f_free_as_collect(t_object* a_p)
	{
		++v_object__collect;
		f_free(a_p);
	}
	t_object* f_object__find(void* a_p)
	{
		if (reinterpret_cast<uintptr_t>(a_p) & t_heap<t_object>::V_UNIT - 1) return nullptr;
		auto p = v_object__heap.f_find(a_p);
		return p && p->v_type ? p : nullptr;
	}
#ifdef __unix__
	void f_epoch_suspend()
	{
		if (sem_post(&v_epoch__received) == -1) _exit(errno);
		auto e = errno;
		sigsuspend(&v_epoch__not_signal_resume);
		errno = e;
	}
	void f_epoch_wait()
	{
		while (sem_wait(&v_epoch__received) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
	}
#endif
	void f_epoch_increment(t_object**& a_p0, t_object**& a_p1, t_object** a_q1, t_object**& a_decrements)
	{
		for (; a_p1 < a_q1; ++a_p1) {
			auto p = *a_p0++;
			auto q = *a_p1;
			if (p == q) continue;
			p = f_object__find(p);
			if (p == q) continue;
			if (p) p->f_increment();
			if (q) *a_decrements++ = q;
			*a_p1 = p;
		}
	}
	XEMMAI__LOCAL void f_collector();
	XEMMAI__LOCAL void f_debug_stop_and_wait(std::unique_lock<std::mutex>& a_lock);
	void f_debug_enter_and_notify()
	{
		++v_debug__safe;
		v_thread__condition.notify_all();
	}
	void f_debug_wait_and_leave(std::unique_lock<std::mutex>& a_lock)
	{
		do v_thread__condition.wait(a_lock); while (v_debug__stopping);
		assert(v_debug__safe > 0);
		--v_debug__safe;
	}
	void f_debug_enter_leave(std::unique_lock<std::mutex>& a_lock)
	{
		f_debug_enter_and_notify();
		f_debug_wait_and_leave(a_lock);
	}
	XEMMAI__LOCAL void f_debug_safe_point(std::unique_lock<std::mutex>& a_lock);
	void f_debug_break_point(std::unique_lock<std::mutex>& a_lock, auto a_do);
	XEMMAI__LOCAL void f_debug_break_point(std::unique_lock<std::mutex>& a_lock);
	XEMMAI__LOCAL void f_debug_script_loaded(t_debug_script& a_debug);
	XEMMAI__LOCAL void f_debug_safe_region_leave(std::unique_lock<std::mutex>& a_lock);
	XEMMAI__LOCAL void f_finalize(t_thread::t_internal* a_thread);

public:
	t_engine(const t_options& a_options, size_t a_count, char** a_arguments);
	~t_engine();
	template<typename T>
	t_object* f_allocate_for_type(size_t a_fields)
	{
		return f_allocate(t_object::f_align_for_fields(sizeof(T)) + (sizeof(std::pair<t_slot, t_svalue>) + sizeof(std::pair<t_object*, size_t>)) * a_fields);
	}
	template<typename T>
	t_object* f_new_type_on_boot(size_t a_fields, t_type* a_super, t_object* a_module)
	{
		auto p = f_allocate_for_type<t_type_of<T>>(a_fields);
		std::uninitialized_default_construct_n((new(p->f_data()) t_type_of<T>(t_type_of<T>::V_ids, a_super, a_module, t_type_of<T>::V_native, 0, std::vector<std::pair<t_root, t_rvalue>>{}, std::map<t_object*, size_t>{}))->f_fields(), a_fields);
		p->f_be(v_type_type);
		return p;
	}
#ifdef _WIN32
	XEMMAI__PUBLIC t_object* f_allocate(size_t a_size);
	t_object* f__allocate(size_t a_size)
#else
	XEMMAI__PORTABLE__ALWAYS_INLINE t_object* f_allocate(size_t a_size)
#endif
	{
		auto p = v_object__heap.f_allocate(sizeof(t_object) - sizeof(t_object::v_data) + a_size);
		p->v_next = nullptr;
		return p;
	}
	void f_tick()
	{
		if (v_collector__running.test(std::memory_order_relaxed)) return;
		v_collector__tick.fetch_add(1, std::memory_order_relaxed);
		if (!v_collector__running.test_and_set(std::memory_order_release)) v_collector__running.notify_one();
	}
	void f_wait()
	{
		v_collector__wait.fetch_add(1, std::memory_order_relaxed);
		if (!v_collector__running.test_and_set(std::memory_order_release)) v_collector__running.notify_one();
		v_collector__running.wait(true, std::memory_order_relaxed);
	}
	t_object* f_module_global() const
	{
		return v_module_global;
	}
	t_object* f_module_system() const
	{
		return v_module_system;
	}
	t_object* f_module_io() const
	{
		return v_module_io;
	}
	XEMMAI__LOCAL t_object* f_fork(const t_pvalue& a_callable, size_t a_stack);
	XEMMAI__LOCAL intptr_t f_run(t_debugger* a_debugger);
	void f_threads(auto a_callback)
	{
		for (auto p = v_thread__internals; p; p = p->v_next) if (p->v_done == 0) a_callback(p->v_thread);
	}
	const std::map<std::wstring, t_slot, std::less<>>& f_modules() const
	{
		return v_module__instances;
	}
	XEMMAI__LOCAL void f_context_print(std::FILE* a_out, t_lambda* a_lambda, void** a_pc);
	XEMMAI__LOCAL void f_debug_safe_point();
	XEMMAI__LOCAL void f_debug_break_point();
	XEMMAI__PUBLIC void f_debug_safe_region_enter();
	XEMMAI__PUBLIC void f_debug_safe_region_leave();
	XEMMAI__LOCAL void f_debug_stop();
	t_thread* f_debug_stepping() const
	{
		return v_debug__stepping;
	}
	XEMMAI__LOCAL void f_debug_continue(t_thread* a_stepping = nullptr);
};

void t_engine::f_debug_break_point(std::unique_lock<std::mutex>& a_lock, auto a_do)
{
	while (v_debug__stopping) f_debug_enter_leave(a_lock);
	++v_debug__safe;
	f_debug_stop_and_wait(a_lock);
	a_do();
	f_debug_wait_and_leave(a_lock);
}

#ifndef _WIN32
inline t_engine* f_engine()
{
	return t_engine::v_instance;
}
#endif

struct t_safe_region
{
	t_safe_region()
	{
		if (f_engine()->v_debugger) f_engine()->f_debug_safe_region_enter();
	}
	~t_safe_region()
	{
		if (f_engine()->v_debugger) f_engine()->f_debug_safe_region_leave();
	}
};

template<typename T>
struct t_lock_with_safe_region : std::lock_guard<T>
{
	static T& f_lock(T& a_mutex)
	{
		t_safe_region region;
		a_mutex.lock();
		return a_mutex;
	}

	t_lock_with_safe_region(T& a_mutex) : std::lock_guard<T>(f_lock(a_mutex), std::adopt_lock)
	{
	}
};

template<typename T>
struct t_shared_lock_with_safe_region : std::shared_lock<T>
{
	t_shared_lock_with_safe_region(T& a_mutex) : std::shared_lock<T>(a_mutex, std::defer_lock)
	{
		t_safe_region region;
		this->lock();
	}
};

template<size_t A_SIZE>
void t_slot::t_queue<A_SIZE>::f_next() noexcept
{
	f_engine()->f_tick();
	if (v_head < v_objects + V_SIZE - 1) {
		++v_head;
		while (v_tail == v_head) f_engine()->f_wait();
		auto tail = v_tail;
		v_next = std::min(tail < v_head ? v_objects + V_SIZE - 1 : tail - 1, v_head + V_SIZE / 8);
	} else {
		v_head = v_objects;
		while (v_tail == v_head) f_engine()->f_wait();
		v_next = std::min(v_tail - 1, v_head + V_SIZE / 8);
	}
}

inline void t_object::f_decrement_step()
{
	f_scan_fields(f_push<&t_object::f_decrement_push>);
	if (v_type->f_finalize) v_type->f_finalize(this, f_push<&t_object::f_decrement_push>);
	t_object::f_of(v_type)->f_decrement_push();
	v_type = nullptr;
	if (v_next) {
		if (!v_previous) return;
		v_next->v_previous = v_previous;
		v_previous->v_next = v_next;
	}
	f_engine()->f_free_as_release(this);
}

template<typename T>
t_object* t_type::f_derive(t_object* a_module, const t_fields& a_fields)
{
	auto [fields, key2index] = f_merge(a_fields);
	auto p = f_engine()->f_allocate_for_type<T>(fields.size());
	new(p->f_data()) T(T::V_ids, this, a_module, T::V_native, v_instance_fields + a_fields.v_instance.size(), fields, key2index);
	p->f_be(t_object::f_of(this)->v_type);
	return p;
}

inline void t_thread::t_internal::f_epoch_suspend()
{
#ifdef __unix__
	pthread_kill(v_handle, XEMMAI__SIGNAL_SUSPEND);
	f_engine()->f_epoch_wait();
#endif
#ifdef _WIN32
	SuspendThread(v_handle);
	v_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
	GetThreadContext(v_handle, &v_context);
	auto sp = reinterpret_cast<t_object**>(v_context.Rsp);
	MEMORY_BASIC_INFORMATION mbi;
	for (auto p = sp;;) {
		VirtualQuery(p, &mbi, sizeof(mbi));
		p = reinterpret_cast<t_object**>(static_cast<char*>(mbi.BaseAddress) + mbi.RegionSize);
		if (mbi.Protect & PAGE_GUARD) {
			sp = p;
			break;
		}
		if (p >= v_active->v_stack_bottom) break;
	}
	v_active->v_stack_top = sp;
	v_increments.v_epoch.store(v_increments.v_head, std::memory_order_relaxed);
	v_decrements.v_epoch.store(v_decrements.v_head, std::memory_order_relaxed);
#endif
}

inline void t_thread::t_internal::f_epoch_resume()
{
#ifdef __unix__
	pthread_kill(v_handle, XEMMAI__SIGNAL_RESUME);
#endif
#ifdef _WIN32
	ResumeThread(v_handle);
#endif
}

inline t_object* f_new_value(t_type* a_type, auto&&... a_xs)
{
	assert(a_type->v_fields_offset == t_object::f_fields_offset(0));
	assert(a_type->v_instance_fields == sizeof...(a_xs));
	auto p = f_engine()->f_allocate(sizeof(t_svalue) * sizeof...(a_xs));
	f__construct(p->f_fields(0), std::forward<decltype(a_xs)>(a_xs)...);
	p->f_be(a_type);
	return p;
}

}

#endif
