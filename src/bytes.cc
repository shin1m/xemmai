#include <xemmai/bytes.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

void t_bytes::f_validate(intptr_t& a_index) const
{
	if (a_index < 0) {
		a_index += v_size;
		if (a_index < 0) t_throwable::f_throw(L"out of range.");
	} else {
		if (a_index >= static_cast<intptr_t>(v_size)) t_throwable::f_throw(L"out of range.");
	}
}

void t_bytes::f_validate(intptr_t& a_index, size_t a_size) const
{
	if (a_index < 0) {
		a_index += v_size;
		if (a_index < 0) t_throwable::f_throw(L"out of range.");
	}
	if (a_index + a_size > v_size) t_throwable::f_throw(L"out of range.");
}

t_scoped t_bytes::f_instantiate(size_t a_size)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_bytes>());
	object.f_pointer__(new(a_size) t_bytes());
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

intptr_t t_bytes::f_get_at(intptr_t a_index) const
{
	f_validate(a_index);
	return (*this)[a_index];
}

intptr_t t_bytes::f_set_at(intptr_t a_index, intptr_t a_value)
{
	f_validate(a_index);
	return (*this)[a_index] = a_value;
}

void t_bytes::f_copy(intptr_t a_index0, size_t a_size, t_bytes& a_other, intptr_t a_index1) const
{
	f_validate(a_index0, a_size);
	a_other.f_validate(a_index1, a_size);
	unsigned char* p = f_entries() + a_index0;
	std::copy(p, p + a_size, a_other.f_entries() + a_index1);
}

void t_type_of<t_bytes>::f__construct(t_object* a_module, t_scoped* a_stack, size_t a_n)
{
	t_scoped self = std::move(a_stack[1]);
	if (self.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"must be class.");
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_scoped a0 = std::move(a_stack[2]);
	f_check<size_t>(a0, L"argument0");
	t_scoped p = t_object::f_allocate(std::move(self));
	p.f_pointer__(new(f_as<size_t>(a0)) t_bytes());
	a_stack[0].f_construct(std::move(p));
}

bool t_type_of<t_bytes>::f_equals(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_bytes>(a_self, L"this");
	if (!f_is<t_bytes>(a_other)) return false;
	const t_bytes& a0 = f_as<const t_bytes&>(a_self);
	const t_bytes& a1 = f_as<const t_bytes&>(a_other);
	if (a0.f_size() != a1.f_size()) return false;
	for (size_t i = 0; i < a0.f_size(); ++i) if (a0[i] != a1[i]) return false;
	return true;
}

void t_type_of<t_bytes>::f_define()
{
	t_define<t_bytes, t_object>(f_global(), L"Bytes")
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<std::wstring (t_bytes::*)() const, &t_bytes::f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t (t_bytes::*)() const, &t_bytes::f_hash>())
		(f_global()->f_symbol_get_at(), t_member<intptr_t (t_bytes::*)(intptr_t) const, &t_bytes::f_get_at>())
		(f_global()->f_symbol_set_at(), t_member<intptr_t (t_bytes::*)(intptr_t, intptr_t), &t_bytes::f_set_at>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
		(L"size", t_member<size_t (t_bytes::*)() const, &t_bytes::f_size>())
		(L"copy", t_member<void (t_bytes::*)(intptr_t, size_t, t_bytes&, intptr_t) const, &t_bytes::f_copy>())
	;
}

t_type* t_type_of<t_bytes>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

void t_type_of<t_bytes>::f_finalize(t_object* a_this)
{
	delete &f_as<t_bytes&>(a_this);
}

t_scoped t_type_of<t_bytes>::f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	const t_scoped& a0 = a_stack[2];
	f_check<size_t>(a0, L"argument0");
	t_scoped p = t_object::f_allocate(a_class);
	p.f_pointer__(new(f_as<size_t>(a0)) t_bytes());
	return p;
}

void t_type_of<t_bytes>::f_hash(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	f_check<t_bytes>(a_this, L"this");
	a_stack[0].f_construct(f_as<const t_bytes&>(a_this).f_hash());
	context.f_done();
}

size_t t_type_of<t_bytes>::f_get_at(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	f_check<t_bytes>(a_this, L"this");
	t_scoped a0 = std::move(a_stack[2]);
	f_check<intptr_t>(a0, L"index");
	a_stack[0].f_construct(f_as<const t_bytes&>(a_this).f_get_at(f_as<intptr_t>(a0)));
	context.f_done();
	return -1;
}

size_t t_type_of<t_bytes>::f_set_at(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	f_check<t_bytes>(a_this, L"this");
	t_scoped a0 = std::move(a_stack[2]);
	t_scoped a1 = std::move(a_stack[3]);
	f_check<intptr_t>(a0, L"index");
	f_check<intptr_t>(a1, L"value");
	a_stack[0].f_construct(f_as<t_bytes&>(a_this).f_set_at(f_as<intptr_t>(a0), f_as<intptr_t>(a1)));
	context.f_done();
	return -1;
}

size_t t_type_of<t_bytes>::f_equals(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(f_equals(a_this, a0));
	context.f_done();
	return -1;
}

size_t t_type_of<t_bytes>::f_not_equals(t_object* a_this, t_scoped* a_stack)
{
	t_native_context context;
	t_scoped a0 = std::move(a_stack[2]);
	a_stack[0].f_construct(f_not_equals(a_this, a0));
	context.f_done();
	return -1;
}

}
