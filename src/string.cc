#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_string::f_instantiate(const wchar_t* a_p, size_t a_n)
{
	return a_n > 0 ? t_type_of<t_string>::f__construct(f_global()->f_type<t_string>(), a_p, a_n) : f_global()->f_string_empty();
}

void t_type_of<t_string>::f_define()
{
	t_define<t_string, t_object>(f_global(), L"String"sv)
		(L"from_code"sv, t_static<t_object*(*)(t_global*, intptr_t), f_from_code>())
		(t_construct_with<t_pvalue(*)(t_type*, const t_string&), f__construct>())
		(f_global()->f_symbol_string(), t_member<t_object*(*)(const t_pvalue&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(t_string::*)() const, &t_string::f_hash>())
		(f_global()->f_symbol_add(), t_member<t_object*(*)(const t_pvalue&, const t_pvalue&), f__add>())
		(f_global()->f_symbol_less(), t_member<bool(*)(const t_string&, const t_string&), f__less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(*)(const t_string&, const t_string&), f__less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(*)(const t_string&, const t_string&), f__greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(*)(const t_string&, const t_string&), f__greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(const t_string&, const t_pvalue&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(const t_string&, const t_pvalue&), f__not_equals>())
		(f_global()->f_symbol_size(), t_member<size_t(t_string::*)() const, &t_string::f_size>())
		(L"substring"sv,
			t_member<t_object*(*)(t_global*, const t_string&, size_t), f_substring>(),
			t_member<t_object*(*)(t_global*, const t_string&, size_t, size_t), f_substring>()
		)
		(L"code_at"sv, t_member<intptr_t(*)(const t_string&, size_t), f_code_at>())
	;
}

t_pvalue t_type_of<t_string>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, const t_string&), f__construct>::t_bind<t_string>::f_do(this, a_stack, a_n);
}

void t_type_of<t_string>::f_do_hash(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = f_as<const t_string&>(a_this).f_hash();
}

size_t t_type_of<t_string>::f_do_add(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = f__add(a_this, a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_less(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = f_as<const t_string&>(a_this) < f_as<const t_string&>(a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_less_equal(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = f_as<const t_string&>(a_this) <= f_as<const t_string&>(a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_greater(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = f_as<const t_string&>(a_this) > f_as<const t_string&>(a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_greater_equal(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = f_as<t_string&>(a_this) >= f_as<const t_string&>(a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_equals(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = f__equals(f_as<const t_string&>(a_this), a_stack[2]);
	return -1;
}

size_t t_type_of<t_string>::f_do_not_equals(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = f__not_equals(f_as<const t_string&>(a_this), a_stack[2]);
	return -1;
}

}
