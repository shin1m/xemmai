#include <xemmai/symbol.h>

#include <xemmai/engine.h>
#include <xemmai/throwable.h>
#include <xemmai/convert.h>

namespace xemmai
{

t_transfer t_symbol::f_instantiate(const std::wstring& a_value)
{
	portable::t_scoped_lock lock(f_engine()->v_symbol__instantiate__mutex);
	std::map<std::wstring, t_slot>& instances = f_engine()->v_symbol__instances;
	f_engine()->v_object__reviving__mutex.f_acquire();
	f_engine()->v_symbol__instances__mutex.f_acquire();
	std::map<std::wstring, t_slot>::iterator i = instances.lower_bound(a_value);
	if (i != instances.end() && i->first == a_value) {
		f_engine()->v_object__reviving = true;
		f_as<t_thread*>(t_thread::f_current())->v_queues->f_revive();
		f_engine()->v_symbol__instances__mutex.f_release();
		f_engine()->v_object__reviving__mutex.f_release();
		return i->second;
	}
	f_engine()->v_symbol__instances__mutex.f_release();
	f_engine()->v_object__reviving__mutex.f_release();
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_symbol>());
	t_transfer second = static_cast<t_object*>(object);
	{
		portable::t_scoped_lock lock(f_engine()->v_symbol__instances__mutex);
		object->v_pointer = new std::map<std::wstring, t_slot>::iterator(instances.insert(i, std::make_pair(a_value, t_slot())));
		f_as<std::map<std::wstring, t_slot>::iterator&>(object)->second = second;
	}
	return object;
}

void t_symbol::f_define(t_object* a_class)
{
	t_define<t_symbol, t_object>(f_global(), L"Symbol", a_class)
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(t_object*), f_string>())
	;
}

t_type* t_symbol::f_derive(t_object* a_this)
{
	return 0;
}

void t_symbol::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<std::map<std::wstring, t_slot>::iterator&>(a_this)->second);
}

void t_symbol::f_finalize(t_object* a_this)
{
	portable::t_scoped_lock lock(f_engine()->v_symbol__instances__mutex);
	std::map<std::wstring, t_slot>::iterator* p = f_as<std::map<std::wstring, t_slot>::iterator*>(a_this);
	f_engine()->v_symbol__instances.erase(*p);
	delete p;
}

void t_symbol::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_transfer a0 = a_stack.f_pop();
	f_check<std::wstring>(a0, L"argument0");
	a_stack.f_return(f_instantiate(f_as<const std::wstring&>(a0)));
}

}
