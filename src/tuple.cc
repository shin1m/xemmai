#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_tuple::f_string() const
{
	t_stringer s;
	s << L'\'' << L'(';
	if (v_size > 0) for (size_t i = 0;;) {
		auto x = (*this)[i].f_string();
		f_check<t_string>(x, L"value");
		s << x->f_as<t_string>();
		if (++i >= v_size) break;
		s << L',' << L' ';
	}
	return s << L')';
}

intptr_t t_tuple::f_hash() const
{
	intptr_t n = 0;
	for (size_t i = 0; i < v_size; ++i) {
		auto x = (*this)[i].f_hash();
		f_check<intptr_t>(x, L"value");
		n ^= f_as<intptr_t>(x);
	}
	return n;
}

int t_tuple::f_compare(const t_tuple& a_other) const
{
	if (this == &a_other) return 0;
	size_t i = 0;
	for (; i < v_size; ++i) {
		if (i >= a_other.f_size()) return 1;
		auto& x = (*this)[i];
		auto& y = a_other[i];
		if (!x.f_equals(y)) return x.f_less(y) ? -1 : 1;
	}
	return i < a_other.f_size() ? -1 : 0;
}

bool t_tuple::f_equals(const t_pvalue& a_other) const
{
	if (!f_is<t_tuple>(a_other)) return false;
	auto& other = a_other->f_as<t_tuple>();
	if (this == &other) return true;
	if (v_size != other.v_size) return false;
	for (size_t i = 0; i < v_size; ++i) if (!(*this)[i].f_equals(other[i])) return false;
	return true;
}

void t_tuple::f_each(const t_pvalue& a_callable) const
{
//	for (size_t i = 0; i < v_size; ++i) a_callable((*this)[i]);
	auto p = static_cast<t_object*>(a_callable);
	if (reinterpret_cast<uintptr_t>(p) < c_tag__OBJECT) f_throw(L"not supported."sv);
	t_scoped_stack stack(3);
	for (size_t i = 0; i < v_size; ++i) {
		stack[1] = nullptr;
		stack[2] = (*this)[i];
		size_t n = p->f_call_without_loop(stack, 1);
		if (n != size_t(-1)) f_loop(stack, n);
	}
}

void t_type_of<t_tuple>::f_define()
{
	auto global = f_global();
	t_define{global}
	(global->f_symbol___string(), t_member<t_object*(t_tuple::*)() const, &t_tuple::f_string>())
	(global->f_symbol_size(), t_member<size_t(t_tuple::*)() const, &t_tuple::f_size>())
	(L"each"sv, t_member<void(t_tuple::*)(const t_pvalue&) const, &t_tuple::f_each>())
	.f_derive<t_tuple, t_object>();
}

void t_type_of<t_tuple>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	a_stack[0] = t_tuple::f_instantiate(a_n, [&](auto& tuple)
	{
		std::uninitialized_copy_n(a_stack + 2, a_n, tuple.f_entries());
	});
}

void t_type_of<t_tuple>::f_do_hash(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = a_this->f_as<t_tuple>().f_hash();
}

size_t t_type_of<t_tuple>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<size_t>(a_stack[2], L"index");
	a_stack[0] = a_this->f_as<t_tuple>().f_get_at(f_as<size_t>(a_stack[2]));
	return -1;
}

size_t t_type_of<t_tuple>::f_do_less(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_tuple>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_tuple>().f_compare(a_stack[2]->f_as<t_tuple>()) < 0;
	return -1;
}

size_t t_type_of<t_tuple>::f_do_less_equal(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_tuple>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_tuple>().f_compare(a_stack[2]->f_as<t_tuple>()) <= 0;
	return -1;
}

size_t t_type_of<t_tuple>::f_do_greater(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_tuple>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_tuple>().f_compare(a_stack[2]->f_as<t_tuple>()) > 0;
	return -1;
}

size_t t_type_of<t_tuple>::f_do_greater_equal(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_tuple>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_as<t_tuple>().f_compare(a_stack[2]->f_as<t_tuple>()) >= 0;
	return -1;
}

size_t t_type_of<t_tuple>::f_do_equals(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = a_this->f_as<t_tuple>().f_equals(a_stack[2]);
	return -1;
}

size_t t_type_of<t_tuple>::f_do_not_equals(t_object* a_this, t_pvalue* a_stack)
{
	a_stack[0] = !a_this->f_as<t_tuple>().f_equals(a_stack[2]);
	return -1;
}

}
