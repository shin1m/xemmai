#ifndef XEMMAI__GLOBAL_H
#define XEMMAI__GLOBAL_H

#include "engine.h"
#include "class.h"
#include "method.h"
#include "native.h"
#include "null.h"
#include "boolean.h"
#include "string.h"
#include "parser.h"

namespace xemmai
{

class t_array;
class t_bytes;

class t_global : public t_extension
{
	friend struct t_thread;
	friend struct t_type_of<t_dictionary>;
#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
	friend t_global* f_global();
#else
	friend XEMMAI__PORTABLE__EXPORT t_global* f_global();
#endif

	static XEMMAI__PORTABLE__THREAD t_global* v_instance;

	t_slot_of<t_type> v_type_object;
	t_slot_of<t_type> v_type_class;
	t_slot_of<t_type> v_type_structure;
	t_slot_of<t_type> v_type_module;
	t_slot_of<t_type> v_type_fiber;
	t_slot_of<t_type> v_type_thread;
	t_slot_of<t_type> v_type_tuple;
	t_slot_of<t_type> v_type_symbol;
	t_slot_of<t_type> v_type_scope;
	t_slot_of<t_type> v_type_method;
	t_slot_of<t_type> v_type_code;
	t_slot_of<t_type> v_type_lambda;
	t_slot_of<t_type> v_type_lambda_shared;
	t_slot_of<t_type> v_type_advanced_lambda;
	t_slot_of<t_type> v_type_advanced_lambda_shared;
	t_slot_of<t_type> v_type_native;
	t_slot_of<t_type> v_type_throwable;
	t_slot_of<t_type> v_type_null;
	t_slot_of<t_type> v_type_boolean;
	t_slot_of<t_type> v_type_integer;
	t_slot_of<t_type> v_type_float;
	t_slot_of<t_type> v_type_string;
	t_slot_of<t_type> v_type_array;
	t_slot_of<t_type> v_type_dictionary__table;
	t_slot_of<t_type> v_type_dictionary;
	t_slot_of<t_type> v_type_bytes;
	t_slot_of<t_type> v_type_lexer__error;
	t_slot_of<t_type> v_type_parser__error;
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

public:
	t_global(t_object* a_module, t_type* a_type_object, t_type* a_type_class, t_type* a_type_structure, t_type* a_type_module, t_type* a_type_fiber, t_type* a_type_thread);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_slot_of<t_type>& f_type_slot();
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_global*>(this)->f_type_slot<T>();
	}
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
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(this, std::forward<T>(a_value));
	}
};

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_object>()
{
	return v_type_object;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_class>()
{
	return v_type_class;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_structure>()
{
	return v_type_structure;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_module>()
{
	return v_type_module;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_fiber>()
{
	return v_type_fiber;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_thread>()
{
	return v_type_thread;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_tuple>()
{
	return v_type_tuple;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_symbol>()
{
	return v_type_symbol;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_scope>()
{
	return v_type_scope;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_method>()
{
	return v_type_method;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_code>()
{
	return v_type_code;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_lambda>()
{
	return v_type_lambda;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_lambda_shared>()
{
	return v_type_lambda_shared;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_advanced_lambda<t_lambda>>()
{
	return v_type_advanced_lambda;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_advanced_lambda<t_lambda_shared>>()
{
	return v_type_advanced_lambda_shared;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_native>()
{
	return v_type_native;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_throwable>()
{
	return v_type_throwable;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<std::nullptr_t>()
{
	return v_type_null;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<bool>()
{
	return v_type_boolean;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<intptr_t>()
{
	return v_type_integer;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<double>()
{
	return v_type_float;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<std::wstring>()
{
	return v_type_string;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_array>()
{
	return v_type_array;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_dictionary::t_table>()
{
	return v_type_dictionary__table;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_dictionary>()
{
	return v_type_dictionary;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_bytes>()
{
	return v_type_bytes;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_lexer::t_error>()
{
	return v_type_lexer__error;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_parser::t_error>()
{
	return v_type_parser__error;
}

#ifdef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
inline t_global* f_global()
{
	return t_global::v_instance;
}
#endif

XEMMAI__PORTABLE__ALWAYS_INLINE inline t_type* t_value::f_type() const
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

XEMMAI__PORTABLE__ALWAYS_INLINE inline t_scoped t_value::f_get(t_object* a_key) const
{
	if (f_tag() < e_tag__OBJECT) {
		t_scoped value = static_cast<t_object*>(f_type()->v_this)->f_get(a_key);
		return value.f_type() == f_global()->f_type<t_method>() ? f_as<t_method&>(value).f_bind(*this) : value;
	} else {
		return v_p->f_get(a_key);
	}
}

template<typename T>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void f_get_of_type(T& a_this, t_object* a_key, t_stacked* a_stack)
{
	t_scoped value = a_this.f_type()->f_get_of_type(a_key);
	if (value.f_type() == f_global()->f_type<t_method>()) {
		a_stack[0].f_construct(*f_as<t_method&>(value).f_function());
		a_stack[1].f_construct(a_this);
	} else {
		a_stack[0].f_construct(std::move(value));
		a_stack[1].f_construct();
	}
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value::f_get(t_object* a_key, t_stacked* a_stack) const
{
	if (f_tag() >= e_tag__OBJECT)
		v_p->f_get(a_key, a_stack);
	else
		f_get_of_type(*this, a_key, a_stack);
}

template<typename T>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void f_call_of_type(T& a_this, t_object* a_key, t_stacked* a_stack, size_t a_n)
{
	t_scoped value = f_do_or_destruct([&]
	{
		return a_this.f_type()->f_get_of_type(a_key);
	}, a_stack, a_n);
	if (value.f_type() == f_global()->f_type<t_method>()) {
		a_stack[1] = a_this;
		f_as<t_method&>(value).f_function().f_call(a_stack, a_n);
	} else {
		value.f_call(a_stack, a_n);
	}
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value::f_call(t_object* a_key, t_stacked* a_stack, size_t a_n) const
{
	if (f_tag() >= e_tag__OBJECT)
		v_p->f_call(a_key, a_stack, a_n);
	else
		f_call_of_type(*this, a_key, a_stack, a_n);
}

inline t_scoped t_value::f_hash() const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return t_scoped(t_type_of<std::nullptr_t>::f__hash(*this));
	case e_tag__BOOLEAN:
		return t_scoped(t_type_of<bool>::f__hash(v_boolean));
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__hash(v_integer));
	case e_tag__FLOAT:
		return t_scoped(t_type_of<double>::f__hash(v_float));
	default:
		{
			t_scoped_stack stack(2);
			v_p->f_type()->f_hash(v_p, stack);
			return stack.f_return();
		}
	}
}

inline t_scoped t_value::f_multiply(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__multiply(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float * f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_multiply)
	}
}

inline t_scoped t_value::f_divide(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__divide(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float / f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_divide)
	}
}

inline t_scoped t_value::f_modulus(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_scoped(v_integer % f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__BINARY(f_modulus)
	}
}

inline t_scoped t_value::f_add(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__add(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float + f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_add)
	}
}

inline t_scoped t_value::f_subtract(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__subtract(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float - f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_subtract)
	}
}

inline t_scoped t_value::f_left_shift(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_scoped(v_integer << f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__BINARY(f_left_shift)
	}
}

inline t_scoped t_value::f_right_shift(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_scoped(static_cast<size_t>(v_integer) >> f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__BINARY(f_right_shift)
	}
}

inline t_scoped t_value::f_less(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__less(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float < f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_less)
	}
}

inline t_scoped t_value::f_less_equal(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__less_equal(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float <= f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_less_equal)
	}
}

inline t_scoped t_value::f_greater(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__greater(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float > f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_greater)
	}
}

inline t_scoped t_value::f_greater_equal(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported.");
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__greater_equal(v_integer, a_value));
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return t_scoped(v_float >= f_as<double>(a_value));
	default:
		XEMMAI__VALUE__BINARY(f_greater_equal)
	}
}

inline t_scoped t_value::f_equals(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return t_scoped(v_p == a_value.v_p);
	case e_tag__BOOLEAN:
		return t_scoped(v_p == a_value.v_p && v_boolean == a_value.v_boolean);
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__equals(v_integer, a_value));
	case e_tag__FLOAT:
		return t_scoped(t_type_of<double>::f__equals(v_float, a_value));
	default:
		XEMMAI__VALUE__BINARY(f_equals)
	}
}

inline t_scoped t_value::f_not_equals(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__NULL:
		return t_scoped(v_p != a_value.v_p);
	case e_tag__BOOLEAN:
		return t_scoped(v_p != a_value.v_p || v_boolean != a_value.v_boolean);
	case e_tag__INTEGER:
		return t_scoped(t_type_of<intptr_t>::f__not_equals(v_integer, a_value));
	case e_tag__FLOAT:
		return t_scoped(t_type_of<double>::f__not_equals(v_float, a_value));
	default:
		XEMMAI__VALUE__BINARY(f_not_equals)
	}
}

inline t_scoped t_value::f_and(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return t_scoped(static_cast<bool>(v_boolean & f_as<bool>(a_value)));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_scoped(v_integer & f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__BINARY(f_and)
	}
}

inline t_scoped t_value::f_xor(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return t_scoped(static_cast<bool>(v_boolean ^ f_as<bool>(a_value)));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_scoped(v_integer ^ f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__BINARY(f_xor)
	}
}

inline t_scoped t_value::f_or(const t_value& a_value) const
{
	switch (f_tag()) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return t_scoped(static_cast<bool>(v_boolean | f_as<bool>(a_value)));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return t_scoped(v_integer | f_as<intptr_t>(a_value));
	case e_tag__NULL:
	case e_tag__FLOAT:
		f_throw(L"not supported.");
	default:
		XEMMAI__VALUE__BINARY(f_or)
	}
}

template<typename T_context, typename T_main>
intptr_t t_fiber::f_main(T_main a_main)
{
	intptr_t n = -1;
	T_context context;
	try {
		try {
			a_main();
			n = 0;
		} catch (const t_scoped& thrown) {
			f_as<t_fiber&>(f_current()).f_caught(thrown, nullptr);
			std::wstring s = L"<unprintable>";
			try {
				t_scoped p = thrown.f_invoke(f_global()->f_symbol_string());
				if (f_is<std::wstring>(p)) s = f_as<const std::wstring&>(p);
			} catch (...) {
			}
			std::fprintf(stderr, "caught: %ls\n", s.c_str());
			if (f_is<t_throwable>(thrown)) thrown.f_invoke(t_symbol::f_instantiate(L"dump"));
		}
	} catch (...) {
		std::fprintf(stderr, "caught <unexpected>.\n");
	}
	auto& stack = f_as<t_fiber&>(f_current()).v_stack;
	assert(stack.v_used == stack.f_head());
	return n;
}

inline size_t t_code::f_loop(t_context& a_context)
{
	try {
#ifdef XEMMAI_ENABLE_JIT
		return a_context.v_lambda->v_jit_loop(a_context);
#else
		return f_loop(&a_context);
#endif
	} catch (const t_scoped& thrown) {
		a_context.f_backtrace(thrown);
		throw thrown;
	} catch (...) {
		t_scoped thrown = t_throwable::f_instantiate(L"<unknown>.");
		a_context.f_backtrace(thrown);
		throw thrown;
	}
}

}

#endif
