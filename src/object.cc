#include <xemmai/object.h>

#include <xemmai/structure.h>
#include <xemmai/tuple.h>
#include <xemmai/global.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_object* t_object::v_roots;

void t_object::f_decrement(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_decrement();
	a_slot.v_p = 0;
}

void t_object::f_mark_gray(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_mark_gray();
	--p->v_cyclic;
}

void t_object::f_scan_gray(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_scan_gray();
}

void t_object::f_scan_black(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_scan_black();
}

void t_object::f_collect_white(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_collect_white();
}

void t_object::f_scan_red(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_scan_red();
}

void t_object::f_cyclic_decrement(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	if (p->v_color != e_color__RED) {
		if (p->v_color == e_color__ORANGE) {
			--p->v_count;
			--p->v_cyclic;
		} else {
			p->f_decrement();
		}
	}
	a_slot.v_p = 0;
}

void t_object::f_collect()
{
	std::list<t_object*>& cycles = f_engine()->v_object__cycles;
	for (std::list<t_object*>::reverse_iterator i = cycles.rbegin(); i != cycles.rend(); ++i) {
		portable::t_scoped_lock lock(f_engine()->v_object__reviving__mutex);
		t_object* cycle = *i;
		t_object* p = cycle;
		do {
			p = p->v_next;
			if (p->v_color != e_color__ORANGE || p->v_cyclic > 0 || f_engine()->v_object__reviving && p->f_type_as_type()->v_revive) {
				p = 0;
				break;
			}
		} while (p != cycle);
		if (p == cycle) {
			do {
				p = p->v_next;
				p->v_color = e_color__RED;
			} while (p != cycle);
			do {
				p = p->v_next;
				p->f_cyclic_decrement();
			} while (p != cycle);
			do {
				p = cycle->v_next;
				cycle->v_next = p->v_next;
				delete &f_as<t_type&>(p);
				f_engine()->f_free_as_collect(p);
			} while (p != cycle);
			t_structure*& q = f_engine()->v_structure__finalizing;
			while (q) {
				t_structure* p = q;
				q = p->v_parent1;
				delete p;
			}
		} else {
			p = cycle->v_next;
			t_object* q = p->v_next;
			if (p->v_color == e_color__ORANGE) {
				p->v_color = e_color__PURPLE;
				f_append(v_roots, p);
			} else if (p->v_color == e_color__PURPLE) {
				f_append(v_roots, p);
			} else {
				p->v_color = e_color__BLACK;
				p->v_next = 0;
			}
			while (p != cycle) {
				p = q;
				q = p->v_next;
				if (p->v_color == e_color__PURPLE) {
					f_append(v_roots, p);
				} else {
					p->v_color = e_color__BLACK;
					p->v_next = 0;
				}
			}
		}
	}
	cycles.clear();
	{
		t_library::t_handle*& finalizing = f_engine()->v_library__handle__finalizing;
		while (finalizing) {
			t_library::t_handle* p = finalizing;
			finalizing = p->v_next;
			delete p;
		}
	}
	if (!v_roots) return;
	{
		bool b = ++f_engine()->v_collector__skip >= t_engine::V_COLLECTOR__SKIP;
		if (b) {
			f_engine()->v_collector__skip = 0;
			++f_engine()->v_collector__collect;
		}
		t_object* p = v_roots;
		while (true) {
			t_object* q = p->v_next;
			if (q->v_color == e_color__PURPLE && q->v_count > 0) {
				if (b) q->f_mark_gray();
				if (q == v_roots) break;
				p = q;
			} else {
				p->v_next = q->v_next;
				if (q->v_count <= 0)
					f_engine()->f_free_as_release(q);
				else
					q->v_next = 0;
				if (q == v_roots) {
					if (p == q) p = 0;
					v_roots = p;
					break;
				}
			}
		}
		if (!b) return;
	}
	if (!v_roots) return;
	{
		t_object* p = v_roots;
		do {
			p = p->v_next;
			p->f_scan_gray();
		} while (p != v_roots);
	}
	t_object*& cycle = f_engine()->v_object__cycle;
	while (true) {
		t_object* p = v_roots->v_next;
		v_roots->v_next = p->v_next;
		if (p->v_color == e_color__WHITE) {
			cycle = 0;
			p->f_collect_white();
			cycles.push_back(cycle);
		} else {
			p->v_next = 0;
		}
		if (p == v_roots) break;
	}
	v_roots = 0;
	for (std::list<t_object*>::const_iterator i = cycles.begin(); i != cycles.end(); ++i) {
		t_object* cycle = *i;
		t_object* p = cycle;
		do {
			p = p->v_next;
			p->v_color = e_color__RED;
			p->v_cyclic = p->v_count;
		} while (p != cycle);
		do {
			p = p->v_next;
			static_cast<t_object*>(p->v_structure->v_this)->f_scan_red();
			if (p->v_fields) p->v_fields->f_scan(f_scan_red);
			p->f_type_as_type()->f_scan(p, f_scan_red);
			f_scan_red(p->v_type);
		} while (p != cycle);
		do {
			p = p->v_next;
			p->v_color = e_color__ORANGE;
		} while (p != cycle);
	}
}

void t_object::f_decrement_tree()
{
	static_cast<t_object*>(v_structure->v_this)->f_decrement();
	if (v_fields) {
		v_fields->f_scan(f_decrement);
		delete v_fields;
		v_fields = 0;
	}
	t_type* type = f_type_as_type();
	type->f_scan(this, f_decrement);
	type->f_finalize(this);
	f_decrement(v_type);
	v_color = e_color__BLACK;
	if (!v_next) f_engine()->f_free_as_release(this);
}

void t_object::f_mark_gray()
{
	if (v_color == e_color__GRAY) return;
	v_color = e_color__GRAY;
	v_cyclic = v_count;
	{
		t_object* p = v_structure->v_this;
		p->f_mark_gray();
		--p->v_cyclic;
	}
	if (v_fields) v_fields->f_scan(f_mark_gray);
	f_type_as_type()->f_scan(this, f_mark_gray);
	f_mark_gray(v_type);
}

void t_object::f_scan_gray()
{
	if (v_color == e_color__GRAY && v_cyclic <= 0) {
		v_color = e_color__WHITE;
		static_cast<t_object*>(v_structure->v_this)->f_scan_gray();
		if (v_fields) v_fields->f_scan(f_scan_gray);
		f_type_as_type()->f_scan(this, f_scan_gray);
		f_scan_gray(v_type);
	} else if (v_color != e_color__WHITE) {
		f_scan_black();
	}
}

void t_object::f_collect_white()
{
	if (v_color != e_color__WHITE) return;
	v_color = e_color__ORANGE;
	f_append(f_engine()->v_object__cycle, this);
	static_cast<t_object*>(v_structure->v_this)->f_collect_white();
	if (v_fields) v_fields->f_scan(f_collect_white);
	f_type_as_type()->f_scan(this, f_collect_white);
	f_collect_white(v_type);
}

void t_object::f_cyclic_decrement()
{
	{
		t_object* p = v_structure->v_this;
		if (p && p->v_color != e_color__RED) {
			if (p->v_color == e_color__ORANGE) {
				--p->v_count;
				--p->v_cyclic;
			} else {
				p->f_decrement();
			}
		}
	}
	if (v_fields) {
		v_fields->f_scan(f_cyclic_decrement);
		delete v_fields;
		v_fields = 0;
	}
	f_type_as_type()->f_scan(this, f_cyclic_decrement);
	if (static_cast<t_object*>(v_type) != f_engine()->v_type_class) {
		f_type_as_type()->f_finalize(this);
		v_type.v_pointer = 0;
	}
	f_cyclic_decrement(v_type);
}

void t_object::f_field_add(const t_transfer& a_structure, const t_transfer& a_value)
{
	size_t index = v_structure->f_size();
	if (!v_fields || index >= v_fields->f_size()) {
		t_transfer tuple = t_tuple::f_instantiate(index + 1);
		t_tuple& fields = f_as<t_tuple&>(tuple);
		for (size_t i = 0; i < index; ++i) fields[i] = (*v_fields)[i].f_transfer();
		tuple.f_pointer__(v_fields);
		v_fields = &fields;
	}
	t_object* structure0 = v_structure->v_this;
	t_slot structure1 = a_structure;
	v_structure = &f_as<t_structure&>(structure1);
	t_value::v_decrements->f_push(structure0);
	(*v_fields)[index] = a_value;
}

t_transfer t_object::f_allocate_on_boot(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	p->v_type.v_pointer = 0;
	p->v_owner = 0;
	return t_transfer(p, t_transfer::t_pass());
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_transfer t_object::f_allocate_uninitialized(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	t_value::v_increments->f_push(f_engine()->v_structure_root);
	p->v_structure = &f_as<t_structure&>(f_engine()->v_structure_root);
	p->v_owner = static_cast<t_type*>(a_type->f_pointer())->v_shared ? 0 : t_value::v_increments;
	return t_transfer(p, t_transfer::t_pass());
}

t_transfer t_object::f_allocate(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	p->v_type.v_pointer = 0;
	t_value::v_increments->f_push(f_engine()->v_structure_root);
	p->v_structure = &f_as<t_structure&>(f_engine()->v_structure_root);
	p->v_owner = static_cast<t_type*>(a_type->f_pointer())->v_shared ? 0 : t_value::v_increments;
	return t_transfer(p, t_transfer::t_pass());
}
#endif

void t_object::f_own()
{
	if (f_type_as_type()->v_fixed) t_throwable::f_throw(L"thread mode is fixed.");
	{
		portable::t_scoped_lock_for_write lock(v_lock);
		if (v_owner) t_throwable::f_throw(L"already owned.");
		v_owner = t_value::v_increments;
	}
	t_slot* p = v_structure->f_fields();
	for (size_t i = 0; i < v_structure->f_size(); ++i) {
		t_object* key = p[i];
		size_t j = t_thread::t_cache::f_index(this, key);
		t_thread::t_cache& cache = t_thread::v_cache[j];
		if (static_cast<t_object*>(cache.v_object) == this && static_cast<t_object*>(cache.v_key) == key) cache.v_object = cache.v_key = cache.v_value = 0;
		cache.v_revision = t_thread::t_cache::f_revise(j);
		cache.v_key_revision = f_as<t_symbol&>(key).v_revision;
	}
}

void t_object::f_share()
{
	if (f_type_as_type()->v_fixed) t_throwable::f_throw(L"thread mode is fixed.");
	if (v_owner != t_value::v_increments) t_throwable::f_throw(L"not owned.");
	portable::t_scoped_lock_for_write lock(v_lock);
	v_owner = 0;
}

void t_object::f_field_put(t_object* a_key, const t_transfer& a_value)
{
	ptrdiff_t index = v_structure->f_index(a_key);
	if (index < 0)
		f_field_add(v_structure->f_append(a_key), a_value);
	else
		(*v_fields)[index] = a_value;
}

void t_object::f_field_remove(size_t a_index)
{
	t_object* structure0 = v_structure->v_this;
	t_slot structure1 = v_structure->f_remove(a_index);
	v_structure = &f_as<t_structure&>(structure1);
	t_value::v_decrements->f_push(structure0);
	size_t size = v_structure->f_size();
	if (size + 4 < v_fields->f_size()) {
		t_transfer tuple = t_tuple::f_instantiate(size);
		t_tuple& fields = f_as<t_tuple&>(tuple);
		for (size_t i = 0; i < a_index; ++i) fields[i] = (*v_fields)[i].f_transfer();
		while (a_index < size) {
			fields[a_index] = (*v_fields)[a_index + 1].f_transfer();
			++a_index;
		}
		tuple.f_pointer__(v_fields);
		v_fields = &fields;
	} else {
		while (a_index < size) {
			(*v_fields)[a_index] = (*v_fields)[a_index + 1].f_transfer();
			++a_index;
		}
	}
}

void t_object::f_call_and_return(const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	f_call(a_self, a_stack, a_n);
	if (f_context()->v_native <= 0) t_code::f_loop();
}

t_transfer t_object::f_call_with_same(t_slot* a_stack, size_t a_n)
{
	t_scoped_stack stack(a_n + 1);
	for (size_t i = 1; i <= a_n; ++i) stack[i].f_construct(a_stack[i]);
	f_call_and_return(t_value(), stack, a_n);
	return stack.f_return();
}

}
