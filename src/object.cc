#include <xemmai/object.h>

#include <xemmai/structure.h>
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
			if (p->v_color != e_color__ORANGE || p->v_cyclic > 0 || f_engine()->v_object__reviving && p->v_type->v_revive) {
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
				if (p->v_type == f_engine()->v_type_class)
					p->f_as<t_type>().f_destruct();
				else if (p->v_type == f_engine()->v_type_structure)
					p->f_as<t_structure>().~t_structure();
				f_engine()->f_free_as_collect(p);
			} while (p != cycle);
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
		size_t live = f_engine()->v_object__pool0.f_live() + f_engine()->v_object__pool1.f_live() + f_engine()->v_object__pool2.f_live() + f_engine()->v_object__pool3.f_live() + f_engine()->v_object__allocated - f_engine()->v_object__freed;
		auto& lower0 = f_engine()->v_object__lower0;
		if (live < lower0) lower0 = live;
		auto& lower1 = f_engine()->v_object__lower1;
		if (live < lower1) lower1 = live;
		bool b = live - lower1 >= f_engine()->v_collector__threshold1;
		if (b) {
			lower1 = live;
			++f_engine()->v_collector__collect;
		} else if (live - lower0 < f_engine()->v_collector__threshold0) {
			return;
		}
		lower0 = live;
		++f_engine()->v_collector__release;
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

void t_object::f_cyclic_decrement()
{
	if (v_structure->v_this) v_structure->v_this->f_cyclic_decrement_push();
	if (v_fields) {
		v_fields->f_scan(f_push_and_clear<&t_object::f_cyclic_decrement_push>);
		delete v_fields;
		v_fields = nullptr;
	}
	v_type->f_scan(this, f_push_and_clear<&t_object::f_cyclic_decrement_push>);
	if (v_type != f_engine()->v_type_class) v_type->f_finalize(this);
	if (v_type->v_this) v_type->v_this->f_cyclic_decrement_push();
}

void t_object::f_field_add(t_scoped&& a_structure, t_scoped&& a_value)
{
	if (!v_fields) {
		v_fields = new(4) t_structure::t_fields(4);
		(*v_fields)[0].f_construct(std::move(a_value));
	} else {
		size_t index = v_structure->v_size;
		if (index >= v_fields->v_size) {
			auto fields = new(index + 4) t_structure::t_fields(index + 4);
			for (size_t i = 0; i < index; ++i) (*fields)[i].f_construct(std::move((*v_fields)[i]));
			std::swap(v_fields, fields);
			f_global()->f_type<t_structure::t_discard>()->f_new<t_structure::t_discard>(true)->f_as<t_structure::t_discard>().v_fields = fields;
		}
		(*v_fields)[index].f_construct(std::move(a_value));
	}
	auto structure0 = t_object::f_of(v_structure);
	v_structure = &t_slot(std::move(a_structure))->f_as<t_structure>();
	t_value::v_decrements->f_push(structure0);
}

void t_object::f_own()
{
	if (f_type()->v_fixed) f_throw(L"thread mode is fixed."sv);
	{
		t_scoped_lock_for_write lock(v_lock);
		if (v_owner) f_throw(L"already owned."sv);
		v_owner = t_value::v_increments;
	}
	t_slot* p = v_structure->f_fields();
	for (size_t i = 0; i < v_structure->v_size; ++i) {
		t_object* key = p[i];
		size_t j = t_thread::t_cache::f_index(this, key);
		auto& cache = t_thread::v_cache[j];
		if (static_cast<t_object*>(cache.v_object) == this && static_cast<t_object*>(cache.v_key) == key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(j);
		cache.v_key_revision = key->f_as<t_symbol>().v_revision;
	}
}

void t_object::f_share()
{
	if (f_type()->v_fixed) f_throw(L"thread mode is fixed."sv);
	if (v_owner != t_value::v_increments) f_throw(L"not owned."sv);
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
	auto structure0 = t_object::f_of(v_structure);
	v_structure = &t_slot(v_structure->f_remove(a_index))->f_as<t_structure>();
	t_value::v_decrements->f_push(structure0);
	size_t size = v_structure->v_size;
	if (size + 4 < v_fields->v_size) {
		auto fields = new(size) t_structure::t_fields(size);
		size_t i = 0;
		for (; i < a_index; ++i) (*fields)[i].f_construct(std::move((*v_fields)[i]));
		for (; i < size; ++i) (*fields)[i].f_construct(std::move((*v_fields)[i + 1]));
		std::swap(v_fields, fields);
		f_global()->f_type<t_structure::t_discard>()->f_new<t_structure::t_discard>(true)->f_as<t_structure::t_discard>().v_fields = fields;
	} else {
		(*v_fields)[a_index].f_destruct();
		for (; a_index < size; ++a_index) (*v_fields)[a_index].f_construct(std::move((*v_fields)[a_index + 1]));
	}
}

}
