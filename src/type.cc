#include <xemmai/convert.h>

namespace xemmai
{

void t_type::f_initialize(xemmai::t_extension* a_extension, t_pvalue* a_stack, size_t a_n)
{
	a_stack[0] = nullptr;
}

t_object* t_type::f_string(const t_pvalue& a_self)
{
	wchar_t cs[13 + sizeof(t_object*) * 2];
	size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
	return t_string::f_instantiate(cs, n);
}

void t_type::f_own(const t_pvalue& a_self)
{
	auto p = static_cast<t_object*>(a_self);
	if (reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT) p->f_own();
}

void t_type::f_share(const t_pvalue& a_self)
{
	auto p = static_cast<t_object*>(a_self);
	if (reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT) p->f_share();
}

void t_type::f_define()
{
	v_builtin = v_primitive = true;
	t_define<t_object, t_object>(f_global(), L"Object"sv, t_object::f_of(this))
		(f_global()->f_symbol_initialize(), f_initialize)
		(f_global()->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(const t_pvalue&), f__hash>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(const t_pvalue&, const t_pvalue&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(const t_pvalue&, const t_pvalue&), f__not_equals>())
		(L"own"sv, t_member<void(*)(const t_pvalue&), f_own>())
		(L"share"sv, t_member<void(*)(const t_pvalue&), f_share>())
	;
}

t_object* t_type::f_do_derive()
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

t_pvalue t_type::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	auto p = f_engine()->f_allocate(false, 0);
	p->f_be(this);
	return p;
}

void t_type::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	auto value = f_construct(a_stack, a_n);
	value.f_call(f_global()->f_symbol_initialize(), a_stack, a_n);
	a_stack[0] = value;
}

t_pvalue t_type::f_get_nonowned(t_object* a_this, t_object* a_key)
{
	if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
	size_t i = t_thread::t_cache::f_index(a_this, a_key);
	auto& cache = t_thread::v_cache[i];
	auto& symbol = f_as<t_symbol&>(a_key);
	if (cache.v_object == a_this && cache.v_key == a_key && cache.v_key_revision == symbol.v_revision) {
		++t_thread::v_cache_hit;
		return cache.v_value;
	}
	++t_thread::v_cache_missed;
	cache.v_key_revision = symbol.v_revision;
	intptr_t index = -1;
	t_pvalue value;
	{
		t_with_lock_for_read lock(a_this);
		if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
		index = a_this->f_field_index(a_key);
		if (index >= 0) value = a_this->f_field_get(index);
	}
	if (index < 0) value = t_method::f_bind(f_get_of_type(a_key), a_this);
	cache.v_object = a_this;
	cache.v_key = a_key;
	return cache.v_value = value;
}

void t_type::f_do_get_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack)
{
	a_stack[0] = f_get_nonowned(a_this, a_key);
	a_stack[1] = nullptr;
}

t_pvalue t_type::f_do_get(t_object* a_this, t_object* a_key)
{
	assert(reinterpret_cast<uintptr_t>(a_this) >= e_tag__OBJECT);
	if (!a_this->f_owned()) return f_get_nonowned(a_this, a_key);
	intptr_t index = a_this->f_field_index(a_key);
	return index < 0 ? t_method::f_bind(f_get_of_type(a_key), a_this) : t_pvalue(a_this->f_field_get(index));
}

void t_type::f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	if (a_this->f_owned()) {
		a_this->f_field_put(a_key, a_value);
	} else {
		{
			t_with_lock_for_write lock(a_this);
			if (!a_this->f_shared()) f_throw(L"owned by another thread."sv);
			a_this->f_field_put(a_key, a_value);
		}
		size_t i = t_thread::t_cache::f_index(a_this, a_key);
		auto& cache = t_thread::v_cache[i];
		if (cache.v_object == a_this && cache.v_key == a_key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
		cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
	}
}

bool t_type::f_do_has(t_object* a_this, t_object* a_key)
{
	try {
		f_get(a_this, a_key);
		return true;
	} catch (const t_rvalue&) {
		f_as<t_fiber&>(t_fiber::f_current()).f_caught({}, nullptr);
		return false;
	}
}

t_pvalue t_type::f_do_remove(t_object* a_this, t_object* a_key)
{
	if (a_this->f_owned()) {
		intptr_t index = a_this->f_field_index(a_key);
		if (index < 0) f_throw(f_as<t_symbol&>(a_key).f_string());
		t_pvalue value = a_this->f_field_get(index);
		a_this->f_field_remove(index);
		return value;
	} else {
		t_pvalue value;
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
		if (cache.v_object == a_this && cache.v_key == a_key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(i);
		cache.v_key_revision = f_as<t_symbol&>(a_key).v_revision;
		return value;
	}
}

void t_type::f_do_call_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n)
{
	a_this->f_type()->f_get_nonowned(a_this, a_key).f_call(a_stack, a_n);
}

size_t t_type::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	a_this->f_get(f_global()->f_symbol_call(), a_stack);
	return a_stack[0].f_call_without_loop(a_stack, a_n);
}

void t_type::f_do_hash(t_object* a_this, t_pvalue* a_stack)
{
	a_this->f_get(f_global()->f_symbol_hash(), a_stack);
	a_stack[0].f_call(a_stack, 0);
}

#define XEMMAI__TYPE__METHOD(a_method, a_n)\
size_t t_type::f_do_##a_method(t_object* a_this, t_pvalue* a_stack)\
{\
	a_this->f_get(f_global()->f_symbol_##a_method(), a_stack);\
	return a_n;\
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

void t_type_immutable::f_do_get_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack)
{
	xemmai::f_get_of_type(*a_this, a_key, a_stack);
}

t_pvalue t_type_immutable::f_do_get(t_object* a_this, t_object* a_key)
{
	assert(reinterpret_cast<uintptr_t>(a_this) >= e_tag__OBJECT);
	return t_method::f_bind(f_get_of_type(a_key), a_this);
}

void t_type_immutable::f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	f_throw(L"immutable."sv);
}

bool t_type_immutable::f_do_has(t_object* a_this, t_object* a_key)
{
	return false;
}

t_pvalue t_type_immutable::f_do_remove(t_object* a_this, t_object* a_key)
{
	f_throw(L"immutable."sv);
}

void t_type_immutable::f_do_call_nonowned(t_object* a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n)
{
	f_call_of_type(*a_this, a_key, a_stack, a_n);
}

}
