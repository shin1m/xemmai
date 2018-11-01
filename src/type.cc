#include <xemmai/type.h>

#include <xemmai/convert.h>

namespace xemmai
{

constexpr decltype(t_type::V_ids) t_type::V_ids;

void t_type::f_initialize(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n)
{
	a_n += 2;
	for (size_t i = 1; i < a_n; ++i) a_stack[i].f_destruct();
	a_stack[0].f_construct();
}

t_scoped t_type::f_string(const t_value& a_self)
{
	wchar_t cs[13 + sizeof(t_object*) * 2];
	size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
	return t_string::f_instantiate(cs, n);
}

void t_type::f_own(const t_value& a_self)
{
	if (a_self.f_tag() >= t_value::e_tag__OBJECT) static_cast<t_object*>(a_self)->f_own();
}

void t_type::f_share(const t_value& a_self)
{
	if (a_self.f_tag() >= t_value::e_tag__OBJECT) static_cast<t_object*>(a_self)->f_share();
}

void t_type::f_define()
{
	v_builtin = v_primitive = true;
	t_define<t_object, t_object>(f_global(), L"Object"sv, t_object::f_of(this))
		(f_global()->f_symbol_initialize(), f_initialize)
		(f_global()->f_symbol_string(), t_member<t_scoped(*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(const t_value&), f__hash>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(const t_value&, const t_value&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(const t_value&, const t_value&), f__not_equals>())
		(L"own"sv, t_member<void(*)(const t_value&), f_own>())
		(L"share"sv, t_member<void(*)(const t_value&), f_share>())
	;
}

t_scoped t_type::f_do_derive()
{
	auto p = f_derive<t_type>();
	p->f_as<t_type>().v_primitive = true;
	return p;
}

bool t_type::f_derives(t_type* a_type)
{
	auto p = this;
	do {
		if (p == a_type) return true;
		p = p->v_super;
	} while (p);
	return false;
}

t_scoped t_type::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_object::f_allocate(this, false, 0);
}

void t_type::f_do_instantiate(t_stacked* a_stack, size_t a_n)
{
	t_scoped value = f_do_or_destruct([&]
	{
		return f_construct(a_stack, a_n);
	}, a_stack, a_n);
	value.f_call(f_global()->f_symbol_initialize(), a_stack, a_n);
	a_stack[0] = std::move(value);
}

t_scoped t_type::f_get_nonowned(t_object* a_this, t_object* a_key)
{
	if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	auto& cache = t_thread::v_cache[i];
	auto& symbol = f_as<t_symbol&>(a_key);
	if (static_cast<t_object*>(cache.v_object) == a_this && static_cast<t_object*>(cache.v_key) == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	intptr_t index = -1;
	t_scoped value;
	{
		t_with_lock_for_read lock(a_this);
		if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
		index = a_this->f_field_index(a_key);
		if (index >= 0) value = a_this->f_field_get(index);
	}
	if (index < 0) {
		value = f_get_of_type(a_key);
		if (value.f_type() == f_global()->f_type<t_method>()) value = f_as<t_method&>(value).f_bind(a_this);
	}
	cache.v_object = a_this;
	cache.v_key = a_key;
	return cache.v_value = std::move(value);
}

void t_type::f_do_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack)
{
	a_stack[0].f_construct(f_get_nonowned(a_this, a_key));
	a_stack[1].f_construct();
}

t_scoped t_type::f_do_get(t_object* a_this, t_object* a_key)
{
	assert(reinterpret_cast<size_t>(a_this) >= t_value::e_tag__OBJECT);
	if (!a_this->f_owned()) return f_get_nonowned(a_this, a_key);
	intptr_t index = a_this->f_field_index(a_key);
	if (index >= 0) return a_this->f_field_get(index);
	t_scoped value = f_get_of_type(a_key);
	return value.f_type() == f_global()->f_type<t_method>() ? f_as<t_method&>(value).f_bind(a_this) : value;
}

void t_type::f_do_put(t_object* a_this, t_object* a_key, t_scoped&& a_value)
{
	if (a_this->f_owned()) {
		a_this->f_field_put(a_key, std::move(a_value));
	} else {
		{
			t_with_lock_for_write lock(a_this);
			if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
			a_this->f_field_put(a_key, std::move(a_value));
		}
		size_t i = t_thread::t_cache::f_index(a_this, a_key);
		auto& cache = t_thread::v_cache[i];
		if (static_cast<t_object*>(cache.v_object) == a_this && static_cast<t_object*>(cache.v_key) == a_key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
		cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
	}
}

bool t_type::f_do_has(t_object* a_this, t_object* a_key)
{
	try {
		f_get(a_this, a_key);
		return true;
	} catch (const t_scoped&) {
		f_as<t_fiber&>(t_fiber::f_current()).f_caught({}, nullptr);
		return false;
	}
}

t_scoped t_type::f_do_remove(t_object* a_this, t_object* a_key)
{
	if (a_this->f_owned()) {
		intptr_t index = a_this->f_field_index(a_key);
		if (index < 0) f_throw(f_as<t_symbol&>(a_key).f_string());
		t_scoped value = a_this->f_field_get(index);
		a_this->f_field_remove(index);
		return value;
	} else {
		t_scoped value;
		{
			t_with_lock_for_write lock(a_this);
			if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
			intptr_t index = a_this->f_field_index(a_key);
			if (index < 0) f_throw(f_as<t_symbol&>(a_key).f_string());
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

void t_type::f_do_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n)
{
	f_do_or_destruct([&]
	{
		return a_this->f_type()->f_get_nonowned(a_this, a_key);
	}, a_stack, a_n).f_call(a_stack, a_n);
}

size_t t_type::f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
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

void t_type::f_do_hash(t_object* a_this, t_stacked* a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash(), a_stack);
	t_scoped x = std::move(a_stack[0]);
	x.f_call(a_stack, 0);
}

#define XEMMAI__TYPE__METHOD(a_method, a_n)\
size_t t_type::f_do_##a_method(t_object* a_this, t_stacked* a_stack)\
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
	f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

void t_type_immutable::f_do_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack)
{
	xemmai::f_get_of_type(*a_this, a_key, a_stack);
}

t_scoped t_type_immutable::f_do_get(t_object* a_this, t_object* a_key)
{
	assert(reinterpret_cast<size_t>(a_this) >= t_value::e_tag__OBJECT);
	t_scoped value = f_get_of_type(a_key);
	return value.f_type() == f_global()->f_type<t_method>() ? f_as<t_method&>(value).f_bind(a_this) : value;
}

void t_type_immutable::f_do_put(t_object* a_this, t_object* a_key, t_scoped&& a_value)
{
	f_throw(L"immutable."sv);
}

bool t_type_immutable::f_do_has(t_object* a_this, t_object* a_key)
{
	return false;
}

t_scoped t_type_immutable::f_do_remove(t_object* a_this, t_object* a_key)
{
	f_throw(L"immutable."sv);
}

void t_type_immutable::f_do_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n)
{
	f_call_of_type(*a_this, a_key, a_stack, a_n);
}

}
