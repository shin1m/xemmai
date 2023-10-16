#ifndef XEMMAI__INTEGER_H
#define XEMMAI__INTEGER_H

#include "class.h"
#include "string.h"
#include "macro.h"
#include <cinttypes>

namespace xemmai
{

template<typename T>
struct t_fundamental<T, std::enable_if_t<std::conjunction_v<std::is_same<T, std::remove_const_t<T>>, std::is_integral<T>, std::negation<std::is_same<T, bool>>>>>
{
	using t_type = intptr_t;
};

template<>
struct t_type_of<intptr_t> : t_derivable<t_bears<intptr_t>, t_derived_primitive<intptr_t>>
{
	template<typename T>
	struct t_cast
	{
		using t_type = typename t_fundamental<T>::t_type;

		static t_type f_as(auto&& a_object)
		{
			return static_cast<t_type>(a_object.f_integer());
		}
		static t_type f_as(t_object* a_object)
		{
			return static_cast<t_type>(a_object->f_as<intptr_t>());
		}
		static bool f_is(t_object* a_object)
		{
			if (!std::is_same_v<typename t_fundamental<T>::t_type, intptr_t>) return reinterpret_cast<uintptr_t>(a_object) >= e_tag__OBJECT && a_object->f_type()->f_derives<typename t_fundamental<T>::t_type>();
			switch (reinterpret_cast<uintptr_t>(a_object)) {
			case e_tag__INTEGER:
				return true;
			case e_tag__NULL:
			case e_tag__BOOLEAN:
			case e_tag__FLOAT:
				return false;
			default:
				return a_object->f_type()->f_derives<intptr_t>();
			}
		}
	};

	static t_pvalue f_construct(t_type* a_class, intptr_t a_value)
	{
		return a_value;
	}
	static t_pvalue f_construct(t_type* a_class, double a_value)
	{
		return static_cast<intptr_t>(a_value);
	}
	static t_pvalue f_construct(t_type* a_class, const t_string& a_value)
	{
		intptr_t value;
		std::swscanf(a_value, XEMMAI__MACRO__L("%" SCNiPTR), &value);
		return value;
	}
	XEMMAI__PUBLIC static t_object* f__string(intptr_t a_self);
	static intptr_t f__hash(intptr_t a_self)
	{
		return a_self;
	}
	static intptr_t f__plus(intptr_t a_self)
	{
		return a_self;
	}
	static intptr_t f__minus(intptr_t a_self)
	{
		return -a_self;
	}
	static intptr_t f__complement(intptr_t a_self)
	{
		return ~a_self;
	}
	XEMMAI__LOCAL static t_pvalue f__multiply(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static t_pvalue f__divide(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static intptr_t f__modulus(intptr_t a_self, intptr_t a_value)
	{
		return a_self % a_value;
	}
	XEMMAI__LOCAL static t_pvalue f__add(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static t_pvalue f__subtract(intptr_t a_self, const t_pvalue& a_value);
	static intptr_t f__left_shift(intptr_t a_self, intptr_t a_value)
	{
		return a_self << a_value;
	}
	static size_t f__right_shift(size_t a_self, intptr_t a_value)
	{
		return a_self >> a_value;
	}
	XEMMAI__LOCAL static bool f__less(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static bool f__less_equal(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static bool f__greater(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static bool f__greater_equal(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static bool f__equals(intptr_t a_self, const t_pvalue& a_value);
	XEMMAI__LOCAL static bool f__not_equals(intptr_t a_self, const t_pvalue& a_value);
	static intptr_t f__and(intptr_t a_self, intptr_t a_value)
	{
		return a_self & a_value;
	}
	static intptr_t f__xor(intptr_t a_self, intptr_t a_value)
	{
		return a_self ^ a_value;
	}
	static intptr_t f__or(intptr_t a_self, intptr_t a_value)
	{
		return a_self | a_value;
	}
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
	XEMMAI__PUBLIC t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
