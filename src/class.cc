#include <xemmai/class.h>

#include <xemmai/engine.h>
#include <xemmai/symbol.h>
#include <xemmai/method.h>
#include <xemmai/throwable.h>

namespace xemmai
{

t_transfer t_class::f_instantiate(t_type* a_type)
{
	t_transfer object = t_object::f_allocate(f_engine()->v_type_class);
	object->v_pointer = a_type;
	return object;
}

t_type* t_class::f_derive(t_object* a_this)
{
	return 0;
}

void t_class::f_scan(t_object* a_this, t_scan a_scan)
{
	t_type* p = f_as<t_type*>(a_this);
	if (p->v_builtin && f_engine()->f_module_global()) return;
	a_scan(p->v_module);
	a_scan(p->v_super);
}

void t_class::f_finalize(t_object* a_this)
{
	delete f_as<t_type*>(a_this);
}

void t_class::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	if (a_n > 1) t_throwable::f_throw(L"must be called with or without an argument.");
	t_transfer x;
	if (a_n > 0) {
		x = a_stack.f_pop();
		if (x->f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	} else {
		x = f_global()->f_type<t_object>();
	}
	t_type* type = f_as<t_type*>(x)->f_derive(x);
	if (!type) t_throwable::f_throw(L"underivable.");
	a_stack.f_return(f_instantiate(type));
}

t_transfer t_class::f_get(t_object* a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_symbol* symbol = f_as<t_symbol*>(a_key);
	if (cache.v_object == a_this && cache.v_key == a_key && cache.v_key_revision == symbol->v_revision) return cache.v_value;
	cache.v_key_revision = symbol->v_revision;
	t_transfer value;
	t_object* type = a_this;
	do {
		portable::t_scoped_lock_for_read lock(type->v_lock);
		t_hash::t_entry* field = type->v_fields.f_find<t_object::t_hash_traits>(a_key);
		if (field) {
			t_object* p = field->v_value;
			if (p->f_type() == f_global()->f_type<t_lambda>() || p->f_type() == f_global()->f_type<t_native>())
				value = t_method::f_instantiate(p, a_this);
			else
				value = p;
			break;
		}
	} while (type = f_as<t_type*>(type)->v_super);
	if (!value) t_throwable::f_throw(f_as<t_symbol*>(a_key)->f_string());
	if (cache.v_modified) {
		{
			portable::t_scoped_lock_for_write lock(cache.v_object->v_lock);
			cache.v_object->v_fields.f_put<t_object::t_hash_traits>(cache.v_key, cache.v_value.f_transfer());
		}
		cache.v_modified = false;
		cache.v_revision = t_thread::t_cache::f_revise(i);
	}
	cache.v_object = a_this;
	cache.v_key = a_key;
	return cache.v_value = value;
}

void t_class::f_put(t_object* a_this, t_object* a_key, const t_transfer& a_value)
{
	{
		portable::t_scoped_lock_for_write lock(a_this->v_lock);
		a_this->v_fields.f_put<t_object::t_hash_traits>(a_key, a_value);
	}
	t_symbol::f_revise(a_key);
}

t_transfer t_class::f_remove(t_object* a_this, t_object* a_key)
{
	t_transfer value;
	{
		portable::t_scoped_lock_for_write lock(a_this->v_lock);
		value = a_this->v_fields.f_remove<t_object::t_hash_traits>(a_key);
	}
	if (!value) t_throwable::f_throw(f_as<t_symbol*>(a_key)->f_string());
	t_symbol::f_revise(a_key);
	return value;
}

void t_class::f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	t_native_context context;
	f_as<t_type*>(a_this)->f_instantiate(a_this, a_n, a_stack);
	context.f_done();
}

}
