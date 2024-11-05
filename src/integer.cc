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
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(*)(intptr_t), f__string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(intptr_t), f__hash>())
		(f_global()->f_symbol_plus(), t_member<intptr_t(*)(intptr_t), f__plus>())
		(f_global()->f_symbol_minus(), t_member<intptr_t(*)(intptr_t), f__minus>())
		(f_global()->f_symbol_complement(), t_member<intptr_t(*)(intptr_t), f__complement>())
		(f_global()->f_symbol_multiply(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__multiply>())
		(f_global()->f_symbol_divide(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__divide>())
		(f_global()->f_symbol_modulus(), t_member<intptr_t(*)(intptr_t, intptr_t), f__modulus>())
		(f_global()->f_symbol_add(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__add>())
		(f_global()->f_symbol_subtract(), t_member<t_pvalue(*)(intptr_t, const t_pvalue&), f__subtract>())
		(f_global()->f_symbol_left_shift(), t_member<intptr_t(*)(intptr_t, intptr_t), f__left_shift>())
		(f_global()->f_symbol_right_shift(), t_member<size_t(*)(size_t, intptr_t), f__right_shift>())
		(f_global()->f_symbol_less(), t_member<bool(*)(intptr_t, const t_pvalue&), f__less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(*)(intptr_t, const t_pvalue&), f__less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(*)(intptr_t, const t_pvalue&), f__greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(*)(intptr_t, const t_pvalue&), f__greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(intptr_t, const t_pvalue&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(intptr_t, const t_pvalue&), f__not_equals>())
		(f_global()->f_symbol_and(), t_member<intptr_t(*)(intptr_t, intptr_t), f__and>())
		(f_global()->f_symbol_xor(), t_member<intptr_t(*)(intptr_t, intptr_t), f__xor>())
		(f_global()->f_symbol_or(), t_member<intptr_t(*)(intptr_t, intptr_t), f__or>())
	.f_derive<intptr_t, t_object>();
}

t_pvalue t_type_of<intptr_t>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, intptr_t), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, double), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, const t_string&), f_construct>
	>::t_bind<intptr_t>::f_do(this, a_stack, a_n);
}

}
