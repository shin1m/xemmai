#include <xemmai/symbol.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_symbol::f_instantiate(const std::wstring& a_value)
{
	std::lock_guard<std::mutex> lock(f_engine()->v_symbol__instantiate__mutex);
	f_engine()->v_object__reviving__mutex.lock();
	auto& instances = f_engine()->v_symbol__instances;
	auto i = instances.lower_bound(a_value);
	if (i == instances.end() || i->first != a_value) {
		i = instances.emplace_hint(i, a_value, t_slot());
	} else if (i->second.f_tag() != t_value::e_tag__NULL) {
		f_engine()->v_object__reviving = true;
		f_as<t_thread&>(t_thread::f_current()).v_internal->f_revive();
		f_engine()->v_object__reviving__mutex.unlock();
		return i->second;
	}
	f_engine()->v_object__reviving__mutex.unlock();
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_symbol>());
	object.f_pointer__(new t_symbol(i));
	i->second = static_cast<t_object*>(object);
	return object;
}

void t_symbol::f_revise(t_object* a_this)
{
	if (f_atomic_increment(f_as<t_symbol&>(a_this).v_revision) != 0) return;
	for (size_t i = 0; i < t_thread::t_cache::V_SIZE; ++i) {
		auto& cache = t_thread::v_cache[i];
		if (static_cast<t_object*>(cache.v_key) == a_this) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
	}
}

void t_type_of<t_symbol>::f_define()
{
	v_builtin = true;
	t_define<t_symbol, t_object>(f_global(), L"Symbol", v_this)
		(f_global()->f_symbol_string(), t_member<const std::wstring&(t_symbol::*)() const, &t_symbol::f_string>())
	;
}

t_type* t_type_of<t_symbol>::f_derive()
{
	return nullptr;
}

void t_type_of<t_symbol>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_symbol&>(a_this).v_entry->second);
}

void t_type_of<t_symbol>::f_finalize(t_object* a_this)
{
	auto& p = f_as<t_symbol&>(a_this);
	f_engine()->v_symbol__instances.erase(p.v_entry);
	delete &p;
}

void t_type_of<t_symbol>::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1) t_throwable::f_throw(a_stack, a_n, L"must be called with an argument.");
	t_destruct<> a0(a_stack[2]);
	f_check<std::wstring>(a0.v_p, L"argument0");
	a_stack[0].f_construct(t_symbol::f_instantiate(f_as<const std::wstring&>(a0.v_p)));
}

}
