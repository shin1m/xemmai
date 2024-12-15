#include <xemmai/convert.h>

namespace xemmai
{

t_symbol::~t_symbol()
{
	f_engine()->v_symbol__instances.erase(v_entry);
}

t_object* t_symbol::f_instantiate(std::wstring_view a_value)
{
	auto engine = f_engine();
	std::lock_guard lock(engine->v_symbol__instantiate__mutex);
	engine->v_object__reviving__mutex.lock();
	auto& instances = engine->v_symbol__instances;
	auto i = instances.lower_bound(a_value);
	if (i == instances.end() || i->first != a_value) {
		i = instances.emplace_hint(i, a_value, nullptr);
		engine->v_object__reviving__mutex.unlock();
		return f_new<t_symbol>(f_global(), i);
	} else {
		i->second->v_reviving = true;
		engine->v_object__reviving__mutex.unlock();
		return i->second;
	}
}

void t_type_of<t_symbol>::f_define()
{
	v_builtin = v_revive = true;
	auto global = f_global();
	t_define{global}
		(global->f_symbol_string(), t_member<const std::wstring&(t_symbol::*)() const, &t_symbol::f_string>())
	.f_derive<t_object>(t_object::f_of(this));
}

void t_type_of<t_symbol>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(L"must be called with an argument."sv);
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = t_symbol::f_instantiate(f_as<std::wstring_view>(a_stack[2]));
}

}
