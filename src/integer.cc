#include <xemmai/integer.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_scoped t_type_of<intptr_t>::f_multiply(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_scoped(a_self * a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_scoped(a_self * a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return t_scoped(a_self * p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported.");
}

t_scoped t_type_of<intptr_t>::f_divide(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_scoped(a_self / a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_scoped(a_self / a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return t_scoped(a_self / p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported.");
}

t_scoped t_type_of<intptr_t>::f_add(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_scoped(a_self + a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_scoped(a_self + a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return t_scoped(a_self + p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported.");
}

t_scoped t_type_of<intptr_t>::f_subtract(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_scoped(a_self - a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_scoped(a_self - a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return t_scoped(a_self - p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported.");
}

bool t_type_of<intptr_t>::f_less(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return a_self < a_value.v_integer;
	case t_value::e_tag__FLOAT:
		return a_self < a_value.v_float;
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return a_self < p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported.");
}

bool t_type_of<intptr_t>::f_less_equal(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return a_self <= a_value.v_integer;
	case t_value::e_tag__FLOAT:
		return a_self <= a_value.v_float;
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return a_self <= p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported.");
}

bool t_type_of<intptr_t>::f_greater(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return a_self > a_value.v_integer;
	case t_value::e_tag__FLOAT:
		return a_self > a_value.v_float;
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return a_self > p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported.");
}

bool t_type_of<intptr_t>::f_greater_equal(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return a_self >= a_value.v_integer;
	case t_value::e_tag__FLOAT:
		return a_self >= a_value.v_float;
	default:
		{
			t_object* p = a_value;
			if (f_is<intptr_t>(p)) return a_self >= p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported.");
}

bool t_type_of<intptr_t>::f_equals(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		return false;
	case t_value::e_tag__INTEGER:
		return a_self == a_value.v_integer;
	case t_value::e_tag__FLOAT:
		return a_self == a_value.v_float;
	default:
		{
			t_object* p = a_value;
			return f_is<intptr_t>(p) && a_self == p->f_integer();
		}
	}
}

bool t_type_of<intptr_t>::f_not_equals(intptr_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		return true;
	case t_value::e_tag__INTEGER:
		return a_self != a_value.v_integer;
	case t_value::e_tag__FLOAT:
		return a_self != a_value.v_float;
	default:
		{
			t_object* p = a_value;
			return !f_is<intptr_t>(p) || a_self != p->f_integer();
		}
	}
}

void t_type_of<intptr_t>::f_define()
{
	t_define<intptr_t, t_object>(f_global(), L"Integer")
		(t_construct_with<t_scoped(*)(t_type*, intptr_t), f_construct_derived>())
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(intptr_t), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(intptr_t), f_hash>())
		(f_global()->f_symbol_plus(), t_member<intptr_t(*)(intptr_t), f_plus>())
		(f_global()->f_symbol_minus(), t_member<intptr_t(*)(intptr_t), f_minus>())
		(f_global()->f_symbol_complement(), t_member<intptr_t(*)(intptr_t), f_complement>())
		(f_global()->f_symbol_multiply(), t_member<t_scoped(*)(intptr_t, const t_value&), f_multiply>())
		(f_global()->f_symbol_divide(), t_member<t_scoped(*)(intptr_t, const t_value&), f_divide>())
		(f_global()->f_symbol_modulus(), t_member<intptr_t(*)(intptr_t, intptr_t), f_modulus>())
		(f_global()->f_symbol_add(), t_member<t_scoped(*)(intptr_t, const t_value&), f_add>())
		(f_global()->f_symbol_subtract(), t_member<t_scoped(*)(intptr_t, const t_value&), f_subtract>())
		(f_global()->f_symbol_left_shift(), t_member<intptr_t(*)(intptr_t, intptr_t), f_left_shift>())
		(f_global()->f_symbol_right_shift(), t_member<size_t(*)(size_t, intptr_t), f_right_shift>())
		(f_global()->f_symbol_less(), t_member<bool(*)(intptr_t, const t_value&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(*)(intptr_t, const t_value&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(*)(intptr_t, const t_value&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(*)(intptr_t, const t_value&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(intptr_t, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(intptr_t, const t_value&), f_not_equals>())
		(f_global()->f_symbol_and(), t_member<intptr_t(*)(intptr_t, intptr_t), f_and>())
		(f_global()->f_symbol_xor(), t_member<intptr_t(*)(intptr_t, intptr_t), f_xor>())
		(f_global()->f_symbol_or(), t_member<intptr_t(*)(intptr_t, intptr_t), f_or>())
	;
}

t_type* t_type_of<intptr_t>::f_derive()
{
	return new t_derived<t_type_of>(t_scoped(v_module), this);
}

t_scoped t_type_of<intptr_t>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped(*)(t_type*, intptr_t), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, double), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, const std::wstring&), f_construct>
	>::t_bind<intptr_t>::f_do(this, a_stack, a_n);
}

}
