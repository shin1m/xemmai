#include <xemmai/object.h>

#include <xemmai/structure.h>
#include <xemmai/tuple.h>
#include <xemmai/global.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_object* t_object::v_roots;
XEMMAI__PORTABLE__THREAD t_object* t_object::v_scan_stack;

void t_object::f_collect()
{
	auto& cycles = f_engine()->v_object__cycles;
	for (auto i = cycles.rbegin(); i != cycles.rend(); ++i) {
		std::lock_guard<std::mutex> lock(f_engine()->v_object__reviving__mutex);
		t_object* cycle = *i;
		t_object* p = cycle;
		do {
			p = p->v_next;
			if (p->v_color != e_color__ORANGE || p->v_cyclic > 0 || f_engine()->v_object__reviving && p->f_type_as_type()->v_revive) {
				p = nullptr;
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
			auto& q = f_engine()->v_structure__finalizing;
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
				p->v_next = nullptr;
			}
			while (p != cycle) {
				p = q;
				q = p->v_next;
				if (p->v_color == e_color__PURPLE) {
					f_append(v_roots, p);
				} else {
					p->v_color = e_color__BLACK;
					p->v_next = nullptr;
				}
			}
		}
	}
	cycles.clear();
	{
		auto& finalizing = f_engine()->v_library__handle__finalizing;
		while (finalizing) {
			auto p = finalizing;
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
		} else if (f_engine()->v_collector__skip % 8 != 0) {
			return;
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
					q->v_next = nullptr;
				if (q == v_roots) {
					if (p == q) p = nullptr;
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
	auto& cycle = f_engine()->v_object__cycle;
	while (true) {
		t_object* p = v_roots->v_next;
		v_roots->v_next = p->v_next;
		if (p->v_color == e_color__WHITE) {
			cycle = nullptr;
			p->f_collect_white();
			cycles.push_back(cycle);
		} else {
			p->v_next = nullptr;
		}
		if (p == v_roots) break;
	}
	v_roots = nullptr;
	for (auto cycle : cycles) {
		t_object* p = cycle;
		do {
			p = p->v_next;
			p->v_color = e_color__RED;
			p->v_cyclic = p->v_count;
		} while (p != cycle);
		do {
			p = p->v_next;
			p->f_step<&t_object::f_scan_red>();
		} while (p != cycle);
		do {
			p = p->v_next;
			p->v_color = e_color__ORANGE;
		} while (p != cycle);
	}
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_object* t_object::f_local_pool__allocate()
{
	return t_local_pool<t_object>::f_allocate(f_pool__allocate);
}
#endif

void t_object::f_cyclic_decrement()
{
	{
		t_object* p = v_structure->v_this;
		if (p) p->f_cyclic_decrement_push();
	}
	if (v_fields) {
		v_fields->f_scan(f_push_and_clear<&t_object::f_cyclic_decrement_push>);
		delete v_fields;
		v_fields = nullptr;
	}
	f_type_as_type()->f_scan(this, f_push_and_clear<&t_object::f_cyclic_decrement_push>);
	if (static_cast<t_object*>(v_type) != f_engine()->v_type_class) {
		f_type_as_type()->f_finalize(this);
		v_type.v_pointer = nullptr;
	}
	f_push_and_clear<&t_object::f_cyclic_decrement_push>(v_type);
}

void t_object::f_field_add(t_scoped&& a_structure, t_scoped&& a_value)
{
	if (!v_fields) {
		v_fields = new(4) t_tuple();
		(*v_fields)[0].f_construct(std::move(a_value));
	} else {
		size_t index = v_structure->f_size();
		if (index >= v_fields->f_size()) {
			auto fields = new(index + 4) t_tuple();
			for (size_t i = 0; i < index; ++i) (*fields)[i].f_construct(std::move((*v_fields)[i]));
			std::swap(v_fields, fields);
			t_object::f_allocate(f_global()->f_type<t_tuple>()).f_pointer__(fields);
		}
		(*v_fields)[index].f_construct(std::move(a_value));
	}
	t_object* structure0 = v_structure->v_this;
	t_slot structure1(std::move(a_structure));
	v_structure = &f_as<t_structure&>(structure1);
	t_value::v_decrements->f_push(structure0);
}

t_scoped t_object::f_allocate_on_boot(t_object* a_type)
{
	t_object* p = f_local_pool__allocate();
	p->v_next = nullptr;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	p->v_type.v_pointer = nullptr;
	p->v_owner = nullptr;
	return t_scoped(p, t_scoped::t_pass());
}

void t_object::f_own()
{
	if (f_type_as_type()->v_fixed) t_throwable::f_throw(L"thread mode is fixed.");
	{
		t_scoped_lock_for_write lock(v_lock);
		if (v_owner) t_throwable::f_throw(L"already owned.");
		v_owner = t_value::v_increments;
	}
	t_slot* p = v_structure->f_fields();
	for (size_t i = 0; i < v_structure->f_size(); ++i) {
		t_object* key = p[i];
		size_t j = t_thread::t_cache::f_index(this, key);
		auto& cache = t_thread::v_cache[j];
		if (static_cast<t_object*>(cache.v_object) == this && static_cast<t_object*>(cache.v_key) == key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(j);
		cache.v_key_revision = f_as<t_symbol&>(key).v_revision;
	}
}

void t_object::f_share()
{
	if (f_type_as_type()->v_fixed) t_throwable::f_throw(L"thread mode is fixed.");
	if (v_owner != t_value::v_increments) t_throwable::f_throw(L"not owned.");
	t_scoped_lock_for_write lock(v_lock);
	v_owner = nullptr;
}

void t_object::f_field_put(t_object* a_key, t_scoped&& a_value)
{
	intptr_t index = v_structure->f_index(a_key);
	if (index < 0)
		f_field_add(v_structure->f_append(a_key), std::move(a_value));
	else
		(*v_fields)[index] = std::move(a_value);
}

void t_object::f_field_remove(size_t a_index)
{
	t_object* structure0 = v_structure->v_this;
	t_slot structure1(v_structure->f_remove(a_index));
	v_structure = &f_as<t_structure&>(structure1);
	t_value::v_decrements->f_push(structure0);
	size_t size = v_structure->f_size();
	if (size + 4 < v_fields->f_size()) {
		t_scoped tuple = t_tuple::f_instantiate(size);
		auto& fields = f_as<t_tuple&>(tuple);
		for (size_t i = 0; i < a_index; ++i) fields[i].f_construct(std::move((*v_fields)[i]));
		while (a_index < size) {
			fields[a_index].f_construct(std::move((*v_fields)[a_index + 1]));
			++a_index;
		}
		tuple.f_pointer__(v_fields);
		v_fields = &fields;
	} else {
		(*v_fields)[a_index].f_destruct();
		while (a_index < size) {
			(*v_fields)[a_index].f_construct(std::move((*v_fields)[a_index + 1]));
			++a_index;
		}
	}
}

}
