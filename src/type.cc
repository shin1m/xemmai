#include <xemmai/type.h>

#include <xemmai/thread.h>
#include <xemmai/method.h>
#include <xemmai/throwable.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name)
{
	t_throwable::f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

bool t_type::f_derives(t_object* a_this, t_object* a_type)
{
	t_object* p = a_this;
	do {
		if (p == a_type) return true;
		p = f_as<t_type*>(p)->v_super;
	} while (p);
	return false;
}

void t_type::f_construct(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	if (a_self->f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	f_as<t_type*>(a_self)->f_construct(a_self, a_n, a_stack);
}

void t_type::f_initialize(t_object* a_module, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	while (a_n > 0) {
		a_stack.f_pop();
		--a_n;
	}
	a_stack.f_return(f_global()->f_null());
}

void t_type::f_define(t_object* a_class)
{
	t_define<t_object*, t_object>(f_global(), L"Object", a_class)
		(f_global()->f_symbol_construct(), f_construct)
		(f_global()->f_symbol_initialize(), f_initialize)
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(t_object*), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(t_object*), f_hash>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(t_object*, t_object*), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(t_object*, t_object*), f_not_equals>())
	;
}

t_type::~t_type()
{
}

t_type* t_type::f_derive(t_object* a_this)
{
	t_type* p = new t_derived<t_type>(v_module, a_this);
	p->v_primitive = true;
	return p;
}

void t_type::f_scan(t_object* a_this, t_scan a_scan)
{
}

void t_type::f_finalize(t_object* a_this)
{
}

void t_type::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	while (a_n > 0) {
		a_stack.f_pop();
		--a_n;
	}
	a_stack.f_return(t_object::f_allocate(a_class));
}

void t_type::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_transfer object = a_class->f_get(f_global()->f_symbol_construct())->f_call(a_n, a_stack.v_top);
	object->f_get(f_global()->f_symbol_initialize())->f_call_and_return(0, a_n, a_stack);
	a_stack.f_top() = object;
}

t_transfer t_type::f_get(t_object* a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_symbol* symbol = f_as<t_symbol*>(a_key);
	if (cache.v_object == a_this && cache.v_key == a_key) {
		if (cache.v_key_revision == symbol->v_revision) return cache.v_value;
		if (cache.v_modified) {
			cache.v_key_revision = symbol->v_revision;
			return cache.v_value;
		}
	}
	cache.v_key_revision = symbol->v_revision;
	t_transfer value;
	{
		portable::t_scoped_lock_for_read lock(a_this->v_lock);
		t_hash::t_entry* field = a_this->v_fields.f_find<t_object::t_hash_traits>(a_key);
		if (field) value = field->v_value;
	}
	if (!value) {
		value = a_this->v_type->f_get(a_key);
		if (value->f_type() == f_global()->f_type<t_method>()) value = f_as<t_method*>(value)->f_bind(a_this);
	}
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

void t_type::f_put(t_object* a_this, t_object* a_key, const t_transfer& a_value)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	if (cache.v_object != a_this || cache.v_key != a_key) {
		if (cache.v_modified) {
			{
				portable::t_scoped_lock_for_write lock(cache.v_object->v_lock);
				cache.v_object->v_fields.f_put<t_object::t_hash_traits>(cache.v_key, cache.v_value.f_transfer());
			}
			cache.v_revision = t_thread::t_cache::f_revise(i);
		}
		cache.v_object = a_this;
		cache.v_key = a_key;
	}
	cache.v_value = a_value;
	cache.v_modified = true;
	cache.v_key_revision = f_as<t_symbol*>(a_key)->v_revision;
}

bool t_type::f_has(t_object* a_this, t_object* a_key)
{
	try {
		f_get(a_this, a_key);
		return true;
	} catch (const t_scoped& thrown) {
		f_as<t_fiber*>(t_fiber::f_current())->f_caught(thrown);
		return false;
	}
}

t_transfer t_type::f_remove(t_object* a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_transfer value;
	if (cache.v_object == a_this && cache.v_key == a_key) {
		{
			portable::t_scoped_lock_for_write lock(a_this->v_lock);
			a_this->v_fields.f_remove<t_object::t_hash_traits>(a_key);
		}
		cache.v_object = cache.v_key = 0;
		value = cache.v_value.f_transfer();
		cache.v_modified = false;
	} else {
		portable::t_scoped_lock_for_write lock(a_this->v_lock);
		value = a_this->v_fields.f_remove<t_object::t_hash_traits>(a_key);
		if (!value) t_throwable::f_throw(f_as<t_symbol*>(a_key)->f_string());
	}
	cache.v_revision = t_thread::t_cache::f_revise(i);
	cache.v_key_revision = f_as<t_symbol*>(a_key)->v_revision;
	return value;
}

void t_type::f_hash(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash())->f_call(0, 0, a_stack);
}

void t_type::f_call(t_object* a_this, t_object* a_self, size_t a_n, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_call())->f_call(0, a_n, a_stack);
}

void t_type::f_get_at(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_get_at())->f_call(0, 1, a_stack);
}

void t_type::f_set_at(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_set_at())->f_call(0, 2, a_stack);
}

void t_type::f_plus(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_plus())->f_call(0, 0, a_stack);
}

void t_type::f_minus(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_minus())->f_call(0, 0, a_stack);
}

void t_type::f_not(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_not())->f_call(0, 0, a_stack);
}

void t_type::f_complement(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_complement())->f_call(0, 0, a_stack);
}

void t_type::f_multiply(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_multiply())->f_call(0, 1, a_stack);
}

void t_type::f_divide(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_divide())->f_call(0, 1, a_stack);
}

void t_type::f_modulus(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_modulus())->f_call(0, 1, a_stack);
}

void t_type::f_add(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_add())->f_call(0, 1, a_stack);
}

void t_type::f_subtract(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_subtract())->f_call(0, 1, a_stack);
}

void t_type::f_left_shift(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_left_shift())->f_call(0, 1, a_stack);
}

void t_type::f_right_shift(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_right_shift())->f_call(0, 1, a_stack);
}

void t_type::f_less(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_less())->f_call(0, 1, a_stack);
}

void t_type::f_less_equal(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_less_equal())->f_call(0, 1, a_stack);
}

void t_type::f_greater(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater())->f_call(0, 1, a_stack);
}

void t_type::f_greater_equal(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater_equal())->f_call(0, 1, a_stack);
}

void t_type::f_equals(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_equals())->f_call(0, 1, a_stack);
}

void t_type::f_not_equals(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_not_equals())->f_call(0, 1, a_stack);
}

void t_type::f_and(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_and())->f_call(0, 1, a_stack);
}

void t_type::f_xor(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_xor())->f_call(0, 1, a_stack);
}

void t_type::f_or(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_or())->f_call(0, 1, a_stack);
}

void t_type::f_send(t_object* a_this, t_stack& a_stack)
{
	a_this->f_get(f_global()->f_symbol_send())->f_call(0, 1, a_stack);
}

}
