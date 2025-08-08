#include <xemmai/convert.h>

namespace xemmai
{

void t_global::f_scan(t_scan a_scan)
{
	XEMMAI__GLOBAL__TYPES(XEMMAI__TYPE__SCAN)
	XEMMAI__GLOBAL__SYMBOLS(XEMMAI__SYMBOL__SCAN)
	a_scan(v_initialize_validate);
	a_scan(v_initialize_ignore);
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
	XEMMAI__GLOBAL__SYMBOLS(XEMMAI__SYMBOL__INSTANTIATE)
	v_initialize_validate = f_new<t_native>(this, [](t_library*, t_pvalue* a_stack, size_t a_n)
	{
		if (a_n > 0) f_throw(L"must be called with no argument."sv);
		a_stack[0] = nullptr;
	}, t_object::f_of(this));
	v_initialize_ignore = f_new<t_native>(this, [](t_library*, t_pvalue* a_stack, size_t)
	{
		a_stack[0] = nullptr;
	}, t_object::f_of(this));
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
