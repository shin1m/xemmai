#ifndef XEMMAI__GLOBAL_H
#define XEMMAI__GLOBAL_H

#include "engine.h"
#include "native.h"
#include "method.h"
#include "null.h"
#include "list.h"
#include "map.h"
#include "bytes.h"
#include "parser.h"

namespace xemmai
{

class t_global : public t_library
{
	friend struct t_type_of<t_map>;
	friend class t_engine;
	friend XEMMAI__PUBLIC t_global* f_global();

	static inline XEMMAI__PORTABLE__THREAD t_global* v_instance;

	t_slot_of<t_type> v_type_object;
	t_slot_of<t_type> v_type_sharable;
	t_slot_of<t_type> v_type_type;
	t_slot_of<t_type> v_type_builder;
	t_slot_of<t_type> v_type_module__body;
	t_slot_of<t_type> v_type_module;
	t_slot_of<t_type> v_type_symbol;
	t_slot_of<t_type> v_type_native;
	t_slot_of<t_type> v_type_fiber;
	t_slot_of<t_type> v_type_thread;
	t_slot_of<t_type> v_type_scope;
	t_slot_of<t_type> v_type_code;
	t_slot_of<t_type> v_type_lambda;
	t_slot_of<t_type> v_type_lambda_shared;
	t_slot_of<t_type> v_type_advanced_lambda;
	t_slot_of<t_type> v_type_advanced_lambda_shared;
	t_slot_of<t_type> v_type_method;
	t_slot_of<t_type> v_type_throwable;
	t_slot_of<t_type> v_type_null;
	t_slot_of<t_type> v_type_boolean;
	t_slot_of<t_type> v_type_integer;
	t_slot_of<t_type> v_type_float;
	t_slot_of<t_type> v_type_string;
	t_slot_of<t_type> v_type_tuple;
	t_slot_of<t_type> v_type_list;
	t_slot_of<t_type> v_type_map__table;
	t_slot_of<t_type> v_type_map;
	t_slot_of<t_type> v_type_bytes;
	t_slot_of<t_type> v_type_lexer__error;
	t_slot_of<t_type> v_type_parser__error;
	t_slot v_symbol_initialize;
	t_slot v_symbol_call;
	t_slot v_symbol_string;
	t_slot v_symbol_hash;
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
	t_slot v_symbol_size;
	t_slot v_string_empty;

public:
	t_global(t_object* a_type_object, t_object* a_type_type, t_object* a_type_module__body) : t_library(nullptr)
	{
		v_instance = this;
		v_type_object.f_construct(a_type_object);
		v_type_type.f_construct(a_type_type);
		v_type_module__body.f_construct(a_type_module__body);
	}
	XEMMAI__LIBRARY__MEMBERS
	t_object* f_symbol_initialize() const
	{
		return v_symbol_initialize;
	}
	t_object* f_symbol_call() const
	{
		return v_symbol_call;
	}
	t_object* f_symbol_string() const
	{
		return v_symbol_string;
	}
	t_object* f_symbol_hash() const
	{
		return v_symbol_hash;
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
	t_object* f_symbol_size() const
	{
		return v_symbol_size;
	}
	t_object* f_string_empty() const
	{
		return v_string_empty;
	}
};

XEMMAI__LIBRARY__TYPE(t_global, object)
XEMMAI__LIBRARY__TYPE(t_global, sharable)
XEMMAI__LIBRARY__TYPE(t_global, type)
XEMMAI__LIBRARY__TYPE(t_global, builder)
XEMMAI__LIBRARY__TYPE_AS(t_global, t_module::t_body, module__body)
XEMMAI__LIBRARY__TYPE(t_global, module)
XEMMAI__LIBRARY__TYPE(t_global, symbol)
XEMMAI__LIBRARY__TYPE(t_global, native)
XEMMAI__LIBRARY__TYPE(t_global, fiber)
XEMMAI__LIBRARY__TYPE(t_global, thread)
XEMMAI__LIBRARY__TYPE(t_global, scope)
XEMMAI__LIBRARY__TYPE(t_global, code)
XEMMAI__LIBRARY__TYPE(t_global, lambda)
XEMMAI__LIBRARY__TYPE(t_global, lambda_shared)
XEMMAI__LIBRARY__TYPE_AS(t_global, t_advanced_lambda<t_lambda>, advanced_lambda)
XEMMAI__LIBRARY__TYPE_AS(t_global, t_advanced_lambda<t_lambda_shared>, advanced_lambda_shared)
XEMMAI__LIBRARY__TYPE(t_global, method)
XEMMAI__LIBRARY__TYPE(t_global, throwable)
XEMMAI__LIBRARY__TYPE_AS(t_global, std::nullptr_t, null)
XEMMAI__LIBRARY__TYPE_AS(t_global, bool, boolean)
XEMMAI__LIBRARY__TYPE_AS(t_global, intptr_t, integer)
XEMMAI__LIBRARY__TYPE_AS(t_global, double, float)
XEMMAI__LIBRARY__TYPE(t_global, string)
XEMMAI__LIBRARY__TYPE(t_global, tuple)
XEMMAI__LIBRARY__TYPE(t_global, list)
XEMMAI__LIBRARY__TYPE_AS(t_global, t_map::t_table, map__table)
XEMMAI__LIBRARY__TYPE(t_global, map)
XEMMAI__LIBRARY__TYPE(t_global, bytes)
XEMMAI__LIBRARY__TYPE_AS(t_global, t_lexer::t_error, lexer__error)
XEMMAI__LIBRARY__TYPE_AS(t_global, t_parser::t_error, parser__error)

#ifdef _WIN32
XEMMAI__PUBLIC t_global* f_global();
#else
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
		[[likely]] return p->f_type();
	}
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline t_pvalue t_value<T_tag>::f_get(t_object* a_key, size_t& a_index) const
{
	return f_type()->f_get(*this, a_key, a_index);
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value<T_tag>::f_bind(t_object* a_key, size_t& a_index, t_pvalue* a_stack) const
{
	f_type()->f_bind(*this, a_key, a_index, a_stack);
}

template<typename T_tag>
XEMMAI__PORTABLE__ALWAYS_INLINE inline void t_value<T_tag>::f_call(t_object* a_key, size_t& a_index, t_pvalue* a_stack, size_t a_n) const
{
	f_type()->f_invoke(*this, a_key, a_index, a_stack, a_n);
}

#define XEMMAI__VALUE__NULLARY(a_name)\
template<typename T_tag>\
inline t_pvalue t_value<T_tag>::f_##a_name() const\
{\
	auto p = static_cast<t_object*>(*this);\
	switch (reinterpret_cast<uintptr_t>(p)) {\
	case e_tag__NULL:\
		return t_type_of<std::nullptr_t>::f__##a_name(*this);\
	case e_tag__BOOLEAN:\
		return t_type_of<bool>::f__##a_name(v_boolean);\
	case e_tag__INTEGER:\
		return t_type_of<intptr_t>::f__##a_name(v_integer);\
	case e_tag__FLOAT:\
		return t_type_of<double>::f__##a_name(v_float);\
	default:\
		{\
			t_scoped_stack stack(2);\
			p->f_type()->f_##a_name(p, stack);\
			return stack[0];\
		}\
	}\
}

XEMMAI__VALUE__NULLARY(string)
XEMMAI__VALUE__NULLARY(hash)

#define XEMMAI__VALUE__BINARY(a_method)\
		{\
			t_scoped_stack stack(3);\
			stack[2] = a_value;\
			size_t n = p->f_type()->a_method(p, stack);\
			if (n != size_t(-1)) f_loop(stack, n);\
			return stack[0];\
		}
#define XEMMAI__VALUE__BINARY_ARITHMETIC(a_name, a_operator)\
template<typename T_tag>\
inline t_pvalue t_value<T_tag>::f_##a_name(const t_pvalue& a_value) const\
{\
	auto p = static_cast<t_object*>(*this);\
	switch (reinterpret_cast<uintptr_t>(p)) {\
	case e_tag__NULL:\
	case e_tag__BOOLEAN:\
		f_throw(L"not supported."sv);\
	case e_tag__INTEGER:\
		return t_type_of<intptr_t>::f__##a_name(v_integer, a_value);\
	case e_tag__FLOAT:\
		f_check<double>(a_value, L"argument0");\
		return v_float a_operator f_as<double>(a_value);\
	default:\
		XEMMAI__VALUE__BINARY(f_##a_name)\
	}\
}
#define XEMMAI__VALUE__BINARY_INTEGRAL(a_name, a_operator)\
template<typename T_tag>\
inline t_pvalue t_value<T_tag>::f_##a_name(const t_pvalue& a_value) const\
{\
	auto p = static_cast<t_object*>(*this);\
	switch (reinterpret_cast<uintptr_t>(p)) {\
	case e_tag__INTEGER:\
		f_check<intptr_t>(a_value, L"argument0");\
		return static_cast<uintptr_t>(v_integer) a_operator f_as<intptr_t>(a_value);\
	case e_tag__NULL:\
	case e_tag__BOOLEAN:\
	case e_tag__FLOAT:\
		f_throw(L"not supported."sv);\
	default:\
		XEMMAI__VALUE__BINARY(f_##a_name)\
	}\
}
#define XEMMAI__VALUE__BINARY_EQUALITY(a_name, a_operator)\
template<typename T_tag>\
inline t_pvalue t_value<T_tag>::f_##a_name(const t_pvalue& a_value) const\
{\
	auto p = static_cast<t_object*>(*this);\
	switch (reinterpret_cast<uintptr_t>(p)) {\
	case e_tag__NULL:\
		return a_operator(p == a_value.v_p);\
	case e_tag__BOOLEAN:\
		return a_operator(p == a_value.v_p && v_boolean == a_value.v_boolean);\
	case e_tag__INTEGER:\
		return t_type_of<intptr_t>::f__##a_name(v_integer, a_value);\
	case e_tag__FLOAT:\
		return t_type_of<double>::f__##a_name(v_float, a_value);\
	default:\
		XEMMAI__VALUE__BINARY(f_##a_name)\
	}\
}
#define XEMMAI__VALUE__BINARY_BITWISE(a_name, a_operator)\
template<typename T_tag>\
inline t_pvalue t_value<T_tag>::f_##a_name(const t_pvalue& a_value) const\
{\
	auto p = static_cast<t_object*>(*this);\
	switch (reinterpret_cast<uintptr_t>(p)) {\
	case e_tag__BOOLEAN:\
		f_check<bool>(a_value, L"argument0");\
		return static_cast<bool>(v_boolean a_operator f_as<bool>(a_value));\
	case e_tag__INTEGER:\
		f_check<intptr_t>(a_value, L"argument0");\
		return v_integer a_operator f_as<intptr_t>(a_value);\
	case e_tag__NULL:\
	case e_tag__FLOAT:\
		f_throw(L"not supported."sv);\
	default:\
		XEMMAI__VALUE__BINARY(f_##a_name)\
	}\
}

XEMMAI__VALUE__BINARY_ARITHMETIC(multiply, *)
XEMMAI__VALUE__BINARY_ARITHMETIC(divide, /)
XEMMAI__VALUE__BINARY_INTEGRAL(modulus, %)
XEMMAI__VALUE__BINARY_ARITHMETIC(add, +)
XEMMAI__VALUE__BINARY_ARITHMETIC(subtract, -)
XEMMAI__VALUE__BINARY_INTEGRAL(left_shift, <<)
XEMMAI__VALUE__BINARY_INTEGRAL(right_shift, >>)
XEMMAI__VALUE__BINARY_ARITHMETIC(less, <)
XEMMAI__VALUE__BINARY_ARITHMETIC(less_equal, <=)
XEMMAI__VALUE__BINARY_ARITHMETIC(greater, >)
XEMMAI__VALUE__BINARY_ARITHMETIC(greater_equal, >=)
XEMMAI__VALUE__BINARY_EQUALITY(equals, )
XEMMAI__VALUE__BINARY_EQUALITY(not_equals, !)
XEMMAI__VALUE__BINARY_BITWISE(and, &)
XEMMAI__VALUE__BINARY_BITWISE(xor, ^)
XEMMAI__VALUE__BINARY_BITWISE(or, |)

template<typename T>
inline t_object* t_type::f_new(auto&&... a_xs)
{
	auto p = f_engine()->f_allocate(t_object::f_align_for_fields(sizeof(T)) + sizeof(t_svalue) * v_instance_fields);
	std::uninitialized_default_construct_n(p->f_fields(sizeof(T)), v_instance_fields);
	try {
		new(p->f_data()) T(std::forward<decltype(a_xs)>(a_xs)...);
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

inline t_pvalue t_type::f_get(const t_pvalue& a_this, t_object* a_key, size_t& a_index)
{
	auto i = a_index;
	if (i < v_instance_fields && f_fields()[i].first == a_key) return a_this->f_fields()[i];
	i += v_instance_fields;
	if (i < v_fields && f_fields()[i].first == a_key) {
		auto& field = f_fields()[i].second;
		return f_is_bindable(field) ? t_pvalue(xemmai::f_new<t_method>(f_global(), field, a_this)) : t_pvalue(field);
	}
	return f__get(a_this, a_key, a_index);
}

inline void t_type::f_bind_class(auto&& a_this, size_t a_index, t_pvalue* a_stack)
{
	auto& field = f_fields()[a_index].second;
	t_object* p = field;
	if (f_is_bindable(p)) {
		a_stack[0] = p;
		a_stack[1] = std::forward<decltype(a_this)>(a_this);
	} else {
		a_stack[0] = field;
		a_stack[1] = nullptr;
	}
}

inline void t_type::f_bind(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack)
{
	auto i = a_index + v_instance_fields;
	if (i < v_fields && f_fields()[i].first == a_key) {
		f_bind_class(a_this, i, a_stack);
	} else {
		i = a_index;
		if (i < v_instance_fields && f_fields()[i].first == a_key) {
			a_stack[0] = a_this->f_fields()[i];
			a_stack[1] = nullptr;
		} else {
			f__bind(a_this, a_key, a_index, a_stack);
		}
	}
}

inline void t_type::f_put(t_object* a_this, t_object* a_key, size_t& a_index, const t_pvalue& a_value)
{
	auto i = a_index;
	if (i < v_instance_fields && f_fields()[i].first == a_key)
		a_this->f_fields()[i] = a_value;
	else
		f__put(a_this, a_key, a_index, a_value);
}

inline void t_type::f_invoke_class(auto&& a_this, size_t a_index, t_pvalue* a_stack, size_t a_n)
{
	auto p = f_fields()[a_index].second.f_object_or_throw();
	if (p->f_type()->v_bindable) a_stack[1] = std::forward<decltype(a_this)>(a_this);
	size_t n = p->f_call_without_loop(a_stack, a_n);
	if (n != size_t(-1)) f_loop(a_stack, n);
}

inline void t_type::f_invoke(const t_pvalue& a_this, t_object* a_key, size_t& a_index, t_pvalue* a_stack, size_t a_n)
{
	auto i = a_index + v_instance_fields;
	if (i < v_fields && f_fields()[i].first == a_key) {
		f_invoke_class(a_this, i, a_stack, a_n);
	} else {
		i = a_index;
		if (i < v_instance_fields && f_fields()[i].first == a_key)
			a_this->f_fields()[i].f_call(a_stack, a_n);
		else
			f__invoke(a_this, a_key, a_index, a_stack, a_n);
	}
}

void t_builder::f_do(t_fields& a_fields, auto a_do)
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

template<typename T_library>
inline t_object* f_new(t_library::t_handle* a_handle, auto&&... a_xs)
{
	return f_global()->f_type<t_module::t_body>()->f_new<T_library>(a_handle, std::forward<decltype(a_xs)>(a_xs)...);
}

inline t_object* f_string_or_null(const auto& a_value)
{
	try {
		auto p = a_value.f_string();
		if (f_is<t_string>(p)) return p;
	} catch (...) {
	}
	return nullptr;
}

template<typename T_context>
intptr_t t_fiber::f_main(auto a_main)
{
	auto& fiber = t_thread::v_current->v_thread->v_fiber->f_as<t_fiber>();
	intptr_t n = -1;
	T_context context;
	try {
		try {
			a_main();
			n = 0;
		} catch (const t_rvalue& thrown) {
			fiber.f_caught(thrown, nullptr);
			if (auto p = f_string_or_null(thrown))
				std::fprintf(stderr, "caught: %ls\n", static_cast<const wchar_t*>(p->f_as<t_string>()));
			else
				std::fprintf(stderr, "caught: <unprintable>\n");
			if (f_is<t_throwable>(thrown)) thrown->f_invoke_class(/*dump*/t_type::V_fields);
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
		t_thread::v_current->v_active = p->v_fiber->v_internal;
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

inline t_object* t_tuple::f_instantiate(size_t a_size, auto a_construct)
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
	T_context context(t_object::f_of(this), a_stack);
	context.v_scope = f_engine()->f_allocate(sizeof(t_scope) + sizeof(t_svalue) * v_shareds);
	new(context.v_scope->f_data()) t_scope(v_shareds, v_scope);
	context.v_scope->f_be(f_global()->f_type<t_scope>());
	return t_code::f_loop(context);
}

t_object* t_string::f_instantiate(size_t a_n, auto a_fill)
{
	auto object = t_type_of<t_string>::f__construct(f_global()->f_type<t_string>(), a_n);
	auto& s = object->f_as<t_string>();
	auto p = a_fill(s.f_entries());
	*p = L'\0';
	s.v_size = p - s.f_entries();
	return object;
}

inline t_object* t_type_of<t_string>::f__construct(t_type* a_class, size_t a_n)
{
	auto object = f_engine()->f_allocate(sizeof(t_string) + sizeof(wchar_t) * (a_n + 1));
	object->f_be(a_class);
	return object;
}

inline t_pvalue t_type_of<t_string>::f_transfer(const t_global* a_library, auto&& a_value)
{
	return a_value.empty() ? a_library->f_string_empty() : f__construct(a_library->f_type<t_string>(), std::forward<decltype(a_value)>(a_value));
}

inline t_object* t_type_of<t_string>::f_from_code(t_global* a_library, intptr_t a_code)
{
	wchar_t c = a_code;
	return f__construct(a_library->f_type<t_string>(), &c, 1);
}

inline t_object* t_type_of<t_string>::f__substring(t_global* a_library, const t_string& a_self, size_t a_i, size_t a_n)
{
	return a_n > 0 ? f__construct(a_library->f_type<t_string>(), static_cast<const wchar_t*>(a_self) + a_i, a_n) : a_library->f_string_empty();
}

inline t_stringer::t_stringer() : v_p(f_global()->f_string_empty()), v_i(const_cast<wchar_t*>(static_cast<const wchar_t*>(v_p->f_as<t_string>()))), v_j(v_i)
{
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
