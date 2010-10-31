#ifndef XEMMAI__INTEGER_H
#define XEMMAI__INTEGER_H

#include "object.h"

namespace xemmai
{

template<>
struct t_fundamental<size_t>
{
	typedef int t_type;
};

template<>
struct t_fundamental<long>
{
	typedef int t_type;
};

template<>
struct t_fundamental<unsigned long>
{
	typedef int t_type;
};

template<>
struct t_type_of<int> : t_type
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
	struct t_of<int, T>
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
				return dynamic_cast<t_type_of<int>*>(&f_as<t_type&>(f_object(a_object)->f_type())) != 0;
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
		return t_transfer(static_cast<int>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, int a_value)
	{
		return t_transfer(a_value);
	}
	static t_transfer f_construct(t_object* a_class, double a_value)
	{
		return f_construct(a_class, static_cast<int>(a_value));
	}
	static t_transfer f_construct(t_object* a_class, const std::wstring& a_value)
	{
		wchar_t* p;
		return f_construct(a_class, static_cast<int>(std::wcstol(a_value.c_str(), &p, 10)));
	}
	static std::wstring f_string(int a_self)
	{
		wchar_t cs[16];
		std::swprintf(cs, sizeof(cs) / sizeof(wchar_t), L"%d", a_self);
		return cs;
	}
	static int f_hash(int a_self)
	{
		return a_self;
	}
	static int f_plus(int a_self)
	{
		return a_self;
	}
	static int f_minus(int a_self)
	{
		return -a_self;
	}
	static int f_complement(int a_self)
	{
		return ~a_self;
	}
	static int f_multiply(int a_self, int a_value)
	{
		return a_self * a_value;
	}
	static int f_divide(int a_self, int a_value)
	{
		return a_self / a_value;
	}
	static int f_modulus(int a_self, int a_value)
	{
		return a_self % a_value;
	}
	static int f_add(int a_self, int a_value)
	{
		return a_self + a_value;
	}
	static int f_subtract(int a_self, int a_value)
	{
		return a_self - a_value;
	}
	static int f_left_shift(int a_self, int a_value)
	{
		return a_self << a_value;
	}
	static int f_right_shift(int a_self, int a_value)
	{
		return a_self >> a_value;
	}
	static bool f_less(int a_self, int a_value)
	{
		return a_self < a_value;
	}
	static bool f_less_equal(int a_self, int a_value)
	{
		return a_self <= a_value;
	}
	static bool f_greater(int a_self, int a_value)
	{
		return a_self > a_value;
	}
	static bool f_greater_equal(int a_self, int a_value)
	{
		return a_self >= a_value;
	}
	static bool f_equals(int a_self, const t_value& a_value);
	static bool f_not_equals(int a_self, const t_value& a_value);
	static int f_and(int a_self, int a_value)
	{
		return a_self & a_value;
	}
	static int f_xor(int a_self, int a_value)
	{
		return a_self ^ a_value;
	}
	static int f_or(int a_self, int a_value)
	{
		return a_self | a_value;
	}
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n);
};

}

#endif
