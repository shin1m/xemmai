#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_type_of<intptr_t>::f__string(intptr_t a_self)
{
	wchar_t cs[32];
	size_t n = std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), XEMMAI__MACRO__L("%" PRIdPTR), a_self);
	return t_string::f_instantiate(cs, n);
}

t_pvalue t_type_of<intptr_t>::f__multiply(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self * a_value.v_integer;
	case c_tag__FLOAT:
		return a_self * a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self * p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

t_pvalue t_type_of<intptr_t>::f__divide(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self / a_value.v_integer;
	case c_tag__FLOAT:
		return a_self / a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self / p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

t_pvalue t_type_of<intptr_t>::f__add(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self + a_value.v_integer;
	case c_tag__FLOAT:
		return a_self + a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self + p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

t_pvalue t_type_of<intptr_t>::f__subtract(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self - a_value.v_integer;
	case c_tag__FLOAT:
		return a_self - a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self - p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

bool t_type_of<intptr_t>::f__less(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self < a_value.v_integer;
	case c_tag__FLOAT:
		return a_self < a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self < p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

bool t_type_of<intptr_t>::f__less_equal(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self <= a_value.v_integer;
	case c_tag__FLOAT:
		return a_self <= a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self <= p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

bool t_type_of<intptr_t>::f__greater(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self > a_value.v_integer;
	case c_tag__FLOAT:
		return a_self > a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self > p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

bool t_type_of<intptr_t>::f__greater_equal(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self >= a_value.v_integer;
	case c_tag__FLOAT:
		return a_self >= a_value.v_float;
	}
	t_object* p = a_value;
	if (f_is<intptr_t>(p)) return a_self >= p->f_as<intptr_t>();
	f_throw(L"not supported."sv);
}

bool t_type_of<intptr_t>::f__equals(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self == a_value.v_integer;
	case c_tag__FLOAT:
		return a_self == a_value.v_float;
	}
	t_object* p = a_value;
	return f_is<intptr_t>(p) && a_self == p->f_as<intptr_t>();
}

bool t_type_of<intptr_t>::f__not_equals(intptr_t a_self, const t_pvalue& a_value)
{
	switch (a_value.f_tag()) {
	case c_tag__INTEGER:
		return a_self != a_value.v_integer;
	case c_tag__FLOAT:
		return a_self != a_value.v_float;
	}
	t_object* p = a_value;
	return !f_is<intptr_t>(p) || a_self != p->f_as<intptr_t>();
}

void t_type_of<intptr_t>::f_define()
{
	auto global = f_global();
	t_define{global}
		(global->f_symbol_string(), t_member<t_object*(*)(intptr_t), f__string>())
		(global->f_symbol_hash(), t_member<intptr_t(*)(intptr_t), f__hash>())
		(global->f_symbol_plus(), t_member<intptr_t(*)(intptr_t), f__plus>())
		(global->f_symbol_minus(), t_member<intptr_t(*)(intptr_t), f__minus>())
		(global->f_symbol_complement(), t_member<intptr_t(*)(intptr_t), f__complement>())
		(global->f_symbol_multiply(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__multiply>())
		(global->f_symbol_divide(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__divide>())
		(global->f_symbol_modulus(), t_member<intptr_t(*)(intptr_t, intptr_t), f__modulus>())
		(global->f_symbol_add(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__add>())
		(global->f_symbol_subtract(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__subtract>())
		(global->f_symbol_left_shift(), t_member<intptr_t(*)(intptr_t, intptr_t), f__left_shift>())
		(global->f_symbol_right_shift(), t_member<size_t(*)(size_t, intptr_t), f__right_shift>())
		(global->f_symbol_less(), t_member<bool(*)(intptr_t, const t_pvalue&), f__less>())
		(global->f_symbol_less_equal(), t_member<bool(*)(intptr_t, const t_pvalue&), f__less_equal>())
		(global->f_symbol_greater(), t_member<bool(*)(intptr_t, const t_pvalue&), f__greater>())
		(global->f_symbol_greater_equal(), t_member<bool(*)(intptr_t, const t_pvalue&), f__greater_equal>())
		(global->f_symbol_equals(), t_member<bool(*)(intptr_t, const t_pvalue&), f__equals>())
		(global->f_symbol_not_equals(), t_member<bool(*)(intptr_t, const t_pvalue&), f__not_equals>())
		(global->f_symbol_and(), t_member<intptr_t(*)(intptr_t, intptr_t), f__and>())
		(global->f_symbol_xor(), t_member<intptr_t(*)(intptr_t, intptr_t), f__xor>())
		(global->f_symbol_or(), t_member<intptr_t(*)(intptr_t, intptr_t), f__or>())
	.f_derive<intptr_t, t_object>();
}

t_pvalue t_type_of<intptr_t>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, intptr_t), [](auto a_class, auto a_value)
		{
			return t_pvalue(a_value);
		}>,
		t_construct_with<t_pvalue(*)(t_type*, double), [](auto a_class, auto a_value)
		{
			return t_pvalue(static_cast<intptr_t>(a_value));
		}>,
		t_construct_with<t_pvalue(*)(t_type*, const t_string&), f_construct>
	>::t_bind<intptr_t>::f_do(this, a_stack, a_n);
}

}
