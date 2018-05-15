#include <xemmai/type.h>

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

void t_type::f_initialize(t_object* a_module, t_stacked* a_stack, size_t a_n)
{
	a_n += 2;
	for (size_t i = 1; i < a_n; ++i) a_stack[i].f_destruct();
	a_stack[0].f_construct();
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
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(const t_value&), f_hash>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(const t_value&, const t_value&), f_not_equals>())
		(L"own", t_member<void(*)(const t_value&), f_own>())
		(L"share", t_member<void(*)(const t_value&), f_share>())
	;
}

t_type* t_type::f_derive(t_object* a_this)
{
	auto p = new t_type(t_scoped(v_module), a_this);
	p->v_primitive = true;
	return p;
}

void t_type::f_scan(t_object* a_this, t_scan a_scan)
{
}

void t_type::f_finalize(t_object* a_this)
{
}

t_scoped t_type::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_object::f_allocate(a_class);
}

void t_type::f_instantiate(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	t_scoped object;
	try {
		object = f_as<t_type&>(a_class).f_construct(a_class, a_stack, a_n);
	} catch (...) {
		t_destruct_n(a_stack, a_n);
		throw;
	}
	object.f_call(f_global()->f_symbol_initialize(), a_stack, a_n);
	a_stack[0] = std::move(object);
}

t_scoped t_type::f_get(const t_value& a_this, t_object* a_key)
{
	t_object* p = static_cast<t_object*>(a_this);
	if (a_this.f_tag() >= t_value::e_tag__OBJECT) {
		if (p->f_owned()) {
			intptr_t index = p->f_field_index(a_key);
			if (index >= 0) return p->f_field_get(index);
			t_scoped value = a_this.f_type()->f_get(a_key);
			return value.f_type() == f_global()->f_type<t_method>() ? f_as<t_method&>(value).f_bind(t_scoped(a_this)) : value;
		}
		if (!p->f_shared()) t_throwable::f_throw(L"owned by another thread.");
	}
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	auto& cache = t_thread::v_cache[i];
	auto& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == a_this && static_cast<t_object*>(cache.v_key) == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	intptr_t index = -1;
	t_scoped value;
	if (a_this.f_tag() >= t_value::e_tag__OBJECT) {
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
		auto& cache = t_thread::v_cache[i];
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
	} catch (const t_scoped&) {
		f_as<t_fiber&>(t_fiber::f_current()).f_caught({});
		return false;
	}
}

t_scoped t_type::f_remove(t_object* a_this, t_object* a_key)
{
	if (a_this->f_owned()) {
		intptr_t index = a_this->f_field_index(a_key);
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
			intptr_t index = a_this->f_field_index(a_key);
			if (index < 0) t_throwable::f_throw(f_as<t_symbol&>(a_key).f_string());
			value = a_this->f_field_get(index);
			a_this->f_field_remove(index);
		}
		size_t i = t_thread::t_cache::f_index(a_this, a_key);
		auto& cache = t_thread::v_cache[i];
		if (static_cast<t_object*>(cache.v_object) == a_this && static_cast<t_object*>(cache.v_key) == a_key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
		cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
		return value;
	}
}

void t_type::f_hash(t_object* a_this, t_stacked* a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash(), a_stack);
	t_scoped x = std::move(a_stack[0]);
	x.f_call(a_stack, 0);
}

size_t t_type::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	try {
		a_this->f_get(f_global()->f_symbol_call(), a_stack);
	} catch (...) {
		a_n += 2;
		for (size_t i = 2; i < a_n; ++i) a_stack[i].f_destruct();
		throw;
	}
	t_scoped x = std::move(a_stack[0]);
	return x.f_call_without_loop(a_stack, a_n);
}

#define XEMMAI__TYPE__METHOD(a_method, a_n)\
size_t t_type::f_##a_method(t_object* a_this, t_stacked* a_stack)\
{\
	try {\
		a_this->f_get(f_global()->f_symbol_##a_method(), a_stack);\
		return a_n;\
	} catch (...) {\
		for (size_t i = 2; i < a_n + 2; ++i) a_stack[i].f_destruct();\
		throw;\
	}\
}

XEMMAI__TYPE__METHOD(get_at, 1)
XEMMAI__TYPE__METHOD(set_at, 2)
XEMMAI__TYPE__METHOD(plus, 0)
XEMMAI__TYPE__METHOD(minus, 0)
XEMMAI__TYPE__METHOD(not, 0)
XEMMAI__TYPE__METHOD(complement, 0)
XEMMAI__TYPE__METHOD(multiply, 1)
XEMMAI__TYPE__METHOD(divide, 1)
XEMMAI__TYPE__METHOD(modulus, 1)
XEMMAI__TYPE__METHOD(add, 1)
XEMMAI__TYPE__METHOD(subtract, 1)
XEMMAI__TYPE__METHOD(left_shift, 1)
XEMMAI__TYPE__METHOD(right_shift, 1)
XEMMAI__TYPE__METHOD(less, 1)
XEMMAI__TYPE__METHOD(less_equal, 1)
XEMMAI__TYPE__METHOD(greater, 1)
XEMMAI__TYPE__METHOD(greater_equal, 1)
XEMMAI__TYPE__METHOD(equals, 1)
XEMMAI__TYPE__METHOD(not_equals, 1)
XEMMAI__TYPE__METHOD(and, 1)
XEMMAI__TYPE__METHOD(xor, 1)
XEMMAI__TYPE__METHOD(or, 1)
XEMMAI__TYPE__METHOD(send, 1)

void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name)
{
	t_throwable::f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

}
