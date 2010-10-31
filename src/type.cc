#include <xemmai/type.h>

#include <xemmai/thread.h>
#include <xemmai/method.h>
#include <xemmai/throwable.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

bool t_type::f_derives(t_object* a_this, t_object* a_type)
{
	t_object* p = a_this;
	do {
		if (p == a_type) return true;
		p = f_as<t_type&>(p).v_super;
	} while (p);
	return false;
}

void t_type::f_construct(t_object* a_module, const t_value& a_self, size_t a_n)
{
	if (a_self.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	f_as<t_type&>(a_self).f_construct(a_self, a_n);
}

void t_type::f_initialize(t_object* a_module, const t_value& a_self, size_t a_n)
{
	t_stack* stack = f_stack();
	while (a_n > 0) {
		stack->f_pop();
		--a_n;
	}
	stack->f_return();
}

void t_type::f_define(t_object* a_class)
{
	t_define<t_object, t_object>(f_global(), L"Object", a_class)
		(f_global()->f_symbol_construct(), f_construct)
		(f_global()->f_symbol_initialize(), f_initialize)
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(const t_value&), f_hash>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
	;
}

t_type::~t_type_of()
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

void t_type::f_construct(t_object* a_class, size_t a_n)
{
	t_stack* stack = f_stack();
	while (a_n > 0) {
		stack->f_pop();
		--a_n;
	}
	stack->f_return(t_object::f_allocate(a_class));
}

void t_type::f_instantiate(t_object* a_class, size_t a_n)
{
	t_transfer object = a_class->f_get(f_global()->f_symbol_construct()).f_call_with_same(a_n);
	object.f_get(f_global()->f_symbol_initialize()).f_call_and_return(t_value(), a_n);
	f_stack()->f_top() = object;
}

t_transfer t_type::f_get(const t_value& a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_symbol& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == a_this && static_cast<t_object*>(cache.v_key) == a_key) {
		if (cache.v_key_revision == symbol.v_revision) return cache.v_value;
		if (cache.v_modified) {
			cache.v_key_revision = symbol.v_revision;
			return cache.v_value;
		}
	}
	cache.v_key_revision = symbol.v_revision;
	t_hash::t_entry* field = 0;
	t_transfer value;
	if (a_this.f_tag() >= t_value::e_tag__OBJECT) {
		t_with_lock_for_read lock(a_this);
		field = static_cast<t_object*>(a_this)->v_fields.f_find<t_object::t_hash_traits>(a_key);
		if (field) value = field->v_value;
	}
	if (!field) {
		value = a_this.f_type()->f_get(a_key);
		if (value.f_type() == f_global()->f_type<t_method>()) value = f_as<t_method&>(value).f_bind(a_this);
	}
	if (cache.v_modified) {
		{
			t_with_lock_for_write lock(cache.v_object);
			static_cast<t_object*>(cache.v_object)->v_fields.f_put<t_object::t_hash_traits>(cache.v_key, cache.v_value.f_transfer());
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
	if (static_cast<t_object*>(cache.v_object) != a_this || static_cast<t_object*>(cache.v_key) != a_key) {
		if (cache.v_modified) {
			{
				t_with_lock_for_write lock(cache.v_object);
				static_cast<t_object*>(cache.v_object)->v_fields.f_put<t_object::t_hash_traits>(cache.v_key, cache.v_value.f_transfer());
			}
			cache.v_revision = t_thread::t_cache::f_revise(i);
		}
		cache.v_object = a_this;
		cache.v_key = a_key;
	}
	cache.v_value = a_value;
	cache.v_modified = true;
	cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
}

bool t_type::f_has(const t_value& a_this, t_object* a_key)
{
	try {
		f_get(a_this, a_key);
		return true;
	} catch (const t_scoped& thrown) {
		f_as<t_fiber&>(t_fiber::f_current()).f_caught(thrown);
		return false;
	}
}

t_transfer t_type::f_remove(t_object* a_this, t_object* a_key)
{
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_transfer value;
	if (static_cast<t_object*>(cache.v_object) == a_this && static_cast<t_object*>(cache.v_key) == a_key) {
		{
			portable::t_scoped_lock_for_write lock(a_this->v_lock);
			a_this->v_fields.f_remove<t_object::t_hash_traits>(a_key);
		}
		cache.v_object = cache.v_key = 0;
		value = cache.v_value.f_transfer();
		cache.v_modified = false;
	} else {
		portable::t_scoped_lock_for_write lock(a_this->v_lock);
		std::pair<bool, t_transfer> pair = a_this->v_fields.f_remove<t_object::t_hash_traits>(a_key);
		if (!pair.first) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
		value = pair.second;
	}
	cache.v_revision = t_thread::t_cache::f_revise(i);
	cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
	return value;
}

void t_type::f_hash(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_hash()).f_call(t_value(), 0);
}

void t_type::f_call(t_object* a_this, const t_value& a_self, size_t a_n)
{
	a_this->f_get(f_global()->f_symbol_call()).f_call(t_value(), a_n);
}

void t_type::f_get_at(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_get_at()).f_call(t_value(), 1);
}

void t_type::f_set_at(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_set_at()).f_call(t_value(), 2);
}

void t_type::f_plus(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_plus()).f_call(t_value(), 0);
}

void t_type::f_minus(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_minus()).f_call(t_value(), 0);
}

void t_type::f_not(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_not()).f_call(t_value(), 0);
}

void t_type::f_complement(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_complement()).f_call(t_value(), 0);
}

void t_type::f_multiply(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_multiply()).f_call(t_value(), 1);
}

void t_type::f_divide(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_divide()).f_call(t_value(), 1);
}

void t_type::f_modulus(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_modulus()).f_call(t_value(), 1);
}

void t_type::f_add(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_add()).f_call(t_value(), 1);
}

void t_type::f_subtract(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_subtract()).f_call(t_value(), 1);
}

void t_type::f_left_shift(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_left_shift()).f_call(t_value(), 1);
}

void t_type::f_right_shift(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_right_shift()).f_call(t_value(), 1);
}

void t_type::f_less(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_less()).f_call(t_value(), 1);
}

void t_type::f_less_equal(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_less_equal()).f_call(t_value(), 1);
}

void t_type::f_greater(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_greater()).f_call(t_value(), 1);
}

void t_type::f_greater_equal(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_greater_equal()).f_call(t_value(), 1);
}

void t_type::f_equals(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_equals()).f_call(t_value(), 1);
}

void t_type::f_not_equals(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_not_equals()).f_call(t_value(), 1);
}

void t_type::f_and(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_and()).f_call(t_value(), 1);
}

void t_type::f_xor(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_xor()).f_call(t_value(), 1);
}

void t_type::f_or(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_or()).f_call(t_value(), 1);
}

void t_type::f_send(t_object* a_this)
{
	a_this->f_get(f_global()->f_symbol_send()).f_call(t_value(), 1);
}

void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name)
{
	t_throwable::f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

}
