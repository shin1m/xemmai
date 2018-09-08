#ifndef XEMMAI__PARSER_H
#define XEMMAI__PARSER_H

#include "integer.h"
#include "float.h"
#include "lexer.h"
#include "ast.h"
#include "module.h"

namespace xemmai
{

class t_parser
{
	t_script& v_module;
	t_lexer v_lexer;
	ast::t_scope* v_scope;
	bool v_can_jump = false;
	bool v_can_return = false;

	bool f_single_colon() const
	{
		return v_lexer.f_token() == t_lexer::e_token__COLON && v_lexer.f_value().size() == 1;
	}
	void f_throw [[noreturn]] (std::wstring_view a_message, const t_at& a_at)
	{
		throw t_error::f_instantiate(a_message, v_lexer.f_path(), a_at);
	}
	void f_throw [[noreturn]] (std::wstring_view a_message)
	{
		f_throw(a_message, v_lexer.f_at());
	}
	t_code::t_variable& f_variable(ast::t_scope* a_scope, t_object* a_symbol);
	intptr_t f_integer()
	{
		return t_type_of<intptr_t>::f_parse(&v_lexer.f_value()[0]);
	}
	double f_float()
	{
		return t_type_of<double>::f_parse(&v_lexer.f_value()[0]);
	}
	std::unique_ptr<ast::t_node> f_target(bool a_assignable);
	std::unique_ptr<ast::t_node> f_action(size_t a_indent, std::unique_ptr<ast::t_node>&& a_target, bool a_assignable);
	std::unique_ptr<ast::t_node> f_action(size_t a_indent, ast::t_node* a_target, bool a_assignable)
	{
		return f_action(a_indent, std::unique_ptr<ast::t_node>(a_target), a_assignable);
	}
	std::unique_ptr<ast::t_node> f_unary(bool a_assignable);
	std::unique_ptr<ast::t_node> f_multiplicative(bool a_assignable);
	std::unique_ptr<ast::t_node> f_additive(bool a_assignable);
	std::unique_ptr<ast::t_node> f_shift(bool a_assignable);
	std::unique_ptr<ast::t_node> f_relational(bool a_assignable);
	std::unique_ptr<ast::t_node> f_equality(bool a_assignable);
	std::unique_ptr<ast::t_node> f_and(bool a_assignable);
	std::unique_ptr<ast::t_node> f_xor(bool a_assignable);
	std::unique_ptr<ast::t_node> f_or(bool a_assignable);
	std::unique_ptr<ast::t_node> f_and_also(bool a_assignable);
	std::unique_ptr<ast::t_node> f_or_else(bool a_assignable);
	std::unique_ptr<ast::t_node> f_send(bool a_assignable);
	std::unique_ptr<ast::t_node> f_conditional(bool a_assignable);
	std::unique_ptr<ast::t_node> f_expression();
	bool f_expressions(size_t a_indent, std::vector<std::unique_ptr<ast::t_node>>& a_nodes);
	void f_expressions(std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
	{
		while (true) {
			a_nodes.push_back(f_expression());
			if (v_lexer.f_token() != t_lexer::e_token__COMMA) break;
			v_lexer.f_next();
		}
	}
	bool f_end_of_expression()
	{
		if (v_lexer.f_newline()) return true;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__IF:
		case t_lexer::e_token__ELSE:
		case t_lexer::e_token__WHILE:
		case t_lexer::e_token__FOR:
		case t_lexer::e_token__TRY:
		case t_lexer::e_token__CATCH:
		case t_lexer::e_token__FINALLY:
			return true;
		default:
			return false;
		}
	}
	void f_block(std::vector<std::unique_ptr<ast::t_node>>& a_nodes);
	void f_block_or_expression(size_t a_indent, std::vector<std::unique_ptr<ast::t_node>>& a_nodes);

public:
	struct t_error : t_throwable
	{
		static t_scoped f_instantiate(std::wstring_view a_message, std::wstring_view a_path, const t_at& a_at);

		std::wstring v_path;
		t_at v_at;

		t_error(std::wstring_view a_message, std::wstring_view a_path, const t_at& a_at) : t_throwable(L"syntax error: " + std::wstring(a_message)), v_path(a_path), v_at(a_at)
		{
		}
		virtual void f_dump() const;
	};

	t_parser(t_script& a_module, std::FILE* a_stream) : v_module(a_module), v_lexer(v_module.v_path, a_stream)
	{
	}
	void operator()(ast::t_scope& a_scope);
};

template<>
struct t_type_of<t_parser::t_error> : t_uninstantiatable<t_underivable<t_bears<t_parser::t_error, t_type_of<t_throwable>>>>
{
	using t_base::t_base;
};

}

#endif
