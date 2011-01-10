#include <xemmai/integer.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_transfer t_type_of<int>::f_multiply(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return t_value(a_self * p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

t_transfer t_type_of<int>::f_divide(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return t_value(a_self / p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

t_transfer t_type_of<int>::f_add(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return t_value(a_self + p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

t_transfer t_type_of<int>::f_subtract(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return t_value(a_self - p->f_integer());
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<int>::f_less(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return a_self < p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<int>::f_less_equal(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return a_self <= p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<int>::f_greater(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return a_self > p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<int>::f_greater_equal(int a_self, const t_value& a_value)
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
			if (f_is<int>(p)) return a_self >= p->f_integer();
		}
	}
	t_throwable::f_throw(L"not supported");
}

bool t_type_of<int>::f_equals(int a_self, const t_value& a_value)
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
			return f_is<int>(p) && a_self == p->f_integer();
		}
	}
}

bool t_type_of<int>::f_not_equals(int a_self, const t_value& a_value)
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
			return !f_is<int>(p) || a_self != p->f_integer();
		}
	}
}

void t_type_of<int>::f_define()
{
	t_define<int, t_object>(f_global(), L"Integer")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(int), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(int), f_hash>())
		(f_global()->f_symbol_plus(), t_member<int (*)(int), f_plus>())
		(f_global()->f_symbol_minus(), t_member<int (*)(int), f_minus>())
		(f_global()->f_symbol_complement(), t_member<int (*)(int), f_complement>())
		(f_global()->f_symbol_multiply(), t_member<t_transfer (*)(int, const t_value&), f_multiply>())
		(f_global()->f_symbol_divide(), t_member<t_transfer (*)(int, const t_value&), f_divide>())
		(f_global()->f_symbol_modulus(), t_member<int (*)(int, int), f_modulus>())
		(f_global()->f_symbol_add(), t_member<t_transfer (*)(int, const t_value&), f_add>())
		(f_global()->f_symbol_subtract(), t_member<t_transfer (*)(int, const t_value&), f_subtract>())
		(f_global()->f_symbol_left_shift(), t_member<int (*)(int, int), f_left_shift>())
		(f_global()->f_symbol_right_shift(), t_member<int (*)(int, int), f_right_shift>())
		(f_global()->f_symbol_less(), t_member<bool (*)(int, const t_value&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(int, const t_value&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(int, const t_value&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(int, const t_value&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(int, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(int, const t_value&), f_not_equals>())
		(f_global()->f_symbol_and(), t_member<int (*)(int, int), f_and>())
		(f_global()->f_symbol_xor(), t_member<int (*)(int, int), f_xor>())
		(f_global()->f_symbol_or(), t_member<int (*)(int, int), f_or>())
	;
}

t_type* t_type_of<int>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<int>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_overload<t_construct_with<t_transfer (*)(t_object*, int), f_construct>,
	t_overload<t_construct_with<t_transfer (*)(t_object*, double), f_construct>,
	t_overload<t_construct_with<t_transfer (*)(t_object*, const std::wstring&), f_construct>
	> > >::f_call(a_class, a_stack, a_n);
}

}
