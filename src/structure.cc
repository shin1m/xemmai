#include <xemmai/structure.h>

#include <xemmai/global.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_structure::t_cache* t_structure::v_cache;

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
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_structure>());
	object.f_pointer__(new(v_size + 1) t_structure(i, static_cast<t_object*>(object), this));
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

t_type* t_type_of<t_structure>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_structure>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_structure&>(a_this).f_scan(a_scan);
}

void t_type_of<t_structure>::f_finalize(t_object* a_this)
{
	t_structure& p = f_as<t_structure&>(a_this);
	if (p.v_parent1) p.v_parent1->v_children.erase(p.v_iterator);
	p.v_parent1 = f_engine()->v_structure__finalizing;
	f_engine()->v_structure__finalizing = &p;
}

void t_type_of<t_structure>::f_instantiate(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
