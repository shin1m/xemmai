#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"
#include <algorithm>

namespace xemmai
{

class t_string
{
	friend struct t_finalizes<t_bears<t_string>>;
	friend struct t_type_of<t_string>;

	size_t v_size;

	t_string(size_t a_size) : v_size(a_size)
	{
	}
	~t_string() = default;
	wchar_t* f_entries() const
	{
		return const_cast<wchar_t*>(static_cast<const wchar_t*>(*this));
	}

public:
	XEMMAI__PORTABLE__EXPORT static t_object* f_instantiate(const wchar_t* a_p, size_t a_n);
	static t_object* f_instantiate(std::wstring_view a_value)
	{
		return f_instantiate(a_value.data(), a_value.size());
	}
	static t_object* f_instantiate(size_t a_n, auto a_fill);

	size_t f_size() const
	{
		return v_size;
	}
	operator const wchar_t*() const
	{
		return reinterpret_cast<const wchar_t*>(this + 1);
	}
	operator std::wstring_view() const
	{
		return {static_cast<const wchar_t*>(*this), v_size};
	}
	operator std::wstring() const
	{
		return {static_cast<const wchar_t*>(*this), v_size};
	}
	intptr_t f_hash() const
	{
		return std::hash<std::wstring_view>{}(*this);
	}
	bool operator==(const t_string& a_x) const
	{
		return v_size == a_x.v_size && std::char_traits<wchar_t>::compare(*this, a_x, v_size) == 0;
	}
	bool operator!=(const t_string& a_x) const
	{
		return !(*this == a_x);
	}
	int f_compare(const t_string& a_x) const
	{
		int n = std::char_traits<wchar_t>::compare(*this, a_x, std::min(v_size, a_x.v_size));
		if (n != 0) return n;
		return v_size < a_x.v_size ? -1 : v_size > a_x.v_size ? 1 : 0;
	}
};

template<>
struct t_fundamental<std::wstring>
{
	using t_type = t_string;
};

template<>
struct t_fundamental<std::wstring_view>
{
	using t_type = t_string;
};

template<>
struct t_type_of<t_string> : t_holds<t_string>
{
	static t_object* f__construct(t_type* a_class, size_t a_n);
	static t_object* f__construct(t_type* a_class, const wchar_t* a_p, size_t a_n)
	{
		auto object = f__construct(a_class, a_n);
		*std::copy_n(a_p, a_n, (new(object->f_data()) t_string(a_n))->f_entries()) = L'\0';
		return object;
	}
	static t_pvalue f__construct(t_type* a_class, const t_string& a_value)
	{
		return f__construct(a_class, static_cast<const wchar_t*>(a_value), a_value.v_size);
	}
	static t_object* f__construct(t_type* a_class, std::wstring_view a_value)
	{
		return f__construct(a_class, a_value.data(), a_value.size());
	}
	static t_object* f__construct(t_type* a_class, const t_string& a_x, const t_string& a_y)
	{
		size_t n = a_x.v_size + a_y.v_size;
		auto object = f__construct(a_class, n);
		*std::copy_n(static_cast<const wchar_t*>(a_y), a_y.v_size, std::copy_n(static_cast<const wchar_t*>(a_x), a_x.v_size, (new(object->f_data()) t_string(n))->f_entries())) = L'\0';
		return object;
	}
	static t_pvalue f_transfer(const t_global* a_library, auto&& a_value);
	static t_object* f_from_code(t_global* a_library, intptr_t a_code);
	static t_object* f_string(const t_pvalue& a_self)
	{
		return a_self;
	}
	static XEMMAI__PORTABLE__ALWAYS_INLINE t_object* f__add(t_object* a_self, const t_pvalue& a_value)
	{
		auto add = [&](t_object* x)
		{
			auto& s0 = a_self->f_as<t_string>();
			if (s0.f_size() <= 0) return x;
			auto& s1 = x->f_as<t_string>();
			return s1.f_size() <= 0 ? a_self : f__construct(a_self->f_type(), s0, s1);
		};
		if (f_is<t_string>(a_value)) return add(a_value);
		auto x = a_value.f_string();
		f_check<t_string>(x, L"argument0");
		return add(x);
	}
	static bool f__equals(const t_string& a_self, const t_pvalue& a_value)
	{
		return f_is<t_string>(a_value) && a_self == a_value->f_as<t_string>();
	}
	static t_object* f__substring(t_global* a_library, const t_string& a_self, size_t a_i, size_t a_n);
	static t_object* f_substring(t_global* a_library, const t_string& a_self, size_t a_i)
	{
		if (a_i > a_self.f_size()) f_throw(L"out of range."sv);
		return f__substring(a_library, a_self, a_i, a_self.f_size() - a_i);
	}
	static t_object* f_substring(t_global* a_library, const t_string& a_self, size_t a_i, size_t a_n)
	{
		if (a_i > a_self.f_size()) f_throw(L"out of range."sv);
		return f__substring(a_library, a_self, a_i, std::min(a_n, a_self.f_size() - a_i));
	}
	static intptr_t f_code_at(const t_string& a_self, size_t a_i)
	{
		if (a_i >= a_self.f_size()) f_throw(L"out of range."sv);
		return a_self[a_i];
	}
	static void f_define();

	using t_base::t_base;
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
	static void f_do_hash(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_add(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_less(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_less_equal(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_greater(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_greater_equal(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_equals(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_not_equals(t_object* a_this, t_pvalue* a_stack);
};

template<>
struct t_type::t_cast<std::wstring_view&&> : t_type::t_cast<std::wstring_view>
{
};

struct t_stringer
{
	t_object* v_p;
	wchar_t* v_i;
	wchar_t* v_j;

	void f_grow();

	t_stringer();
	operator t_object*() const
	{
		*v_i = L'\0';
		v_p->f_as<size_t>() = v_i - v_p->f_as<t_string>();
		return v_p;
	}
	t_stringer& operator<<(wchar_t a_c)
	{
		if (v_i == v_j) f_grow();
		*v_i++ = a_c;
		return *this;
	}
	t_stringer& operator<<(std::wstring_view a_s)
	{
		while (v_j - v_i < a_s.size()) f_grow();
		v_i = std::copy(a_s.begin(), a_s.end(), v_i);
		return *this;
	}
};

}

#endif
