#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::wstring> : t_type
{
	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T a_value)
	{
		t_scoped object = t_object::f_allocate_uninitialized(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new std::wstring(a_value));
		return object;
	}
	static std::wstring f_from_code(ptrdiff_t a_code)
	{
		return std::wstring(1, a_code);
	}
	static t_scoped f_string(t_scoped&& a_self)
	{
		return a_self;
	}
	static ptrdiff_t f_hash(const std::wstring& a_self)
	{
		ptrdiff_t n = 0;
		for (auto c : a_self) n ^= c;
		return n;
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
	static ptrdiff_t f_code_at(const std::wstring& a_self, size_t a_i)
	{
		return a_self[a_i];
	}
	static void f_define();

	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_fixed = v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_slot* a_stack);
	virtual void f_add(t_object* a_this, t_slot* a_stack);
	virtual void f_less(t_object* a_this, t_slot* a_stack);
	virtual void f_less_equal(t_object* a_this, t_slot* a_stack);
	virtual void f_greater(t_object* a_this, t_slot* a_stack);
	virtual void f_greater_equal(t_object* a_this, t_slot* a_stack);
	virtual void f_equals(t_object* a_this, t_slot* a_stack);
	virtual void f_not_equals(t_object* a_this, t_slot* a_stack);
};

}

#endif
