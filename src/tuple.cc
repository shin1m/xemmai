#include <xemmai/tuple.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_transfer t_tuple::f_instantiate(size_t a_size)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_tuple>());
	object.f_pointer__(new(a_size) t_tuple());
	return object;
}

std::wstring t_tuple::f_string() const
{
	if (v_size <= 0) return L"'()";
	t_transfer x = (*this)[0].f_get(f_global()->f_symbol_string())();
	f_check<const std::wstring&>(x, L"value");
	std::wstring s = f_as<const std::wstring&>(x);
	for (size_t i = 1; i < v_size; ++i) {
		x = (*this)[i].f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(x, L"value");
		s += L", " + f_as<const std::wstring&>(x);
	}
	return L"'(" + s + L')';
}

int t_tuple::f_hash() const
{
	int n = 0;
	for (size_t i = 0; i < v_size; ++i) {
		t_transfer x = (*this)[i].f_hash();
		f_check<int>(x, L"value");
		n ^= f_as<int>(x);
	}
	return n;
}

bool t_tuple::f_less(const t_tuple& a_other) const
{
	if (this == &a_other) return false;
	size_t i = 0;
	for (; i < v_size; ++i) {
		if (i >= a_other.f_size()) return false;
		const t_slot& x = (*this)[i];
		const t_slot& y = a_other[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_less(y));
	}
	return i < a_other.f_size();
}

bool t_tuple::f_less_equal(const t_tuple& a_other) const
{
	if (this == &a_other) return true;
	for (size_t i = 0; i < v_size; ++i) {
		if (i >= a_other.f_size()) return false;
		const t_slot& x = (*this)[i];
		const t_slot& y = a_other[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_less(y));
	}
	return true;
}

bool t_tuple::f_greater(const t_tuple& a_other) const
{
	if (this == &a_other) return false;
	for (size_t i = 0; i < v_size; ++i) {
		if (i >= a_other.f_size()) return true;
		const t_slot& x = (*this)[i];
		const t_slot& y = a_other[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_greater(y));
	}
	return false;
}

bool t_tuple::f_greater_equal(const t_tuple& a_other) const
{
	if (this == &a_other) return true;
	size_t i = 0;
	for (; i < v_size; ++i) {
		if (i >= a_other.f_size()) return true;
		const t_slot& x = (*this)[i];
		const t_slot& y = a_other[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_greater(y));
	}
	return i >= a_other.f_size();
}

bool t_tuple::f_equals(const t_value& a_other) const
{
	if (!f_is<t_tuple>(a_other)) return false;
	const t_tuple& other = f_as<const t_tuple&>(a_other);
	if (this == &other) return true;
	if (v_size != other.f_size()) return false;
	for (size_t i = 0; i < v_size; ++i) if (!f_as<bool>((*this)[i].f_equals(other[i]))) return false;
	return true;
}

void t_tuple::f_each(const t_value& a_callable) const
{
	for (size_t i = 0; i < v_size; ++i) a_callable((*this)[i]);
}

void t_type_of<t_tuple>::f_define(t_object* a_class)
{
	t_define<t_tuple, t_object>(f_global(), L"Tuple", a_class)
		(f_global()->f_symbol_string(), t_member<std::wstring (t_tuple::*)() const, &t_tuple::f_string>())
		(f_global()->f_symbol_hash(), t_member<int (t_tuple::*)() const, &t_tuple::f_hash>())
		(f_global()->f_symbol_get_at(), t_member<const t_slot& (t_tuple::*)(size_t) const, &t_tuple::operator[]>())
		(f_global()->f_symbol_less(), t_member<bool (t_tuple::*)(const t_tuple&) const, &t_tuple::f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (t_tuple::*)(const t_tuple&) const, &t_tuple::f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (t_tuple::*)(const t_tuple&) const, &t_tuple::f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (t_tuple::*)(const t_tuple&) const, &t_tuple::f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (t_tuple::*)(const t_value&) const, &t_tuple::f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (t_tuple::*)(const t_value&) const, &t_tuple::f_not_equals>())
		(f_global()->f_symbol_size(), t_member<size_t (t_tuple::*)() const, &t_tuple::f_size>())
		(L"each", t_member<void (t_tuple::*)(const t_value&) const, &t_tuple::f_each>())
	;
}

t_type* t_type_of<t_tuple>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_tuple>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_tuple* p = &f_as<t_tuple&>(a_this);
	if (p) p->f_scan(a_scan);
}

void t_type_of<t_tuple>::f_finalize(t_object* a_this)
{
	delete &f_as<t_tuple&>(a_this);
}

void t_type_of<t_tuple>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_transfer p = t_tuple::f_instantiate(a_n);
	t_tuple& tuple = f_as<t_tuple&>(p);
	for (size_t i = 0; i < a_n; ++i) tuple[i].f_construct(a_stack[i + 1].f_transfer());
	a_stack[0].f_construct(p);
}

void t_type_of<t_tuple>::f_hash(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_hash());
	context.f_done();
}

void t_type_of<t_tuple>::f_get_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<int>(a0, L"index");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this)[f_as<int>(a0)]);
	context.f_done();
}

void t_type_of<t_tuple>::f_less(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<t_tuple>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_less(f_as<const t_tuple&>(a0)));
	context.f_done();
}

void t_type_of<t_tuple>::f_less_equal(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<t_tuple>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_less_equal(f_as<const t_tuple&>(a0)));
	context.f_done();
}

void t_type_of<t_tuple>::f_greater(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<t_tuple>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_greater(f_as<const t_tuple&>(a0)));
	context.f_done();
}

void t_type_of<t_tuple>::f_greater_equal(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	f_check<t_tuple>(a0, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_greater_equal(f_as<const t_tuple&>(a0)));
	context.f_done();
}

void t_type_of<t_tuple>::f_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_equals(a0));
	context.f_done();
}

void t_type_of<t_tuple>::f_not_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_not_equals(a0));
	context.f_done();
}

}
