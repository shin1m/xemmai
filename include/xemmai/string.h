#ifndef XEMMAI__STRING_H
#define XEMMAI__STRING_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<std::wstring> : t_type
{
	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		t_transfer object = t_object::f_allocate_uninitialized(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new std::wstring(a_value));
		return object;
	}
	static std::wstring f_from_code(int a_code)
	{
		return std::wstring(1, a_code);
	}
	static t_transfer f_string(const t_transfer& a_self)
	{
		return a_self;
	}
	static int f_hash(const std::wstring& a_self)
	{
		int n = 0;
		for (std::wstring::const_iterator i = a_self.begin(); i != a_self.end(); ++i) n ^= static_cast<int>(*i);
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
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_fixed = v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
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
