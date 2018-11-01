#include <xemmai/tuple.h>

#include <xemmai/convert.h>

namespace xemmai
{

t_scoped t_tuple::f_instantiate(size_t a_size)
{
	return f_global()->f_type<t_tuple>()->f_new_sized<t_tuple>(true, sizeof(t_slot) * a_size, a_size);
}

t_scoped t_tuple::f_string() const
{
	std::vector<wchar_t> cs{L'\'', L'('};
	if (v_size > 0) for (size_t i = 0;;) {
		t_scoped x = (*this)[i].f_invoke(f_global()->f_symbol_string());
		f_check<t_string>(x, L"value");
		auto& s = f_as<const t_string&>(x);
		auto p = static_cast<const wchar_t*>(s);
		cs.insert(cs.end(), p, p + s.f_size());
		if (++i >= v_size) break;
		cs.push_back(L',');
		cs.push_back(L' ');
	}
	cs.push_back(L')');
	return t_string::f_instantiate(cs.data(), cs.size());
}

intptr_t t_tuple::f_hash() const
{
	intptr_t n = 0;
	for (size_t i = 0; i < v_size; ++i) {
		t_scoped x = (*this)[i].f_hash();
		f_check<intptr_t>(x, L"value");
		n ^= f_as<intptr_t>(x);
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
	auto& other = f_as<const t_tuple&>(a_other);
	if (this == &other) return true;
	if (v_size != other.f_size()) return false;
	for (size_t i = 0; i < v_size; ++i) if (!f_as<bool>((*this)[i].f_equals(other[i]))) return false;
	return true;
}

void t_tuple::f_each(const t_value& a_callable) const
{
//	for (size_t i = 0; i < v_size; ++i) a_callable((*this)[i]);
	if (a_callable.f_tag() < t_value::e_tag__OBJECT) f_throw(L"not supported."sv);
	auto p = static_cast<t_object*>(a_callable);
	t_scoped_stack stack(3);
	for (size_t i = 0; i < v_size; ++i) {
		stack[1].f_construct();
		stack[2].f_construct((*this)[i]);
		size_t n = p->f_call_without_loop(stack, 1);
		if (n != size_t(-1)) t_value::f_loop(stack, n);
		stack[0].f_destruct();
	}
}

void t_type_of<t_tuple>::f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n)
{
	if (a_stack[1].f_type() != f_global()->f_type<t_class>()) f_throw(L"must be class."sv);
	auto object = a_stack[1]->f_as<t_type>().f_new_sized<t_tuple>(true, sizeof(t_slot) * a_n, a_n);
	a_stack[1].f_destruct();
	auto& tuple = object->f_as<t_tuple>();
	for (size_t i = 0; i < a_n; ++i) tuple[i].f_construct(std::move(a_stack[i + 2]));
	a_stack[0].f_construct(std::move(object));
}

void t_type_of<t_tuple>::f_define()
{
	v_builtin = true;
	t_define<t_tuple, t_object>(f_global(), L"Tuple"sv, t_object::f_of(this))
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<t_scoped(t_tuple::*)() const, &t_tuple::f_string>())
		(f_global()->f_symbol_hash(), t_member<intptr_t(t_tuple::*)() const, &t_tuple::f_hash>())
		(f_global()->f_symbol_get_at(), t_member<const t_value&(t_tuple::*)(size_t) const, &t_tuple::f_get_at>())
		(f_global()->f_symbol_less(), t_member<bool(t_tuple::*)(const t_tuple&) const, &t_tuple::f_less>())
		(f_global()->f_symbol_less_equal(), t_member<bool(t_tuple::*)(const t_tuple&) const, &t_tuple::f_less_equal>())
		(f_global()->f_symbol_greater(), t_member<bool(t_tuple::*)(const t_tuple&) const, &t_tuple::f_greater>())
		(f_global()->f_symbol_greater_equal(), t_member<bool(t_tuple::*)(const t_tuple&) const, &t_tuple::f_greater_equal>())
		(f_global()->f_symbol_equals(), t_member<bool(t_tuple::*)(const t_value&) const, &t_tuple::f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool(t_tuple::*)(const t_value&) const, &t_tuple::f_not_equals>())
		(f_global()->f_symbol_size(), t_member<size_t(t_tuple::*)() const, &t_tuple::f_size>())
		(L"each"sv, t_member<void(t_tuple::*)(const t_value&) const, &t_tuple::f_each>())
	;
}

void t_type_of<t_tuple>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_tuple&>(a_this).f_scan(a_scan);
}

t_scoped t_type_of<t_tuple>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	auto object = f_new_sized<t_tuple>(true, sizeof(t_slot) * a_n, a_n);
	auto& tuple = object->f_as<t_tuple>();
	for (size_t i = 0; i < a_n; ++i) tuple[i].f_construct(a_stack[i + 2]);
	return object;
}

void t_type_of<t_tuple>::f_do_hash(t_object* a_this, t_stacked* a_stack)
{
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_hash());
}

size_t t_type_of<t_tuple>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<size_t>(a0.v_p, L"index");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_get_at(f_as<size_t>(a0.v_p)));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_less(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<t_tuple>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_less(f_as<const t_tuple&>(a0.v_p)));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_less_equal(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<t_tuple>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_less_equal(f_as<const t_tuple&>(a0.v_p)));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_greater(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<t_tuple>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_greater(f_as<const t_tuple&>(a0.v_p)));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_greater_equal(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<t_tuple>(a0.v_p, L"argument0");
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_greater_equal(f_as<const t_tuple&>(a0.v_p)));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_equals(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_equals(a0.v_p));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_not_equals(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	a_stack[0].f_construct(f_as<const t_tuple&>(a_this).f_not_equals(a0.v_p));
	return -1;
}

}
