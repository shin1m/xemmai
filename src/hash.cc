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

}
