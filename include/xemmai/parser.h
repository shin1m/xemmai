#ifndef XEMMAI__PARSER_H
#define XEMMAI__PARSER_H

#include "code.h"
#include "lexer.h"

namespace xemmai
{

class t_parser
{
	struct t_scope
	{
		t_scope* v_outer;
		t_object* v_code;

		t_scope(t_scope* a_outer, t_object* a_code) : v_outer(a_outer), v_code(a_code)
		{
		}
	};
	struct t_targets
	{
		std::vector<size_t>* v_break;
		std::vector<size_t>* v_continue;
		std::vector<size_t>* v_return;

		t_targets(std::vector<size_t>* a_break, std::vector<size_t>* a_continue, std::vector<size_t>* a_return) :
		v_break(a_break), v_continue(a_continue), v_return(a_return)
		{
		}
	};

	t_lexer v_lexer;
	t_scope* v_scope;
	std::vector<void*>* v_instructions;
	std::vector<void*>* v_objects;
	t_targets* v_targets;

	void f_throw(const std::wstring& a_message);
	void f_emit(t_instruction a_instruction)
	{
		v_instructions->push_back(reinterpret_cast<void*>(a_instruction | e_instruction__DEAD));
	}
	void f_operand(size_t a_operand)
	{
		v_instructions->push_back(reinterpret_cast<void*>(a_operand));
	}
	void f_operand(int* a_operand)
	{
		v_instructions->push_back(static_cast<void*>(a_operand));
	}
	void f_operand(const t_transfer& a_operand)
	{
		v_objects->push_back(0);
		v_instructions->push_back(static_cast<t_object*>(*new(&v_objects->back()) t_slot(a_operand)));
	}
	void f_operand(std::vector<size_t>& a_label)
	{
		a_label.push_back(v_instructions->size());
		f_operand(size_t(0));
	}
	void f_resolve(const std::vector<size_t>& a_label, size_t a_n)
	{
		for (std::vector<size_t>::const_iterator i = a_label.begin(); i != a_label.end(); ++i) (*v_instructions)[*i] = reinterpret_cast<void*>(a_n);
	}
	void f_resolve(const std::vector<size_t>& a_label)
	{
		f_resolve(a_label, v_instructions->size());
	}
	void f_at(long a_position, size_t a_line, size_t a_column)
	{
		f_as<t_code*>(v_scope->v_code)->f_at(v_instructions->size(), a_position, a_line, a_column);
	}
	void f_get(long a_position, size_t a_line, size_t a_column, size_t a_outer, t_scope* a_scope, const t_transfer& a_symbol);
	int* f_index(t_scope* a_scope, const t_transfer& a_symbol, bool a_loop);
	void f_number(long a_position, size_t a_line, size_t a_column, t_lexer::t_token a_token);
	void f_target();
	void f_call();
	void f_dot();
	void f_action();
	void f_primary();
	void f_unary();
	void f_multiplicative();
	void f_additive();
	void f_shift();
	void f_relational();
	void f_equality();
	void f_and();
	void f_xor();
	void f_or();
	void f_and_also();
	void f_or_else();
	void f_extension();
	void f_conditional();
	void f_expression();
	size_t f_expressions();
	void f_statement();
	void f_block();
	void f_block_or_statement();

public:
	struct t_error : t_throwable
	{
		static t_transfer f_instantiate(const std::wstring& a_message, t_lexer& a_lexer);

		std::wstring v_path;
		long v_position;
		size_t v_line;
		size_t v_column;

		t_error(const std::wstring& a_message, t_lexer& a_lexer) :
		t_throwable(L"syntax error: " + a_message), v_path(a_lexer.f_path()), v_position(a_lexer.f_position()), v_line(a_lexer.f_line()), v_column(a_lexer.f_column())
		{
		}
		virtual void f_dump() const;
	};

	t_parser(const std::wstring& a_path, FILE* a_stream) : v_lexer(a_path, a_stream)
	{
	}
	t_transfer f_parse();
};

template<>
struct t_type_of<t_parser::t_error> : t_type_of<t_throwable>
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<t_throwable>(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

#endif
