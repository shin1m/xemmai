#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"
#include <algorithm>

//#define XEMMAI__STRING__HASHED

namespace xemmai
{

class t_string
{
	friend struct t_finalizes<t_bears<t_string, t_type_immutable>>;
	friend struct t_type_of<t_string>;

	size_t v_size;
#ifdef XEMMAI__STRING__HASHED
	mutable bool v_hashed = false;
	mutable size_t v_hash;
#endif

	void* operator new(size_t a_size, size_t a_n)
	{
		return new char[a_size + sizeof(wchar_t) * a_n];
	}
	void operator delete(void* a_p)
	{
		delete[] static_cast<char*>(a_p);
	}
	void operator delete(void* a_p, size_t)
	{
		delete[] static_cast<char*>(a_p);
	}

	t_string(size_t a_size) : v_size(a_size)
	{
	}
	~t_string() = default;
	wchar_t* f_entries() const
	{
		return const_cast<wchar_t*>(static_cast<const wchar_t*>(*this));
	}

public:
	static t_string* f_new(const wchar_t* a_p, size_t a_n)
	{
		auto p = new(a_n) t_string(a_n);
		std::copy_n(a_p, a_n, p->f_entries());
		return p;
	}
	static t_string* f_new(const t_string& a_value)
	{
		return f_new(static_cast<const wchar_t*>(a_value), a_value.v_size);
	}
	static t_string* f_new(std::wstring_view a_value)
	{
		return f_new(a_value.data(), a_value.size());
	}
	static t_string* f_new(const t_string& a_x, const t_string& a_y)
	{
		size_t n = a_x.v_size + a_y.v_size;
		auto p = new(n) t_string(n);
		std::copy_n(static_cast<const wchar_t*>(a_y), a_y.v_size, std::copy_n(static_cast<const wchar_t*>(a_x), a_x.v_size, p->f_entries()));
		return p;
	}
	static t_scoped f_instantiate(const wchar_t* a_p, size_t a_n);
	static t_scoped f_instantiate(std::wstring_view a_value)
	{
		return f_instantiate(a_value.data(), a_value.size());
	}

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
#ifdef XEMMAI__STRING__HASHED
		if (!v_hashed) {
			v_hash = std::hash<std::wstring_view>{}(*this);
			v_hashed = true;
		}
		return v_hash;
#else
		return std::hash<std::wstring_view>{}(*this);
#endif
	}
	bool operator==(const t_string& a_x) const
	{
#ifdef XEMMAI__STRING__HASHED
		return f_hash() == a_x.f_hash() && static_cast<std::wstring_view>(*this) == a_x;
#else
		return v_size == a_x.v_size && std::char_traits<wchar_t>::compare(*this, a_x, v_size) == 0;
#endif
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
	typedef t_string t_type;
};

template<>
struct t_fundamental<std::wstring_view>
{
	typedef t_string t_type;
};

template<>
struct t_type_of<t_string> : t_derivable<t_holds<t_string, t_type_immutable>>
{
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *static_cast<t_string*>(f_object(std::forward<T1>(a_object))->f_pointer());
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		static_assert(std::is_same<std::decay_t<T0>, t_string>::value);

		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<size_t>(p) == t_value::e_tag__NULL ? nullptr : static_cast<T0*>(p->f_pointer());
		}
	};

	template<typename... T_an>
	static t_scoped f__construct(t_type* a_class, T_an&&... a_an)
	{
		t_scoped object = t_object::f_allocate(a_class, true);
		object.f_pointer__(t_string::f_new(std::forward<T_an>(a_an)...));
		return object;
	}
	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		return f__construct(a_extension->template f_type<typename t_fundamental<T>::t_type>(), std::forward<T>(a_value));
	}
	template<typename T>
	static t_scoped f_transfer(const t_global* a_extension, T&& a_value);
	static t_scoped f_from_code(t_global* a_extension, intptr_t a_code);
	static t_scoped f_string(t_scoped&& a_self)
	{
		return a_self;
	}
	static t_scoped f__add(t_global* a_extension, t_object* a_self, t_scoped&& a_value);
	static t_scoped f__add(t_global* a_extension, const t_value& a_self, t_scoped&& a_value)
	{
		return f__add(a_extension, static_cast<t_object*>(a_self), std::move(a_value));
	}
	static bool f__less(const t_string& a_self, const t_string& a_value)
	{
		return a_self.f_compare(a_value) < 0;
	}
	static bool f__less_equal(const t_string& a_self, const t_string& a_value)
	{
		return a_self.f_compare(a_value) <= 0;
	}
	static bool f__greater(const t_string& a_self, const t_string& a_value)
	{
		return a_self.f_compare(a_value) > 0;
	}
	static bool f__greater_equal(const t_string& a_self, const t_string& a_value)
	{
		return a_self.f_compare(a_value) >= 0;
	}
	static bool f__equals(const t_string& a_self, const t_value& a_value);
	static bool f__not_equals(const t_string& a_self, const t_value& a_value);
	static t_scoped f__substring(t_global* a_extension, const t_string& a_self, size_t a_i, size_t a_n);
	static t_scoped f_substring(t_global* a_extension, const t_string& a_self, size_t a_i)
	{
		return f__substring(a_extension, a_self, a_i, a_self.f_size() - a_i);
	}
	static t_scoped f_substring(t_global* a_extension, const t_string& a_self, size_t a_i, size_t a_n)
	{
		return f__substring(a_extension, a_self, a_i, std::min(a_n, a_self.f_size() - a_i));
	}
	static intptr_t f_code_at(const t_string& a_self, size_t a_i)
	{
		return a_self[a_i];
	}
	static void f_define();

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static void f_do_hash(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_add(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_less(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_less_equal(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_greater(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_greater_equal(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_equals(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_not_equals(t_object* a_this, t_stacked* a_stack);
};

template<>
struct t_type_of<t_string>::t_as<std::wstring_view&&>
{
	template<typename T>
	static std::wstring_view f_call(T&& a_object)
	{
		return *static_cast<t_string*>(f_object(std::forward<T>(a_object))->f_pointer());
	}
};

}

#endif
