#ifndef XEMMAI__INTEGER_H
#define XEMMAI__INTEGER_H

#include "object.h"

namespace xemmai
{

template<>
struct t_fundamental<short>
{
	typedef ptrdiff_t t_type;
};

template<>
struct t_fundamental<unsigned short>
{
	typedef ptrdiff_t t_type;
};

template<>
struct t_fundamental<int>
{
	typedef ptrdiff_t t_type;
};

template<>
struct t_fundamental<unsigned>
{
	typedef ptrdiff_t t_type;
};

template<>
struct t_fundamental<long>
{
	typedef ptrdiff_t t_type;
};

template<>
struct t_fundamental<unsigned long>
{
	typedef ptrdiff_t t_type;
};

template<>
struct t_type_of<ptrdiff_t> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			return static_cast<T0>(a_object.f_integer());
		}
	};
	template<typename T>
	struct t_as<T, t_object*>
	{
		typedef T t_type;

		static T f_call(t_object* a_object)
		{
			return static_cast<T>(a_object->f_integer());
		}
	};
	template<typename T0, typename T1>
	struct t_of
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
		}
	};
	template<typename T>
	struct t_of<ptrdiff_t, T>
	{
		static bool f_call(T a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__INTEGER:
				return true;
			case t_value::e_tag__NULL:
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return dynamic_cast<t_type_of<ptrdiff_t>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
			}
		}
	};
	template<typename T0, typename T1>
	struct t_is
	{
		static bool f_call(T1 a_object)
		{
			return t_of<typename t_fundamental<T0>::t_type, T1>::f_call(a_object);
		}
	};

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		return t_transfer(static_cast<ptrdiff_t>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, ptrdiff_t a_value)
	{
		return t_transfer(a_value);
	}
	static t_transfer f_construct(t_object* a_class, double a_value)
	{
		return f_construct(a_class, static_cast<ptrdiff_t>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, const std::wstring& a_value)
	{
		return f_construct(a_class, f_parse(a_value.c_str()));
	}
	static t_transfer f_construct_derived(t_object* a_class, ptrdiff_t a_value)
	{
		t_transfer object = t_object::f_allocate_uninitialized(a_class);
		object.f_integer__(a_value);
		return object;
	}
	static ptrdiff_t f_parse(const wchar_t* a_value)
	{
		ptrdiff_t value;
		std::swscanf(a_value, L"%" XEMMAI__MACRO__L(XEMMAI__PORTABLE__FORMAT_SIZE_T) L"i", &value);
		return value;
	}
	static std::wstring f_string(ptrdiff_t a_self)
	{
		wchar_t cs[32];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%" XEMMAI__MACRO__L(XEMMAI__PORTABLE__FORMAT_SIZE_T) L"d", a_self);
		return cs;
	}
	static ptrdiff_t f_hash(ptrdiff_t a_self)
	{
		return a_self;
	}
	static ptrdiff_t f_plus(ptrdiff_t a_self)
	{
		return a_self;
	}
	static ptrdiff_t f_minus(ptrdiff_t a_self)
	{
		return -a_self;
	}
	static ptrdiff_t f_complement(ptrdiff_t a_self)
	{
		return ~a_self;
	}
	static t_transfer f_multiply(ptrdiff_t a_self, const t_value& a_value);
	static t_transfer f_divide(ptrdiff_t a_self, const t_value& a_value);
	static ptrdiff_t f_modulus(ptrdiff_t a_self, ptrdiff_t a_value)
	{
		return a_self % a_value;
	}
	static t_transfer f_add(ptrdiff_t a_self, const t_value& a_value);
	static t_transfer f_subtract(ptrdiff_t a_self, const t_value& a_value);
	static ptrdiff_t f_left_shift(ptrdiff_t a_self, ptrdiff_t a_value)
	{
		return a_self << a_value;
	}
	static ptrdiff_t f_right_shift(ptrdiff_t a_self, ptrdiff_t a_value)
	{
		return a_self >> a_value;
	}
	static bool f_less(ptrdiff_t a_self, const t_value& a_value);
	static bool f_less_equal(ptrdiff_t a_self, const t_value& a_value);
	static bool f_greater(ptrdiff_t a_self, const t_value& a_value);
	static bool f_greater_equal(ptrdiff_t a_self, const t_value& a_value);
	static bool f_equals(ptrdiff_t a_self, const t_value& a_value);
	static bool f_not_equals(ptrdiff_t a_self, const t_value& a_value);
	static ptrdiff_t f_and(ptrdiff_t a_self, ptrdiff_t a_value)
	{
		return a_self & a_value;
	}
	static ptrdiff_t f_xor(ptrdiff_t a_self, ptrdiff_t a_value)
	{
		return a_self ^ a_value;
	}
	static ptrdiff_t f_or(ptrdiff_t a_self, ptrdiff_t a_value)
	{
		return a_self | a_value;
	}
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_shared = v_immutable = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
