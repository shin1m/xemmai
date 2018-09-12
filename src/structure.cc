#include <xemmai/structure.h>

#include <xemmai/global.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_structure::t_cache* t_structure::v_cache;

intptr_t t_structure::f_index(t_object* a_key, t_cache& a_cache) const
{
	a_cache.v_structure = v_this;
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
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_structure>(), true);
	object.f_pointer__(new(v_size + 1) t_structure(v_size + 1, i, static_cast<t_object*>(object), this));
	i->second = object;
	return object;
}

t_scoped t_structure::f_remove(size_t a_index)
{
	t_structure* p = this;
	while (p->v_size > a_index) p = p->v_parent1;
	t_scoped q = p->v_this;
	while (++a_index < v_size) q = f_as<t_structure&>(q).f_append(f_fields()[a_index]);
	return q;
}

void t_type_of<t_structure>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_structure&>(a_this).f_scan(a_scan);
}

void t_type_of<t_structure>::f_do_finalize(t_object* a_this)
{
	auto& p = f_as<t_structure&>(a_this);
	if (p.v_parent1) p.v_parent1->v_children.erase(p.v_iterator);
	p.v_parent1 = f_engine()->v_structure__finalizing;
	f_engine()->v_structure__finalizing = &p;
}

}
