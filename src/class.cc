#include <xemmai/class.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_class::t_type_of(t_type* a_super) : t_type(a_super)
{
	t_value::f_increments()->f_push(v_this);
	t_value::f_increments()->f_push(v_this);
	static_cast<t_object*>(v_this)->v_type = static_cast<t_object*>(v_super->v_this)->v_type = this;
	v_fixed = v_shared = true;
}

t_type* t_class::f_derive()
{
	return nullptr;
}

void t_class::f_scan(t_object* a_this, t_scan a_scan)
{
	auto& p = f_as<t_type&>(a_this);
	if (p.v_builtin && f_engine()->f_module_global()) return;
	a_scan(p.v_this);
	a_scan(p.v_module);
	a_scan(p.v_super);
}

void t_class::f_finalize(t_object* a_this)
{
	delete &f_as<t_type&>(a_this);
}

void t_class::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	if (a_n > 1) t_throwable::f_throw(a_stack, a_n, L"must be called with or without an argument.");
	t_scoped x;
	if (a_n > 0) {
		x = std::move(a_stack[2]);
		if (x.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	} else {
		x = f_global()->f_type<t_object>()->v_this;
	}
	auto type = f_as<t_type&>(x).f_derive();
	if (!type) t_throwable::f_throw(L"underivable.");
	a_stack[0].f_construct(type->v_this);
}

t_scoped t_class::f_get(const t_value& a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	auto& cache = t_thread::v_cache[i];
	auto& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == a_this && static_cast<t_object*>(cache.v_key) == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	t_scoped value;
	t_object* type = a_this;
	while (true) {
		{
			t_with_lock_for_read lock(type);
			intptr_t index = type->f_field_index(a_key);
			if (index >= 0) {
				const t_slot& slot = type->f_field_get(index);
				t_object* p = slot;
				if (reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && (f_is<t_lambda>(p) || p->f_type() == f_global()->f_type<t_native>()))
					value = t_method::f_instantiate(p, t_scoped(a_this));
				else
					value = slot;
				break;
			}
		}
		type = static_cast<t_slot&>(f_as<t_type&>(type).v_super);
		if (!type) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
	}
	cache.v_object = a_this;
	cache.v_key = a_key;
	return cache.v_value = std::move(value);
}

void t_class::f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value)
{
	{
		t_with_lock_for_write lock(a_this);
		a_this->f_field_put(a_key, std::move(a_value));
	}
	t_symbol::f_revise(a_key);
}

t_scoped t_class::f_remove(t_object* a_this, t_object* a_key)
{
	t_scoped value;
	{
		t_with_lock_for_write lock(a_this);
		intptr_t index = a_this->f_field_index(a_key);
		if (index < 0) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
		value = a_this->f_field_get(index);
		a_this->f_field_remove(index);
	}
	t_symbol::f_revise(a_key);
	return value;
}

size_t t_class::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	f_as<t_type&>(a_this).f_instantiate(a_stack, a_n);
	return -1;
}

size_t t_class::f_send(t_object* a_this, t_stacked* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[1].f_construct_nonnull(a_this);
	a0.f_call(a_stack, 0);
	a_stack[0] = a_this;
	return -1;
}

}
