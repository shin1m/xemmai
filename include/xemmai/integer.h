#ifndef XEMMAI__INTEGER_H
#define XEMMAI__INTEGER_H

#include "object.h"
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
struct t_type_of<intptr_t> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef typename t_fundamental<T0>::t_type t_type;

		static t_type f_call(T1 a_object)
		{
			return static_cast<t_type>(a_object.f_integer());
		}
	};
	template<typename T>
	struct t_as<T, t_object*>
	{
		typedef typename t_fundamental<T>::t_type t_type;

		static t_type f_call(t_object* a_object)
		{
			return static_cast<t_type>(a_object->f_integer());
		}
	};
	template<typename T0, typename T1>
	struct t_of
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != nullptr;
		}
	};
	template<typename T>
	struct t_of<intptr_t, T>
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
				return dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != nullptr;
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
	static t_scoped f_transfer(T_extension* a_extension, T a_value)
	{
		return t_value(static_cast<intptr_t>(a_value));
	}
	static t_scoped f_construct(t_object* a_class, intptr_t a_value)
	{
		return t_value(a_value);
	}
	static t_scoped f_construct(t_object* a_class, double a_value)
	{
		return f_construct(a_class, static_cast<intptr_t>(a_value));
	}
	static t_scoped f_construct(t_object* a_class, const std::wstring& a_value)
	{
		return f_construct(a_class, f_parse(a_value.c_str()));
	}
	static t_scoped f_construct_derived(t_object* a_class, intptr_t a_value)
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

	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_shared = v_immutable = true;
	}
	XEMMAI__PORTABLE__EXPORT virtual t_type* f_derive(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
