#include <xemmai/bytes.h>

#include <xemmai/boolean.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

void t_bytes::f_validate(int& a_index) const
{
	if (a_index < 0) {
		if (a_index < -static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
		a_index = v_size - a_index;
	} else {
		if (a_index >= static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	}
}

t_transfer t_bytes::f_instantiate(size_t a_size)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_bytes>());
	object->v_pointer = new(a_size) t_bytes();
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

int t_bytes::f_get_at(int a_index) const
{
	f_validate(a_index);
	return (*this)[a_index];
}

int t_bytes::f_set_at(int a_index, int a_value)
{
	f_validate(a_index);
	return (*this)[a_index] = a_value;
}

void t_bytes::f_copy(int a_index0, size_t a_size, t_bytes& a_other, int a_index1) const
{
	f_validate(a_index0);
	if (a_index0 + a_size > static_cast<int>(v_size)) t_throwable::f_throw(L"out of range.");
	a_other.f_validate(a_index1);
	if (a_index1 + a_size > static_cast<int>(a_other.v_size)) t_throwable::f_throw(L"out of range.");
	unsigned char* p = f_entries() + a_index0;
	std::copy(p, p + a_size, a_other.f_entries() + a_index1);
}

bool t_type_of<t_bytes>::f_equals(t_object* a_self, t_object* a_other)
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
		(f_global()->f_symbol_string(), t_member<std::wstring (t_bytes::*)() const, &t_bytes::f_string>())
		(f_global()->f_symbol_hash(), t_member<int (t_bytes::*)() const, &t_bytes::f_hash>())
		(f_global()->f_symbol_get_at(), t_member<int (t_bytes::*)(int) const, &t_bytes::f_get_at>())
		(f_global()->f_symbol_set_at(), t_member<int (t_bytes::*)(int, int), &t_bytes::f_set_at>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(t_object*, t_object*), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(t_object*, t_object*), f_not_equals>())
		(L"size", t_member<size_t (t_bytes::*)() const, &t_bytes::f_size>())
		(L"copy", t_member<void (t_bytes::*)(int, size_t, t_bytes&, int) const, &t_bytes::f_copy>())
	;
}

t_type* t_type_of<t_bytes>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_bytes>::f_finalize(t_object* a_this)
{
	delete f_as<t_bytes*>(a_this);
}

void t_type_of<t_bytes>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_transfer a0 = a_stack.f_pop();
	f_check<size_t>(a0, L"argument0");
	a_stack.f_return(t_bytes::f_instantiate(f_as<size_t>(a0)));
}

void t_type_of<t_bytes>::f_hash(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	f_check<t_bytes>(a_this, L"this");
	a_stack.f_return(f_global()->f_as(f_as<const t_bytes&>(a_this).f_hash()));
	context.f_done();
}

void t_type_of<t_bytes>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	f_check<t_bytes>(a_this, L"this");
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"index");
	a_stack.f_return(f_global()->f_as(f_as<const t_bytes&>(a_this).f_get_at(f_as<int>(a0))));
	context.f_done();
}

void t_type_of<t_bytes>::f_set_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	f_check<t_bytes>(a_this, L"this");
	t_transfer a1 = a_stack.f_pop();
	t_transfer a0 = a_stack.f_pop();
	f_check<int>(a0, L"index");
	f_check<int>(a1, L"value");
	a_stack.f_return(f_global()->f_as(f_as<t_bytes&>(a_this).f_set_at(f_as<int>(a0), f_as<int>(a1))));
	context.f_done();
}

void t_type_of<t_bytes>::f_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_equals(a_this, a0)));
	context.f_done();
}

void t_type_of<t_bytes>::f_not_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_not_equals(a_this, a0)));
	context.f_done();
}

}
