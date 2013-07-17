#ifndef XEMMAI__SYNTAX_H
#define XEMMAI__SYNTAX_H

#include <deque>
#include <map>

#include "code.h"

namespace xemmai
{

struct t_generator;

namespace ast
{

struct t_operand
{
	enum t_tag
	{
		e_tag__LITERAL,
		e_tag__VARIABLE,
		e_tag__TEMPORARY
	};

	t_tag v_tag;
	t_transfer v_value;

	t_operand(size_t a_value) : v_tag(e_tag__TEMPORARY), v_value(a_value)
	{
	}
	t_operand(t_tag a_tag, size_t a_value) : v_tag(a_tag), v_value(a_value)
	{
	}
	t_operand(const t_transfer& a_value) : v_tag(e_tag__LITERAL), v_value(a_value)
	{
	}
};

class t_node
{
	friend struct xemmai::t_generator;

	t_at v_at;

public:
	t_node(const t_at& a_at) : v_at(a_at)
	{
	}
	virtual ~t_node() = default;
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand) = 0;
};

void f_generate_block(t_generator& a_generator, size_t a_stack, const std::vector<std::unique_ptr<t_node>>& a_nodes, bool a_tail);
void f_generate_block_without_value(t_generator& a_generator, size_t a_stack, const std::vector<std::unique_ptr<t_node>>& a_nodes);

struct t_variable
{
	bool v_shared = false;
	bool v_varies = false;
	size_t v_index;
};

struct t_scope
{
	t_scope* v_outer;
	std::vector<std::unique_ptr<t_node>> v_block;
	bool v_shared = false;
	bool v_self_shared = false;
	std::map<t_scoped, t_variable> v_variables;
	std::vector<t_variable*> v_privates;
	size_t v_shareds = 0;

	t_scope(t_scope* a_outer) : v_outer(a_outer)
	{
	}
};

struct t_lambda : t_node, t_scope
{
	bool v_variadic = false;
	size_t v_arguments = 0;
	std::vector<std::unique_ptr<t_node>> v_defaults;

	t_lambda(const t_at& a_at, t_scope* a_outer) : t_node(a_at), t_scope(a_outer)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_if : t_node
{
	std::unique_ptr<t_node> v_expression;
	std::vector<std::unique_ptr<t_node>> v_true;
	std::vector<std::unique_ptr<t_node>> v_false;

	t_if(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_while : t_node
{
	std::unique_ptr<t_node> v_expression;
	std::vector<std::unique_ptr<t_node>> v_block;

	t_while(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_for : t_node
{
	std::vector<std::unique_ptr<t_node>> v_initialization;
	std::unique_ptr<t_node> v_condition;
	std::vector<std::unique_ptr<t_node>> v_next;
	std::vector<std::unique_ptr<t_node>> v_block;

	using t_node::t_node;
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_break : t_node
{
	std::unique_ptr<t_node> v_expression;

	t_break(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_continue : t_node
{
	using t_node::t_node;
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_return : t_node
{
	std::unique_ptr<t_node> v_expression;

	t_return(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_try : t_node
{
	struct t_catch
	{
		std::unique_ptr<t_node> v_expression;
		const t_variable& v_variable;
		std::vector<std::unique_ptr<t_node>> v_block;

		t_catch(std::unique_ptr<t_node>&& a_expression, const t_variable& a_variable) : v_expression(std::move(a_expression)), v_variable(a_variable)
		{
		}
	};

	std::vector<std::unique_ptr<t_node>> v_block;
	std::vector<std::unique_ptr<t_catch>> v_catches;
	std::vector<std::unique_ptr<t_node>> v_finally;

	using t_node::t_node;
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_throw : t_node
{
	std::unique_ptr<t_node> v_expression;

	t_throw(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_get : t_node
{
	std::unique_ptr<t_node> v_target;
	t_scoped v_key;

	t_object_get(const t_at& a_at, std::unique_ptr<t_node>&& a_target, const t_transfer& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_get_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;

	t_object_get_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_put : t_node
{
	std::unique_ptr<t_node> v_target;
	t_scoped v_key;
	std::unique_ptr<t_node> v_value;

	t_object_put(const t_at& a_at, std::unique_ptr<t_node>&& a_target, const t_transfer& a_key, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key), v_value(std::move(a_value))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_put_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;
	std::unique_ptr<t_node> v_value;

	t_object_put_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key)), v_value(std::move(a_value))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_has : t_node
{
	std::unique_ptr<t_node> v_target;
	t_scoped v_key;

	t_object_has(const t_at& a_at, std::unique_ptr<t_node>&& a_target, const t_transfer& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_has_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;

	t_object_has_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_remove : t_node
{
	std::unique_ptr<t_node> v_target;
	t_scoped v_key;

	t_object_remove(const t_at& a_at, std::unique_ptr<t_node>&& a_target, const t_transfer& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_object_remove_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;

	t_object_remove_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_global_get : t_node
{
	t_scoped v_key;

	t_global_get(const t_at& a_at, const t_transfer& a_key) : t_node(a_at), v_key(a_key)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_scope_get : t_node
{
	size_t v_outer;
	const t_variable& v_variable;

	t_scope_get(const t_at& a_at, size_t a_outer, const t_variable& a_variable) : t_node(a_at), v_outer(a_outer), v_variable(a_variable)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_scope_put : t_node
{
	size_t v_outer;
	const t_variable& v_variable;
	std::unique_ptr<t_node> v_value;

	t_scope_put(const t_at& a_at, size_t a_outer, const t_variable& a_variable, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_outer(a_outer), v_variable(a_variable), v_value(std::move(a_value))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_self : t_node
{
	size_t v_outer;

	t_self(const t_at& a_at, size_t a_outer) : t_node(a_at), v_outer(a_outer)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_class : t_node
{
	std::unique_ptr<t_node> v_target;

	t_class(const t_at& a_at, std::unique_ptr<t_node>&& a_target) : t_node(a_at), v_target(std::move(a_target))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_super : t_node
{
	std::unique_ptr<t_node> v_target;

	t_super(const t_at& a_at, std::unique_ptr<t_node>&& a_target) : t_node(a_at), v_target(std::move(a_target))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_null : t_node
{
	using t_node::t_node;
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_boolean : t_node
{
	bool v_value;

	t_boolean(const t_at& a_at, bool a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_integer : t_node
{
	ptrdiff_t v_value;

	t_integer(const t_at& a_at, ptrdiff_t a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_float : t_node
{
	double v_value;

	t_float(const t_at& a_at, double a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_instance : t_node
{
	t_scoped v_value;

	t_instance(const t_at& a_at, const t_transfer& a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_unary : t_node
{
	t_instruction v_instruction;
	std::unique_ptr<t_node> v_expression;

	t_unary(const t_at& a_at, t_instruction a_instruction, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_instruction(a_instruction), v_expression(std::move(a_expression))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_binary : t_node
{
	t_instruction v_instruction;
	std::unique_ptr<t_node> v_left;
	std::unique_ptr<t_node> v_right;

	t_binary(const t_at& a_at, t_instruction a_instruction, std::unique_ptr<t_node>&& a_left, std::unique_ptr<t_node>&& a_right) : t_node(a_at), v_instruction(a_instruction), v_left(std::move(a_left)), v_right(std::move(a_right))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_call : t_node
{
	std::unique_ptr<t_node> v_target;
	std::vector<std::unique_ptr<t_node>> v_arguments;
	bool v_expand;

	t_call(const t_at& a_at, std::unique_ptr<t_node>&& a_target) : t_node(a_at), v_target(std::move(a_target)), v_expand(false)
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_get_at : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_index;

	t_get_at(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_index) : t_node(a_at), v_target(std::move(a_target)), v_index(std::move(a_index))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_set_at : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_index;
	std::unique_ptr<t_node> v_value;

	t_set_at(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_index, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_target(std::move(a_target)), v_index(std::move(a_index)), v_value(std::move(a_value))
	{
	}
	virtual t_operand f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand);
};

struct t_module : t_scope
{
	std::wstring v_path;

	t_module(const std::wstring& a_path) : t_scope(nullptr), v_path(a_path)
	{
	}
	const std::wstring& f_path() const
	{
		return v_path;
	}
};

}

struct t_generator
{
	struct t_targets
	{
		t_code::t_label* v_break;
		bool v_break_is_tail;
		t_code::t_label* v_continue;
		t_code::t_label* v_return;
		bool v_return_is_tail;

		t_targets(t_code::t_label* a_break, bool a_break_is_tail, t_code::t_label* a_continue, t_code::t_label* a_return, bool a_return_is_tail) :
		v_break(a_break), v_break_is_tail(a_break_is_tail),
		v_continue(a_continue),
		v_return(a_return), v_return_is_tail(a_return_is_tail)
		{
		}
	};

	std::wstring v_path;
	ast::t_scope* v_scope;
	t_code* v_code;
	std::deque<t_code::t_label>* v_labels;
	t_targets* v_targets;

	t_transfer f_generate(ast::t_module& a_module);
	void f_emit(t_instruction a_instruction)
	{
		v_code->f_emit(a_instruction);
	}
	void f_reserve(size_t a_size)
	{
		if (a_size > v_code->v_size) v_code->v_size = a_size;
	}
	void f_operand(size_t a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(bool a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(short a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(int a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(long a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(double a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(t_object* a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(const t_transfer& a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(t_code::t_label& a_label)
	{
		v_code->f_operand(a_label);
	}
	void f_target(t_code::t_label& a_label)
	{
		v_code->f_target(a_label);
	}
	t_code::t_label& f_label()
	{
		v_labels->push_back(t_code::t_label());
		return v_labels->back();
	}
	void f_resolve()
	{
		for (const auto& label : *v_labels) v_code->f_resolve(label);
	}
	void f_at(ast::t_node* a_node)
	{
		v_code->f_at(a_node->v_at);
	}
};

}

#endif
