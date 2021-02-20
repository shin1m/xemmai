#include <xemmai/convert.h>

namespace xemmai
{

void t_class::f_do_scan(t_object* a_this, t_scan a_scan)
{
	auto& p = a_this->f_as<t_type>();
	if (p.v_builtin && f_engine()->f_module_global()) return;
	a_scan(p.v_this);
	a_scan(p.v_module);
	a_scan(p.v_super);
}

void t_class::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	if (a_n > 1) f_throw(L"must be called with or without an argument."sv);
	t_object* x;
	if (a_n > 0) {
		x = a_stack[2];
		if (x->f_type() != f_global()->f_type<t_class>()) f_throw(L"must be class."sv);
	} else {
		x = t_object::f_of(f_global()->f_type<t_object>());
	}
	auto& p = f_as<t_type&>(x);
	auto type = (p.*p.v_derive)();
	if (!type) f_throw(L"underivable."sv);
	a_stack[0] = type;
}

void t_class::f_do_get_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack)
{
	a_stack[0] = f_do_get(a_this, a_key);
	a_stack[1] = nullptr;
}

t_pvalue t_class::f_do_get(t_object* a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	auto& cache = t_thread::v_cache[i];
	auto& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == a_this && cache.v_key == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	t_pvalue value;
	auto type = a_this;
	while (true) {
		{
			t_with_lock_for_read lock(type);
			intptr_t index = type->f_field_index(a_key);
			if (index >= 0) {
				auto& slot = type->f_field_get(index);
				t_object* p = slot;
				if (reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT && (f_is<t_lambda>(p) || p->f_type() == f_global()->f_type<t_native>()))
					value = t_method::f_instantiate(f_global()->f_type<t_method>(), p, a_this);
				else
					value = slot;
				break;
			}
		}
		type = static_cast<t_slot&>(f_as<t_type&>(type).v_super);
		if (!type) f_throw(f_as<t_symbol&>(a_key).f_string());
	}
	cache.v_object = a_this;
	cache.v_key = a_key;
	return cache.v_value = value;
}

void t_class::f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	{
		t_with_lock_for_write lock(a_this);
		a_this->f_field_put(a_key, a_value);
	}
	t_symbol::f_revise(a_key);
}

t_pvalue t_class::f_do_remove(t_object* a_this, t_object* a_key)
{
	t_pvalue value;
	{
		t_with_lock_for_write lock(a_this);
		intptr_t index = a_this->f_field_index(a_key);
		if (index < 0) f_throw(f_as<t_symbol&>(a_key).f_string());
		value = a_this->f_field_get(index);
		a_this->f_field_remove(index);
	}
	t_symbol::f_revise(a_key);
	return value;
}

void t_class::f_do_call_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n)
{
	static_cast<t_class*>(a_this->v_type)->f_do_get(a_this, a_key).f_call(a_stack, a_n);
}

size_t t_class::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	auto& p = f_as<t_type&>(a_this);
	(p.*p.v_instantiate)(a_stack, a_n);
	return -1;
}

size_t t_class::f_do_send(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[1] = a_this;
	a_stack[2].f_call(a_stack, 0);
	a_stack[0] = a_this;
	return -1;
}

}
