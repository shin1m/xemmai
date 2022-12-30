#include <xemmai/bytes.h>
#include <xemmai/convert.h>

namespace xemmai
{

t_object* t_bytes::f_instantiate(size_t a_size)
{
	return t_type_of<t_bytes>::f__construct(f_global()->f_type<t_bytes>(), a_size);
}

t_object* t_bytes::f_string() const
{
	std::vector<wchar_t> cs{L'\'', L'['};
	if (v_size > 0) for (size_t i = 0;;) {
		wchar_t s[3];
		std::swprintf(s, 3, L"%02x", (*this)[i]);
		cs.insert(cs.end(), s, s + 2);
		if (++i >= v_size) break;
		cs.push_back(L',');
		cs.push_back(L' ');
	}
	cs.push_back(L']');
	return t_string::f_instantiate(cs.data(), cs.size());
}

void t_type_of<t_bytes>::f_define()
{
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(t_bytes::*)() const, &t_bytes::f_string>())
		(L"size"sv, t_member<size_t(t_bytes::*)() const, &t_bytes::f_size>())
		(L"copy"sv, t_member<void(t_bytes::*)(intptr_t, size_t, t_bytes&, intptr_t) const, &t_bytes::f_copy>())
	.f_derive<t_bytes, t_object>();
}

void t_type_of<t_bytes>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	a_stack[0] = t_construct_with<t_pvalue(*)(t_type*, size_t), f__construct>::t_bind<t_bytes>::f_do(this, a_stack, a_n);
}

size_t t_type_of<t_bytes>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<intptr_t>(a_stack[2], L"index");
	a_stack[0] = a_this->f_as<t_bytes>().f_get_at(f_as<intptr_t>(a_stack[2]));
	return -1;
}

size_t t_type_of<t_bytes>::f_do_set_at(t_object* a_this, t_pvalue* a_stack)
{
	f_check<intptr_t>(a_stack[2], L"index");
	f_check<intptr_t>(a_stack[3], L"value");
	a_stack[0] = a_this->f_as<t_bytes>().f_set_at(f_as<intptr_t>(a_stack[2]), f_as<intptr_t>(a_stack[3]));
	return -1;
}

}
