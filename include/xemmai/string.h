#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::wstring> : t_type
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
	static intptr_t f_hash(const std::wstring& a_self)
	{
		return std::hash<std::wstring>{}(a_self);
	}
	static std::wstring f_add(const std::wstring& a_self, const t_value& a_value);
	static bool f_less(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self < a_value;
	}
	static bool f_less_equal(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self <= a_value;
	}
	static bool f_greater(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self > a_value;
	}
	static bool f_greater_equal(const std::wstring& a_self, const std::wstring& a_value)
	{
		return a_self >= a_value;
	}
	static bool f_equals(const std::wstring& a_self, const t_value& a_value);
	static bool f_not_equals(const std::wstring& a_self, const t_value& a_value);
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

	t_type_of(t_scoped&& a_module, t_type* a_super) : t_type(std::move(a_module), a_super)
	{
		v_fixed = v_shared = v_immutable = true;
	}
	virtual t_type* f_derive();
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_add(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_less(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_less_equal(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_greater(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_greater_equal(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_equals(t_object* a_this, t_stacked* a_stack);
	virtual size_t f_not_equals(t_object* a_this, t_stacked* a_stack);
};

}

#endif
