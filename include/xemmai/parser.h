#ifndef XEMMAI__PARSER_H
#define XEMMAI__PARSER_H

#include "integer.h"
#include "float.h"
#include "lexer.h"
#include "ast.h"
#include "module.h"

namespace xemmai
{

class XEMMAI__LOCAL t_parser
{
	t_script& v_module;
	t_lexer v_lexer;
	ast::t_arena& v_arena;
	ast::t_scope* v_scope;
	bool v_can_jump = false;
	bool v_can_return = false;

	bool f_single_colon() const
	{
		return v_lexer.f_token() == t_lexer::c_token__COLON && v_lexer.f_integer() == 1;
	}
	void f_throw [[noreturn]] (std::wstring_view a_message);
	t_object* f_symbol() const
	{
		return v_module.f_slot(t_symbol::f_instantiate(v_lexer.f_value()));
	}
	t_code::t_variable& f_variable(ast::t_scope* a_scope, t_object* a_symbol);
	bool f_any(size_t a_indent, t_lexer::t_token a_right) const
	{
		return (!v_lexer.f_newline() || v_lexer.f_indent() > a_indent) && v_lexer.f_token() != a_right;
	}
	void f_close(size_t a_indent, t_lexer::t_token a_right)
	{
		if ((!v_lexer.f_newline() || v_lexer.f_indent() >= a_indent) && v_lexer.f_token() == a_right) v_lexer.f_next();
	}
	bool f_has_expression() const;
	ast::t_node* f_target(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_action(size_t a_depth, size_t a_head, size_t a_indent, ast::t_node* a_target, bool a_assignable);
	ast::t_node* f_unary(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_multiplicative(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_additive(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_shift(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_relational(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_equality(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_and(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_xor(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_or(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_and_also(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_or_else(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_conditional(size_t a_depth, size_t a_head, bool a_assignable);
	ast::t_node* f_expression(size_t a_depth, size_t a_head);
	bool f_argument(size_t a_depth, size_t a_head, size_t a_indent, ast::t_nodes& a_nodes);
	void f_arguments(size_t a_depth, size_t a_head, size_t a_indent, ast::t_call* a_call);
	void f_expressions(size_t a_depth, size_t a_head, ast::t_nodes& a_nodes);
	ast::t_nodes* f_options(size_t a_depth, size_t a_head);
	ast::t_node* f_body(size_t a_depth, size_t a_head, size_t a_indent);

public:
	struct t_error : t_throwable
	{
		std::wstring v_path;
		const t_at v_at;

		t_error(std::wstring_view a_message, std::wstring_view a_path, const t_at& a_at) : t_throwable(L"syntax error: " + std::wstring(a_message)), v_path(a_path), v_at(a_at)
		{
		}
		virtual void f_dump() const;
	};

	t_parser(t_script& a_module, std::FILE* a_stream, ast::t_arena& a_arena) : v_module(a_module), v_lexer(v_module.v_path, a_stream), v_arena(a_arena)
	{
	}
	void operator()(ast::t_scope& a_scope);
};

template<>
struct t_type_of<t_parser::t_error> : t_uninstantiatable<t_bears<t_parser::t_error, t_type_of<t_throwable>>>
{
	using t_base::t_base;
};

}

#endif
