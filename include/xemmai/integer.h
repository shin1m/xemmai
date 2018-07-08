#ifndef XEMMAI__INTEGER_H
#define XEMMAI__INTEGER_H

#include "class.h"
#include "macro.h"

namespace xemmai
{

template<>
struct t_fundamental<short>
{
	typedef intptr_t t_type;
};

template<>
struct t_fundamental<unsigned short>
{
	typedef intptr_t t_type;
};

template<>
struct t_fundamental<int>
{
	typedef intptr_t t_type;
};

template<>
struct t_fundamental<unsigned>
{
	typedef intptr_t t_type;
};

template<>
struct t_fundamental<long>
{
	typedef intptr_t t_type;
};

template<>
struct t_fundamental<unsigned long>
{
	typedef intptr_t t_type;
};

template<>
struct t_type_of<intptr_t> : t_derivable<t_bears<intptr_t, t_type_immutable>>
{
	template<typename T0>
	struct t_as
	{
		typedef typename t_fundamental<T0>::t_type t_type;

		template<typename T1>
		static t_type f_call(T1&& a_object)
		{
			return static_cast<t_type>(a_object.f_integer());
		}
		static t_type f_call(t_object* a_object)
		{
			return static_cast<t_type>(a_object->f_integer());
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			if (!std::is_same<typename t_fundamental<T0>::t_type, intptr_t>::value) return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__INTEGER:
				return true;
			case t_value::e_tag__NULL:
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<intptr_t>();
			}
		}
	};

	static t_scoped f_construct(t_type* a_class, intptr_t a_value)
	{
		return t_scoped(a_value);
	}
	static t_scoped f_construct(t_type* a_class, double a_value)
	{
		return t_scoped(static_cast<intptr_t>(a_value));
	}
	static t_scoped f_construct(t_type* a_class, const std::wstring& a_value)
	{
		return t_scoped(f_parse(a_value.c_str()));
	}
	static t_scoped f_construct_derived(t_type* a_class, intptr_t a_value)
	{
		t_scoped object = t_object::f_allocate_uninitialized(a_class);
		object.f_integer__(a_value);
		return object;
	}
	static intptr_t f_parse(const wchar_t* a_value)
	{
		intptr_t value;
		std::swscanf(a_value, XEMMAI__MACRO__L("%" SCNiPTR), &value);
		return value;
	}
	static std::wstring f_string(intptr_t a_self)
	{
		wchar_t cs[32];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), XEMMAI__MACRO__L("%" PRIdPTR), a_self);
		return cs;
	}
	static intptr_t f_hash(intptr_t a_self)
	{
		return a_self;
	}
	static intptr_t f_plus(intptr_t a_self)
	{
		return a_self;
	}
	static intptr_t f_minus(intptr_t a_self)
	{
		return -a_self;
	}
	static intptr_t f_complement(intptr_t a_self)
	{
		return ~a_self;
	}
	static t_scoped f_multiply(intptr_t a_self, const t_value& a_value);
	static t_scoped f_divide(intptr_t a_self, const t_value& a_value);
	static intptr_t f_modulus(intptr_t a_self, intptr_t a_value)
	{
		return a_self % a_value;
	}
	static t_scoped f_add(intptr_t a_self, const t_value& a_value);
	static t_scoped f_subtract(intptr_t a_self, const t_value& a_value);
	static intptr_t f_left_shift(intptr_t a_self, intptr_t a_value)
	{
		return a_self << a_value;
	}
	static size_t f_right_shift(size_t a_self, intptr_t a_value)
	{
		return a_self >> a_value;
	}
	static bool f_less(intptr_t a_self, const t_value& a_value);
	static bool f_less_equal(intptr_t a_self, const t_value& a_value);
	static bool f_greater(intptr_t a_self, const t_value& a_value);
	static bool f_greater_equal(intptr_t a_self, const t_value& a_value);
	static bool f_equals(intptr_t a_self, const t_value& a_value);
	static bool f_not_equals(intptr_t a_self, const t_value& a_value);
	static intptr_t f_and(intptr_t a_self, intptr_t a_value)
	{
		return a_self & a_value;
	}
	static intptr_t f_xor(intptr_t a_self, intptr_t a_value)
	{
		return a_self ^ a_value;
	}
	static intptr_t f_or(intptr_t a_self, intptr_t a_value)
	{
		return a_self | a_value;
	}
	static void f_define();

	using t_base::t_base;
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
