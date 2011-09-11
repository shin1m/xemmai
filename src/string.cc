#include <xemmai/string.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

std::wstring t_type_of<std::wstring>::f_add(const std::wstring& a_self, const t_value& a_value)
{
	if (f_is<std::wstring>(a_value)) return a_self + f_as<const std::wstring&>(a_value);
	t_transfer x = a_value.f_get(f_global()->f_symbol_string())();
	f_check<std::wstring>(x, L"argument0");
	return a_self + f_as<const std::wstring&>(x);
}

bool t_type_of<std::wstring>::f_equals(const std::wstring& a_self, const t_value& a_value)
{
	return f_is<std::wstring>(a_value) && a_self == f_as<const std::wstring&>(a_value);
}

bool t_type_of<std::wstring>::f_not_equals(const std::wstring& a_self, const t_value& a_value)
{
	return !f_is<std::wstring>(a_value) || a_self != f_as<const std::wstring&>(a_value);
}

void t_type_of<std::wstring>::f_define()
{
	t_define<std::wstring, t_object>(f_global(), L"String")
		(L"from_code", t_static<std::wstring (*)(int), f_from_code>())
		(t_construct<const std::wstring&>())
		(f_global()->f_symbol_string(), t_member<t_transfer (*)(const t_transfer&), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(const std::wstring&), f_hash>())
		(f_global()->f_symbol_add(), t_member<std::wstring (*)(const std::wstring&, const t_value&), f_add>())
		(f_global()->f_symbol_less(), t_member<bool (*)(const std::wstring&, const std::wstring&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(const std::wstring&, const std::wstring&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(const std::wstring&, const std::wstring&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(const std::wstring&, const std::wstring&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const std::wstring&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const std::wstring&, const t_value&), f_not_equals>())
		(L"substring",
			t_member<std::wstring (*)(const std::wstring&, size_t), f_substring>(),
			t_member<std::wstring (*)(const std::wstring&, size_t, size_t), f_substring>()
		)
	;
}

t_type* t_type_of<std::wstring>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<std::wstring>::f_finalize(t_object* a_this)
{
	delete &f_as<std::wstring&>(a_this);
}

t_transfer t_type_of<std::wstring>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct<const std::wstring&>::t_bind<std::wstring>::f_do(a_class, a_stack, a_n);
}

void t_type_of<std::wstring>::f_hash(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	a_stack[0].f_construct(f_hash(f_as<const std::wstring&>(a_this)));
	context.f_done();
}

void t_type_of<std::wstring>::f_add(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_global()->f_as(f_add(f_as<const std::wstring&>(a_this), a0)));
	context.f_done();
}

void t_type_of<std::wstring>::f_less(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) < f_as<const std::wstring&>(a0));
	context.f_done();
}

void t_type_of<std::wstring>::f_less_equal(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) <= f_as<const std::wstring&>(a0));
	context.f_done();
}

void t_type_of<std::wstring>::f_greater(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) > f_as<const std::wstring&>(a0));
	context.f_done();
}

void t_type_of<std::wstring>::f_greater_equal(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) >= f_as<const std::wstring&>(a0));
	context.f_done();
}

void t_type_of<std::wstring>::f_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_equals(f_as<const std::wstring&>(a_this), a0));
	context.f_done();
}

void t_type_of<std::wstring>::f_not_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_not_equals(f_as<const std::wstring&>(a_this), a0));
	context.f_done();
}

}
