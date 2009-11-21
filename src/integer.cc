#include <xemmai/integer.h>

#include <xemmai/boolean.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

bool t_type_of<int>::f_equals(int a_self, t_object* a_value)
{
	return f_is<int>(a_value) && a_self == f_as<int>(a_value);
}

bool t_type_of<int>::f_not_equals(int a_self, t_object* a_value)
{
	return !f_is<int>(a_value) || a_self != f_as<int>(a_value);
}

void t_type_of<int>::f_define()
{
	t_define<int, t_object>(f_global(), L"Integer")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(int), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(int), f_hash>())
		(f_global()->f_symbol_plus(), t_member<int (*)(int), f_plus>())
		(f_global()->f_symbol_minus(), t_member<int (*)(int), f_minus>())
		(f_global()->f_symbol_complement(), t_member<int (*)(int), f_complement>())
		(f_global()->f_symbol_multiply(), t_member<int (*)(int, int), f_multiply>())
		(f_global()->f_symbol_divide(), t_member<int (*)(int, int), f_divide>())
		(f_global()->f_symbol_modulus(), t_member<int (*)(int, int), f_modulus>())
		(f_global()->f_symbol_add(), t_member<int (*)(int, int), f_add>())
		(f_global()->f_symbol_subtract(), t_member<int (*)(int, int), f_subtract>())
		(f_global()->f_symbol_left_shift(), t_member<int (*)(int, int), f_left_shift>())
		(f_global()->f_symbol_right_shift(), t_member<int (*)(int, int), f_right_shift>())
		(f_global()->f_symbol_less(), t_member<bool (*)(int, int), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(int, int), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(int, int), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(int, int), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(int, t_object*), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(int, t_object*), f_not_equals>())
		(f_global()->f_symbol_and(), t_member<int (*)(int, int), f_and>())
		(f_global()->f_symbol_xor(), t_member<int (*)(int, int), f_xor>())
		(f_global()->f_symbol_or(), t_member<int (*)(int, int), f_or>())
	;
}

t_type* t_type_of<int>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<int>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_overload<t_construct_with<t_transfer (*)(t_object*, int), f_construct>,
	t_overload<t_construct_with<t_transfer (*)(t_object*, double), f_construct>,
	t_overload<t_construct_with<t_transfer (*)(t_object*, const std::wstring&), f_construct>
	> > >::f_call(a_class, a_n, a_stack);
}

void t_type_of<int>::f_hash(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this)));
	context.f_done();
}

void t_type_of<int>::f_plus(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this)));
	context.f_done();
}

void t_type_of<int>::f_minus(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(-f_as<int>(a_this)));
	context.f_done();
}

void t_type_of<int>::f_complement(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(~f_as<int>(a_this)));
	context.f_done();
}

void t_type_of<int>::f_multiply(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) * f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_divide(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) / f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_modulus(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) % f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_add(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) + f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_subtract(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) - f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_left_shift(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) << f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_right_shift(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) >> f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_less(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) < f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_less_equal(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) <= f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_greater(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) > f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_greater_equal(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) >= f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_equals(f_as<int>(a_this), a0)));
	context.f_done();
}

void t_type_of<int>::f_not_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_not_equals(f_as<int>(a_this), a0)));
	context.f_done();
}

void t_type_of<int>::f_and(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) & f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_xor(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) ^ f_as<int>(a0)));
	context.f_done();
}

void t_type_of<int>::f_or(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"argument0");
	a_stack.f_return(f_global()->f_as(f_as<int>(a_this) | f_as<int>(a0)));
	context.f_done();
}

}
