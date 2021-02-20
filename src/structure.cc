#include <xemmai/structure.h>
#include <xemmai/global.h>

namespace xemmai
{

t_structure::t_structure(size_t a_size, std::map<t_object*, t_object*>::iterator a_iterator, t_structure* a_parent) : v_size(a_size), v_iterator(a_iterator), v_this(t_object::f_of(this)), v_parent(t_object::f_of(a_parent))
{
	v_iterator->second = t_object::f_of(this);
	auto key = v_iterator->first;
	size_t n = a_parent->v_size;
	{
		auto p0 = a_parent->f_fields();
		auto p1 = f_fields();
		for (size_t i = 0; i < n; ++i) new(p1++) t_svalue(*p0++);
		new(p1) t_svalue(key);
	}
	auto p0 = a_parent->f_entries();
	auto p1 = f_entries();
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
	auto p = f_entries();
	size_t i = 0;
	size_t j = v_size;
	while (i < j) {
		size_t k = (i + j) / 2;
		auto& entry = p[k];
		if (entry.v_key == a_key) return a_cache.v_index = entry.v_index;
		if (entry.v_key < a_key)
			i = k + 1;
		else
			j = k;
	}
	return a_cache.v_index = -1;
}

t_object* t_structure::f_append(t_object* a_key)
{
	std::lock_guard lock(v_mutex);
	f_engine()->v_object__reviving__mutex.lock();
	auto i = v_children.lower_bound(a_key);
	if (i == v_children.end() || i->first != a_key) {
		i = v_children.emplace_hint(i, a_key, nullptr);
		f_engine()->v_object__reviving__mutex.unlock();
		return t_object::f_of(this)->f_type()->f_new_sized<t_structure>(true, (sizeof(t_svalue) + sizeof(t_entry)) * (v_size + 1), v_size + 1, i, this);
	} else {
		f_engine()->v_object__reviving = true;
		f_as<t_thread&>(t_thread::f_current()).v_internal->f_revive();
		f_engine()->v_object__reviving__mutex.unlock();
		return i->second;
	}
}

t_object* t_structure::f_remove(size_t a_index)
{
	auto p = this;
	while (p->v_size > a_index) p = &f_as<t_structure&>(p->v_parent);
	auto q = t_object::f_of(p);
	while (++a_index < v_size) q = f_as<t_structure&>(q).f_append(f_fields()[a_index]);
	return q;
}

void t_type_of<t_structure>::f_do_finalize(t_object* a_this)
{
	auto& p = a_this->f_as<t_structure>();
	if (p.v_parent) p.v_parent->f_as<t_structure>().v_children.erase(p.v_iterator);
}

}
