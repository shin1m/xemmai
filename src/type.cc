#include <xemmai/convert.h>

namespace xemmai
{

void t_type::f_initialize(xemmai::t_library* a_library, t_pvalue* a_stack, size_t a_n)
{
	a_stack[0] = nullptr;
}

t_object* t_type::f_string(const t_pvalue& a_self)
{
	wchar_t cs[13 + sizeof(t_object*) * 2];
	size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"object at %p", static_cast<t_object*>(a_self));
	return t_string::f_instantiate(cs, n);
}

void t_type::f_define()
{
	v_builtin = true;
	t_define{f_global()}
		(f_global()->f_symbol_initialize(), f_initialize)
		(f_global()->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(const t_pvalue&), f__hash>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(const t_pvalue&, const t_pvalue&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(const t_pvalue&, const t_pvalue&), f__not_equals>())
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
		if (key2index.find(x) != key2index.end()) f_throw(f_as<t_symbol&>(x).f_string());
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
			if (i->second < instance_fields) f_throw(f_as<t_symbol&>(x.first).f_string());
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
	value.f_call(f_global()->f_symbol_initialize(), a_stack, a_n);
	a_stack[0] = value;
}

t_pvalue t_type::f_do_get(t_object* a_this, t_object* a_key)
{
	f_throw(f_as<t_symbol&>(a_key).f_string());
}

void t_type::f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	f_throw(f_as<t_symbol&>(a_key).f_string());
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

void f_throw_type_error(const std::type_info& a_type, const wchar_t* a_name)
{
	f_throw(std::wstring(a_name) + L" must be " + portable::f_convert(a_type.name()) + L'.');
}

}
