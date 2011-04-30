#ifndef XEMMAI__POOL_H
#define XEMMAI__POOL_H

#include <cstddef>
#include <new>

#include "portable/define.h"
#include "portable/thread.h"

namespace xemmai
{

template<typename T, size_t A_size>
class t_shared_pool
{
	struct t_block
	{
		t_block* v_next;
		T v_cells[A_size];
	};

	t_block* v_blocks;
	T* v_head;
	portable::t_mutex v_mutex;
	size_t v_allocated;
	size_t v_freed;

public:
	t_shared_pool() : v_blocks(0), v_head(0), v_allocated(0), v_freed(0)
	{
	}
	void f_clear()
	{
		while (v_blocks) {
			t_block* block = v_blocks;
			v_blocks = block->v_next;
			delete block;
		}
	}
	void f_grow();
	size_t f_allocated() const
	{
		return v_allocated;
	}
	size_t f_freed() const
	{
		return v_freed;
	}
	T* f_allocate()
	{
		portable::t_scoped_lock lock(v_mutex);
		if (!v_head) f_grow();
		T* p = v_head;
		if (p) {
			++v_allocated;
			v_head = p->v_next;
		}
		return p;
	}
	T* f_allocate(size_t a_n, bool a_grow = true)
	{
		portable::t_scoped_lock lock(v_mutex);
		if (!v_head && a_grow) f_grow();
		T* p = v_head;
		if (!p) return 0;
		T* q = p;
		while (true) {
			++v_allocated;
			if (--a_n <= 0 || !q->v_next) break;
			q = q->v_next;
		}
		v_head = q->v_next;
		q->v_next = 0;
		return p;
	}
	void f_free(T* a_p)
	{
		portable::t_scoped_lock lock(v_mutex);
		T* p = a_p;
		while (true) {
			++v_freed;
			if (!p->v_next) break;
			p = p->v_next;
		}
		p->v_next = v_head;
		v_head = a_p;
	}
};

template<typename T, size_t A_size>
void t_shared_pool<T, A_size>::f_grow()
{
	t_block* block = new t_block();
	block->v_next = v_blocks;
	v_blocks = block;
	T* p = block->v_cells + A_size;
	while (p > block->v_cells) {
		--p;
		p->v_next = v_head;
		v_head = p;
	}
}

template<typename T>
class t_local_pool
{
	static XEMMAI__PORTABLE__THREAD T* v_head;

public:
	template<typename T_allocate>
	static T* f_allocate(T_allocate a_allocate)
	{
		T* p = v_head;
		if (!p) p = a_allocate();
		v_head = p->v_next;
		return p;
	}
	static void f_free(T* a_p)
	{
		a_p->v_next = v_head;
		v_head = a_p;
	}
	template<typename T_free>
	static void f_return(T_free a_free)
	{
		T* p = v_head;
		if (!p) return;
		a_free(p);
		v_head = 0;
	}
};

template<typename T>
XEMMAI__PORTABLE__THREAD T* t_local_pool<T>::v_head;

}

#endif
