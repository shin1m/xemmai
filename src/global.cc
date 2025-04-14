#include <xemmai/convert.h>

namespace xemmai
{

void t_global::f_scan(t_scan a_scan)
{
	a_scan(v_type_object);
	a_scan(v_type_sharable);
	a_scan(v_type_type);
	a_scan(v_type_builder);
	a_scan(v_type_module__body);
	a_scan(v_type_module);
	a_scan(v_type_symbol);
	a_scan(v_type_native);
	a_scan(v_type_fiber);
	a_scan(v_type_thread);
	a_scan(v_type_scope);
	a_scan(v_type_code);
	a_scan(v_type_lambda);
	a_scan(v_type_lambda_shared);
	a_scan(v_type_advanced_lambda);
	a_scan(v_type_advanced_lambda_shared);
	a_scan(v_type_method);
	a_scan(v_type_throwable);
	a_scan(v_type_null);
	a_scan(v_type_boolean);
	a_scan(v_type_integer);
	a_scan(v_type_float);
	a_scan(v_type_string);
	a_scan(v_type_tuple);
	a_scan(v_type_list);
	a_scan(v_type_map__table);
	a_scan(v_type_map);
	a_scan(v_type_bytes);
	a_scan(v_type_lexer__error);
	a_scan(v_type_parser__error);
	a_scan(v_symbol_initialize);
	a_scan(v_symbol_call);
	a_scan(v_symbol_string);
	a_scan(v_symbol_hash);
	a_scan(v_symbol_get_at);
	a_scan(v_symbol_set_at);
	a_scan(v_symbol_plus);
	a_scan(v_symbol_minus);
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
	a_scan(v_symbol_size);
	a_scan(v_string_empty);
}

std::vector<std::pair<t_root, t_rvalue>> t_global::f_define()
{
	v_type_object->v_module = t_object::f_of(this);
	v_type_type->v_module = t_object::f_of(this);
	v_type_module__body->v_module = t_object::f_of(this);
	auto engine = f_engine();
	v_type_symbol.f_construct(engine->f_new_type_on_boot<t_symbol>(t_type::c_FIELDS, v_type_object, t_object::f_of(this)));
	v_type_native.f_construct(engine->f_new_type_on_boot<t_native>(t_type::c_FIELDS, v_type_object, t_object::f_of(this)));
	v_symbol_initialize = t_symbol::f_instantiate(L"__initialize"sv);
	v_symbol_call = t_symbol::f_instantiate(L"__call"sv);
	v_symbol_string = t_symbol::f_instantiate(L"__string"sv);
	v_symbol_hash = t_symbol::f_instantiate(L"__hash"sv);
	v_symbol_get_at = t_symbol::f_instantiate(L"__get_at"sv);
	v_symbol_set_at = t_symbol::f_instantiate(L"__set_at"sv);
	v_symbol_plus = t_symbol::f_instantiate(L"__plus"sv);
	v_symbol_minus = t_symbol::f_instantiate(L"__minus"sv);
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
	v_symbol_size = t_symbol::f_instantiate(L"size"sv);
	v_type_object->f_define();
	t_type_of<t_sharable>::f_define();
	v_type_type->v_builtin = true;
	t_define(this).f_derive<t_builder, t_object>();
	v_type_builder->v_builtin = true;
	t_define(this).f_derive<t_object>(v_type_module__body);
	v_type_module__body->v_builtin = true;
	t_define(this).f_derive<t_module, t_object>();
	v_type_module->v_builtin = v_type_module->v_revive = true;
	static_cast<t_type_of<t_symbol>*>(static_cast<t_type*>(v_type_symbol))->f_define();
	t_define(this).f_derive<t_object>(v_type_native);
	v_type_native->v_builtin = v_type_native->v_bindable = true;
	t_type_of<t_fiber>::f_define();
	v_type_fiber->v_builtin = true;
	t_type_of<t_thread>::f_define();
	v_type_thread->v_builtin = true;
	t_define(this).f_derive<t_scope, t_object>();
	v_type_scope->v_builtin = true;
	t_define(this).f_derive<t_code, t_object>();
	v_type_code->v_builtin = true;
	t_define(this).f_derive<t_lambda, t_object>();
	v_type_lambda->v_builtin = v_type_lambda->v_bindable = true;
	t_define(this).f_derive<t_lambda_shared, t_lambda>();
	v_type_lambda_shared->v_builtin = v_type_lambda_shared->v_bindable = true;
	t_define(this).f_derive<t_advanced_lambda<t_lambda>, t_lambda>();
	v_type_advanced_lambda->v_builtin = v_type_advanced_lambda->v_bindable = true;
	t_define(this).f_derive<t_advanced_lambda<t_lambda_shared>, t_lambda_shared>();
	v_type_advanced_lambda_shared->v_builtin = v_type_advanced_lambda_shared->v_bindable = true;
	t_define(this).f_derive<t_method, t_object>();
	v_type_method->v_builtin = true;
	t_type_of<t_throwable>::f_define();
	v_type_throwable->v_builtin = true;
	t_type_of<std::nullptr_t>::f_define();
	v_type_null->v_builtin = true;
	t_type_of<bool>::f_define();
	v_type_boolean->v_builtin = true;
	t_type_of<intptr_t>::f_define();
	v_type_integer->v_builtin = true;
	t_type_of<double>::f_define();
	v_type_float->v_builtin = true;
	t_type_of<t_string>::f_define();
	v_type_string->v_builtin = true;
	v_string_empty = t_type_of<t_string>::f__construct(static_cast<t_type*>(v_type_string), L""sv);
	t_type_of<t_tuple>::f_define();
	v_type_tuple->v_builtin = true;
	t_type_of<t_list>::f_define();
	v_type_list->v_builtin = true;
	t_type_of<t_map>::f_define();
	v_type_map__table->v_builtin = true;
	v_type_map->v_builtin = true;
	t_type_of<t_bytes>::f_define();
	v_type_bytes->v_builtin = true;
	t_define(this).f_derive<t_lexer::t_error, t_throwable>();
	v_type_lexer__error->v_builtin = true;
	t_define(this).f_derive<t_parser::t_error, t_throwable>();
	v_type_parser__error->v_builtin = true;
	v_tag_types[c_tag__NULL] = v_type_null;
	v_tag_types[c_tag__FALSE] = v_tag_types[c_tag__TRUE] = v_type_boolean;
	v_tag_types[c_tag__INTEGER] = v_type_integer;
	v_tag_types[c_tag__FLOAT] = v_type_float;
	return t_define(this)
	(L"Object"sv, static_cast<t_object*>(v_type_object))
	(L"Class"sv, static_cast<t_object*>(v_type_type))
	(L"Module"sv, static_cast<t_object*>(v_type_module))
	(L"Symbol"sv, static_cast<t_object*>(v_type_symbol))
	(L"Native"sv, static_cast<t_object*>(v_type_native))
	(L"Fiber"sv, static_cast<t_object*>(v_type_fiber))
	(L"Thread"sv, static_cast<t_object*>(v_type_thread))
	(L"Lambda"sv, static_cast<t_object*>(v_type_lambda))
	(L"Method"sv, static_cast<t_object*>(v_type_method))
	(L"Throwable"sv, static_cast<t_object*>(v_type_throwable))
	(L"Null"sv, static_cast<t_object*>(v_type_null))
	(L"Boolean"sv, static_cast<t_object*>(v_type_boolean))
	(L"Integer"sv, static_cast<t_object*>(v_type_integer))
	(L"Float"sv, static_cast<t_object*>(v_type_float))
	(L"String"sv, static_cast<t_object*>(v_type_string))
	(L"Tuple"sv, static_cast<t_object*>(v_type_tuple))
	(L"List"sv, static_cast<t_object*>(v_type_list))
	(L"Map"sv, static_cast<t_object*>(v_type_map))
	(L"Bytes"sv, static_cast<t_object*>(v_type_bytes))
	(L"LexerError"sv, static_cast<t_object*>(v_type_lexer__error))
	(L"ParserError"sv, static_cast<t_object*>(v_type_parser__error))
	;
}

#ifdef _WIN32
t_global* f_global()
{
	return t_global::v_instance;
}
#endif

}
