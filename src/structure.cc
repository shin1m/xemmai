#include <xemmai/structure.h>

#include <xemmai/engine.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_structure::t_cache* t_structure::v_cache;

t_structure::t_structure(size_t a_size, std::map<t_object*, t_object*>::iterator a_iterator, t_structure* a_parent) : v_size(a_size), v_iterator(a_iterator), v_this(t_object::f_of(this)), v_parent(t_object::f_of(a_parent))
{
	t_object* key = v_iterator->first;
	size_t n = a_parent->v_size;
	{
		t_slot* p0 = a_parent->f_fields();
		t_slot* p1 = f_fields();
		for (size_t i = 0; i < n; ++i) new(p1++) t_slot(*p0++);
		new(p1) t_slot(key);
	}
	t_entry* p0 = a_parent->f_entries();
	t_entry* p1 = f_entries();
	size_t i = 0;
	for (; i < n && p0->v_key < key; ++i) {
		new(p1++) t_entry{p0->v_key, p0->v_index};
		++p0;
	}
	new(p1++) t_entry{key, n};
	for (; i < n; ++i) {
		new(p1++) t_entry{p0->v_key, p0->v_index};
		++p0;
	}
}

intptr_t t_structure::f_index(t_object* a_key, t_cache& a_cache) const
{
	a_cache.v_structure = t_object::f_of(const_cast<t_structure*>(this));
	a_cache.v_key = a_key;
	const t_entry* p = f_entries();
	size_t i = 0;
	size_t j = v_size;
	while (i < j) {
		size_t k = (i + j) / 2;
		const t_entry& entry = p[k];
		if (entry.v_key == a_key) return a_cache.v_index = entry.v_index;
		if (entry.v_key < a_key)
			i = k + 1;
		else
			j = k;
	}
	return a_cache.v_index = -1;
}

t_scoped t_structure::f_append(t_object* a_key)
{
	std::lock_guard<std::mutex> lock(v_mutex);
	f_engine()->v_object__reviving__mutex.lock();
	auto i = v_children.lower_bound(a_key);
	if (i == v_children.end() || i->first != a_key) {
		i = v_children.emplace_hint(i, a_key, nullptr);
	} else if (i->second) {
		f_engine()->v_object__reviving = true;
		f_as<t_thread&>(t_thread::f_current()).v_internal->f_revive();
		f_engine()->v_object__reviving__mutex.unlock();
		return i->second;
	}
	f_engine()->v_object__reviving__mutex.unlock();
	auto object = t_object::f_allocate(t_object::f_of(this)->f_type(), true, sizeof(t_structure) + (sizeof(t_slot) + sizeof(t_entry)) * (v_size + 1));
	new(object->f_data()) t_structure(v_size + 1, i, this);
	i->second = object;
	return object;
}

t_scoped t_structure::f_remove(size_t a_index)
{
	t_structure* p = this;
	while (p->v_size > a_index) p = &f_as<t_structure&>(p->v_parent);
	t_scoped q = t_object::f_of(p);
	while (++a_index < v_size) q = f_as<t_structure&>(q).f_append(f_fields()[a_index]);
	return q;
}

void t_type_of<t_structure>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_structure&>(a_this).f_scan(a_scan);
}

void t_type_of<t_structure>::f_do_finalize(t_object* a_this)
{
	auto& p = a_this->f_as<t_structure>();
	if (p.v_parent) p.v_parent->f_as<t_structure>().v_children.erase(p.v_iterator);
}

}
