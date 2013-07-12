#include <xemmai/global.h>

#include <xemmai/method.h>
#include <xemmai/lambda.h>
#include <xemmai/array.h>
#include <xemmai/bytes.h>
#include <xemmai/convert.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_global* t_global::v_instance;

t_global::t_global(t_object* a_module, const t_transfer& a_type_object, const t_transfer& a_type_class, const t_transfer& a_type_structure, const t_transfer& a_type_module, const t_transfer& a_type_fiber, const t_transfer& a_type_thread) :
t_extension(a_module),
v_type_object(a_type_object),
v_type_class(a_type_class),
v_type_structure(a_type_structure),
v_type_module(a_type_module),
v_type_fiber(a_type_fiber),
v_type_thread(a_type_thread)
{
	v_instance = this;
	v_type_tuple = t_class::f_instantiate(new t_type_of<t_tuple>(a_module, v_type_object));
	v_type_symbol = t_class::f_instantiate(new t_type_of<t_symbol>(a_module, v_type_object));
	v_type_method = t_class::f_instantiate(new t_type_of<t_method>(a_module, v_type_object));
	v_type_native = t_class::f_instantiate(new t_type_of<t_native>(a_module, v_type_object));
	v_symbol_construct = t_symbol::f_instantiate(L"__construct");
	v_symbol_initialize = t_symbol::f_instantiate(L"__initialize");
	v_symbol_string = t_symbol::f_instantiate(L"__string");
	v_symbol_hash = t_symbol::f_instantiate(L"__hash");
	v_symbol_call = t_symbol::f_instantiate(L"__call");
	v_symbol_get_at = t_symbol::f_instantiate(L"__get_at");
	v_symbol_set_at = t_symbol::f_instantiate(L"__set_at");
	v_symbol_plus = t_symbol::f_instantiate(L"__plus");
	v_symbol_minus = t_symbol::f_instantiate(L"__minus");
	v_symbol_not = t_symbol::f_instantiate(L"__not");
	v_symbol_complement = t_symbol::f_instantiate(L"__complement");
	v_symbol_multiply = t_symbol::f_instantiate(L"__multiply");
	v_symbol_divide = t_symbol::f_instantiate(L"__divide");
	v_symbol_modulus = t_symbol::f_instantiate(L"__modulus");
	v_symbol_add = t_symbol::f_instantiate(L"__add");
	v_symbol_subtract = t_symbol::f_instantiate(L"__subtract");
	v_symbol_left_shift = t_symbol::f_instantiate(L"__left_shift");
	v_symbol_right_shift = t_symbol::f_instantiate(L"__right_shift");
	v_symbol_less = t_symbol::f_instantiate(L"__less");
	v_symbol_less_equal = t_symbol::f_instantiate(L"__less_equal");
	v_symbol_greater = t_symbol::f_instantiate(L"__greater");
	v_symbol_greater_equal = t_symbol::f_instantiate(L"__greater_equal");
	v_symbol_equals = t_symbol::f_instantiate(L"__equals");
	v_symbol_not_equals = t_symbol::f_instantiate(L"__not_equals");
	v_symbol_and = t_symbol::f_instantiate(L"__and");
	v_symbol_xor = t_symbol::f_instantiate(L"__xor");
	v_symbol_or = t_symbol::f_instantiate(L"__or");
	v_symbol_send = t_symbol::f_instantiate(L"__send");
	v_symbol_path = t_symbol::f_instantiate(L"path");
	v_symbol_executable = t_symbol::f_instantiate(L"executable");
	v_symbol_script = t_symbol::f_instantiate(L"script");
	v_symbol_arguments = t_symbol::f_instantiate(L"arguments");
	v_symbol_size = t_symbol::f_instantiate(L"size");
	v_symbol_push = t_symbol::f_instantiate(L"push");
	t_type::f_define(v_type_object);
	xemmai::f_as<t_type&>(v_type_object).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_object).v_primitive = true;
	a_module->f_put(t_symbol::f_instantiate(L"Class"), v_type_class);
	xemmai::f_as<t_type&>(v_type_class).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_structure).v_builtin = true;
	a_module->f_put(t_symbol::f_instantiate(L"Module"), v_type_module);
	xemmai::f_as<t_type&>(v_type_module).v_builtin = true;
	t_fiber::f_define(v_type_fiber);
	xemmai::f_as<t_type&>(v_type_fiber).v_builtin = true;
	t_thread::f_define(v_type_thread);
	xemmai::f_as<t_type&>(v_type_thread).v_builtin = true;
	t_type_of<t_tuple>::f_define(v_type_tuple);
	xemmai::f_as<t_type&>(v_type_tuple).v_builtin = true;
	t_symbol::f_define(v_type_symbol);
	xemmai::f_as<t_type&>(v_type_symbol).v_builtin = true;
	v_type_scope = t_class::f_instantiate(new t_type_of<t_scope>(a_module, v_type_object));
	xemmai::f_as<t_type&>(v_type_scope).v_builtin = true;
	t_define<t_method, t_object>(this, L"Method", v_type_method);
	xemmai::f_as<t_type&>(v_type_method).v_builtin = true;
	t_define<t_code, t_object>(this, L"Code");
	xemmai::f_as<t_type&>(v_type_code).v_builtin = true;
	t_define<t_lambda, t_object>(this, L"Lambda");
	xemmai::f_as<t_type&>(v_type_lambda).v_builtin = true;
	v_type_advanced_lambda = t_class::f_instantiate(new t_type_of<t_advanced_lambda>(a_module, v_type_lambda));
	xemmai::f_as<t_type&>(v_type_advanced_lambda).v_builtin = true;
	a_module->f_put(t_symbol::f_instantiate(L"Native"), v_type_native);
	xemmai::f_as<t_type&>(v_type_native).v_builtin = true;
	t_type_of<t_throwable>::f_define();
	xemmai::f_as<t_type&>(v_type_throwable).v_builtin = true;
	t_type_of<std::nullptr_t>::f_define();
	xemmai::f_as<t_type&>(v_type_null).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_null).v_primitive = true;
	t_type_of<bool>::f_define();
	xemmai::f_as<t_type&>(v_type_boolean).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_boolean).v_primitive = true;
	t_type_of<ptrdiff_t>::f_define();
	xemmai::f_as<t_type&>(v_type_integer).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_integer).v_primitive = true;
	t_type_of<double>::f_define();
	xemmai::f_as<t_type&>(v_type_float).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_float).v_primitive = true;
	t_type_of<std::wstring>::f_define();
	xemmai::f_as<t_type&>(v_type_string).v_builtin = true;
	t_type_of<t_array>::f_define();
	xemmai::f_as<t_type&>(v_type_array).v_builtin = true;
	t_type_of<t_dictionary>::f_define();
	xemmai::f_as<t_type&>(v_type_dictionary__table).v_builtin = true;
	xemmai::f_as<t_type&>(v_type_dictionary).v_builtin = true;
	t_type_of<t_bytes>::f_define();
	xemmai::f_as<t_type&>(v_type_bytes).v_builtin = true;
	t_define<t_lexer::t_error, t_throwable>(this, L"LexerError");
	xemmai::f_as<t_type&>(v_type_lexer__error).v_builtin = true;
	t_define<t_parser::t_error, t_throwable>(this, L"ParserError");
	xemmai::f_as<t_type&>(v_type_parser__error).v_builtin = true;
}

void t_global::f_scan(t_scan a_scan)
{
	a_scan(v_type_object);
	a_scan(v_type_class);
	a_scan(v_type_structure);
	a_scan(v_type_module);
	a_scan(v_type_fiber);
	a_scan(v_type_thread);
	a_scan(v_type_tuple);
	a_scan(v_type_symbol);
	a_scan(v_type_scope);
	a_scan(v_type_method);
	a_scan(v_type_code);
	a_scan(v_type_lambda);
	a_scan(v_type_advanced_lambda);
	a_scan(v_type_native);
	a_scan(v_type_throwable);
	a_scan(v_type_null);
	a_scan(v_type_boolean);
	a_scan(v_type_integer);
	a_scan(v_type_float);
	a_scan(v_type_string);
	a_scan(v_type_array);
	a_scan(v_type_dictionary__table);
	a_scan(v_type_dictionary);
	a_scan(v_type_bytes);
	a_scan(v_type_lexer__error);
	a_scan(v_type_parser__error);
	a_scan(v_symbol_construct);
	a_scan(v_symbol_initialize);
	a_scan(v_symbol_string);
	a_scan(v_symbol_hash);
	a_scan(v_symbol_call);
	a_scan(v_symbol_get_at);
	a_scan(v_symbol_set_at);
	a_scan(v_symbol_plus);
	a_scan(v_symbol_minus);
	a_scan(v_symbol_not);
	a_scan(v_symbol_complement);
	a_scan(v_symbol_multiply);
	a_scan(v_symbol_divide);
	a_scan(v_symbol_modulus);
	a_scan(v_symbol_add);
	a_scan(v_symbol_subtract);
	a_scan(v_symbol_left_shift);
	a_scan(v_symbol_right_shift);
	a_scan(v_symbol_less);
	a_scan(v_symbol_less_equal);
	a_scan(v_symbol_greater);
	a_scan(v_symbol_greater_equal);
	a_scan(v_symbol_equals);
	a_scan(v_symbol_not_equals);
	a_scan(v_symbol_and);
	a_scan(v_symbol_xor);
	a_scan(v_symbol_or);
	a_scan(v_symbol_send);
	a_scan(v_symbol_path);
	a_scan(v_symbol_executable);
	a_scan(v_symbol_script);
	a_scan(v_symbol_arguments);
	a_scan(v_symbol_size);
	a_scan(v_symbol_push);
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_global* f_global()
{
	return t_global::v_instance;
}
#endif

}
