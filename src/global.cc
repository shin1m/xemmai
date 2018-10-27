#include <xemmai/global.h>

#include <xemmai/structure.h>
#include <xemmai/array.h>
#include <xemmai/bytes.h>
#include <xemmai/convert.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_global* t_global::v_instance;

t_global::t_global(t_object* a_module, t_type* a_type_object, t_type* a_type_class, t_type* a_type_structure, t_type* a_type_module, t_type* a_type_fiber, t_type* a_type_thread) : t_extension(a_module)
{
	v_instance = this;
	v_type_object.f_construct(t_object::f_of(a_type_object));
	v_type_class.f_construct(t_object::f_of(a_type_class));
	v_type_structure__discard.f_construct(t_object::f_of(v_type_object->f_derive<t_type_of<t_structure::t_discard>>()));
	v_type_structure.f_construct(t_object::f_of(a_type_structure));
	v_type_module.f_construct(t_object::f_of(a_type_module));
	v_type_fiber.f_construct(t_object::f_of(a_type_fiber));
	v_type_thread.f_construct(t_object::f_of(a_type_thread));
	v_type_tuple.f_construct(t_object::f_of(v_type_object->f_derive<t_type_of<t_tuple>>()));
	v_type_symbol.f_construct(t_object::f_of(v_type_object->f_derive<t_type_of<t_symbol>>()));
	v_type_method.f_construct(t_object::f_of(v_type_object->f_derive<t_type_of<t_method>>()));
	v_type_native.f_construct(t_object::f_of(v_type_object->f_derive<t_type_of<t_native>>()));
	v_symbol_construct = t_symbol::f_instantiate(L"__construct"sv);
	v_symbol_initialize = t_symbol::f_instantiate(L"__initialize"sv);
	v_symbol_string = t_symbol::f_instantiate(L"__string"sv);
	v_symbol_hash = t_symbol::f_instantiate(L"__hash"sv);
	v_symbol_call = t_symbol::f_instantiate(L"__call"sv);
	v_symbol_get_at = t_symbol::f_instantiate(L"__get_at"sv);
	v_symbol_set_at = t_symbol::f_instantiate(L"__set_at"sv);
	v_symbol_plus = t_symbol::f_instantiate(L"__plus"sv);
	v_symbol_minus = t_symbol::f_instantiate(L"__minus"sv);
	v_symbol_not = t_symbol::f_instantiate(L"__not"sv);
	v_symbol_complement = t_symbol::f_instantiate(L"__complement"sv);
	v_symbol_multiply = t_symbol::f_instantiate(L"__multiply"sv);
	v_symbol_divide = t_symbol::f_instantiate(L"__divide"sv);
	v_symbol_modulus = t_symbol::f_instantiate(L"__modulus"sv);
	v_symbol_add = t_symbol::f_instantiate(L"__add"sv);
	v_symbol_subtract = t_symbol::f_instantiate(L"__subtract"sv);
	v_symbol_left_shift = t_symbol::f_instantiate(L"__left_shift"sv);
	v_symbol_right_shift = t_symbol::f_instantiate(L"__right_shift"sv);
	v_symbol_less = t_symbol::f_instantiate(L"__less"sv);
	v_symbol_less_equal = t_symbol::f_instantiate(L"__less_equal"sv);
	v_symbol_greater = t_symbol::f_instantiate(L"__greater"sv);
	v_symbol_greater_equal = t_symbol::f_instantiate(L"__greater_equal"sv);
	v_symbol_equals = t_symbol::f_instantiate(L"__equals"sv);
	v_symbol_not_equals = t_symbol::f_instantiate(L"__not_equals"sv);
	v_symbol_and = t_symbol::f_instantiate(L"__and"sv);
	v_symbol_xor = t_symbol::f_instantiate(L"__xor"sv);
	v_symbol_or = t_symbol::f_instantiate(L"__or"sv);
	v_symbol_send = t_symbol::f_instantiate(L"__send"sv);
	v_symbol_path = t_symbol::f_instantiate(L"path"sv);
	v_symbol_executable = t_symbol::f_instantiate(L"executable"sv);
	v_symbol_script = t_symbol::f_instantiate(L"script"sv);
	v_symbol_arguments = t_symbol::f_instantiate(L"arguments"sv);
	v_symbol_size = t_symbol::f_instantiate(L"size"sv);
	v_symbol_dump = t_symbol::f_instantiate(L"dump"sv);
	v_type_object->f_define();
	a_module->f_put(t_symbol::f_instantiate(L"Class"sv), t_object::f_of(v_type_class));
	v_type_class->v_builtin = true;
	v_type_structure__discard->v_builtin = true;
	v_type_structure->v_builtin = true;
	a_module->f_put(t_symbol::f_instantiate(L"Module"sv), t_object::f_of(v_type_module));
	v_type_module->v_builtin = true;
	static_cast<t_type_of<t_fiber>*>(static_cast<t_type*>(v_type_fiber))->f_define();
	static_cast<t_type_of<t_thread>*>(static_cast<t_type*>(v_type_thread))->f_define();
	static_cast<t_type_of<t_tuple>*>(static_cast<t_type*>(v_type_tuple))->f_define();
	static_cast<t_type_of<t_symbol>*>(static_cast<t_type*>(v_type_symbol))->f_define();
	v_type_scope.f_construct(t_object::f_of(v_type_object->f_derive<t_type_of<t_scope>>()));
	v_type_scope->v_builtin = true;
	t_define<t_method, t_object>(this, L"Method"sv, t_object::f_of(v_type_method));
	v_type_method->v_builtin = true;
	t_define<t_code, t_object>(this, L"Code"sv);
	v_type_code->v_builtin = true;
	t_define<t_lambda, t_object>(this, L"Lambda"sv);
	v_type_lambda->v_builtin = true;
	v_type_lambda_shared.f_construct(t_object::f_of(v_type_lambda->f_derive<t_type_of<t_lambda_shared>>()));
	v_type_lambda_shared->v_builtin = true;
	v_type_advanced_lambda.f_construct(t_object::f_of(v_type_lambda->f_derive<t_type_of<t_advanced_lambda<t_lambda>>>()));
	v_type_advanced_lambda->v_builtin = true;
	v_type_advanced_lambda_shared.f_construct(t_object::f_of(v_type_lambda_shared->f_derive<t_type_of<t_advanced_lambda<t_lambda_shared>>>()));
	v_type_advanced_lambda_shared->v_builtin = true;
	a_module->f_put(t_symbol::f_instantiate(L"Native"sv), t_object::f_of(v_type_native));
	v_type_native->v_builtin = true;
	t_type_of<t_throwable>::f_define();
	v_type_throwable->v_builtin = true;
	t_type_of<std::nullptr_t>::f_define();
	v_type_null->v_builtin = v_type_null->v_primitive = true;
	t_type_of<bool>::f_define();
	v_type_boolean->v_builtin = v_type_boolean->v_primitive = true;
	t_type_of<intptr_t>::f_define();
	v_type_integer->v_builtin = v_type_integer->v_primitive = true;
	t_type_of<double>::f_define();
	v_type_float->v_builtin = v_type_float->v_primitive = true;
	t_type_of<t_string>::f_define();
	v_type_string->v_builtin = true;
	v_string_empty = t_type_of<t_string>::f__construct(static_cast<t_type*>(v_type_string), L""sv);
	t_type_of<t_array>::f_define();
	v_type_array->v_builtin = true;
	t_type_of<t_dictionary>::f_define();
	v_type_dictionary__table->v_builtin = true;
	v_type_dictionary->v_builtin = true;
	t_type_of<t_bytes>::f_define();
	v_type_bytes->v_builtin = true;
	t_define<t_lexer::t_error, t_throwable>(this, L"LexerError"sv);
	v_type_lexer__error->v_builtin = true;
	t_define<t_parser::t_error, t_throwable>(this, L"ParserError"sv);
	v_type_parser__error->v_builtin = true;
}

void t_global::f_scan(t_scan a_scan)
{
	a_scan(v_type_object);
	a_scan(v_type_class);
	a_scan(v_type_structure__discard);
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
	a_scan(v_type_lambda_shared);
	a_scan(v_type_advanced_lambda);
	a_scan(v_type_advanced_lambda_shared);
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
	a_scan(v_symbol_dump);
	a_scan(v_string_empty);
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_global* f_global()
{
	return t_global::v_instance;
}
#endif

}
