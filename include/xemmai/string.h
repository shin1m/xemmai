#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"

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
	template<typename T>
	static t_object* f_instantiate(size_t a_n, T a_fill);

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
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return f_object(std::forward<T1>(a_object))->template f_as<t_string>();
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		static_assert(std::is_same_v<std::decay_t<T0>, t_string>);

		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return p ? &p->template f_as<t_string>() : nullptr;
		}
	};

	static t_object* f__construct(t_type* a_class, const wchar_t* a_p, size_t a_n);
	static t_pvalue f__construct(t_type* a_class, const t_string& a_value)
	{
		return f__construct(a_class, static_cast<const wchar_t*>(a_value), a_value.v_size);
	}
	static t_object* f__construct(t_type* a_class, std::wstring_view a_value)
	{
		return f__construct(a_class, a_value.data(), a_value.size());
	}
	static t_object* f__construct(t_type* a_class, const t_string& a_x, const t_string& a_y);
	template<typename T>
	static t_pvalue f_transfer(const t_global* a_library, T&& a_value);
	static t_object* f_from_code(t_global* a_library, intptr_t a_code);
	static t_object* f_string(const t_pvalue& a_self)
	{
		return a_self;
	}
	static t_object* f__add(t_object* a_self, const t_pvalue& a_value);
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
struct t_type_of<t_string>::t_as<std::wstring_view&&>
{
	template<typename T>
	static std::wstring_view f_call(T&& a_object)
	{
		return f_object(std::forward<T>(a_object))->template f_as<t_string>();
	}
};

}

#endif
