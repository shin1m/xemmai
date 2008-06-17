#include <xemmai/hash.h>

#include <xemmai/engine.h>

namespace xemmai
{

t_hash::t_entry* t_hash::t_entry::f_allocate()
{
	return f_engine()->v_hash__entry__pool.f_allocate(t_engine::V_POOL__ALLOCATION__UNIT);
}

t_hash::t_table* t_hash::t_table::f_allocate(size_t a_rank)
{
	return f_engine()->f_hash__table__allocate(a_rank);
}

void t_hash::t_table::f_free(t_hash::t_table* a_p)
{
	f_engine()->f_hash__table__free(a_p);
}

void t_hash::t_table::f_scan(t_scan a_scan)
{
	for (size_t i = 0; i < v_capacity; ++i) {
		for (t_entry* p = v_entries[i]; p; p = p->v_next) {
			if (!p->v_key) return;
			a_scan(p->v_key);
			a_scan(p->v_value);
		}
	}
}

void t_hash::t_table::f_finalize()
{
	for (size_t i = 0; i < v_capacity; ++i) {
		t_entry* p = v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			t_local_pool<t_entry>::f_free(p);
			p = q;
		}
	}
	f_engine()->f_hash__table__free(this);
}

void t_hash::t_table::f_clear()
{
	for (size_t i = 0; i < v_capacity; ++i) {
		t_entry* p = v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			p->v_key = 0;
			p->v_value = 0;
			t_local_pool<t_entry>::f_free(p);
			p = q;
		}
	}
	f_engine()->f_hash__table__free(this);
}
/*
void t_hash::f_rehash(size_t a_rank)
{
	t_table* table = v_table;
	v_table = f_engine()->f_hash__table__allocate(a_rank);
	v_table->v_size = table->v_size;
	size_t n = table->v_capacity;
	for (size_t i = 0; i < n; ++i) {
		t_entry* p = table->v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			t_entry** bucket = v_table->f_bucket(p->v_key);
			p->v_next = *bucket;
			t_scoped(p->v_key);
			t_scoped(p->v_value);
			*bucket = p;
			p = q;
		}
	}
	f_engine()->f_hash__table__free(table);
}

bool t_hash::f_put(t_object* a_key, const t_transfer& a_value)
{
	t_entry* p = f_find(a_key);
	if (p) {
		p->v_value = a_value;
		return false;
	} else {
		if (!v_table) {
			v_table = f_engine()->f_hash__table__allocate(0);
			v_table->v_size = 0;
		} else if (v_table->v_rank < t_engine::V_HASH__TABLE__POOLS__SIZE - 1 && v_table->v_size >= v_table->v_capacity) {
			f_rehash(v_table->v_rank + 1);
		}
		t_entry** bucket = v_table->f_bucket(a_key);
		t_entry* p = t_local_pool<t_entry>::f_allocate(t_entry::f_allocate);
		p->v_next = *bucket;
		p->v_key = a_key;
		p->v_value = a_value;
		*bucket = p;
		++v_table->v_size;
		return true;
	}
}

bool t_hash::f_remove(t_object* a_key)
{
	if (!v_table) return false;
	t_entry** bucket = v_table->f_bucket(a_key);
	while (true) {
		t_entry* p = *bucket;
		if (!p) return false;
		if (p->v_key == a_key) break;
		bucket = &p->v_next;
	}
	t_entry* p = *bucket;
	*bucket = p->v_next;
	p->v_key = 0;
	p->v_value = 0;
	t_local_pool<t_entry>::f_free(p);
	--v_table->v_size;
	if (v_table->v_rank > 0 && v_table->v_size < v_table->v_capacity / 2) f_rehash(v_table->v_rank - 1);
	return true;
}
*/
}
