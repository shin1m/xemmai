#include <xemmai/integer.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_transfer t_type_of<ptrdiff_t>::f_multiply(ptrdiff_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_value(a_self * a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_value(a_self * a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<ptrdiff_t>(p)) return t_value(a_self * p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

t_transfer t_type_of<ptrdiff_t>::f_divide(ptrdiff_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_value(a_self / a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_value(a_self / a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<ptrdiff_t>(p)) return t_value(a_self / p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

t_transfer t_type_of<ptrdiff_t>::f_add(ptrdiff_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_value(a_self + a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_value(a_self + a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<ptrdiff_t>(p)) return t_value(a_self + p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

t_transfer t_type_of<ptrdiff_t>::f_subtract(ptrdiff_t a_self, const t_value& a_value)
{
	switch (a_value.f_tag()) {
	case t_value::e_tag__NULL:
	case t_value::e_tag__BOOLEAN:
		break;
	case t_value::e_tag__INTEGER:
		return t_value(a_self - a_value.v_integer);
	case t_value::e_tag__FLOAT:
		return t_value(a_self - a_value.v_float);
	default:
		{
			t_object* p = a_value;
			if (f_is<ptrdiff_t>(p)) return t_value(a_self - p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<ptrdiff_t>::f_less(ptrdiff_t a_self, const t_value& a_value)
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
			if (f_is<ptrdiff_t>(p)) return a_self < p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<ptrdiff_t>::f_less_equal(ptrdiff_t a_self, const t_value& a_value)
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
			if (f_is<ptrdiff_t>(p)) return a_self <= p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<ptrdiff_t>::f_greater(ptrdiff_t a_self, const t_value& a_value)
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
			if (f_is<ptrdiff_t>(p)) return a_self > p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<ptrdiff_t>::f_greater_equal(ptrdiff_t a_self, const t_value& a_value)
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
			if (f_is<ptrdiff_t>(p)) return a_self >= p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<ptrdiff_t>::f_equals(ptrdiff_t a_self, const t_value& a_value)
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
			return f_is<ptrdiff_t>(p) && a_self == p->f_integer();
		}
	}
}

bool t_type_of<ptrdiff_t>::f_not_equals(ptrdiff_t a_self, const t_value& a_value)
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
			return !f_is<ptrdiff_t>(p) || a_self != p->f_integer();
		}
	}
}

void t_type_of<ptrdiff_t>::f_define()
{
	t_define<ptrdiff_t, t_object>(f_global(), L"Integer")
		(t_construct_with<t_transfer (*)(t_object*, ptrdiff_t), f_construct_derived>())
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(ptrdiff_t), f_string>())
		(f_global()->f_symbol_hash(), t_member<ptrdiff_t (*)(ptrdiff_t), f_hash>())
		(f_global()->f_symbol_plus(), t_member<ptrdiff_t (*)(ptrdiff_t), f_plus>())
		(f_global()->f_symbol_minus(), t_member<ptrdiff_t (*)(ptrdiff_t), f_minus>())
		(f_global()->f_symbol_complement(), t_member<ptrdiff_t (*)(ptrdiff_t), f_complement>())
		(f_global()->f_symbol_multiply(), t_member<t_transfer (*)(ptrdiff_t, const t_value&), f_multiply>())
		(f_global()->f_symbol_divide(), t_member<t_transfer (*)(ptrdiff_t, const t_value&), f_divide>())
		(f_global()->f_symbol_modulus(), t_member<ptrdiff_t (*)(ptrdiff_t, ptrdiff_t), f_modulus>())
		(f_global()->f_symbol_add(), t_member<t_transfer (*)(ptrdiff_t, const t_value&), f_add>())
		(f_global()->f_symbol_subtract(), t_member<t_transfer (*)(ptrdiff_t, const t_value&), f_subtract>())
		(f_global()->f_symbol_left_shift(), t_member<ptrdiff_t (*)(ptrdiff_t, ptrdiff_t), f_left_shift>())
		(f_global()->f_symbol_right_shift(), t_member<ptrdiff_t (*)(ptrdiff_t, ptrdiff_t), f_right_shift>())
		(f_global()->f_symbol_less(), t_member<bool (*)(ptrdiff_t, const t_value&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(ptrdiff_t, const t_value&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(ptrdiff_t, const t_value&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(ptrdiff_t, const t_value&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(ptrdiff_t, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(ptrdiff_t, const t_value&), f_not_equals>())
		(f_global()->f_symbol_and(), t_member<ptrdiff_t (*)(ptrdiff_t, ptrdiff_t), f_and>())
		(f_global()->f_symbol_xor(), t_member<ptrdiff_t (*)(ptrdiff_t, ptrdiff_t), f_xor>())
		(f_global()->f_symbol_or(), t_member<ptrdiff_t (*)(ptrdiff_t, ptrdiff_t), f_or>())
	;
}

t_type* t_type_of<ptrdiff_t>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

t_transfer t_type_of<ptrdiff_t>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return
		t_overload<t_construct_with<t_transfer (*)(t_object*, ptrdiff_t), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(t_object*, double), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(t_object*, const std::wstring&), f_construct>
	> > >::t_bind<ptrdiff_t>::f_do(a_class, a_stack, a_n);
}

}
