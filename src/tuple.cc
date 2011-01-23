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

std::wstring t_type_of<t_tuple>::f_string(const t_value& a_self)
{
	f_check<t_tuple>(a_self, L"this");
	const t_tuple& tuple = f_as<const t_tuple&>(a_self);
	if (tuple.f_size() <= 0) return L"'()";
	t_transfer x = tuple[0].f_get(f_global()->f_symbol_string())();
	f_check<const std::wstring&>(x, L"value");
	std::wstring s = f_as<const std::wstring&>(x);
	for (size_t i = 1; i < tuple.f_size(); ++i) {
		x = tuple[i].f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(x, L"value");
		s += L", " + f_as<const std::wstring&>(x);
	}
	return L"'(" + s + L')';
}

int t_type_of<t_tuple>::f_hash(const t_value& a_self)
{
	f_check<t_tuple>(a_self, L"this");
	const t_tuple& tuple = f_as<const t_tuple&>(a_self);
	int n = 0;
	for (size_t i = 0; i < tuple.f_size(); ++i) {
		t_transfer x = tuple[i].f_hash();
		f_check<int>(x, L"value");
		n ^= f_as<int>(x);
	}
	return n;
}

bool t_type_of<t_tuple>::f_less(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return false;
	f_check<t_tuple>(a_self, L"this");
	f_check<t_tuple>(a_other, L"other");
	const t_tuple& t0 = f_as<const t_tuple&>(a_self);
	const t_tuple& t1 = f_as<const t_tuple&>(a_other);
	size_t i;
	for (i = 0; i < t0.f_size(); ++i) {
		if (i >= t1.f_size()) return false;
		const t_slot& x = t0[i];
		const t_slot& y = t1[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_less(y));
	}
	return i < t1.f_size();
}

bool t_type_of<t_tuple>::f_less_equal(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_tuple>(a_self, L"this");
	f_check<t_tuple>(a_other, L"other");
	const t_tuple& t0 = f_as<const t_tuple&>(a_self);
	const t_tuple& t1 = f_as<const t_tuple&>(a_other);
	for (size_t i = 0; i < t0.f_size(); ++i) {
		if (i >= t1.f_size()) return false;
		const t_slot& x = t0[i];
		const t_slot& y = t1[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_less(y));
	}
	return true;
}

bool t_type_of<t_tuple>::f_greater(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return false;
	f_check<t_tuple>(a_self, L"this");
	f_check<t_tuple>(a_other, L"other");
	const t_tuple& t0 = f_as<const t_tuple&>(a_self);
	const t_tuple& t1 = f_as<const t_tuple&>(a_other);
	for (size_t i = 0; i < t0.f_size(); ++i) {
		if (i >= t1.f_size()) return true;
		const t_slot& x = t0[i];
		const t_slot& y = t1[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_greater(y));
	}
	return false;
}

bool t_type_of<t_tuple>::f_greater_equal(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_tuple>(a_self, L"this");
	f_check<t_tuple>(a_other, L"other");
	const t_tuple& t0 = f_as<const t_tuple&>(a_self);
	const t_tuple& t1 = f_as<const t_tuple&>(a_other);
	size_t i;
	for (i = 0; i < t0.f_size(); ++i) {
		if (i >= t1.f_size()) return true;
		const t_slot& x = t0[i];
		const t_slot& y = t1[i];
		if (!f_as<bool>(x.f_equals(y))) return f_as<bool>(x.f_greater(y));
	}
	return i >= t1.f_size();
}

bool t_type_of<t_tuple>::f_equals(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_tuple>(a_self, L"this");
	if (!f_is<t_tuple>(a_other)) return false;
	const t_tuple& t0 = f_as<const t_tuple&>(a_self);
	const t_tuple& t1 = f_as<const t_tuple&>(a_other);
	if (t0.f_size() != t1.f_size()) return false;
	for (size_t i = 0; i < t0.f_size(); ++i) if (!f_as<bool>(t0[i].f_equals(t1[i]))) return false;
	return true;
}

void t_type_of<t_tuple>::f_each(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_tuple>(a_self, L"this");
	const t_tuple& t0 = f_as<const t_tuple&>(a_self);
	for (size_t i = 0; i < t0.f_size(); ++i) a_callable(t0[i]);
}

void t_type_of<t_tuple>::f_define()
{
	t_define<t_tuple, t_object>(f_global(), L"Tuple")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(const t_value&), f_hash>())
		(f_global()->f_symbol_get_at(), t_member<const t_slot& (t_tuple::*)(size_t) const, &t_tuple::operator[]>())
		(f_global()->f_symbol_less(), t_member<bool (*)(const t_value&, const t_value&), f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool (*)(const t_value&, const t_value&), f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool (*)(const t_value&, const t_value&), f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool (*)(const t_value&, const t_value&), f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
		(f_global()->f_symbol_size(), t_member<size_t (t_tuple::*)() const, &t_tuple::f_size>())
		(L"each", t_member<void (*)(const t_value&, const t_value&), f_each>())
	;
}

t_type* t_type_of<t_tuple>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_tuple>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_tuple& p = f_as<t_tuple&>(a_this);
	for (size_t i = 0; i < p.f_size(); ++i) a_scan(p[i]);
}

void t_type_of<t_tuple>::f_finalize(t_object* a_this)
{
	delete &f_as<t_tuple&>(a_this);
}

void t_type_of<t_tuple>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_transfer p = t_tuple::f_instantiate(a_n);
	t_tuple& tuple = f_as<t_tuple&>(p);
	for (size_t i = 0; i < a_n; ++i) tuple[i] = a_stack[i + 1].f_transfer();
	a_stack[0].f_construct(p);
}

void t_type_of<t_tuple>::f_hash(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	a_stack[0].f_construct(f_hash(t_value(a_this)));
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
	a_stack[0].f_construct(f_less(a_this, a0));
	context.f_done();
}

void t_type_of<t_tuple>::f_less_equal(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_less_equal(a_this, a0));
	context.f_done();
}

void t_type_of<t_tuple>::f_greater(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_greater(a_this, a0));
	context.f_done();
}

void t_type_of<t_tuple>::f_greater_equal(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_greater_equal(a_this, a0));
	context.f_done();
}

void t_type_of<t_tuple>::f_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_equals(a_this, a0));
	context.f_done();
}

void t_type_of<t_tuple>::f_not_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_not_equals(a_this, a0));
	context.f_done();
}

}
