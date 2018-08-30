#include <xemmai/string.h>

#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

XEMMAI__PORTABLE__ALWAYS_INLINE inline t_scoped f_add(t_object* a_self, t_scoped&& a_value)
{
	auto& s0 = f_as<const std::wstring&>(a_self);
	auto add = [&](t_scoped&& x)
	{
		if (s0.empty()) return x;
		auto& s1 = f_as<const std::wstring&>(x);
		if (s1.empty()) return t_scoped(a_self);
		std::wstring s;
		s.reserve(s0.size() + s1.size());
		s.append(s0).append(s1);
		return f_global()->f_as(std::move(s));
	};
	if (f_is<std::wstring>(a_value)) return add(std::move(a_value));
	t_scoped x = a_value.f_invoke(f_global()->f_symbol_string());
	f_check<std::wstring>(x, L"argument0");
	return add(std::move(x));
}

inline t_scoped f_add(const t_value& a_self, t_scoped&& a_value)
{
	return f_add(static_cast<t_object*>(a_self), std::move(a_value));
}

}

bool t_type_of<std::wstring>::f__equals(const std::wstring& a_self, const t_value& a_value)
{
	return f_is<std::wstring>(a_value) && a_self == f_as<const std::wstring&>(a_value);
}

bool t_type_of<std::wstring>::f__not_equals(const std::wstring& a_self, const t_value& a_value)
{
	return !f_is<std::wstring>(a_value) || a_self != f_as<const std::wstring&>(a_value);
}

void t_type_of<std::wstring>::f_define()
{
	t_define<std::wstring, t_object>(f_global(), L"String")
		(L"from_code", t_static<std::wstring(*)(intptr_t), f_from_code>())
		(t_construct<const std::wstring&>())
		(f_global()->f_symbol_string(), t_member<t_scoped(*)(t_scoped&&), f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(*)(const std::wstring&), f__hash>())
		(f_global()->f_symbol_add(), t_member<t_scoped(*)(const t_value&, t_scoped&&), xemmai::f_add>())
		(f_global()->f_symbol_less(), t_member<bool(*)(const std::wstring&, const std::wstring&), f__less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(*)(const std::wstring&, const std::wstring&), f__less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(*)(const std::wstring&, const std::wstring&), f__greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(*)(const std::wstring&, const std::wstring&), f__greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(*)(const std::wstring&, const t_value&), f__equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(*)(const std::wstring&, const t_value&), f__not_equals>())
		(f_global()->f_symbol_size(), t_member<size_t(std::wstring::*)() const, &std::wstring::size>())
		(L"substring",
			t_member<std::wstring(*)(const std::wstring&, size_t), f_substring>(),
			t_member<std::wstring(*)(const std::wstring&, size_t, size_t), f_substring>()
		)
		(L"code_at", t_member<intptr_t(*)(const std::wstring&, size_t), f_code_at>())
	;
}

t_scoped t_type_of<std::wstring>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<const std::wstring&>::t_bind<std::wstring>::f_do(this, a_stack, a_n);
}

void t_type_of<std::wstring>::f_do_hash(t_object* a_this, t_stacked* a_stack)
{
	a_stack[0].f_construct(f__hash(f_as<const std::wstring&>(a_this)));
}

size_t t_type_of<std::wstring>::f_do_add(t_object* a_this, t_stacked* a_stack)
{
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(xemmai::f_add(a_this, std::move(a0)));
	return -1;
}

size_t t_type_of<std::wstring>::f_do_less(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<std::wstring>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) < f_as<const std::wstring&>(a0.v_p));
	return -1;
}

size_t t_type_of<std::wstring>::f_do_less_equal(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<std::wstring>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) <= f_as<const std::wstring&>(a0.v_p));
	return -1;
}

size_t t_type_of<std::wstring>::f_do_greater(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<std::wstring>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) > f_as<const std::wstring&>(a0.v_p));
	return -1;
}

size_t t_type_of<std::wstring>::f_do_greater_equal(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<std::wstring>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const std::wstring&>(a_this) >= f_as<const std::wstring&>(a0.v_p));
	return -1;
}

size_t t_type_of<std::wstring>::f_do_equals(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	a_stack[0].f_construct(f__equals(f_as<const std::wstring&>(a_this), a0.v_p));
	return -1;
}

size_t t_type_of<std::wstring>::f_do_not_equals(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	a_stack[0].f_construct(f__not_equals(f_as<const std::wstring&>(a_this), a0.v_p));
	return -1;
}

}
