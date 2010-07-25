#ifndef XEMMAI__HASH_H
#define XEMMAI__HASH_H

#include <utility>

#include "pool.h"
#include "value.h"

namespace xemmai
{

class t_engine;

class t_hash
{
	friend class t_engine;

public:
	class t_iterator;
	class t_entry
	{
		friend class t_hash;
		friend class t_iterator;
		template<typename T_base> friend class t_shared_pool;
		template<typename T, size_t A_size> friend class t_fixed_pool;
		friend class t_local_pool<t_entry>;
		friend class t_engine;

		static t_entry* f_allocate();

		t_entry* v_next;
		t_slot v_key;

	public:
		t_slot v_value;

		const t_slot& f_key() const
		{
			return v_key;
		}
	};

private:
	struct t_table
	{
		typedef t_entry* t_variable;

		static const size_t V_POOLS__SIZE = 4;

		size_t v_rank;
		size_t v_capacity;
		union
		{
			t_table* v_next;
			size_t v_size;
		};
		t_entry* v_entries[1];

		static t_table* f_allocate(size_t a_rank);
		static void f_free(t_table* a_p);

		t_table()
		{
		}
		void f_scan(t_scan a_scan);
		void f_finalize();
		void f_clear();
		t_entry** f_bucket(size_t a_key)
		{
			return &v_entries[a_key % v_capacity];
		}
	};

	t_table* v_table;

	template<typename T_traits>
	void f_rehash(size_t a_rank);

public:
	class t_iterator
	{
		t_table* v_table;
		size_t v_i;
		t_entry* v_entry;

	public:
		t_iterator(const t_hash& a_hash) : v_table(a_hash.v_table), v_i(0), v_entry(0)
		{
			if (!v_table) return;
			size_t n = v_table->v_capacity;
			do {
				v_entry = v_table->v_entries[v_i];
				if (v_entry) break;
			} while (++v_i < n);
		}
		t_entry* f_entry() const
		{
			return v_entry;
		}
		void f_next()
		{
			v_entry = v_entry->v_next;
			if (v_entry) return;
			size_t n = v_table->v_capacity;
			while (++v_i < n) {
				v_entry = v_table->v_entries[v_i];
				if (v_entry) break;
			}
		}
	};
	friend class t_iterator;

	t_hash() : v_table(0)
	{
	}
	void f_scan(t_scan a_scan)
	{
		t_table* table = v_table;
		if (table) table->f_scan(a_scan);
	}
	void f_finalize()
	{
		if (!v_table) return;
		v_table->f_finalize();
		v_table = 0;
	}
	void f_clear()
	{
		if (!v_table) return;
		t_table* table = v_table;
		v_table = 0;
		table->f_clear();
	}
	template<typename T_traits>
	t_entry* f_find(const t_value& a_key) const
	{
		if (!v_table) return 0;
		t_entry* p = *v_table->f_bucket(T_traits::f_hash(a_key));
		while (p) {
			if (T_traits::f_equals(p->v_key, a_key)) return p;
			p = p->v_next;
		}
		return 0;
	}
	template<typename T_traits>
	std::pair<bool, t_entry*> f_put(const t_value& a_key, const t_transfer& a_value);
	template<typename T_traits>
	std::pair<bool, t_transfer> f_remove(const t_value& a_key);
};

template<typename T_traits>
void t_hash::f_rehash(size_t a_rank)
{
	t_table* table = v_table;
	v_table = t_table::f_allocate(a_rank);
	v_table->v_size = table->v_size;
	size_t n = table->v_capacity;
	for (size_t i = 0; i < n; ++i) {
		t_entry* p = table->v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			t_entry** bucket = v_table->f_bucket(T_traits::f_hash(p->v_key));
			p->v_next = *bucket;
			t_scoped(p->v_key);
			t_scoped(p->v_value);
			*bucket = p;
			p = q;
		}
	}
	t_table::f_free(table);
}

template<typename T_traits>
std::pair<bool, t_hash::t_entry*> t_hash::f_put(const t_value& a_key, const t_transfer& a_value)
{
	t_entry* p = f_find<T_traits>(a_key);
	if (p) {
		p->v_value = a_value;
		return std::make_pair(false, p);
	} else {
		if (!v_table) {
			v_table = t_table::f_allocate(0);
			v_table->v_size = 0;
		} else if (v_table->v_rank < t_table::V_POOLS__SIZE - 1 && v_table->v_size >= v_table->v_capacity) {
			f_rehash<T_traits>(v_table->v_rank + 1);
		}
		t_entry** bucket = v_table->f_bucket(T_traits::f_hash(a_key));
		t_entry* p = t_local_pool<t_entry>::f_allocate(t_entry::f_allocate);
		p->v_next = *bucket;
		p->v_key.f_construct(a_key);
		p->v_value.f_construct(a_value);
		*bucket = p;
		++v_table->v_size;
		return std::make_pair(true, p);
	}
}

template<typename T_traits>
std::pair<bool, t_transfer> t_hash::f_remove(const t_value& a_key)
{
	if (!v_table) return std::make_pair(false, t_value());
	t_entry** bucket = v_table->f_bucket(T_traits::f_hash(a_key));
	while (true) {
		t_entry* p = *bucket;
		if (!p) return std::make_pair(false, t_value());
		if (T_traits::f_equals(p->v_key, a_key)) break;
		bucket = &p->v_next;
	}
	t_entry* p = *bucket;
	*bucket = p->v_next;
	p->v_key = 0;
	t_transfer value = p->v_value.f_transfer();
	t_local_pool<t_entry>::f_free(p);
	--v_table->v_size;
	if (v_table->v_rank > 0 && v_table->v_size < v_table->v_capacity / 2) f_rehash<T_traits>(v_table->v_rank - 1);
	return std::make_pair(true, value);
}

}

#endif
