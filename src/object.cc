#include <xemmai/object.h>

#include <xemmai/structure.h>
#include <xemmai/global.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD decltype(t_object::v_roots) t_object::v_roots;
XEMMAI__PORTABLE__THREAD t_object* t_object::v_scan_stack;
XEMMAI__PORTABLE__THREAD t_object* t_object::v_cycle;
XEMMAI__PORTABLE__THREAD t_object* t_object::v_cycles;

void t_object::f_collect()
{
	while (v_cycles) {
		std::lock_guard<std::mutex> lock(f_engine()->v_object__reviving__mutex);
		auto p = v_cycles;
		if (f_engine()->v_object__reviving) {
			while (p->v_color == e_color__ORANGE && p->v_cyclic <= 0 && !p->v_type->v_revive) if (!(p = p->v_next)) break;
		} else {
			while (p->v_color == e_color__ORANGE && p->v_cyclic <= 0) if (!(p = p->v_next)) break;
		}
		if (p) {
			p = v_cycles;
			v_cycles = p->v_next_cycle;
			auto q = p->v_next;
			if (p->v_color == e_color__ORANGE) {
				p->v_color = e_color__PURPLE;
				f_append(p);
			} else if (p->v_color == e_color__PURPLE) {
				f_append(p);
			} else {
				p->v_color = e_color__BLACK;
				p->v_next = nullptr;
			}
			while (q) {
				p = q;
				q = p->v_next;
				if (p->v_color == e_color__PURPLE) {
					f_append(p);
				} else {
					p->v_color = e_color__BLACK;
					p->v_next = nullptr;
				}
			}
		} else {
			p = v_cycles;
			do p->v_color = e_color__RED; while (p = p->v_next);
			p = v_cycles;
			do p->f_cyclic_decrement(); while (p = p->v_next);
			p = v_cycles;
			v_cycles = p->v_next_cycle;
			do {
				auto q = p->v_next;
				if (p->v_type == f_engine()->v_type_class)
					p->f_as<t_type>().f_destruct();
				else if (p->v_type == f_engine()->v_type_structure)
					p->f_as<t_structure>().~t_structure();
				f_engine()->f_free_as_collect(p);
				p = q;
			} while (p);
		}
	}
	for (auto& p = f_engine()->v_library__handle__finalizing; p;) {
		auto q = p;
		p = q->v_next;
		delete q;
	}
	auto roots = reinterpret_cast<t_object*>(&v_roots);
	if (roots->v_next == roots) return;
	{
		size_t live = f_engine()->v_object__pool0.f_live() + f_engine()->v_object__pool1.f_live() + f_engine()->v_object__pool2.f_live() + f_engine()->v_object__pool3.f_live() + f_engine()->v_object__allocated - f_engine()->v_object__freed;
		auto& lower = f_engine()->v_object__lower;
		if (live < lower) lower = live;
		if (live - lower < f_engine()->v_collector__threshold) return;
		lower = live;
		++f_engine()->v_collector__collect;
		auto p = roots;
		auto q = p->v_next;
		do {
			assert(q->v_count > 0);
			if (q->v_color == e_color__PURPLE) {
				q->f_mark_gray();
				p = q;
			} else {
				p->v_next = q->v_next;
				q->v_next = nullptr;
			}
			q = p->v_next;
		} while (q != roots);
	}
	if (roots->v_next == roots) {
		roots->v_previous = roots;
		return;
	}
	{
		auto p = roots->v_next;
		do {
			p->f_scan_gray();
			p = p->v_next;
		} while (p != roots);
	}
	do {
		auto p = roots->v_next;
		roots->v_next = p->v_next;
		if (p->v_color == e_color__WHITE) {
			p->f_collect_white();
			v_cycle->v_next_cycle = v_cycles;
			v_cycles = v_cycle;
		} else {
			p->v_next = nullptr;
		}
	} while (roots->v_next != roots);
	roots->v_previous = roots;
	for (auto cycle = v_cycles; cycle; cycle = cycle->v_next_cycle) {
		auto p = cycle;
		do {
			p->v_color = e_color__RED;
			p->v_cyclic = p->v_count;
		} while (p = p->v_next);
		p = cycle;
		do p->f_step<&t_object::f_scan_red>(); while (p = p->v_next);
		p = cycle;
		do p->v_color = e_color__ORANGE; while (p = p->v_next);
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
