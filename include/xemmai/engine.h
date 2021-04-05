#ifndef XEMMAI__ENGINE_H
#define XEMMAI__ENGINE_H

#include "module.h"
#include "thread.h"
#include <condition_variable>
#include <csignal>
#ifdef __unix__
#include <unistd.h>
#include <semaphore.h>
#endif

namespace xemmai
{

class t_symbol;
class t_global;
struct t_safe_region;

struct t_debugger
{
	virtual void f_stopped(t_thread* a_thread) = 0;
	virtual void f_loaded(t_thread* a_thread) = 0;
};

class t_engine
{
	friend class t_object;
	friend class t_structure;
	friend struct t_module;
	friend struct t_library;
	friend struct t_fiber;
	friend struct t_type_of<t_fiber>;
	friend struct t_thread;
	friend struct t_type_of<t_thread>;
	friend class t_symbol;
	friend struct t_code;
	friend struct t_safe_region;
	friend XEMMAI__PORTABLE__EXPORT t_engine* f_engine();

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

	bool v_collector__running = true;
	bool v_collector__quitting = false;
	std::mutex v_collector__mutex;
	std::condition_variable v_collector__wake;
	std::condition_variable v_collector__done;
	size_t v_collector__tick = 0;
	size_t v_collector__wait = 0;
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
	sigset_t v_epoch__notsigusr2;
	struct sigaction v_epoch__old_sigusr1;
	struct sigaction v_epoch__old_sigusr2;
#endif
	t_thread::t_internal* v_thread__internals = new t_thread::t_internal{nullptr};
	size_t v_thread__cache_hit = 0;
	size_t v_thread__cache_missed = 0;
	std::mutex v_thread__mutex;
	std::condition_variable v_thread__condition;
	t_type* v_type_class;
	t_type* v_type_structure;
	std::map<std::wstring, t_slot, std::less<>> v_module__instances;
	std::mutex v_module__mutex;
	std::condition_variable v_module__condition;
	t_object* v_module__thread = nullptr;
	t_library::t_handle* v_library__handle__finalizing = nullptr;
	std::map<std::wstring, t_slot, std::less<>> v_symbol__instances;
	std::mutex v_symbol__instantiate__mutex;
	t_structure* v_structure_root;
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

	t_object* f_allocate_on_boot(size_t a_size)
	{
		auto p = v_object__heap.f_allocate(sizeof(t_object) - sizeof(t_object::v_data) + a_size);
		p->v_next = nullptr;
		p->v_structure = v_structure_root;
		return p;
	}
	void f_free(t_object* a_p)
	{
		a_p->v_owner = nullptr;
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
		if (reinterpret_cast<uintptr_t>(a_p) & 127) return nullptr;
		auto p = v_object__heap.f_find(a_p);
		return p && p->v_type ? p : nullptr;
	}
#ifdef __unix__
	void f_epoch_suspend()
	{
		if (sem_post(&v_epoch__received) == -1) _exit(errno);
		sigsuspend(&v_epoch__notsigusr2);
		if (sem_post(&v_epoch__received) == -1) _exit(errno);
	}
	void f_epoch_send(pthread_t a_thread, int a_signal)
	{
		pthread_kill(a_thread, a_signal);
		while (sem_wait(&v_epoch__received) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
	}
#endif
	void f_collector();
	void f_debug_stop_and_wait(std::unique_lock<std::mutex>& a_lock);
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
	void f_debug_safe_point(std::unique_lock<std::mutex>& a_lock);
	template<typename T>
	void f_debug_break_point(std::unique_lock<std::mutex>& a_lock, T a_member);
	void f_debug_break_point(std::unique_lock<std::mutex>& a_lock);
	void f_debug_script_loaded();
	void f_debug_safe_region_leave(std::unique_lock<std::mutex>& a_lock);

public:
	t_engine(const t_options& a_options, size_t a_count, char** a_arguments);
	~t_engine();
#ifdef _WIN32
	XEMMAI__PORTABLE__EXPORT t_object* f_allocate(bool a_shared, size_t a_size);
	t_object* f__allocate(bool a_shared, size_t a_size)
#else
	XEMMAI__PORTABLE__ALWAYS_INLINE t_object* f_allocate(bool a_shared, size_t a_size)
#endif
	{
		auto p = v_object__heap.f_allocate(sizeof(t_object) - sizeof(t_object::v_data) + a_size);
		p->v_next = nullptr;
		if (!a_shared) p->v_owner = t_slot::t_increments::v_instance;
		t_slot::t_increments::f_push(t_object::f_of(v_structure_root));
		p->v_structure = v_structure_root;
		return p;
	}
	void f_tick()
	{
		if (v_collector__running) return;
		std::lock_guard lock(v_collector__mutex);
		++v_collector__tick;
		if (v_collector__running) return;
		v_collector__running = true;
		v_collector__wake.notify_one();
	}
	void f_wait()
	{
		std::unique_lock lock(v_collector__mutex);
		++v_collector__wait;
		if (!v_collector__running) {
			v_collector__running = true;
			v_collector__wake.notify_one();
		}
		v_collector__done.wait(lock);
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
	t_object* f_fork(const t_pvalue& a_callable, size_t a_stack);
	intptr_t f_run(t_debugger* a_debugger);
	template<typename T>
	void f_threads(T a_callback)
	{
		for (auto p = v_thread__internals; p; p = p->v_next) if (p->v_done == 0) a_callback(p->v_thread);
	}
	const std::map<std::wstring, t_slot, std::less<>>& f_modules() const
	{
		return v_module__instances;
	}
	void f_context_print(std::FILE* a_out, t_lambda* a_lambda, void** a_pc);
	void f_debug_safe_point();
	void f_debug_break_point();
	XEMMAI__PORTABLE__EXPORT void f_debug_safe_region_enter();
	XEMMAI__PORTABLE__EXPORT void f_debug_safe_region_leave();
	void f_debug_stop();
	t_thread* f_debug_stepping() const
	{
		return v_debug__stepping;
	}
	void f_debug_continue(t_thread* a_stepping = nullptr);
};

template<typename T>
void t_engine::f_debug_break_point(std::unique_lock<std::mutex>& a_lock, T a_member)
{
	while (v_debug__stopping) f_debug_enter_leave(a_lock);
	++v_debug__safe;
	f_debug_stop_and_wait(a_lock);
	(v_debugger->*a_member)(t_thread::v_current->v_thread);
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
	t_object::f_of(v_structure)->f_decrement_push();
	if (v_fields) {
		v_fields->f_scan(f_push_and_clear<&t_object::f_decrement_push>);
		delete v_fields;
		v_fields = nullptr;
	}
	if (!v_type->v_primitive) {
		v_type->f_scan(this, f_push_and_clear<&t_object::f_decrement_push>);
		v_type->f_finalize(this);
	}
	t_object::f_of(v_type)->f_decrement_push();
	v_type = nullptr;
	v_color = e_color__BLACK;
	if (v_next) {
		if (!v_previous) return;
		v_next->v_previous = v_previous;
		v_previous->v_next = v_next;
	}
	f_engine()->f_free_as_release(this);
}

inline void t_thread::t_internal::f_epoch_suspend()
{
#ifdef __unix__
	f_engine()->f_epoch_send(v_handle, SIGUSR1);
#endif
#ifdef _WIN32
	SuspendThread(v_handle);
	CONTEXT context;
	context.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(v_handle, &context);
	auto sp = reinterpret_cast<void*>(context.Rsp);
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(sp, &mbi, sizeof(mbi));
	if (mbi.Protect & PAGE_GUARD) sp = static_cast<char*>(mbi.BaseAddress) + mbi.RegionSize;
	v_active->v_internal->v_stack_top = static_cast<t_object**>(sp);
	v_increments.v_epoch.store(v_increments.v_head, std::memory_order_relaxed);
	v_decrements.v_epoch.store(v_decrements.v_head, std::memory_order_relaxed);
#endif
}

inline void t_thread::t_internal::f_epoch_resume()
{
#ifdef __unix__
	f_engine()->f_epoch_send(v_handle, SIGUSR2);
#endif
#ifdef _WIN32
	ResumeThread(v_handle);
#endif
}

}

#endif
