#include <xemmai/bytes.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_bytes::f_instantiate(size_t a_size)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_bytes>());
	object.f_pointer__(new(a_size) t_bytes(a_size));
	return object;
}

std::wstring t_bytes::f_string() const
{
	if (v_size <= 0) return L"[]";
	wchar_t cs[6];
	std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%02x", (*this)[0]);
	std::wstring s = cs;
	for (size_t i = 1; i < v_size; ++i) {
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L", %02x", (*this)[i]);
		s += cs;
	}
	return L'[' + s + L']';
}

void t_type_of<t_bytes>::f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(a_stack, a_n, L"must be called with an argument.");
	t_destruct<> self(a_stack[1]);
	t_destruct<> a0(a_stack[2]);
	if (self.v_p.f_type() != f_global()->f_type<t_class>()) f_throw(L"must be class.");
	f_check<size_t>(a0.v_p, L"argument0");
	t_scoped p = t_object::f_allocate(&f_as<t_type&>(self.v_p));
	size_t n = f_as<size_t>(a0.v_p);
	p.f_pointer__(new(n) t_bytes(n));
	a_stack[0].f_construct(std::move(p));
}

void t_type_of<t_bytes>::f_define()
{
	t_define<t_bytes, t_object>(f_global(), L"Bytes")
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<std::wstring(t_bytes::*)() const, &t_bytes::f_string>())
		(f_global()->f_symbol_get_at(), t_member<intptr_t(t_bytes::*)(intptr_t) const, &t_bytes::f_get_at>())
		(f_global()->f_symbol_set_at(), t_member<intptr_t(t_bytes::*)(intptr_t, intptr_t), &t_bytes::f_set_at>())
		(L"size", t_member<size_t(t_bytes::*)() const, &t_bytes::f_size>())
		(L"copy", t_member<void(t_bytes::*)(intptr_t, size_t, t_bytes&, intptr_t) const, &t_bytes::f_copy>())
	;
}

t_scoped t_type_of<t_bytes>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(L"must be called with an argument.");
	auto& a0 = a_stack[2];
	f_check<size_t>(a0, L"argument0");
	t_scoped p = t_object::f_allocate(this);
	size_t n = f_as<size_t>(a0);
	p.f_pointer__(new(n) t_bytes(n));
	return p;
}

size_t t_type_of<t_bytes>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<intptr_t>(a0.v_p, L"index");
	a_stack[0].f_construct(f_as<const t_bytes&>(a_this).f_get_at(f_as<intptr_t>(a0.v_p)));
	return -1;
}

size_t t_type_of<t_bytes>::f_do_set_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	t_destruct<> a1(a_stack[3]);
	f_check<intptr_t>(a0.v_p, L"index");
	f_check<intptr_t>(a1.v_p, L"value");
	a_stack[0].f_construct(f_as<t_bytes&>(a_this).f_set_at(f_as<intptr_t>(a0.v_p), f_as<intptr_t>(a1.v_p)));
	return -1;
}

}
