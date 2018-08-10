#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::wstring> : t_derivable<t_holds<std::wstring, t_type_immutable>>
{
	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		t_scoped object = t_object::f_allocate_uninitialized(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new std::wstring(std::forward<T>(a_value)));
		return object;
	}
	static std::wstring f_from_code(intptr_t a_code)
	{
		return std::wstring(1, a_code);
	}
	static t_scoped f_string(t_scoped&& a_self)
	{
		return a_self;
	}
	static intptr_t f__hash(const std::wstring& a_self)
	{
		return std::hash<std::wstring>{}(a_self);
	}
	static bool f__less(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self < a_value;
	}
	static bool f__less_equal(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self <= a_value;
	}
	static bool f__greater(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self > a_value;
	}
	static bool f__greater_equal(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self >= a_value;
	}
	static bool f__equals(const std::wstring& a_self, const t_value& a_value);
	static bool f__not_equals(const std::wstring& a_self, const t_value& a_value);
	static std::wstring f_substring(const std::wstring& a_self, size_t a_i)
	{
		return a_self.substr(a_i);
	}
	static std::wstring f_substring(const std::wstring& a_self, size_t a_i, size_t a_n)
	{
		return a_self.substr(a_i, a_n);
	}
	static intptr_t f_code_at(const std::wstring& a_self, size_t a_i)
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

}

#endif
