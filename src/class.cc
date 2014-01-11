#include <xemmai/class.h>

#include <xemmai/symbol.h>
#include <xemmai/method.h>
#include <xemmai/lambda.h>
#include <xemmai/throwable.h>
#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_class::f_instantiate(t_type* a_type)
{
	t_scoped object = t_object::f_allocate(f_engine()->v_type_class);
	object.f_pointer__(a_type);
	return object;
}

t_type* t_class::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_class::f_scan(t_object* a_this, t_scan a_scan)
{
	t_type& p = f_as<t_type&>(a_this);
	if (p.v_builtin && f_engine()->f_module_global()) return;
	a_scan(p.v_module);
	a_scan(p.v_super);
}

void t_class::f_finalize(t_object* a_this)
{
	delete &f_as<t_type&>(a_this);
}

void t_class::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	if (a_n > 1) t_throwable::f_throw(L"must be called with or without an argument.");
	t_scoped x;
	if (a_n > 0) {
		x = std::move(a_stack[1]);
		if (x.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	} else {
		x = f_global()->f_type<t_object>();
	}
	t_type* type = f_as<t_type&>(x).f_derive(x);
	if (!type) t_throwable::f_throw(L"underivable.");
	a_stack[0].f_construct(f_instantiate(type));
}

t_scoped t_class::f_get(const t_value& a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_symbol& symbol = f_as<t_symbol&>(a_key);
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
			ptrdiff_t index = type->f_field_index(a_key);
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
		type = f_as<t_type&>(type).v_super;
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
		ptrdiff_t index = a_this->f_field_index(a_key);
		if (index < 0) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
		value = a_this->f_field_get(index);
		a_this->f_field_remove(index);
	}
	t_symbol::f_revise(a_key);
	return value;
}

void t_class::f_call(t_object* a_this, t_slot* a_stack, size_t a_n)
{
	t_native_context context;
	f_as<t_type&>(a_this).f_instantiate(a_this, a_stack, a_n);
	context.f_done();
}

void t_class::f_send(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	a_stack[0].f_construct(a_this);
	t_scoped a0 = std::move(a_stack[1]);
	a0.f_call_and_return(a_stack, 0);
	a_stack[0] = a_this;
	context.f_done();
}

}
