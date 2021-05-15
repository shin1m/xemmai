#ifndef XEMMAI__GLOBAL_H
#define XEMMAI__GLOBAL_H

#include "engine.h"
#include "method.h"
#include "native.h"
#include "null.h"
#include "string.h"
#include "dictionary.h"
#include "bytes.h"
#include "parser.h"
#include <algorithm>

namespace xemmai
{

class t_array;

class t_global : public t_library
{
	friend struct t_type_of<t_dictionary>;
	friend class t_engine;
	friend XEMMAI__PORTABLE__EXPORT t_global* f_global();

	static inline XEMMAI__PORTABLE__THREAD t_global* v_instance;

	t_slot_of<t_type> v_type_object;
	t_slot_of<t_type> v_type_class;
	t_slot_of<t_type> v_type_builder;
	t_slot_of<t_type> v_type_module__body;
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
	t_slot v_symbol_path;
	t_slot v_symbol_executable;
	t_slot v_symbol_script;
	t_slot v_symbol_arguments;
	t_slot v_symbol_size;
	t_slot v_symbol_dump;
	t_slot v_string_empty;

public:
	t_global() : t_library(nullptr)
	{
		v_instance = this;
	}
	void f_define(t_object* a_type_object, t_object* a_type_class, t_object* a_type_fiber, t_object* a_type_thread, t_object* a_type_module__body, std::vector<std::pair<t_root, t_rvalue>>& a_fields);
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
	t_object* f_symbol_dump() const
	{
		return v_symbol_dump;
	}
	t_object* f_string_empty() const
	{
		return v_string_empty;
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		using t = t_type_of<typename t_fundamental<T>::t_type>;
		return t::f_transfer(this, std::forward<T>(a_value));
	}
};

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_object>()
{
	return v_type_object;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_type>()
{
	return v_type_class;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_builder>()
{
	return v_type_builder;
}

template<>
inline t_slot_of<t_type>& t_global::f_type_slot<t_module::t_body>()
{
	return v_type_module__body;
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
inline t_slot_of<t_type>& t_global::f_type_slot<t_string>()
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

#ifndef _WIN32
inline t_global* f_global()
{
	return t_global::v_instance;
}
#endif

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline t_type* t_value<T_tag>::f_type() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
		return f_global()->f_type<std::nullptr_t>();
	case e_tag__BOOLEAN:
		return f_global()->f_type<bool>();
	case e_tag__INTEGER:
		return f_global()->f_type<intptr_t>();
	case e_tag__FLOAT:
		return f_global()->f_type<double>();
	default:
		return p->f_type();
	}
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline t_pvalue t_value<T_tag>::f_get(t_object* a_key) const
{
	return f_type()->f_get(*this, a_key);
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value<T_tag>::f_get(t_object* a_key, t_pvalue* a_stack) const
{
	f_type()->f_get(*this, a_key, a_stack);
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value<T_tag>::f_call(t_object* a_key, t_pvalue* a_stack, size_t a_n) const
{
	f_type()->f_invoke(*this, a_key, a_stack, a_n);
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_hash() const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
		return t_type_of<std::nullptr_t>::f__hash(*this);
	case e_tag__BOOLEAN:
		return t_type_of<bool>::f__hash(v_boolean);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__hash(v_integer);
	case e_tag__FLOAT:
		return t_type_of<double>::f__hash(v_float);
	default:
		{
			t_scoped_stack stack(2);
			p->f_type()->f_hash(p, stack);
			return stack[0];
		}
	}
}

#define XEMMAI__VALUE__BINARY(a_method)\
		{\
			t_scoped_stack stack(3);\
			stack[2] = a_value;\
			size_t n = p->f_type()->a_method(p, stack);\
			if (n != size_t(-1)) f_loop(stack, n);\
			return stack[0];\
		}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_multiply(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__multiply(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float * f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_multiply)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_divide(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__divide(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float / f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_divide)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_modulus(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return v_integer % f_as<intptr_t>(a_value);
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__BINARY(f_modulus)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_add(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__add(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float + f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_add)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_subtract(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__subtract(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float - f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_subtract)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_left_shift(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return v_integer << f_as<intptr_t>(a_value);
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__BINARY(f_left_shift)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_right_shift(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return static_cast<size_t>(v_integer) >> f_as<intptr_t>(a_value);
	case e_tag__NULL:
	case e_tag__BOOLEAN:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__BINARY(f_right_shift)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_less(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__less(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float < f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_less)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_less_equal(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__less_equal(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float <= f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_less_equal)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_greater(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__greater(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float > f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_greater)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_greater_equal(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
	case e_tag__BOOLEAN:
		f_throw(L"not supported."sv);
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__greater_equal(v_integer, a_value);
	case e_tag__FLOAT:
		f_check<double>(a_value, L"argument0");
		return v_float >= f_as<double>(a_value);
	default:
		XEMMAI__VALUE__BINARY(f_greater_equal)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_equals(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
		return p == a_value.v_p;
	case e_tag__BOOLEAN:
		return p == a_value.v_p && v_boolean == a_value.v_boolean;
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__equals(v_integer, a_value);
	case e_tag__FLOAT:
		return t_type_of<double>::f__equals(v_float, a_value);
	default:
		XEMMAI__VALUE__BINARY(f_equals)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_not_equals(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__NULL:
		return p != a_value.v_p;
	case e_tag__BOOLEAN:
		return p != a_value.v_p || v_boolean != a_value.v_boolean;
	case e_tag__INTEGER:
		return t_type_of<intptr_t>::f__not_equals(v_integer, a_value);
	case e_tag__FLOAT:
		return t_type_of<double>::f__not_equals(v_float, a_value);
	default:
		XEMMAI__VALUE__BINARY(f_not_equals)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_and(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return static_cast<bool>(v_boolean & f_as<bool>(a_value));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return v_integer & f_as<intptr_t>(a_value);
	case e_tag__NULL:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__BINARY(f_and)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_xor(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return static_cast<bool>(v_boolean ^ f_as<bool>(a_value));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return v_integer ^ f_as<intptr_t>(a_value);
	case e_tag__NULL:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__BINARY(f_xor)
	}
}

template<typename T_tag>
inline t_pvalue t_value<T_tag>::f_or(const t_pvalue& a_value) const
{
	auto p = static_cast<t_object*>(*this);
	switch (reinterpret_cast<uintptr_t>(p)) {
	case e_tag__BOOLEAN:
		f_check<bool>(a_value, L"argument0");
		return static_cast<bool>(v_boolean | f_as<bool>(a_value));
	case e_tag__INTEGER:
		f_check<intptr_t>(a_value, L"argument0");
		return v_integer | f_as<intptr_t>(a_value);
	case e_tag__NULL:
	case e_tag__FLOAT:
		f_throw(L"not supported."sv);
	default:
		XEMMAI__VALUE__BINARY(f_or)
	}
}

template<typename T, typename... T_an>
inline t_object* t_type::f_new(T_an&&... a_an)
{
	auto p = f_engine()->f_allocate(t_object::f_align_for_fields(sizeof(T)) + sizeof(t_svalue) * v_instance_fields);
	std::uninitialized_default_construct_n(p->f_fields(sizeof(T)), v_instance_fields);
	try {
		new(p->f_data()) T(std::forward<T_an>(a_an)...);
		p->f_be(this);
		return p;
	} catch (...) {
		p->f_be(this);
		throw;
	}
}

inline bool f_is_bindable(t_object* a_p)
{
	return reinterpret_cast<uintptr_t>(a_p) >= e_tag__OBJECT && a_p->f_type()->v_bindable;
}

inline t_pvalue t_type::f_get(const t_pvalue& a_this, t_object* a_key)
{
	auto index = f_index(a_key);
	if (index < v_instance_fields) return a_this->f_fields()[index];
	if (index < v_fields) {
		auto& field = f_fields()[index].second;
		return f_is_bindable(field) ? t_pvalue(xemmai::f_new<t_method>(f_global(), field, a_this)) : t_pvalue(field);
	}
	return (this->*v_get)(a_this, a_key);
}

inline void t_type::f_get(const t_pvalue& a_this, t_object* a_key, t_pvalue* a_stack)
{
	auto index = f_index(a_key);
	if (index < v_instance_fields) {
		a_stack[0] = a_this->f_fields()[index];
		a_stack[1] = nullptr;
	} else if (index < v_fields) {
		auto& field = f_fields()[index].second;
		t_object* p = field;
		if (f_is_bindable(p)) {
			a_stack[0] = p;
			a_stack[1] = a_this;
		} else {
			a_stack[0] = field;
			a_stack[1] = nullptr;
		}
	} else {
		a_stack[0] = (this->*v_get)(a_this, a_key);
		a_stack[1] = nullptr;
	}
}

inline void t_type::f_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value)
{
	auto index = f_index(a_key);
	if (index < v_instance_fields)
		a_this->f_fields()[index] = a_value;
	else
		v_put(a_this, a_key, a_value);
}

inline void t_type::f_invoke(const t_pvalue& a_this, t_object* a_key, t_pvalue* a_stack, size_t a_n)
{
	auto index = f_index(a_key);
	if (index < v_instance_fields) {
		a_this->f_fields()[index].f_call(a_stack, a_n);
	} else if (index < v_fields) {
		auto& field = f_fields()[index].second;
		t_object* p = field;
		if (f_is_bindable(p)) {
			a_stack[1] = a_this;
			size_t n = p->f_call_without_loop(a_stack, a_n);
			if (n != size_t(-1)) f_loop(a_stack, n);
		} else {
			field.f_call(a_stack, a_n);
		}
	} else {
		(this->*v_get)(a_this, a_key).f_call(a_stack, a_n);
	}
}

template<typename T>
inline t_pvalue t_derived<T>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	size_t n = a_n + 2;
	t_scoped_stack stack(n);
	stack[1] = t_object::f_of(this);
	for (size_t i = 2; i < n; ++i) stack[i] = a_stack[i];
	t_object::f_of(this)->f_call(f_global()->f_symbol_construct(), stack, a_n);
	return stack[0];
}

template<typename T>
void t_builder::f_do(t_fields& a_fields, T a_do)
{
	auto builder = f_new<t_builder>(f_global());
	builder->f_as<t_builder>().v_fields = &a_fields;
	try {
		a_do(builder);
		builder->f_as<t_builder>().v_fields = nullptr;
	} catch (...) {
		builder->f_as<t_builder>().v_fields = nullptr;
		throw;
	}
}

template<typename T_context, typename T_main>
intptr_t t_fiber::f_main(T_main a_main)
{
	auto& fiber = f_as<t_fiber&>(t_thread::v_current->v_thread->v_fiber);
	intptr_t n = -1;
	T_context context;
	try {
		try {
			a_main();
			n = 0;
		} catch (const t_rvalue& thrown) {
			fiber.f_caught(thrown, nullptr);
			do {
				try {
					auto p = thrown.f_invoke(f_global()->f_symbol_string());
					if (f_is<t_string>(p)) {
						auto& s = f_as<const t_string&>(p);
						std::fprintf(stderr, "caught: %.*ls\n", static_cast<int>(s.f_size()), static_cast<const wchar_t*>(s));
						break;
					}
				} catch (...) {
				}
				std::fprintf(stderr, "caught: <unprintable>\n");
			} while (false);
			if (f_is<t_throwable>(thrown)) thrown.f_invoke(f_global()->f_symbol_dump());
		}
	} catch (...) {
		std::fprintf(stderr, "caught: <unexpected>\n");
	}
	assert(f_stack() == fiber.v_internal->v_estack.get());
	t_thread::v_current->v_mutex.lock();
	fiber.v_return = f_stack();
	f_stack__(fiber.v_return + 1);
	fiber.v_internal->f_epoch_get();
	while (true) {
		auto p = t_thread::v_current->v_fibers;
		while (p != fiber.v_internal && !p->v_thread) p = p->v_next;
		if (p == fiber.v_internal) break;
		p->v_fiber->v_throw = true;
		*p->v_fiber->v_return = f_engine()->v_fiber_exit;
		t_thread::v_current->v_active = p->v_fiber;
#ifdef __unix__
		f_stack__(p->v_estack_used);
		swapcontext(&fiber.v_internal->v_context, &p->v_context);
#endif
#ifdef _WIN32
		v_current = p;
		SwitchToFiber(p->v_handle);
#endif
	}
#ifdef _WIN32
	fiber.v_internal->v_handle = NULL;
	ConvertFiberToThread();
#endif
	t_thread::v_current->v_mutex.unlock();
	return n;
}

template<typename T>
inline t_object* t_tuple::f_instantiate(size_t a_size, T a_construct)
{
	auto p = f_engine()->f_allocate(sizeof(t_tuple) + sizeof(t_svalue) * a_size);
	a_construct(*new(p->f_data()) t_tuple(a_size));
	p->f_be(f_global()->f_type<t_tuple>());
	return p;
}

inline size_t t_code::f_loop(t_context& a_context)
{
	try {
		try {
			return f_loop(&a_context);
		} catch (...) {
			f_rethrow();
		}
	} catch (const t_rvalue& thrown) {
		a_context.f_backtrace(thrown);
		throw;
	}
}

template<typename T_context>
inline size_t t_lambda_shared::f_call(t_pvalue* a_stack)
{
	auto scope = f_engine()->f_allocate(sizeof(t_scope) + sizeof(t_svalue) * v_shareds);
	T_context context(t_object::f_of(this), a_stack);
	context.v_scope = (new(scope->f_data()) t_scope(v_shareds, v_scope_entries))->f_entries();
	scope->f_be(f_global()->f_type<t_scope>());
	return t_code::f_loop(context);
}

inline t_object* t_type_of<t_string>::f__construct(t_type* a_class, const wchar_t* a_p, size_t a_n)
{
	auto object = f_engine()->f_allocate(sizeof(t_string) + sizeof(wchar_t) * (a_n + 1));
	*std::copy_n(a_p, a_n, (new(object->f_data()) t_string(a_n))->f_entries()) = L'\0';
	object->f_be(a_class);
	return object;
}

inline t_object* t_type_of<t_string>::f__construct(t_type* a_class, const t_string& a_x, const t_string& a_y)
{
	size_t n = a_x.v_size + a_y.v_size;
	auto object = f_engine()->f_allocate(sizeof(t_string) + sizeof(wchar_t) * (n + 1));
	*std::copy_n(static_cast<const wchar_t*>(a_y), a_y.v_size, std::copy_n(static_cast<const wchar_t*>(a_x), a_x.v_size, (new(object->f_data()) t_string(n))->f_entries())) = L'\0';
	object->f_be(a_class);
	return object;
}

template<typename T>
inline t_pvalue t_type_of<t_string>::f_transfer(const t_global* a_library, T&& a_value)
{
	return a_value.empty() ? a_library->f_string_empty() : f__construct(a_library->f_type<t_string>(), std::forward<T>(a_value));
}

inline t_object* t_type_of<t_string>::f_from_code(t_global* a_library, intptr_t a_code)
{
	wchar_t c = a_code;
	return f__construct(a_library->f_type<t_string>(), &c, 1);
}

XEMMAI__PORTABLE__ALWAYS_INLINE inline t_object* t_type_of<t_string>::f__add(t_object* a_self, const t_pvalue& a_value)
{
	auto add = [&](t_object* x)
	{
		auto& s0 = f_as<const t_string&>(a_self);
		if (s0.f_size() <= 0) return x;
		auto& s1 = f_as<const t_string&>(x);
		return s1.f_size() <= 0 ? a_self : f__construct(a_self->f_type(), s0, s1);
	};
	if (f_is<t_string>(a_value)) return add(a_value);
	auto x = a_value.f_invoke(f_global()->f_symbol_string());
	f_check<t_string>(x, L"argument0");
	return add(x);
}

inline bool t_type_of<t_string>::f__equals(const t_string& a_self, const t_pvalue& a_value)
{
	return f_is<t_string>(a_value) && a_self == f_as<const t_string&>(a_value);
}

inline bool t_type_of<t_string>::f__not_equals(const t_string& a_self, const t_pvalue& a_value)
{
	return !f_is<t_string>(a_value) || a_self != f_as<const t_string&>(a_value);
}

inline t_object* t_type_of<t_string>::f__substring(t_global* a_library, const t_string& a_self, size_t a_i, size_t a_n)
{
	return a_n > 0 ? f__construct(a_library->f_type<t_string>(), static_cast<const wchar_t*>(a_self) + a_i, a_n) : a_library->f_string_empty();
}

inline t_pvalue t_type_of<t_bytes>::f__construct(t_type* a_class, size_t a_size)
{
	auto p = f_engine()->f_allocate(sizeof(t_bytes) + a_size);
	new(p->f_data()) t_bytes(a_size);
	p->f_be(a_class);
	return p;
}

}

#endif
