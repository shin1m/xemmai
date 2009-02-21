#ifndef XEMMAI__POOL_H
#define XEMMAI__POOL_H

#include <cstddef>
#include <new>

#include "portable/define.h"
#include "portable/thread.h"

namespace xemmai
{

template<typename T_base>
class t_shared_pool : public T_base
{
	portable::t_mutex v_mutex;
	size_t v_allocated;
	size_t v_freed;

public:
	typedef typename T_base::t_element t_element;

	t_shared_pool() : v_allocated(0), v_freed(0)
	{
	}
	size_t f_allocated() const
	{
		return v_allocated;
	}
	size_t f_freed() const
	{
		return v_freed;
	}
	t_element* f_allocate()
	{
		portable::t_scoped_lock lock(v_mutex);
		if (!T_base::v_head) T_base::f_grow();
		t_element* p = T_base::v_head;
		if (p) {
			++v_allocated;
			T_base::v_head = p->v_next;
		}
		return p;
	}
	t_element* f_allocate(size_t a_n, bool a_grow = true)
	{
		portable::t_scoped_lock lock(v_mutex);
		if (!T_base::v_head && a_grow) T_base::f_grow();
		t_element* p = T_base::v_head;
		if (!p) return 0;
		t_element* q = p;
		while (true) {
			++v_allocated;
			if (--a_n <= 0 || !q->v_next) break;
			q = q->v_next;
		}
		T_base::v_head = q->v_next;
		q->v_next = 0;
		return p;
	}
	void f_free(t_element* a_p)
	{
		portable::t_scoped_lock lock(v_mutex);
		t_element* p = a_p;
		while (true) {
			++v_freed;
			if (!p->v_next) break;
			p = p->v_next;
		}
		p->v_next = T_base::v_head;
		T_base::v_head = a_p;
	}
};

template<typename T, size_t A_size>
class t_fixed_pool
{
	struct t_block
	{
		t_block* v_next;
		T v_cells[A_size];
	};

	t_block* v_blocks;

protected:
	T* v_head;

public:
	typedef T t_element;

	t_fixed_pool() : v_blocks(0), v_head(0)
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
};

template<typename T, size_t A_size = 1024>
class t_variable_pool
{
	struct t_block
	{
		t_block* v_next;
		char v_bytes[sizeof(typename T::t_variable) * A_size];
	};

	size_t v_rank;
	size_t v_capacity;
	t_block* v_blocks;

	size_t f_unit() const
	{
		return sizeof(T) + sizeof(typename T::t_variable) * (v_capacity - 1);
	}

protected:
	T* v_head;

public:
	typedef T t_element;

	t_variable_pool() : v_blocks(0), v_head(0)
	{
	}
	void f_initialize(size_t a_rank, size_t a_capacity)
	{
		v_rank = a_rank;
		v_capacity = a_capacity;
	}
	void f_clear()
	{
		size_t unit = f_unit();
		while (v_blocks) {
			t_block* block = v_blocks;
			v_blocks = block->v_next;
			char* p = block->v_bytes;
			char* q = p + sizeof(block->v_bytes);
			while (p < q) {
				reinterpret_cast<T*>(p)->~T();
				p += unit;
			}
			delete block;
		}
	}
	void f_grow();
};

template<typename T, size_t A_size>
void t_fixed_pool<T, A_size>::f_grow()
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

template<typename T, size_t A_size>
void t_variable_pool<T, A_size>::f_grow()
{
	t_block* block = new t_block();
	block->v_next = v_blocks;
	v_blocks = block;
	size_t unit = f_unit();
	char* p = block->v_bytes + sizeof(block->v_bytes) / unit * unit;
	while (p > block->v_bytes) {
		p -= unit;
		T* q = new(p) T();
		q->v_rank = v_rank;
		q->v_capacity = v_capacity;
		q->v_next = v_head;
		v_head = q;
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
