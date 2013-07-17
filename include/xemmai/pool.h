#ifndef XEMMAI__POOL_H
#define XEMMAI__POOL_H

#include <cstddef>
#include <list>
#include <mutex>
#include <new>

#include "portable/define.h"

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
	struct t_chunk
	{
		T* v_head;
		size_t v_size;

		t_chunk(T* a_head, size_t a_size) : v_head(a_head), v_size(a_size)
		{
		}
	};

	t_block* v_blocks = nullptr;
	std::list<t_chunk> v_chunks;
	std::mutex v_mutex;
	size_t v_allocated = 0;
	size_t v_freed = 0;

public:
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
	T* f_allocate(bool a_grow = true)
	{
		std::lock_guard<std::mutex> lock(v_mutex);
		if (v_chunks.empty()) {
			if (!a_grow) return nullptr;
			f_grow();
			if (v_chunks.empty()) return nullptr;
		}
		T* p = v_chunks.front().v_head;
		v_allocated += v_chunks.front().v_size;
		v_chunks.pop_front();
		return p;
	}
	void f_free(T* a_p, size_t a_n)
	{
		std::lock_guard<std::mutex> lock(v_mutex);
		v_chunks.push_back(t_chunk(a_p, a_n));
		v_freed += a_n;
	}
};

template<typename T, size_t A_size>
void t_shared_pool<T, A_size>::f_grow()
{
	t_block* block = new t_block();
	block->v_next = v_blocks;
	v_blocks = block;
	T* p = block->v_cells;
	for (size_t i = 1; i < A_size; ++i) {
		p->v_next = p + 1;
		++p;
	}
	p->v_next = nullptr;
	v_chunks.push_back(t_chunk(block->v_cells, A_size));
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
	static T* f_detach()
	{
		T* p = v_head;
		v_head = nullptr;
		return p;
	}
};

template<typename T>
XEMMAI__PORTABLE__THREAD T* t_local_pool<T>::v_head;

}

#endif
