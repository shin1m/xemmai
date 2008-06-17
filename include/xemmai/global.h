#ifndef XEMMAI__GLOBAL_H
#define XEMMAI__GLOBAL_H

#include "module.h"
#include "integer.h"
#include "float.h"
#include "string.h"
#include "parser.h"

namespace xemmai
{

struct t_class;
struct t_symbol;
class t_scope;
struct t_method;
struct t_code;
struct t_lambda;
class t_null;
class t_tuple;
class t_array;
class t_dictionary;

t_global* f_global();

class t_global : public t_extension
{
	friend t_global* f_global();

	XEMMAI__PORTABLE__EXPORT static t_global* v_instance;

	t_slot v_type_object;
	t_slot v_type_class;
	t_slot v_type_module;
	t_slot v_type_fiber;
	t_slot v_type_thread;
	t_slot v_type_symbol;
	t_slot v_type_scope;
	t_slot v_type_method;
	t_slot v_type_code;
	t_slot v_type_lambda;
	t_slot v_type_native;
	t_slot v_type_throwable;
	t_slot v_type_null;
	t_slot v_type_boolean;
	t_slot v_type_integer;
	t_slot v_type_float;
	t_slot v_type_string;
	t_slot v_type_tuple;
	t_slot v_type_array;
	t_slot v_type_dictionary;
	t_slot v_type_lexer__error;
	t_slot v_type_parser__error;
	t_slot v_symbol_construct;
	t_slot v_symbol_initialize;
	t_slot v_symbol_string;
	t_slot v_symbol_hash;
	t_slot v_symbol_call;
	t_slot v_symbol_get_at;
	t_slot v_symbol_set_at;
	t_slot v_symbol_plus;
	t_slot v_symbol_minus;
	t_slot v_symbol_not;
	t_slot v_symbol_complement;
	t_slot v_symbol_multiply;
	t_slot v_symbol_divide;
	t_slot v_symbol_modulus;
	t_slot v_symbol_add;
	t_slot v_symbol_subtract;
	t_slot v_symbol_left_shift;
	t_slot v_symbol_right_shift;
	t_slot v_symbol_less;
	t_slot v_symbol_less_equal;
	t_slot v_symbol_greater;
	t_slot v_symbol_greater_equal;
	t_slot v_symbol_equals;
	t_slot v_symbol_not_equals;
	t_slot v_symbol_and;
	t_slot v_symbol_xor;
	t_slot v_symbol_or;
	t_slot v_null;
	t_slot v_true;
	t_slot v_false;
	t_slot v_tuple_empty;

public:
	t_global(t_object* a_module, const t_transfer& a_type_object, const t_transfer& a_type_class, const t_transfer& a_type_module, const t_transfer& a_type_fiber, const t_transfer& a_type_thread);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const;
	t_object* f_symbol_construct() const
	{
		return v_symbol_construct;
	}
	t_object* f_symbol_initialize() const
	{
		return v_symbol_initialize;
	}
	t_object* f_symbol_string() const
	{
		return v_symbol_string;
	}
	t_object* f_symbol_hash() const
	{
		return v_symbol_hash;
	}
	t_object* f_symbol_call() const
	{
		return v_symbol_call;
	}
	t_object* f_symbol_get_at() const
	{
		return v_symbol_get_at;
	}
	t_object* f_symbol_set_at() const
	{
		return v_symbol_set_at;
	}
	t_object* f_symbol_plus() const
	{
		return v_symbol_plus;
	}
	t_object* f_symbol_minus() const
	{
		return v_symbol_minus;
	}
	t_object* f_symbol_not() const
	{
		return v_symbol_not;
	}
	t_object* f_symbol_complement() const
	{
		return v_symbol_complement;
	}
	t_object* f_symbol_multiply() const
	{
		return v_symbol_multiply;
	}
	t_object* f_symbol_divide() const
	{
		return v_symbol_divide;
	}
	t_object* f_symbol_modulus() const
	{
		return v_symbol_modulus;
	}
	t_object* f_symbol_add() const
	{
		return v_symbol_add;
	}
	t_object* f_symbol_subtract() const
	{
		return v_symbol_subtract;
	}
	t_object* f_symbol_left_shift() const
	{
		return v_symbol_left_shift;
	}
	t_object* f_symbol_right_shift() const
	{
		return v_symbol_right_shift;
	}
	t_object* f_symbol_less() const
	{
		return v_symbol_less;
	}
	t_object* f_symbol_less_equal() const
	{
		return v_symbol_less_equal;
	}
	t_object* f_symbol_greater() const
	{
		return v_symbol_greater;
	}
	t_object* f_symbol_greater_equal() const
	{
		return v_symbol_greater_equal;
	}
	t_object* f_symbol_equals() const
	{
		return v_symbol_equals;
	}
	t_object* f_symbol_not_equals() const
	{
		return v_symbol_not_equals;
	}
	t_object* f_symbol_and() const
	{
		return v_symbol_and;
	}
	t_object* f_symbol_xor() const
	{
		return v_symbol_xor;
	}
	t_object* f_symbol_or() const
	{
		return v_symbol_or;
	}
	t_object* f_null() const
	{
		return v_null;
	}
	t_object* f_true() const
	{
		return v_true;
	}
	t_object* f_false() const
	{
		return v_false;
	}
	t_object* f_tuple_empty() const
	{
		return v_tuple_empty;
	}
	t_transfer f_as(t_object* a_value) const
	{
		return a_value;
	}
	t_transfer f_as(const t_transfer& a_value) const
	{
		return a_value;
	}
	t_transfer f_as(bool a_value) const
	{
		return a_value ? v_true : v_false;
	}
	t_transfer f_as(int a_value) const
	{
		t_transfer object = t_object::f_allocate_uninitialized(v_type_integer);
		object->v_integer = a_value;
		return object;
	}
	t_transfer f_as(size_t a_value) const
	{
		return f_as(static_cast<int>(a_value));
	}
	t_transfer f_as(double a_value) const
	{
		t_transfer object = t_object::f_allocate_uninitialized(v_type_float);
		object->v_float = a_value;
		return object;
	}
	t_transfer f_as(const std::wstring& a_value) const
	{
		t_transfer object = t_object::f_allocate_uninitialized(v_type_string);
		object->v_pointer = new std::wstring(a_value);
		return object;
	}
};

template<>
inline t_object* t_global::f_type<t_object>() const
{
	return v_type_object;
}

template<>
inline t_object* t_global::f_type<t_class>() const
{
	return v_type_class;
}

template<>
inline t_object* t_global::f_type<t_module>() const
{
	return v_type_module;
}

template<>
inline t_object* t_global::f_type<t_fiber>() const
{
	return v_type_fiber;
}

template<>
inline t_object* t_global::f_type<t_thread>() const
{
	return v_type_thread;
}

template<>
inline t_object* t_global::f_type<t_symbol>() const
{
	return v_type_symbol;
}

template<>
inline t_object* t_global::f_type<t_scope>() const
{
	return v_type_scope;
}

template<>
inline t_object* t_global::f_type<t_method>() const
{
	return v_type_method;
}

template<>
inline t_object* t_global::f_type<t_code>() const
{
	return v_type_code;
}

template<>
inline t_object* t_global::f_type<t_lambda>() const
{
	return v_type_lambda;
}

template<>
inline t_object* t_global::f_type<t_native>() const
{
	return v_type_native;
}

template<>
inline t_object* t_global::f_type<t_throwable>() const
{
	return v_type_throwable;
}

template<>
inline t_object* t_global::f_type<t_null>() const
{
	return v_type_null;
}

template<>
inline t_object* t_global::f_type<bool>() const
{
	return v_type_boolean;
}

template<>
inline t_object* t_global::f_type<int>() const
{
	return v_type_integer;
}

template<>
inline t_object* t_global::f_type<double>() const
{
	return v_type_float;
}

template<>
inline t_object* t_global::f_type<std::wstring>() const
{
	return v_type_string;
}

template<>
inline t_object* t_global::f_type<t_tuple>() const
{
	return v_type_tuple;
}

template<>
inline t_object* t_global::f_type<t_array>() const
{
	return v_type_array;
}

template<>
inline t_object* t_global::f_type<t_dictionary>() const
{
	return v_type_dictionary;
}

template<>
inline t_object* t_global::f_type<t_lexer::t_error>() const
{
	return v_type_lexer__error;
}

template<>
inline t_object* t_global::f_type<t_parser::t_error>() const
{
	return v_type_parser__error;
}

inline t_global* f_global()
{
	return t_global::v_instance;
}

}

#endif
