#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_string::f_instantiate(const wchar_t* a_p, size_t a_n)
{
	return a_n > 0 ? t_type_of<t_string>::f__construct(f_global()->f_type<t_string>(), a_p, a_n) : f_global()->f_string_empty();
}

void t_type_of<t_string>::f_define()
{
	auto global = f_global();
	t_define{global}
	(L"from_code"sv, t_static<t_object*(*)(t_global*, intptr_t), [](t_global* a_library, intptr_t a_code)
	{
		wchar_t c = a_code;
		return f__construct(a_library->template f_type<t_string>(), &c, 1);
	}>())
	(global->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), [](const t_pvalue& a_self) -> t_object*
	{
		return a_self;
	}>())
	(global->f_symbol_size(), t_member<size_t(t_string::*)() const, &t_string::f_size>())
	(L"substring"sv,
		t_member<t_object*(*)(t_global*, const t_string&, size_t), [](t_global* a_library, const t_string& a_self, size_t a_i)
		{
			if ((a_i > a_self.f_size())) f_throw(L"out of range."sv);
			return f__substring(a_library, a_self, a_i, a_self.f_size() - a_i);
		}>(),
		t_member<t_object*(*)(t_global*, const t_string&, size_t, size_t), [](t_global* a_library, const t_string& a_self, size_t a_i, size_t a_n)
		{
			if ((a_i > a_self.f_size())) f_throw(L"out of range."sv);
			return f__substring(a_library, a_self, a_i, std::min(a_n, a_self.f_size() - a_i));
		}>()
	)
	(L"code_at"sv, t_member<intptr_t(*)(const t_string&, size_t), [](const t_string& a_self, size_t a_i) -> intptr_t
	{
		if (a_i >= a_self.f_size()) f_throw(L"out of range."sv);
		return a_self[a_i];
	}>())
	.f_derive<t_string, t_object>();
}

void t_type_of<t_string>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	a_stack[0] = t_construct_with<t_object*(*)(t_type*, const t_string&), f__construct>::f_do(this, a_stack, a_n);
}

void t_type_of<t_string>::f_do_hash(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = a_this->f_as<t_string>().f_hash();
}

size_t t_type_of<t_string>::f_do_add(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = f__add(a_this, a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_less(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_string>().f_compare(a_stack[2]->f_as<t_string>()) < 0;
	return -1;
}

size_t t_type_of<t_string>::f_do_less_equal(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_string>().f_compare(a_stack[2]->f_as<t_string>()) <= 0;
	return -1;
}

size_t t_type_of<t_string>::f_do_greater(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_string>().f_compare(a_stack[2]->f_as<t_string>()) > 0;
	return -1;
}

size_t t_type_of<t_string>::f_do_greater_equal(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_string>().f_compare(a_stack[2]->f_as<t_string>()) >= 0;
	return -1;
}

size_t t_type_of<t_string>::f_do_equals(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = f__equals(a_this->f_as<t_string>(), a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_not_equals(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = !f__equals(a_this->f_as<t_string>(), a_stack[2]);
	return -1;
}

void t_stringer::f_grow()
{
	auto& s = v_p->f_as<t_string>();
	auto n0 = v_j - s;
	auto n1 = t_object::f_size_to_capacity<t_string, wchar_t>(n0 > 0 ? t_object::f_capacity_to_size<t_string, wchar_t>(n0 + 1) * 2 : sizeof(t_object)) - 1;
	auto object = t_type_of<t_string>::f__construct(f_global()->f_type<t_string>(), n1);
	auto p = const_cast<wchar_t*>(static_cast<const wchar_t*>(object->f_as<t_string>()));
	v_i = std::copy(static_cast<const wchar_t*>(s), const_cast<const wchar_t*>(v_i), p);
	v_j = p + n1;
	v_p = object;
}

}
