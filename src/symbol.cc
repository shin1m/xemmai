#include <xemmai/symbol.h>

#include <xemmai/throwable.h>
#include <xemmai/convert.h>

namespace xemmai
{

t_transfer t_symbol::f_instantiate(const std::wstring& a_value)
{
	std::lock_guard<std::mutex> lock(f_engine()->v_symbol__instantiate__mutex);
	f_engine()->v_object__reviving__mutex.lock();
	std::map<std::wstring, t_slot>& instances = f_engine()->v_symbol__instances;
	auto i = instances.lower_bound(a_value);
	if (i == instances.end() || i->first != a_value) {
		i = instances.insert(i, std::make_pair(a_value, t_slot()));
	} else if (i->second.f_tag() != t_value::e_tag__NULL) {
		f_engine()->v_object__reviving = true;
		f_as<t_thread&>(t_thread::f_current()).v_internal->f_revive();
		f_engine()->v_object__reviving__mutex.unlock();
		return i->second;
	}
	f_engine()->v_object__reviving__mutex.unlock();
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_symbol>());
	object.f_pointer__(new t_symbol(i));
	i->second = static_cast<t_object*>(object);
	return object;
}

void t_symbol::f_define(t_object* a_class)
{
	t_define<t_symbol, t_object>(f_global(), L"Symbol", a_class)
		(f_global()->f_symbol_string(), t_member<const std::wstring& (t_symbol::*)() const, &t_symbol::f_string>())
	;
}

void t_symbol::f_revise(t_object* a_this)
{
	if (f_atomic_increment(f_as<t_symbol&>(a_this).v_revision) != 0) return;
	for (size_t i = 0; i < t_thread::t_cache::V_SIZE; ++i) {
		t_thread::t_cache& cache = t_thread::v_cache[i];
		if (static_cast<t_object*>(cache.v_key) == a_this) cache.v_object = cache.v_key = cache.v_value = 0;
		cache.v_revision = t_thread::t_cache::f_revise(i);
	}
}

t_type* t_type_of<t_symbol>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_symbol>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_symbol&>(a_this).v_entry->second);
}

void t_type_of<t_symbol>::f_finalize(t_object* a_this)
{
	t_symbol& p = f_as<t_symbol&>(a_this);
	f_engine()->v_symbol__instances.erase(p.v_entry);
	delete &p;
}

void t_type_of<t_symbol>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(t_symbol::f_instantiate(f_as<const std::wstring&>(a0)));
}

}
