#ifndef XEMMAI__GLOBAL_H
#define XEMMAI__GLOBAL_H

#include "engine.h"
#include "class.h"
#include "method.h"
#include "null.h"
#include "boolean.h"
#include "string.h"
#include "parser.h"

namespace xemmai
{

class t_tuple;
class t_array;
class t_bytes;

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_global* f_global();
#else
XEMMAI__PORTABLE__EXPORT t_global* f_global();
#endif

class t_global : public t_extension
{
	friend struct t_thread;
	friend struct t_type_of<t_dictionary>;
	template<typename T, typename T_super> friend class t_define;
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	friend t_global* f_global();
#else
	friend XEMMAI__PORTABLE__EXPORT t_global* f_global();
#endif

	static XEMMAI__PORTABLE__THREAD t_global* v_instance;

	t_slot v_type_object;
	t_slot v_type_class;
	t_slot v_type_structure;
	t_slot v_type_module;
	t_slot v_type_fiber;
	t_slot v_type_thread;
	t_slot v_type_tuple;
	t_slot v_type_symbol;
	t_slot v_type_scope;
	t_slot v_type_method;
	t_slot v_type_code;
	t_slot v_type_lambda;
	t_slot v_type_advanced_lambda;
	t_slot v_type_native;
	t_slot v_type_throwable;
	t_slot v_type_null;
	t_slot v_type_boolean;
	t_slot v_type_integer;
	t_slot v_type_float;
	t_slot v_type_string;
	t_slot v_type_array;
	t_slot v_type_dictionary__table;
	t_slot v_type_dictionary;
	t_slot v_type_bytes;
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
	t_slot v_symbol_send;
	t_slot v_symbol_path;
	t_slot v_symbol_executable;
	t_slot v_symbol_script;
	t_slot v_symbol_arguments;
	t_slot v_symbol_size;
	t_slot v_symbol_push;

	template<typename T>
	void f_type__(t_scoped&& a_type);

public:
	t_global(t_object* a_module, t_scoped&& a_type_object, t_scoped&& a_type_class, t_scoped&& a_type_structure, t_scoped&& a_type_module, t_scoped&& a_type_fiber, t_scoped&& a_type_thread);
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
	t_object* f_symbol_send() const
	{
		return v_symbol_send;
	}
	t_object* f_symbol_path() const
	{
		return v_symbol_path;
	}
	t_object* f_symbol_executable() const
	{
		return v_symbol_executable;
	}
	t_object* f_symbol_script() const
	{
		return v_symbol_script;
	}
	t_object* f_symbol_arguments() const
	{
		return v_symbol_arguments;
	}
	t_object* f_symbol_size() const
	{
		return v_symbol_size;
	}
	t_object* f_symbol_push() const
	{
		return v_symbol_push;
	}
	template<typename T>
	t_scoped f_as(const T& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(this, a_value);
	}
};

template<>
inline void t_global::f_type__<t_scope>(t_scoped&& a_type)
{
	v_type_scope = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_code>(t_scoped&& a_type)
{
	v_type_code = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_lambda>(t_scoped&& a_type)
{
	v_type_lambda = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_advanced_lambda>(t_scoped&& a_type)
{
	v_type_advanced_lambda = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_throwable>(t_scoped&& a_type)
{
	v_type_throwable = std::move(a_type);
}

template<>
inline void t_global::f_type__<std::nullptr_t>(t_scoped&& a_type)
{
	v_type_null = std::move(a_type);
}

template<>
inline void t_global::f_type__<bool>(t_scoped&& a_type)
{
	v_type_boolean = std::move(a_type);
}

template<>
inline void t_global::f_type__<intptr_t>(t_scoped&& a_type)
{
	v_type_integer = std::move(a_type);
}

template<>
inline void t_global::f_type__<double>(t_scoped&& a_type)
{
	v_type_float = std::move(a_type);
}

template<>
inline void t_global::f_type__<std::wstring>(t_scoped&& a_type)
{
	v_type_string = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_array>(t_scoped&& a_type)
{
	v_type_array = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_dictionary>(t_scoped&& a_type)
{
	v_type_dictionary = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_bytes>(t_scoped&& a_type)
{
	v_type_bytes = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_lexer::t_error>(t_scoped&& a_type)
{
	v_type_lexer__error = std::move(a_type);
}

template<>
inline void t_global::f_type__<t_parser::t_error>(t_scoped&& a_type)
{
	v_type_parser__error = std::move(a_type);
}

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
inline t_object* t_global::f_type<t_structure>() const
{
	return v_type_structure;
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
inline t_object* t_global::f_type<t_tuple>() const
{
	return v_type_tuple;
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
inline t_object* t_global::f_type<t_advanced_lambda>() const
{
	return v_type_advanced_lambda;
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
inline t_object* t_global::f_type<std::nullptr_t>() const
{
	return v_type_null;
}

template<>
inline t_object* t_global::f_type<bool>() const
{
	return v_type_boolean;
}

template<>
inline t_object* t_global::f_type<intptr_t>() const
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
inline t_object* t_global::f_type<t_array>() const
{
	return v_type_array;
}

template<>
inline t_object* t_global::f_type<t_dictionary::t_table>() const
{
	return v_type_dictionary__table;
}

template<>
inline t_object* t_global::f_type<t_dictionary>() const
{
	return v_type_dictionary;
}

template<>
inline t_object* t_global::f_type<t_bytes>() const
{
	return v_type_bytes;
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

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_global* f_global()
{
	return t_global::v_instance;
}
#endif

inline intptr_t t_value::f_integer() const
{
	return f_tag() < e_tag__OBJECT ? v_integer : v_p->f_integer();
}

inline void t_value::f_integer__(intptr_t a_value)
{
	v_p->v_type.v_integer = a_value;
}

inline double t_value::f_float() const
{
	return f_tag() < e_tag__OBJECT ? v_float : v_p->f_float();
}

inline void t_value::f_float__(double a_value)
{
	v_p->v_type.v_float = a_value;
}

inline void t_value::f_pointer__(void* a_value)
{
	v_p->v_type.v_pointer = a_value;
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline t_object* t_value::f_type() const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return f_global()->f_type<std::nullptr_t>();
	case e_tag__BOOLEAN:
		return f_global()->f_type<bool>();
	case e_tag__INTEGER:
		return f_global()->f_type<intptr_t>();
	case e_tag__FLOAT:
		return f_global()->f_type<double>();
	default:
		return v_p->f_type();
	}
}

inline bool t_value::f_is(t_object* a_class) const
{
	return t_type::f_derives(f_type(), a_class);
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline t_scoped t_value::f_get(t_object* a_key) const
{
	return f_as<t_type&>(f_type()).f_get(*this, a_key);
}

inline void t_value::f_put(t_object* a_key, t_scoped&& a_value) const
{
	if (f_tag() < e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	f_as<t_type&>(v_p->f_type()).f_put(v_p, a_key, std::move(a_value));
}

inline bool t_value::f_has(t_object* a_key) const
{
	return static_cast<t_type*>(f_type()->f_pointer())->f_has(*this, a_key);
}

inline t_scoped t_value::f_remove(t_object* a_key) const
{
	if (f_tag() < e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	return f_as<t_type&>(v_p->f_type()).f_remove(v_p, a_key);
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline size_t t_value::f_call_without_loop(t_scoped* a_stack, size_t a_n) const
{
	if (f_tag() < e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	return v_p->f_call_without_loop(a_stack, a_n);
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value::f_loop(t_scoped* a_stack, size_t a_n)
{
	while (a_n != size_t(-1)) {
		t_scoped x = std::move(a_stack[0]);
		a_n = x.f_call_without_loop(a_stack, a_n);
	}
}

inline void t_value::f_call(t_object* a_key, t_scoped* a_stack, size_t a_n) const
{
	if (f_tag() >= e_tag__OBJECT && v_p->f_owned()) {
		intptr_t index = v_p->f_field_index(a_key);
		if (index < 0) {
			t_scoped value = v_p->f_type()->f_get(a_key);
			if (value.f_type() == f_global()->f_type<t_method>()) {
				a_stack[1].f_construct_nonnull(v_p);
				f_as<t_method&>(value).f_function().f_call(a_stack, a_n);
			} else {
				value.f_call(a_stack, a_n);
			}
		} else {
			v_p->f_field_get(index).f_call(a_stack, a_n);
		}
	} else {
		f_get(a_key).f_call(a_stack, a_n);
	}
}

inline void t_object::f_get_owned(t_object* a_key, t_scoped* a_stack)
{
	intptr_t index = f_field_index(a_key);
	if (index < 0) {
		t_scoped value = f_type()->f_get(a_key);
		if (value.f_type() == f_global()->f_type<t_method>()) {
			a_stack[0].f_construct_nonnull(f_as<t_method&>(value).f_function());
			a_stack[1].f_construct_nonnull(this);
		} else {
			a_stack[0].f_construct(std::move(value));
			a_stack[1].f_construct();
		}
	} else {
		a_stack[0].f_construct(f_field_get(index));
		a_stack[1].f_construct();
	}
}

inline void t_object::f_get(t_object* a_key, t_scoped* a_stack)
{
	if (f_owned()) {
		f_get_owned(a_key, a_stack);
	} else {
		a_stack[0].f_construct(f_get(a_key));
		a_stack[1].f_construct();
	}
}

inline void t_value::f_get(t_object* a_key, t_scoped* a_stack) const
{
	if (f_tag() >= e_tag__OBJECT && v_p->f_owned()) {
		v_p->f_get_owned(a_key, a_stack);
	} else {
		a_stack[0].f_construct(f_get(a_key));
		a_stack[1].f_construct();
	}
}

inline t_scoped t_value::f_call_with_same(t_scoped* a_stack, size_t a_n) const
{
	size_t n = a_n + 2;
	t_scoped_stack stack(n);
	stack[1].f_construct();
	for (size_t i = 2; i < n; ++i) stack[i].f_construct(a_stack[i]);
	f_call(stack, a_n);
	return stack.f_return();
}

#define XEMMAI__VALUE__UNARY(a_method)\
		{\
			t_scoped_stack stack(2);\
			f_loop(stack, f_as<t_type&>(v_p->f_type()).a_method(v_p, stack));\
			return stack.f_return();\
		}
#define XEMMAI__VALUE__BINARY(a_method)\
		{\
			t_scoped_stack stack(3);\
			stack[2].f_construct(a_value);\
			f_loop(stack, f_as<t_type&>(v_p->f_type()).a_method(v_p, stack));\
			return stack.f_return();\
		}

inline t_scoped t_value::f_hash() const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return t_value(t_type_of<std::nullptr_t>::f_hash(*this));
	case e_tag__BOOLEAN:
		return t_value(t_type_of<bool>::f_hash(v_boolean));
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_hash(v_integer));
	case e_tag__FLOAT:
		return t_value(t_type_of<double>::f_hash(v_float));
	default:
		{
			t_scoped_stack stack(2);
			f_as<t_type&>(v_p->f_type()).f_hash(v_p, stack);
			return stack.f_return();
		}
	}
}

template<typename... T>
inline t_scoped t_value::operator()(T&&... a_arguments) const
{
	t_scoped_stack stack(sizeof...(a_arguments) + 2, std::forward<T>(a_arguments)...);
	stack[1].f_construct();
	f_call(stack, sizeof...(a_arguments));
	return stack.f_return();
}

inline t_scoped t_value::f_get_at(const t_value& a_index) const
{
	if (f_tag() < e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_scoped_stack stack(3);
	stack[2].f_construct(a_index);
	f_loop(stack, f_as<t_type&>(v_p->f_type()).f_get_at(v_p, stack));
	return stack.f_return();
}

inline t_scoped t_value::f_set_at(const t_value& a_index, const t_value& a_value) const
{
	if (f_tag() < e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_scoped_stack stack(4);
	stack[2].f_construct(a_index);
	stack[3].f_construct(a_value);
	f_loop(stack, f_as<t_type&>(v_p->f_type()).f_set_at(v_p, stack));
	return stack.f_return();
}

inline t_scoped t_value::f_plus() const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_value(v_integer);
	case e_tag__FLOAT:
		return t_value(v_float);
	default:
		XEMMAI__VALUE__UNARY(f_plus)
	}
}

inline t_scoped t_value::f_minus() const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_value(-v_integer);
	case e_tag__FLOAT:
		return t_value(-v_float);
	default:
		XEMMAI__VALUE__UNARY(f_minus)
	}
}

inline t_scoped t_value::f_not() const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		return t_value(!v_boolean);
	case e_tag__NULL:
	case e_tag__INTEGER:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__UNARY(f_not)
	}
}

inline t_scoped t_value::f_complement() const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		return t_value(~v_integer);
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__UNARY(f_complement)
	}
}

inline t_scoped t_value::f_multiply(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f_multiply(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float * f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_multiply)
	}
}

inline t_scoped t_value::f_divide(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f_divide(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float / f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_divide)
	}
}

inline t_scoped t_value::f_modulus(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_value(v_integer % f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__BINARY(f_modulus)
	}
}

inline t_scoped t_value::f_add(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f_add(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float + f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_add)
	}
}

inline t_scoped t_value::f_subtract(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f_subtract(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float - f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_subtract)
	}
}

inline t_scoped t_value::f_left_shift(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_value(v_integer << f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__BINARY(f_left_shift)
	}
}

inline t_scoped t_value::f_right_shift(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_value(static_cast<size_t>(v_integer) >> f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__BINARY(f_right_shift)
	}
}

inline t_scoped t_value::f_less(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_less(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float < f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_less)
	}
}

inline t_scoped t_value::f_less_equal(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_less_equal(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float <= f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_less_equal)
	}
}

inline t_scoped t_value::f_greater(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_greater(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float > f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_greater)
	}
}

inline t_scoped t_value::f_greater_equal(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		t_throwable::f_throw(L"not supported");
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_greater_equal(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_value(v_float >= f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_greater_equal)
	}
}

inline t_scoped t_value::f_equals(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return t_value(v_p == a_value.v_p);
	case e_tag__BOOLEAN:
		return t_value(v_p == a_value.v_p && v_boolean == a_value.v_boolean);
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_equals(v_integer, a_value));
	case e_tag__FLOAT:
		return t_value(t_type_of<double>::f_equals(v_float, a_value));
	default:
		XEMMAI__VALUE__BINARY(f_equals)
	}
}

inline t_scoped t_value::f_not_equals(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return t_value(v_p != a_value.v_p);
	case e_tag__BOOLEAN:
		return t_value(v_p != a_value.v_p || v_boolean != a_value.v_boolean);
	case e_tag__INTEGER:
		return t_value(t_type_of<intptr_t>::f_not_equals(v_integer, a_value));
	case e_tag__FLOAT:
		return t_value(t_type_of<double>::f_not_equals(v_float, a_value));
	default:
		XEMMAI__VALUE__BINARY(f_not_equals)
	}
}

inline t_scoped t_value::f_and(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return t_value(static_cast<bool>(v_boolean & f_as<bool>(a_value)));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_value(v_integer & f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__BINARY(f_and)
	}
}

inline t_scoped t_value::f_xor(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return t_value(static_cast<bool>(v_boolean ^ f_as<bool>(a_value)));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_value(v_integer ^ f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__BINARY(f_xor)
	}
}

inline t_scoped t_value::f_or(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return t_value(static_cast<bool>(v_boolean | f_as<bool>(a_value)));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_value(v_integer | f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__FLOAT:
		t_throwable::f_throw(L"not supported");
	default:
		XEMMAI__VALUE__BINARY(f_or)
	}
}

inline t_scoped t_value::f_send(const t_value& a_value) const
{
	if (f_tag() < e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	XEMMAI__VALUE__BINARY(f_send)
}

template<typename T_main>
intptr_t t_fiber::f_main(T_main a_main)
{
	intptr_t n = -1;
	t_context context;
	try {
		try {
			a_main();
			n = 0;
		} catch (const t_scoped& thrown) {
			f_as<t_fiber&>(f_current()).f_caught(thrown);
			std::wstring s = L"<unprintable>";
			try {
				t_scoped p = thrown.f_get(f_global()->f_symbol_string())();
				if (f_is<std::wstring>(p)) s = f_as<const std::wstring&>(p);
			} catch (...) {
			}
			std::fprintf(stderr, "caught: %ls\n", s.c_str());
			if (f_is<t_throwable>(thrown)) thrown.f_get(t_symbol::f_instantiate(L"dump"))();
		}
	} catch (...) {
		std::fprintf(stderr, "caught <unexpected>.\n");
	}
	context.f_terminate();
	return n;
}

template<size_t (t_type::*A_function)(t_object*, t_scoped*)>
XEMMAI__PORTABLE__NOINLINE void t_code::f_operator(t_object* a_this, t_scoped* a_stack)
{
	t_value::f_loop(a_stack, (f_as<t_type&>(a_this->f_type()).*A_function)(a_this, a_stack));
}

template<size_t (t_type::*A_function)(t_object*, t_scoped*)>
XEMMAI__PORTABLE__NOINLINE size_t t_code::f_operator(t_context& a_context, t_scoped* a_base, t_object* a_this, t_scoped* a_stack)
{
	size_t n = (f_as<t_type&>(a_this->f_type()).*A_function)(a_this, a_stack);
	if (n == size_t(-1)) {
		a_base[-2].f_construct(std::move(a_stack[0]));
		a_context.f_pop();
	} else {
		a_context.f_tail(a_stack, n);
	}
	return n;
}

inline size_t t_code::f_loop(t_object* a_lambda, t_lambda& a_as_lambda, t_scoped* a_stack)
{
	t_context context(a_lambda, a_as_lambda, a_stack);
	try {
#ifdef XEMMAI_ENABLE_JIT
		return a_as_lambda.v_jit_loop(&context);
#else
		context.f_pc() = a_as_lambda.v_instructions;
		return f_loop(&context);
#endif
	} catch (const t_scoped& thrown) {
		context.f_backtrace(thrown);
		throw thrown;
	} catch (...) {
		t_scoped thrown = t_throwable::f_instantiate(L"<unknown>.");
		context.f_backtrace(thrown);
		throw thrown;
	}
}

}

#endif
