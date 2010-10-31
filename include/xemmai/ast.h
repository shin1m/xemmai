#ifndef XEMMAI__SYNTAX_H
#define XEMMAI__SYNTAX_H

#include <map>

#include "code.h"

namespace xemmai
{

class t_generator;

namespace ast
{

template<typename U>
class t_pointers;

template<typename T>
class t_pointer
{
	template<typename U>
	friend class t_pointer;
	template<typename U>
	friend class t_pointers;

	mutable T* v_p;

	T* f_release() const
	{
		T* p = v_p;
		v_p = 0;
		return p;
	}

public:
	t_pointer(T* a_p = 0) : v_p(a_p)
	{
	}
	t_pointer(const t_pointer& a_p) : v_p(a_p.f_release())
	{
	}
	template<typename U>
	t_pointer(const t_pointer<U>& a_p) : v_p(a_p.f_release())
	{
	}
	~t_pointer()
	{
		delete v_p;
	}
	t_pointer& operator=(const t_pointer& a_p)
	{
		delete v_p;
		v_p = a_p.f_release();
		return *this;
	}
	template<typename U>
	t_pointer& operator=(const t_pointer<U>& a_p)
	{
		delete v_p;
		v_p = a_p.f_release();
		return *this;
	}
	operator T*() const
	{
		return v_p;
	}
	T* operator->() const
	{
		return v_p;
	}
};

template<typename T>
class t_pointers
{
	std::vector<T*> v_ps;

public:
	typedef typename std::vector<T*>::const_iterator t_iterator;

	~t_pointers()
	{
		for (t_iterator i = f_begin(); i != f_end(); ++i) delete *i;
	}
	size_t f_size() const
	{
		return v_ps.size();
	}
	t_iterator f_begin() const
	{
		return v_ps.begin();
	}
	t_iterator f_end() const
	{
		return v_ps.end();
	}
	void f_add(T* a_p)
	{
		v_ps.push_back(a_p);
	}
	template<typename U>
	void f_add(const t_pointer<U>& a_p)
	{
		v_ps.push_back(a_p.f_release());
	}
};

class t_node
{
	friend class xemmai::t_generator;

	t_at v_at;

public:
	t_node(const t_at& a_at) : v_at(a_at)
	{
	}
	virtual ~t_node();
	virtual void f_generate(t_generator& a_generator, bool a_tail) = 0;
};

void f_generate_block(t_generator& a_generator, const t_pointers<t_node>& a_nodes, bool a_tail);
void f_generate_block_without_value(t_generator& a_generator, const t_pointers<t_node>& a_nodes);

struct t_variable
{
	bool v_shared;
	bool v_varies;
	size_t v_index;

	t_variable() : v_shared(false), v_varies(false)
	{
	}
};

struct t_scope
{
	t_scope* v_outer;
	t_pointers<t_node> v_block;
	bool v_shared;
	bool v_self_shared;
	std::map<t_scoped, t_variable> v_variables;
	std::vector<t_variable*> v_privates;
	size_t v_shareds;

	t_scope(t_scope* a_outer) : v_outer(a_outer), v_shared(false), v_self_shared(false), v_shareds(0)
	{
	}
};

struct t_lambda : t_node, t_scope
{
	size_t v_arguments;

	t_lambda(const t_at& a_at, t_scope* a_outer) : t_node(a_at), t_scope(a_outer), v_arguments(0)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_if : t_node
{
	t_pointer<t_node> v_expression;
	t_pointers<t_node> v_true;
	t_pointers<t_node> v_false;

	t_if(const t_at& a_at, const t_pointer<t_node>& a_expression) : t_node(a_at), v_expression(a_expression)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_while : t_node
{
	t_pointer<t_node> v_expression;
	t_pointers<t_node> v_block;

	t_while(const t_at& a_at, const t_pointer<t_node>& a_expression) : t_node(a_at), v_expression(a_expression)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_break : t_node
{
	t_pointer<t_node> v_expression;

	t_break(const t_at& a_at, const t_pointer<t_node>& a_expression) : t_node(a_at), v_expression(a_expression)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_continue : t_node
{
	t_continue(const t_at& a_at) : t_node(a_at)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_return : t_node
{
	t_pointer<t_node> v_expression;

	t_return(const t_at& a_at, const t_pointer<t_node>& a_expression) : t_node(a_at), v_expression(a_expression)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_try : t_node
{
	struct t_catch
	{
		t_pointer<t_node> v_expression;
		const t_variable& v_variable;
		t_pointers<t_node> v_block;

		t_catch(const t_pointer<t_node>& a_expression, const t_variable& a_variable) : v_expression(a_expression), v_variable(a_variable)
		{
		}
	};

	t_pointers<t_node> v_block;
	t_pointers<t_catch> v_catches;
	t_pointers<t_node> v_finally;

	t_try(const t_at& a_at) : t_node(a_at)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_throw : t_node
{
	t_pointer<t_node> v_expression;

	t_throw(const t_at& a_at, const t_pointer<t_node>& a_expression) : t_node(a_at), v_expression(a_expression)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_get : t_node
{
	t_pointer<t_node> v_target;
	t_scoped v_key;

	t_object_get(const t_at& a_at, const t_pointer<t_node>& a_target, const t_transfer& a_key) : t_node(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_get_indirect : t_node
{
	t_pointer<t_node> v_target;
	t_pointer<t_node> v_key;

	t_object_get_indirect(const t_at& a_at, const t_pointer<t_node>& a_target, const t_pointer<t_node>& a_key) : t_node(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_put : t_node
{
	t_pointer<t_node> v_target;
	t_scoped v_key;
	t_pointer<t_node> v_value;

	t_object_put(const t_at& a_at, const t_pointer<t_node>& a_target, const t_transfer& a_key, const t_pointer<t_node>& a_value) : t_node(a_at), v_target(a_target), v_key(a_key), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_put_indirect : t_node
{
	t_pointer<t_node> v_target;
	t_pointer<t_node> v_key;
	t_pointer<t_node> v_value;

	t_object_put_indirect(const t_at& a_at, const t_pointer<t_node>& a_target, const t_pointer<t_node>& a_key, const t_pointer<t_node>& a_value) : t_node(a_at), v_target(a_target), v_key(a_key), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_has : t_node
{
	t_pointer<t_node> v_target;
	t_scoped v_key;

	t_object_has(const t_at& a_at, const t_pointer<t_node>& a_target, const t_transfer& a_key) : t_node(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_has_indirect : t_node
{
	t_pointer<t_node> v_target;
	t_pointer<t_node> v_key;

	t_object_has_indirect(const t_at& a_at, const t_pointer<t_node>& a_target, const t_pointer<t_node>& a_key) : t_node(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_remove : t_node
{
	t_pointer<t_node> v_target;
	t_scoped v_key;

	t_object_remove(const t_at& a_at, const t_pointer<t_node>& a_target, const t_transfer& a_key) : t_node(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_object_remove_indirect : t_node
{
	t_pointer<t_node> v_target;
	t_pointer<t_node> v_key;

	t_object_remove_indirect(const t_at& a_at, const t_pointer<t_node>& a_target, const t_pointer<t_node>& a_key) : t_node(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_global_get : t_node
{
	t_scoped v_key;

	t_global_get(const t_at& a_at, const t_transfer& a_key) : t_node(a_at), v_key(a_key)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_scope_get : t_node
{
	size_t v_outer;
	const t_variable& v_variable;

	t_scope_get(const t_at& a_at, size_t a_outer, const t_variable& a_variable) : t_node(a_at), v_outer(a_outer), v_variable(a_variable)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_scope_put : t_node
{
	size_t v_outer;
	const t_variable& v_variable;
	t_pointer<t_node> v_value;

	t_scope_put(const t_at& a_at, size_t a_outer, const t_variable& a_variable, const t_pointer<t_node>& a_value) : t_node(a_at), v_outer(a_outer), v_variable(a_variable), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_self : t_node
{
	size_t v_outer;

	t_self(const t_at& a_at, size_t a_outer) : t_node(a_at), v_outer(a_outer)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_class : t_node
{
	t_pointer<t_node> v_target;

	t_class(const t_at& a_at, const t_pointer<t_node>& a_target) : t_node(a_at), v_target(a_target)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_super : t_node
{
	t_pointer<t_node> v_target;

	t_super(const t_at& a_at, const t_pointer<t_node>& a_target) : t_node(a_at), v_target(a_target)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_null : t_node
{
	t_null(const t_at& a_at) : t_node(a_at)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_boolean : t_node
{
	bool v_value;

	t_boolean(const t_at& a_at, bool a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_integer : t_node
{
	int v_value;

	t_integer(const t_at& a_at, int a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_float : t_node
{
	double v_value;

	t_float(const t_at& a_at, double a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_instance : t_node
{
	t_scoped v_value;

	t_instance(const t_at& a_at, const t_transfer& a_value) : t_node(a_at), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_unary : t_node
{
	t_instruction v_instruction;
	t_pointer<t_node> v_expression;

	t_unary(const t_at& a_at, t_instruction a_instruction, const t_pointer<t_node>& a_expression) : t_node(a_at), v_instruction(a_instruction), v_expression(a_expression)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_binary : t_node
{
	t_instruction v_instruction;
	t_pointer<t_node> v_left;
	t_pointer<t_node> v_right;

	t_binary(const t_at& a_at, t_instruction a_instruction, const t_pointer<t_node>& a_left, const t_pointer<t_node>& a_right) : t_node(a_at), v_instruction(a_instruction), v_left(a_left), v_right(a_right)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_call : t_node
{
	t_pointer<t_node> v_target;
	t_pointers<t_node> v_arguments;

	t_call(const t_at& a_at, const t_pointer<t_node>& a_target) : t_node(a_at), v_target(a_target)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_get_at : t_node
{
	t_pointer<t_node> v_target;
	t_pointer<t_node> v_index;

	t_get_at(const t_at& a_at, const t_pointer<t_node>& a_target, const t_pointer<t_node>& a_index) : t_node(a_at), v_target(a_target), v_index(a_index)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_set_at : t_node
{
	t_pointer<t_node> v_target;
	t_pointer<t_node> v_index;
	t_pointer<t_node> v_value;

	t_set_at(const t_at& a_at, const t_pointer<t_node>& a_target, const t_pointer<t_node>& a_index, const t_pointer<t_node>& a_value) : t_node(a_at), v_target(a_target), v_index(a_index), v_value(a_value)
	{
	}
	virtual void f_generate(t_generator& a_generator, bool a_tail);
};

struct t_module : t_scope
{
	std::wstring v_path;

	t_module(const std::wstring& a_path) : t_scope(0), v_path(a_path)
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
		std::vector<size_t>* v_break;
		bool v_break_is_tail;
		std::vector<size_t>* v_continue;
		std::vector<size_t>* v_return;
		bool v_return_is_tail;

		t_targets(std::vector<size_t>* a_break, bool a_break_is_tail, std::vector<size_t>* a_continue, std::vector<size_t>* a_return, bool a_return_is_tail) :
		v_break(a_break), v_break_is_tail(a_break_is_tail),
		v_continue(a_continue),
		v_return(a_return), v_return_is_tail(a_return_is_tail)
		{
		}
	};

	std::wstring v_path;
	ast::t_scope* v_scope;
	t_code* v_code;
	t_targets* v_targets;

	t_transfer f_generate(ast::t_module& a_module);
	void f_emit(t_instruction a_instruction)
	{
		v_code->f_emit(a_instruction);
	}
	void f_operand(size_t a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(int* a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(bool a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(int a_operand)
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
	void f_operand(const t_value& a_operand)
	{
		v_code->f_operand(t_transfer(a_operand));
	}
	void f_operand(const t_transfer& a_operand)
	{
		v_code->f_operand(a_operand);
	}
	void f_operand(std::vector<size_t>& a_label)
	{
		v_code->f_operand(a_label);
	}
	void f_resolve(const std::vector<size_t>& a_label, size_t a_n)
	{
		v_code->f_resolve(a_label, a_n);
	}
	void f_resolve(const std::vector<size_t>& a_label)
	{
		v_code->f_resolve(a_label);
	}
	void f_at(ast::t_node* a_node)
	{
		v_code->f_at(a_node->v_at);
	}
};

}

#endif
