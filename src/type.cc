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

void t_type::f_initialize(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	for (size_t i = 1; i <= a_n; ++i) a_stack[i] = nullptr;
}

void t_type::f_own(const t_value& a_self)
{
	if (a_self.f_tag() >= t_value::e_tag__OBJECT) static_cast<t_object*>(a_self)->f_own();
}

void t_type::f_share(const t_value& a_self)
{
	if (a_self.f_tag() >= t_value::e_tag__OBJECT) static_cast<t_object*>(a_self)->f_share();
}

void t_type::f_define(t_object* a_class)
{
	t_define<t_object, t_object>(f_global(), L"Object", a_class)
		(f_global()->f_symbol_initialize(), f_initialize)
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<ptrdiff_t (*)(const t_value&), f_hash>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
		(L"own", t_member<void (*)(const t_value&), f_own>())
		(L"share", t_member<void (*)(const t_value&), f_share>())
	;
}

t_type* t_type::f_derive(t_object* a_this)
{
	t_type* p = new t_type(t_scoped(v_module), a_this);
	p->v_primitive = true;
	return p;
}

void t_type::f_scan(t_object* a_this, t_scan a_scan)
{
}

void t_type::f_finalize(t_object* a_this)
{
}

t_scoped t_type::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_object::f_allocate(a_class);
}

void t_type::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_scoped object = f_as<t_type&>(a_class).f_construct(a_class, a_stack, a_n);
	object.f_get(f_global()->f_symbol_initialize()).f_call_and_return(t_value(), a_stack, a_n);
	a_stack[0] = std::move(object);
}

t_scoped t_type::f_get(const t_value& a_this, t_object* a_key)
{
	t_object* p = static_cast<t_object*>(a_this);
	if (a_this.f_tag() >= t_value::e_tag__OBJECT) {
		if (p->f_owned()) {
			ptrdiff_t index = p->f_field_index(a_key);
			if (index >= 0) return p->f_field_get(index);
		} else if (!p->f_shared()) {
			t_throwable::f_throw(L"owned by another thread.");
		}
	}
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	t_thread::t_cache& cache = t_thread::v_cache[i];
	t_symbol& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == a_this && static_cast<t_object*>(cache.v_key) == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	ptrdiff_t index = -1;
	t_scoped value;
	if (a_this.f_tag() >= t_value::e_tag__OBJECT && !p->f_owned()) {
		t_with_lock_for_read lock(a_this);
		if (!p->f_shared()) t_throwable::f_throw(L"owned by another thread.");
		index = p->f_field_index(a_key);
		if (index >= 0) value = p->f_field_get(index);
	}
	if (index < 0) {
		value = a_this.f_type()->f_get(a_key);
		if (value.f_type() == f_global()->f_type<t_method>()) value = f_as<t_method&>(value).f_bind(t_scoped(a_this));
	}
	cache.v_object = a_this;
	cache.v_key = a_key;
	return cache.v_value = std::move(value);
}

void t_type::f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value)
{
	if (a_this->f_owned()) {
		a_this->f_field_put(a_key, std::move(a_value));
	} else {
		if (f_as<t_type&>(a_this->f_type()).v_immutable) t_throwable::f_throw(L"immutable.");
		{
			t_with_lock_for_write lock(a_this);
			if (!a_this->f_shared()) t_throwable::f_throw(L"owned by another thread.");
			a_this->f_field_put(a_key, std::move(a_value));
		}
		size_t i = t_thread::t_cache::f_index(a_this, a_key);
		t_thread::t_cache& cache = t_thread::v_cache[i];
		if (static_cast<t_object*>(cache.v_object) == a_this && static_cast<t_object*>(cache.v_key) == a_key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
		cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
	}
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

t_scoped t_type::f_remove(t_object* a_this, t_object* a_key)
{
	if (a_this->f_owned()) {
		ptrdiff_t index = a_this->f_field_index(a_key);
		if (index < 0) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
		t_scoped value = a_this->f_field_get(index);
		a_this->f_field_remove(index);
		return value;
	} else {
		if (f_as<t_type&>(a_this->f_type()).v_immutable) t_throwable::f_throw(L"immutable.");
		t_scoped value;
		{
			t_with_lock_for_write lock(a_this);
			if (!a_this->f_shared()) t_throwable::f_throw(L"owned by another thread.");
			ptrdiff_t index = a_this->f_field_index(a_key);
			if (index < 0) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
			value = a_this->f_field_get(index);
			a_this->f_field_remove(index);
		}
		size_t i = t_thread::t_cache::f_index(a_this, a_key);
		t_thread::t_cache& cache = t_thread::v_cache[i];
		if (static_cast<t_object*>(cache.v_object) == a_this && static_cast<t_object*>(cache.v_key) == a_key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
		cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
		return value;
	}
}

void t_type::f_hash(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash()).f_call(t_value(), a_stack, 0);
}

void t_type::f_call(t_object* a_this, const t_value& a_self, t_slot* a_stack, size_t a_n)
{
	a_this->f_get(f_global()->f_symbol_call()).f_call(t_value(), a_stack, a_n);
}

void t_type::f_get_at(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_get_at()).f_call(t_value(), a_stack, 1);
}

void t_type::f_set_at(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_set_at()).f_call(t_value(), a_stack, 2);
}

void t_type::f_plus(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_plus()).f_call(t_value(), a_stack, 0);
}

void t_type::f_minus(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_minus()).f_call(t_value(), a_stack, 0);
}

void t_type::f_not(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_not()).f_call(t_value(), a_stack, 0);
}

void t_type::f_complement(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_complement()).f_call(t_value(), a_stack, 0);
}

void t_type::f_multiply(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_multiply()).f_call(t_value(), a_stack, 1);
}

void t_type::f_divide(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_divide()).f_call(t_value(), a_stack, 1);
}

void t_type::f_modulus(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_modulus()).f_call(t_value(), a_stack, 1);
}

void t_type::f_add(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_add()).f_call(t_value(), a_stack, 1);
}

void t_type::f_subtract(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_subtract()).f_call(t_value(), a_stack, 1);
}

void t_type::f_left_shift(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_left_shift()).f_call(t_value(), a_stack, 1);
}

void t_type::f_right_shift(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_right_shift()).f_call(t_value(), a_stack, 1);
}

void t_type::f_less(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_less()).f_call(t_value(), a_stack, 1);
}

void t_type::f_less_equal(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_less_equal()).f_call(t_value(), a_stack, 1);
}

void t_type::f_greater(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater()).f_call(t_value(), a_stack, 1);
}

void t_type::f_greater_equal(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_greater_equal()).f_call(t_value(), a_stack, 1);
}

void t_type::f_equals(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_equals()).f_call(t_value(), a_stack, 1);
}

void t_type::f_not_equals(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_not_equals()).f_call(t_value(), a_stack, 1);
}

void t_type::f_and(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_and()).f_call(t_value(), a_stack, 1);
}

void t_type::f_xor(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_xor()).f_call(t_value(), a_stack, 1);
}

void t_type::f_or(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_or()).f_call(t_value(), a_stack, 1);
}

void t_type::f_send(t_object* a_this, t_slot* a_stack)
{
	a_this->f_get(f_global()->f_symbol_send()).f_call(t_value(), a_stack, 1);
}

void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name)
{
	t_throwable::f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

}
