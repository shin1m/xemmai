#ifndef XEMMAI__AST_H
#define XEMMAI__AST_H

#include <deque>
#include <set>

#include "code.h"

namespace xemmai
{

struct t_emit;
#ifdef XEMMAI_ENABLE_JIT
struct t_jit_emit;
#endif

namespace ast
{

struct t_block
{
	t_block* v_queue = nullptr;
	std::vector<size_t> v_defines;
	std::vector<bool> v_uses;
	std::vector<t_block*> v_nexts;

	bool f_merge(const std::vector<bool>& a_uses);
};

struct t_flow
{
	struct t_targets
	{
		t_block* v_break;
		t_block* v_continue;
		t_block* v_return;
	};

	size_t v_arguments;
	t_targets* v_targets;
	t_block* v_current;
	t_block* v_queue = nullptr;

	void f_queue(t_block* a_block);
	void operator()(t_block* a_block);
};

struct t_operand
{
	enum t_tag
	{
		e_tag__INTEGER,
		e_tag__FLOAT,
		e_tag__LITERAL,
		e_tag__VARIABLE,
		e_tag__TEMPORARY
	};

	t_tag v_tag;
	union
	{
		intptr_t v_integer;
		double v_float;
		const t_value& v_value;
		size_t v_index;
	};

	t_operand(intptr_t a_value) : v_tag(e_tag__INTEGER), v_integer(a_value)
	{
	}
	t_operand(double a_value) : v_tag(e_tag__FLOAT), v_float(a_value)
	{
	}
	t_operand(const t_value& a_value) : v_tag(e_tag__LITERAL), v_value(a_value)
	{
	}
	t_operand() : v_tag(e_tag__TEMPORARY)
	{
	}
	t_operand(t_tag a_tag, size_t a_value) : v_tag(a_tag), v_index(a_value)
	{
	}
};

class t_node
{
	friend struct xemmai::t_emit;

protected:
	t_at v_at;

public:
	t_node(const t_at& a_at) : v_at(a_at)
	{
	}
	virtual ~t_node() = default;
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear = false) = 0;
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear = false) = 0;
#endif
};

struct t_scope
{
	t_scope* v_outer;
	std::vector<std::unique_ptr<t_node>> v_block;
	bool v_shared = false;
	bool v_self_shared = false;
	std::set<t_object*> v_references;
	std::set<t_object*> v_unresolveds;
	std::map<t_object*, t_code::t_variable> v_variables;
	std::vector<t_code::t_variable*> v_privates;
	size_t v_shareds = 0;
	t_block v_block_block;
	t_block v_junction;

	t_scope(t_scope* a_outer) : v_outer(a_outer)
	{
	}
};

struct t_lambda : t_node, t_scope
{
	bool v_variadic = false;
	size_t v_arguments = 0;
	std::vector<std::unique_ptr<t_node>> v_defaults;

	t_object* f_code(t_object* a_module);
	void f_safe_points(t_code& a_code, std::map<std::pair<size_t, void**>, size_t>& a_safe_points, const std::vector<std::tuple<size_t, size_t, size_t>>& a_safe_positions);

	t_lambda(const t_at& a_at, t_scope* a_outer) : t_node(a_at), t_scope(a_outer)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	void f_jit_emit(t_emit& a_emit, t_code& a_code);
	void f_jit_emit_with_lock(t_emit& a_emit, t_code& a_code);
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_if : t_node
{
	std::unique_ptr<t_node> v_condition;
	std::vector<std::unique_ptr<t_node>> v_true;
	std::vector<std::unique_ptr<t_node>> v_false;
	t_block v_block_true;
	t_block v_block_false;
	t_block v_junction;
	t_block v_block_exit;

	t_if(const t_at& a_at, std::unique_ptr<t_node>&& a_condition) : t_node(a_at), v_condition(std::move(a_condition))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_while : t_node
{
	std::unique_ptr<t_node> v_condition;
	std::vector<std::unique_ptr<t_node>> v_block;
	t_block v_junction_condition;
	t_block v_block_condition;
	t_block v_block_block;
	t_block v_junction_exit;
	t_block v_block_exit;

	t_while(const t_at& a_at, std::unique_ptr<t_node>&& a_condition) : t_node(a_at), v_condition(std::move(a_condition))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_for : t_node
{
	std::vector<std::unique_ptr<t_node>> v_initialization;
	std::unique_ptr<t_node> v_condition;
	std::vector<std::unique_ptr<t_node>> v_next;
	std::vector<std::unique_ptr<t_node>> v_block;
	t_block v_junction_condition;
	t_block v_block_condition;
	t_block v_block_block;
	t_block v_junction_next;
	t_block v_block_next;
	t_block v_junction_exit;
	t_block v_block_exit;

	using t_node::t_node;
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_break : t_node
{
	std::unique_ptr<t_node> v_expression;

	t_break(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_continue : t_node
{
	using t_node::t_node;
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_return : t_node
{
	std::unique_ptr<t_node> v_expression;

	t_return(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_try : t_node
{
	struct t_catch
	{
		std::unique_ptr<t_node> v_expression;
		const t_code::t_variable& v_variable;
		std::vector<std::unique_ptr<t_node>> v_block;
		t_block v_block_expression;
		t_block v_block_block;

		t_catch(std::unique_ptr<t_node>&& a_expression, const t_code::t_variable& a_variable) : v_expression(std::move(a_expression)), v_variable(a_variable)
		{
		}
	};

	std::vector<std::unique_ptr<t_node>> v_block;
	std::vector<std::unique_ptr<t_catch>> v_catches;
	std::vector<std::unique_ptr<t_node>> v_finally;
	t_block v_junction_try;
	t_block v_block_try;
	t_block v_junction_finally;
	t_block v_block_finally;
	t_block v_block_exit;

	using t_node::t_node;
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	void* f_jit_try(t_jit_emit& a_emit, bool a_clear);
	void* f_jit_catch(t_jit_emit& a_emit, bool a_clear);
	void* f_jit_finally(t_jit_emit& a_emit);
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_throw : t_node
{
	std::unique_ptr<t_node> v_expression;

	t_throw(const t_at& a_at, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_expression(std::move(a_expression))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_get : t_node
{
	std::unique_ptr<t_node> v_target;
	t_object* v_key;

	t_object_get(const t_at& a_at, std::unique_ptr<t_node>&& a_target, t_object* a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
	void f_method(t_emit& a_emit);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
	void f_method(t_jit_emit& a_emit);
#endif
};

struct t_object_get_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;

	t_object_get_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_put : t_node
{
	std::unique_ptr<t_node> v_target;
	t_object* v_key;
	std::unique_ptr<t_node> v_value;

	t_object_put(const t_at& a_at, std::unique_ptr<t_node>&& a_target, t_object* a_key, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key), v_value(std::move(a_value))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_put_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;
	std::unique_ptr<t_node> v_value;

	t_object_put_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key)), v_value(std::move(a_value))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_has : t_node
{
	std::unique_ptr<t_node> v_target;
	t_object* v_key;

	t_object_has(const t_at& a_at, std::unique_ptr<t_node>&& a_target, t_object* a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_has_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;

	t_object_has_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_remove : t_node
{
	std::unique_ptr<t_node> v_target;
	t_object* v_key;

	t_object_remove(const t_at& a_at, std::unique_ptr<t_node>&& a_target, t_object* a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_object_remove_indirect : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_key;

	t_object_remove_indirect(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_key) : t_node(a_at), v_target(std::move(a_target)), v_key(std::move(a_key))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_symbol_get : t_node
{
	size_t v_outer;
	t_scope* v_scope;
	t_object* v_symbol;
	size_t v_resolved = -1;
	const t_code::t_variable* v_variable = nullptr;

	t_symbol_get(const t_at& a_at, size_t a_outer, t_scope* a_scope, t_object* a_symbol) : t_node(a_at), v_outer(a_outer), v_scope(a_scope), v_symbol(a_symbol)
	{
	}
	void f_resolve();
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_scope_put : t_node
{
	size_t v_outer;
	const t_code::t_variable& v_variable;
	std::unique_ptr<t_node> v_value;

	t_scope_put(const t_at& a_at, size_t a_outer, const t_code::t_variable& a_variable, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_outer(a_outer), v_variable(a_variable), v_value(std::move(a_value))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_self : t_node
{
	size_t v_outer;

	t_self(const t_at& a_at, size_t a_outer) : t_node(a_at), v_outer(a_outer)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_class : t_node
{
	std::unique_ptr<t_node> v_target;

	t_class(const t_at& a_at, std::unique_ptr<t_node>&& a_target) : t_node(a_at), v_target(std::move(a_target))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_super : t_node
{
	std::unique_ptr<t_node> v_target;

	t_super(const t_at& a_at, std::unique_ptr<t_node>&& a_target) : t_node(a_at), v_target(std::move(a_target))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_null : t_node
{
	using t_node::t_node;
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_boolean : t_node
{
	bool v_value;

	t_boolean(const t_at& a_at, bool a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_integer : t_node
{
	intptr_t v_value;

	t_integer(const t_at& a_at, intptr_t a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_float : t_node
{
	double v_value;

	t_float(const t_at& a_at, double a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_instance : t_node
{
	const t_value& v_value;

	t_instance(const t_at& a_at, const t_value& a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_unary : t_node
{
	t_instruction v_instruction;
	std::unique_ptr<t_node> v_expression;

	t_unary(const t_at& a_at, t_instruction a_instruction, std::unique_ptr<t_node>&& a_expression) : t_node(a_at), v_instruction(a_instruction), v_expression(std::move(a_expression))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_binary : t_node
{
	t_instruction v_instruction;
	std::unique_ptr<t_node> v_left;
	std::unique_ptr<t_node> v_right;

	t_binary(const t_at& a_at, t_instruction a_instruction, std::unique_ptr<t_node>&& a_left, std::unique_ptr<t_node>&& a_right) : t_node(a_at), v_instruction(a_instruction), v_left(std::move(a_left)), v_right(std::move(a_right))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_call : t_node
{
	std::unique_ptr<t_node> v_target;
	std::vector<std::unique_ptr<t_node>> v_arguments;
	bool v_expand;

	t_call(const t_at& a_at, std::unique_ptr<t_node>&& a_target) : t_node(a_at), v_target(std::move(a_target)), v_expand(false)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

struct t_get_at : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_index;

	t_get_at(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_index) : t_node(a_at), v_target(std::move(a_target)), v_index(std::move(a_index))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
	void f_bind(t_emit& a_emit);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
	void f_bind(t_jit_emit& a_emit);
#endif
};

struct t_set_at : t_node
{
	std::unique_ptr<t_node> v_target;
	std::unique_ptr<t_node> v_index;
	std::unique_ptr<t_node> v_value;

	t_set_at(const t_at& a_at, std::unique_ptr<t_node>&& a_target, std::unique_ptr<t_node>&& a_index, std::unique_ptr<t_node>&& a_value) : t_node(a_at), v_target(std::move(a_target)), v_index(std::move(a_index)), v_value(std::move(a_value))
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#ifdef XEMMAI_ENABLE_JIT
	virtual t_operand f_emit(t_jit_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
#endif
};

}

struct t_emit
{
	struct t_targets
	{
		t_code::t_label* v_break;
		ast::t_block* v_break_junction;
		bool v_break_is_tail;
		bool v_break_must_clear;
		t_code::t_label* v_continue;
		ast::t_block* v_continue_junction;
		t_code::t_label* v_return;
		ast::t_block* v_return_junction;
		bool v_return_is_tail;
	};

	t_object* v_module;
	std::map<std::pair<size_t, void**>, size_t>* v_safe_points;
	ast::t_scope* v_scope;
	t_code* v_code;
	size_t v_arguments;
	std::vector<bool>* v_stack;
	std::deque<t_code::t_label>* v_labels;
	t_targets* v_targets;
	std::vector<std::tuple<size_t, size_t, size_t>>* v_safe_positions;
#ifdef XEMMAI_ENABLE_JIT
	void* v_jit;

	t_emit(t_object* a_module, std::map<std::pair<size_t, void**>, size_t>* a_safe_points);
	~t_emit();
	void f_jit_install();
#else

	t_emit(t_object* a_module, std::map<std::pair<size_t, void**>, size_t>* a_safe_points) : v_module(a_module), v_safe_points(a_safe_points)
	{
	}
#endif
	t_scoped operator()(ast::t_scope& a_scope);
	t_emit& operator<<(t_instruction a_instruction)
	{
		v_code->f_emit(a_instruction);
		return *this;
	}
	size_t f_stack() const
	{
		return v_arguments + v_stack->size();
	}
	t_emit& f_push(bool a_live)
	{
		v_stack->push_back(a_live);
		if (f_stack() > v_code->v_size) v_code->v_size = f_stack();
		return *this;
	}
	t_emit& f_pop()
	{
		v_stack->pop_back();
		return *this;
	}
	t_emit& f_stack_map(int a_offset = 0)
	{
		v_code->f_stack_map(a_offset, *v_stack);
		return *this;
	}
	template<typename T>
	t_emit& operator<<(T&& a_operand)
	{
		v_code->f_operand(std::forward<T>(a_operand));
		return *this;
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
	void f_emit_clear()
	{
		f_pop();
		*this << e_instruction__CLEAR << f_stack();
	}
	void f_emit_null()
	{
		(*this << e_instruction__NUL << f_stack()).f_push(false);
	}
	void f_emit_safe_point(ast::t_node* a_node)
	{
		if (!v_safe_points) return;
		v_safe_positions->push_back(std::make_tuple(a_node->v_at.f_line(), v_code->f_last(), a_node->v_at.f_column()));
		*this << e_instruction__SAFE_POINT;
		f_at(a_node);
	}
	void f_join(const ast::t_block& a_junction);
	void f_merge(const ast::t_block& a_junction)
	{
		std::copy(a_junction.v_uses.begin(), a_junction.v_uses.end(), v_stack->begin());
	}
};

}

#endif
