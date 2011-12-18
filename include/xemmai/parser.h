#ifndef XEMMAI__PARSER_H
#define XEMMAI__PARSER_H

#include "integer.h"
#include "float.h"
#include "lexer.h"
#include "ast.h"

namespace xemmai
{

class t_parser
{
	struct t_targets
	{
		bool v_loop;
		bool v_return;

		t_targets(bool a_loop, bool a_return) : v_loop(a_loop), v_return(a_return)
		{
		}
	};

	t_lexer v_lexer;
	ast::t_scope* v_scope;
	t_targets* v_targets;

	bool f_single_colon() const
	{
		return v_lexer.f_token() == t_lexer::e_token__COLON && v_lexer.f_value().size() == 1;
	}
	void f_throw(const std::wstring& a_message);
	ast::t_variable& f_variable(ast::t_scope* a_scope, const t_value& a_symbol, bool a_loop);
	ptrdiff_t f_integer()
	{
		return t_type_of<ptrdiff_t>::f_parse(&v_lexer.f_value()[0]);
	}
	double f_float()
	{
		return t_type_of<double>::f_parse(&v_lexer.f_value()[0]);
	}
	t_pointer<ast::t_node> f_target(bool a_assignable);
	t_pointer<ast::t_node> f_action(const t_pointer<ast::t_node>& a_target, bool a_assignable);
	t_pointer<ast::t_node> f_unary(bool a_assignable);
	t_pointer<ast::t_node> f_multiplicative(bool a_assignable);
	t_pointer<ast::t_node> f_additive(bool a_assignable);
	t_pointer<ast::t_node> f_shift(bool a_assignable);
	t_pointer<ast::t_node> f_relational(bool a_assignable);
	t_pointer<ast::t_node> f_equality(bool a_assignable);
	t_pointer<ast::t_node> f_and(bool a_assignable);
	t_pointer<ast::t_node> f_xor(bool a_assignable);
	t_pointer<ast::t_node> f_or(bool a_assignable);
	t_pointer<ast::t_node> f_and_also(bool a_assignable);
	t_pointer<ast::t_node> f_or_else(bool a_assignable);
	t_pointer<ast::t_node> f_send(bool a_assignable);
	t_pointer<ast::t_node> f_conditional(bool a_assignable);
	t_pointer<ast::t_node> f_expression();
	bool f_expressions(t_pointers<ast::t_node>& a_nodes);
	t_pointer<ast::t_node> f_statement();
	void f_statements(t_pointers<ast::t_node>& a_nodes, t_lexer::t_token a_token);
	void f_block(t_pointers<ast::t_node>& a_nodes);
	void f_block_or_statement(t_pointers<ast::t_node>& a_nodes);

public:
	struct t_error : t_throwable
	{
		static t_transfer f_instantiate(const std::wstring& a_message, t_lexer& a_lexer);

		std::wstring v_path;
		t_at v_at;

		t_error(const std::wstring& a_message, t_lexer& a_lexer) :
		t_throwable(L"syntax error: " + a_message), v_path(a_lexer.f_path()), v_at(a_lexer.f_at())
		{
		}
		virtual void f_dump() const;
	};

	t_parser(const std::wstring& a_path, FILE* a_stream) : v_lexer(a_path, a_stream)
	{
	}
	void f_parse(ast::t_module& a_module);
};

template<>
struct t_type_of<t_parser::t_error> : t_type_of<t_throwable>
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<t_throwable>(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
