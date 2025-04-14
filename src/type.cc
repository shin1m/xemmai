#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

void f_not_supported(xemmai::t_library* a_library, t_pvalue* a_stack, size_t a_n)
{
	f_throw(L"not supported."sv);
}

}

void t_type::f_define()
{
	v_builtin = true;
	auto global = f_global();
	t_define{global}
	(global->f_symbol_initialize(), +[](xemmai::t_library* a_library, t_pvalue* a_stack, size_t)
	{
		a_stack[0] = nullptr;
	})
	(global->f_symbol_call(), f_not_supported)
	(global->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), [](auto a_self)
	{
		wchar_t cs[13 + sizeof(t_object*) * 2];
		size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
		return t_string::f_instantiate(cs, n);
	}>())
	(global->f_symbol_hash(), t_member<intptr_t(*)(const t_pvalue&), f__hash>())
	(global->f_symbol_get_at(), f_not_supported)
	(global->f_symbol_set_at(), f_not_supported)
	(global->f_symbol_plus(), f_not_supported)
	(global->f_symbol_minus(), f_not_supported)
	(global->f_symbol_complement(), f_not_supported)
	(global->f_symbol_multiply(), f_not_supported)
	(global->f_symbol_divide(), f_not_supported)
	(global->f_symbol_modulus(), f_not_supported)
	(global->f_symbol_add(), f_not_supported)
	(global->f_symbol_subtract(), f_not_supported)
	(global->f_symbol_left_shift(), f_not_supported)
	(global->f_symbol_right_shift(), f_not_supported)
	(global->f_symbol_less(), f_not_supported)
	(global->f_symbol_less_equal(), f_not_supported)
	(global->f_symbol_greater(), f_not_supported)
	(global->f_symbol_greater_equal(), f_not_supported)
	(global->f_symbol_equals(), t_member<bool(*)(const t_pvalue&, const t_pvalue&), f__equals>())
	(global->f_symbol_not_equals(), t_member<bool(*)(const t_pvalue&, const t_pvalue&), f__not_equals>())
	(global->f_symbol_and(), f_not_supported)
	(global->f_symbol_xor(), f_not_supported)
	(global->f_symbol_or(), f_not_supported)
	.f_derive<t_object>(t_object::f_of(this));
}

size_t t_type::f_index(t_object* a_key)
{
	auto p = f_key2index();
	size_t i = 0;
	size_t j = v_fields;
	while (i < j) {
		size_t k = (i + j) / 2;
		auto& entry = p[k];
		if (entry.first == a_key) return entry.second;
		if (entry.first < a_key)
			i = k + 1;
		else
			j = k;
	}
	return v_fields;
}

std::pair<std::vector<std::pair<t_root, t_rvalue>>, std::map<t_object*, size_t>> t_type::f_merge(const t_fields& a_fields)
{
	std::vector<std::pair<t_root, t_rvalue>> fields{f_fields(), f_fields() + v_instance_fields};
	std::map<t_object*, size_t> key2index{f_key2index(), f_key2index() + v_fields};
	for (auto& x : a_fields.v_instance) {
		if (key2index.contains(x)) f_throw(x->f_as<t_symbol>().f_string());
		key2index.emplace(x, fields.size());
		fields.emplace_back(x, nullptr);
	}
	auto instance_fields = fields.size();
	for (size_t i = v_instance_fields; i < v_fields; ++i) {
		auto& x = f_fields()[i];
		key2index.at(x.first) = fields.size();
		fields.emplace_back(x);
	}
	for (auto& x : a_fields.v_class) {
		auto i = key2index.lower_bound(x.first);
		if (i == key2index.end() || i->first != x.first) {
			key2index.emplace_hint(i, x.first, fields.size());
			fields.push_back(x);
		} else {
			if (i->second < instance_fields) f_throw(x.first->f_as<t_symbol>().f_string());
			fields[i->second].second = x.second;
		}
	}
	return {fields, key2index};
}

t_object* t_type::f_do_derive(const t_fields& a_fields)
{
	auto p = f_derive<t_type>(v_module, a_fields);
	p->f_as<t_type>().v_derive = &t_type::f_do_derive;
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
	auto p = f_engine()->f_allocate(sizeof(t_svalue) * v_instance_fields);
	std::uninitialized_default_construct_n(p->f_fields(0), v_instance_fields);
	p->f_be(this);
	return p;
}

void t_type::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	auto value = f_construct(a_stack, a_n);
	f_invoke_class(value, v_instance_fields, a_stack, a_n);
	a_stack[0] = value;
}

void t_type::f_throw_undefined_field(t_object* a_key)
{
	f_check<t_symbol>(a_key, L"key");
	f_throw(L"undefined field: "s + a_key->f_as<t_symbol>().f_string());
}

t_pvalue t_type::f_do_get(t_object* a_this, t_object* a_key, size_t& a_index)
{
	f_throw_undefined_field(a_key);
}

t_pvalue t_type::f__get(const t_pvalue& a_this, t_object* a_key, size_t& a_index)
{
	auto i = f_index(a_key);
	if (i < v_instance_fields) {
		a_index = i;
		return a_this->f_fields()[i];
	}
	if (i < v_fields) {
		a_index = i - v_instance_fields;
		auto& field = f_fields()[i].second;
		return f_is_bindable(field) ? t_pvalue(xemmai::f_new<t_method>(f_global(), field, a_this)) : t_pvalue(field);
	}
	return (this->*v_get)(a_this, a_key, a_index);
}

void t_type::f__bind(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack)
{
	auto i = f_index(a_key);
	if (i < v_instance_fields) {
		a_index = i;
		a_stack[0] = a_this->f_fields()[i];
		a_stack[1] = nullptr;
	} else if (i < v_fields) {
		a_index = i - v_instance_fields;
		f_bind_class(a_this, i, a_stack);
	} else {
		a_stack[0] = (this->*v_get)(a_this, a_key, a_index);
		a_stack[1] = nullptr;
	}
}

void t_type::f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	f_throw_undefined_field(a_key);
}

void t_type::f__put(t_object* a_this, t_object* a_key, size_t& a_index, const t_pvalue& a_value)
{
	auto i = f_index(a_key);
	if (i < v_instance_fields) {
		a_index = i;
		a_this->f_fields()[i] = a_value;
	} else {
		v_put(a_this, a_key, a_value);
	}
}

bool t_type::f_do_has(t_object* a_this, t_object* a_key)
{
	return false;
}

bool t_type::f_has(t_object* a_this, t_object* a_key, size_t& a_index)
{
	auto i = a_index;
	if (i < v_fields && f_fields()[i].first == a_key) return true;
	i = f_index(a_key);
	if (i >= v_fields) return (this->*v_has)(a_this, a_key);
	a_index = i;
	return true;
}

void t_type::f__invoke(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack, size_t a_n)
{
	auto i = f_index(a_key);
	if (i < v_instance_fields) {
		a_index = i;
		a_this->f_fields()[i].f_call(a_stack, a_n);
	} else if (i < v_fields) {
		a_index = i - v_instance_fields;
		f_invoke_class(a_this, i, a_stack, a_n);
	} else {
		(this->*v_get)(a_this, a_key, a_index).f_call(a_stack, a_n);
	}
}

size_t t_type::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	auto type = a_this->f_type();
	type->f_bind_class(a_this, type->v_instance_fields + 1, a_stack);
	return a_n;
}

void t_type::f_do_string(t_object* a_this, t_pvalue* a_stack)
{
	auto type = a_this->f_type();
	type->f_invoke_class(a_this, type->v_instance_fields + 2, a_stack, 0);
}

void t_type::f_do_hash(t_object* a_this, t_pvalue* a_stack)
{
	auto type = a_this->f_type();
	type->f_invoke_class(a_this, type->v_instance_fields + 3, a_stack, 0);
}

#define XEMMAI__TYPE__METHOD(a_method, a_index, a_n)\
size_t t_type::f_do_##a_method(t_object* a_this, t_pvalue* a_stack)\
{\
	auto type = a_this->f_type();\
	type->f_bind_class(a_this, type->v_instance_fields + a_index, a_stack);\
	return a_n;\
}

XEMMAI__TYPE__METHOD(get_at, 4, 1)
XEMMAI__TYPE__METHOD(set_at, 5, 2)
XEMMAI__TYPE__METHOD(plus, 6, 0)
XEMMAI__TYPE__METHOD(minus, 7, 0)
XEMMAI__TYPE__METHOD(complement, 8, 0)
XEMMAI__TYPE__METHOD(multiply, 9, 1)
XEMMAI__TYPE__METHOD(divide, 10, 1)
XEMMAI__TYPE__METHOD(modulus, 11, 1)
XEMMAI__TYPE__METHOD(add, 12, 1)
XEMMAI__TYPE__METHOD(subtract, 13, 1)
XEMMAI__TYPE__METHOD(left_shift, 14, 1)
XEMMAI__TYPE__METHOD(right_shift, 15, 1)
XEMMAI__TYPE__METHOD(less, 16, 1)
XEMMAI__TYPE__METHOD(less_equal, 17, 1)
XEMMAI__TYPE__METHOD(greater, 18, 1)
XEMMAI__TYPE__METHOD(greater_equal, 19, 1)
XEMMAI__TYPE__METHOD(equals, 20, 1)
XEMMAI__TYPE__METHOD(not_equals, 21, 1)
XEMMAI__TYPE__METHOD(and, 22, 1)
XEMMAI__TYPE__METHOD(xor, 23, 1)
XEMMAI__TYPE__METHOD(or, 24, 1)

void f_throw_type_error [[noreturn]] (const std::type_info& a_type, const wchar_t* a_name)
{
	f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

}
