#ifndef XEMMAI__AST_H
#define XEMMAI__AST_H

#include "code.h"
#include <set>

namespace xemmai
{

struct t_emit;

namespace ast XEMMAI__LOCAL
{

struct t_arena
{
	struct t_of
	{
		unsigned char* v_p = nullptr;
		unsigned char* v_q = nullptr;
		std::deque<t_root> v_blocks;

		void* f_allocate(size_t a_n);
	};

	t_of v_alignments[std::countr_zero(std::max(alignof(void*), alignof(double))) + 1];

	template<typename T>
	T* f_allocate(size_t a_n)
	{
		static_assert(alignof(T) <= std::max(alignof(void*), alignof(double)));
		return static_cast<T*>(v_alignments[std::countr_zero(alignof(T))].f_allocate(a_n * sizeof(T)));
	}
	template<typename T>
	T* f_new(auto&&... a_xs)
	{
		return new(f_allocate<T>(1)) T(std::forward<decltype(a_xs)>(a_xs)...);
	}
};

template<typename T>
struct t_allocator
{
	using value_type = T;

	t_arena& v_arena;

	t_allocator(t_arena& a_arena) : v_arena(a_arena)
	{
	}
	template<typename U>
	t_allocator(const t_allocator<U>& a_other) : v_arena(a_other.v_arena)
	{
	}
	[[nodiscard]] T* allocate(size_t a_n)
	{
		return v_arena.f_allocate<T>(a_n);
	}
	void deallocate(T*, size_t)
	{
	}
};

template<typename T, typename U>
constexpr bool operator==(const t_allocator<T>& a_x, const t_allocator<U>& a_y)
{
	return &a_x.v_arena == &a_y.v_arena;
}

struct t_block
{
	struct t_private
	{
		bool v_in = false;
		bool v_out = false;
		bool v_define = false;
		bool v_use = false;
	};

	t_block* v_queue = nullptr;
	bool v_forward = false;
	bool v_backward = false;
	std::vector<t_private, t_allocator<t_private>> v_privates;
	std::vector<t_block*, t_allocator<t_block*>> v_previouses;
	std::vector<t_block*, t_allocator<t_block*>> v_nexts;

	t_block(t_arena& a_arena) : v_privates(t_allocator<t_private>(a_arena)), v_previouses(t_allocator<t_block*>(a_arena)), v_nexts(t_allocator<t_block*>(a_arena))
	{
	}
	void f_use(size_t a_i, bool a_put)
	{
		if (v_privates.size() < a_i + 1) v_privates.resize(a_i + 1);
		auto& x = v_privates[a_i];
		if (x.v_out) return;
		x.v_out = true;
		x.v_define = a_put;
		x.v_use = !a_put;
	}
	void f_next(t_block* a_next)
	{
		a_next->v_previouses.push_back(this);
		v_nexts.push_back(a_next);
	}
	bool f_forward(const std::vector<t_private, t_allocator<t_private>>& a_privates);
	bool f_backward(const std::vector<t_private, t_allocator<t_private>>& a_privates);
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
};

struct t_operand
{
	enum t_tag
	{
		c_tag__INTEGER,
		c_tag__FLOAT,
		c_tag__LITERAL,
		c_tag__VARIABLE,
		c_tag__TEMPORARY
	};

	t_tag v_tag;
	union
	{
		intptr_t v_integer;
		double v_float;
		t_object* v_value;
		size_t v_index;
	};

	t_operand(bool a_value) : v_tag(c_tag__LITERAL), v_value(reinterpret_cast<t_object*>(a_value ? c_tag__TRUE : c_tag__FALSE))
	{
	}
	t_operand(intptr_t a_value) : v_tag(c_tag__INTEGER), v_integer(a_value)
	{
	}
	t_operand(double a_value) : v_tag(c_tag__FLOAT), v_float(a_value)
	{
	}
	t_operand(t_object* a_value) : v_tag(c_tag__LITERAL), v_value(a_value)
	{
	}
	t_operand() : v_tag(c_tag__TEMPORARY)
	{
	}
	t_operand(t_tag a_tag, size_t a_value) : v_tag(a_tag), v_index(a_value)
	{
	}
};

struct t_node
{
	t_node* v_next;

	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear = false) = 0;
};

template<typename T>
struct t_list
{
	T* v_tail = nullptr;

	void f_push(T* a_p)
	{
		if (v_tail) {
			a_p->v_next = v_tail->v_next;
			v_tail->v_next = a_p;
		} else {
			a_p->v_next = a_p;
		}
		v_tail = a_p;
	}
	void f_each(auto a_do)
	{
		if (!v_tail) return;
		auto p = v_tail;
		do {
			p = p->v_next;
			a_do(p);
		} while (p != v_tail);
	}
};

template<typename T>
struct t_list_with_size : t_list<T>
{
	size_t v_size = 0;

	void f_push(T* a_p)
	{
		t_list<T>::f_push(a_p);
		++v_size;
	}
};

struct t_nodes : t_node, t_list_with_size<t_node>
{
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

class t_node_at : public t_node
{
	friend struct xemmai::t_emit;

protected:
	const t_at v_at;

public:
	t_node_at(const t_at& a_at) : v_at(a_at)
	{
	}
};

struct t_scope
{
	struct t_variable : t_code::t_variable
	{
		t_variable* v_next;
	};

	t_scope* v_outer;
	t_node* v_body;
	bool v_shared = false;
	bool v_self_shared = false;
	std::set<t_object*, std::less<t_object*>, t_allocator<t_object*>> v_references;
	std::set<t_object*, std::less<t_object*>, t_allocator<t_object*>> v_unresolveds;
	std::map<t_object*, t_variable, std::less<t_object*>, t_allocator<std::pair<t_object* const, t_variable>>> v_variables;
	t_list_with_size<t_variable> v_privates;
	size_t v_shareds = 0;
	t_block v_block_body;
	t_block v_junction;

	t_scope(t_arena& a_arena, t_scope* a_outer) : v_outer(a_outer), v_references(t_allocator<t_object*>(a_arena)), v_unresolveds(t_allocator<t_object*>(a_arena)), v_variables(t_allocator<std::pair<t_object* const, t_variable>>(a_arena)), v_block_body(a_arena), v_junction(a_arena)
	{
	}
	void f_vary(t_variable* a_p);
	void f_classify(t_variable* a_p);
	void f_analyze(size_t a_arguments);
};

struct t_lambda : t_node_at, t_scope
{
	bool v_variadic = false;
	size_t v_arguments = 0;
	t_nodes v_defaults;

	t_object* f_code(t_object* a_module);
	void f_safe_points(t_code& a_code, std::map<std::pair<size_t, void**>, size_t>& a_safe_points, const std::vector<std::tuple<size_t, size_t, size_t>>& a_safe_positions);

	t_lambda(t_arena& a_arena, const t_at& a_at, t_scope* a_outer) : t_node_at(a_at), t_scope(a_arena, a_outer)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_if : t_node
{
	t_node* v_condition;
	t_node* v_true;
	t_node* v_false;
	t_block v_block_true;
	t_block v_block_false;
	t_block v_junction;
	t_block v_block_exit;

	t_if(t_arena& a_arena, t_node* a_condition) : v_condition(a_condition), v_block_true(a_arena), v_block_false(a_arena), v_junction(a_arena), v_block_exit(a_arena)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_while : t_node
{
	t_node* v_condition;
	t_node* v_body;
	t_block v_junction_condition;
	t_block v_block_condition;
	t_block v_block_body;
	t_block v_junction_exit;
	t_block v_block_exit;

	t_while(t_arena& a_arena, t_node* a_condition) : v_condition(a_condition), v_junction_condition(a_arena), v_block_condition(a_arena), v_block_body(a_arena), v_junction_exit(a_arena), v_block_exit(a_arena)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_for : t_node
{
	t_node* v_initialization;
	t_node* v_condition;
	t_node* v_next;
	t_node* v_body;
	t_block v_junction_condition;
	t_block v_block_condition;
	t_block v_block_body;
	t_block v_junction_next;
	t_block v_block_next;
	t_block v_junction_exit;
	t_block v_block_exit;

	t_for(t_arena& a_arena) : v_junction_condition(a_arena), v_block_condition(a_arena), v_block_body(a_arena), v_junction_next(a_arena), v_block_next(a_arena), v_junction_exit(a_arena), v_block_exit(a_arena)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_break : t_node
{
	t_node* v_expression;

	t_break(t_node* a_expression) : v_expression(a_expression)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_continue : t_node_at
{
	using t_node_at::t_node_at;
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_return : t_node
{
	t_node* v_expression;

	t_return(t_node* a_expression) : v_expression(a_expression)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_try : t_node
{
	struct t_catch
	{
		t_catch* v_next;
		t_node* v_expression;
		const t_code::t_variable& v_variable;
		t_node* v_body;
		t_block v_block_expression;
		t_block v_block_body;

		t_catch(t_arena& a_arena, t_node* a_expression, const t_code::t_variable& a_variable) : v_expression(a_expression), v_variable(a_variable), v_block_expression(a_arena), v_block_body(a_arena)
		{
		}
	};

	t_node* v_body;
	t_list<t_catch> v_catches;
	t_node* v_finally;
	t_block v_junction_try;
	t_block v_block_try;
	t_block v_junction_finally;
	t_block v_block_finally;
	t_block v_block_exit;

	t_try(t_arena& a_arena) : v_junction_try(a_arena), v_block_try(a_arena), v_junction_finally(a_arena), v_block_finally(a_arena), v_block_exit(a_arena)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_throw : t_node_at
{
	t_node* v_expression;

	t_throw(const t_at& a_at, t_node* a_expression) : t_node_at(a_at), v_expression(a_expression)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_object_get : t_node_at
{
	t_node* v_target;
	t_object* v_key;

	t_object_get(const t_at& a_at, t_node* a_target, t_object* a_key) : t_node_at(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
	void f_method(t_emit& a_emit);
};

struct t_object_get_indirect : t_node_at
{
	t_node* v_target;
	t_node* v_key;

	t_object_get_indirect(const t_at& a_at, t_node* a_target, t_node* a_key) : t_node_at(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_object_put : t_node_at
{
	t_node* v_target;
	t_object* v_key;
	t_node* v_value;

	t_object_put(const t_at& a_at, t_node* a_target, t_object* a_key, t_node* a_value) : t_node_at(a_at), v_target(a_target), v_key(a_key), v_value(a_value)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_object_put_indirect : t_node_at
{
	t_node* v_target;
	t_node* v_key;
	t_node* v_value;

	t_object_put_indirect(const t_at& a_at, t_node* a_target, t_node* a_key, t_node* a_value) : t_node_at(a_at), v_target(a_target), v_key(a_key), v_value(a_value)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_object_has : t_node_at
{
	t_node* v_target;
	t_object* v_key;

	t_object_has(const t_at& a_at, t_node* a_target, t_object* a_key) : t_node_at(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_object_has_indirect : t_node_at
{
	t_node* v_target;
	t_node* v_key;

	t_object_has_indirect(const t_at& a_at, t_node* a_target, t_node* a_key) : t_node_at(a_at), v_target(a_target), v_key(a_key)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_symbol_get : t_node_at
{
	size_t v_outer;
	t_scope* v_scope;
	t_object* v_symbol;
	size_t v_resolved;
	const t_code::t_variable* v_variable = nullptr;

	t_symbol_get(const t_at& a_at, size_t a_outer, t_scope* a_scope, t_object* a_symbol) : t_node_at(a_at), v_outer(a_outer), v_scope(a_scope), v_symbol(a_symbol)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_scope_put : t_node_at
{
	size_t v_outer;
	const t_code::t_variable& v_variable;
	t_node* v_value;

	t_scope_put(const t_at& a_at, size_t a_outer, const t_code::t_variable& a_variable, t_node* a_value) : t_node_at(a_at), v_outer(a_outer), v_variable(a_variable), v_value(a_value)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_self : t_node_at
{
	size_t v_outer;

	t_self(const t_at& a_at, size_t a_outer) : t_node_at(a_at), v_outer(a_outer)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_class : t_node_at
{
	t_node* v_target;

	t_class(const t_at& a_at, t_node* a_target) : t_node_at(a_at), v_target(a_target)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_super : t_node_at
{
	t_node* v_target;

	t_super(const t_at& a_at, t_node* a_target) : t_node_at(a_at), v_target(a_target)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_null : t_node_at
{
	using t_node_at::t_node_at;
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

template<typename T>
struct t_literal : t_node_at
{
	T v_value;

	t_literal(const t_at& a_at, T a_value) : t_node_at(a_at), v_value(a_value)
	{
	}
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

template<>
struct t_literal<bool> : t_literal<t_object*>
{
	t_literal(const t_at& a_at, bool a_value) : t_literal<t_object*>(a_at, reinterpret_cast<t_object*>(a_value ? c_tag__TRUE : c_tag__FALSE))
	{
	}
};

struct t_unary : t_node_at
{
	t_instruction v_instruction;
	t_node* v_expression;

	t_unary(const t_at& a_at, t_instruction a_instruction, t_node* a_expression) : t_node_at(a_at), v_instruction(a_instruction), v_expression(a_expression)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_binary : t_node_at
{
	t_instruction v_instruction;
	t_node* v_left;
	t_node* v_right;

	t_binary(const t_at& a_at, t_instruction a_instruction, t_node* a_left, t_node* a_right) : t_node_at(a_at), v_instruction(a_instruction), v_left(a_left), v_right(a_right)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_preserve : t_node
{
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_call : t_node_at
{
	struct t_expand
	{
		t_expand* v_next;
		t_at v_at;
		size_t v_index;

		t_expand(const t_at& a_at, size_t a_index) : v_at(a_at), v_index(a_index)
		{
		}
	};

	t_node* v_target;
	t_nodes v_arguments;
	t_list<t_expand> v_expands;

	t_call(const t_at& a_at, t_node* a_target) : t_node_at(a_at), v_target(a_target)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

struct t_get_at : t_node_at
{
	t_node* v_target;
	t_node* v_index;

	t_get_at(const t_at& a_at, t_node* a_target, t_node* a_index) : t_node_at(a_at), v_target(a_target), v_index(a_index)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
	void f_bind(t_emit& a_emit);
};

struct t_set_at : t_node_at
{
	t_node* v_target;
	t_node* v_index;
	t_node* v_value;

	t_set_at(const t_at& a_at, t_node* a_target, t_node* a_index, t_node* a_value) : t_node_at(a_at), v_target(a_target), v_index(a_index), v_value(a_value)
	{
	}
	virtual void f_flow(t_flow& a_flow);
	virtual t_operand f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear);
};

}

struct XEMMAI__LOCAL t_emit
{
	class t_label : std::vector<size_t>
	{
		friend struct t_emit;

		size_t v_target;
	};
	struct t_targets
	{
		t_label* v_break;
		ast::t_block* v_break_junction;
		size_t v_break_stack;
		bool v_break_is_tail;
		bool v_break_must_clear;
		t_label* v_continue;
		ast::t_block* v_continue_junction;
		t_label* v_return;
		ast::t_block* v_return_junction;
		size_t v_return_stack;
		bool v_return_is_tail;
	};

	template<typename T>
	static constexpr t_instruction c_instruction_of = c_instruction__INSTANCE;

	t_object* v_module;
	std::map<std::pair<size_t, void**>, size_t>* v_safe_points;
	ast::t_scope* v_scope;
	t_code* v_code;
	size_t v_arguments;
	std::vector<bool>* v_privates;
	size_t v_stack;
	std::deque<t_label>* v_labels;
	t_targets* v_targets;
	std::vector<std::tuple<size_t, size_t, size_t>>* v_safe_positions;

	t_emit(t_object* a_module, std::map<std::pair<size_t, void**>, size_t>* a_safe_points) : v_module(a_module), v_safe_points(a_safe_points)
	{
	}
	t_object* operator()(ast::t_scope& a_scope);
	t_emit& operator<<(t_instruction a_instruction)
	{
		v_code->v_instructions.push_back(v_code->f_p(a_instruction));
		return *this;
	}
	size_t f_last() const
	{
		return v_code->v_instructions.size();
	}
	t_emit& f_push()
	{
		if (++v_stack > v_code->v_size) v_code->v_size = v_stack;
		return *this;
	}
	t_emit& f_pop()
	{
		--v_stack;
		return *this;
	}
	t_emit& operator<<(auto a_operand)
	{
		v_code->v_instructions.push_back(reinterpret_cast<void*>(a_operand));
		return *this;
	}
	t_emit& operator<<(bool a_operand)
	{
		return *this << (a_operand ? 1 : 0);
	}
	t_emit& operator<<(double a_operand)
	{
		union
		{
			double v0;
			void* v1[sizeof(double) / sizeof(void*)];
		};
		v0 = a_operand;
		for (size_t i = 0; i < sizeof(double) / sizeof(void*); ++i) v_code->v_instructions.push_back(v1[i]);
		return *this;
	}
	t_emit& operator<<(t_object* a_operand)
	{
		v_code->v_instructions.push_back(a_operand);
		return *this;
	}
	t_emit& operator<<(t_label& a_label)
	{
		a_label.push_back(f_last());
		return *this << size_t(0);
	}
	void f_target(t_label& a_label)
	{
		a_label.v_target = f_last();
	}
	t_label& f_label()
	{
		return v_labels->emplace_back();
	}
	void f_resolve()
	{
		for (const auto& label : *v_labels) {
			void* p = &v_code->v_instructions[label.v_target];
			for (auto i : label) v_code->v_instructions[i] = p;
		}
	}
	void f_at(const t_at& a_at)
	{
		v_code->v_ats.emplace_back(a_at, f_last());
	}
	void f_at(ast::t_node_at* a_node)
	{
		f_at(a_node->v_at);
	}
	t_emit& f_emit_null()
	{
		return (*this << c_instruction__NUL << v_stack).f_push();
	}
	void f_emit_safe_point(ast::t_node_at* a_node)
	{
		if (!v_safe_points) return;
		v_safe_positions->emplace_back(a_node->v_at.v_line, f_last(), a_node->v_at.v_column);
		*this << c_instruction__SAFE_POINT;
		f_at(a_node);
	}
	void f_join(const ast::t_block& a_junction);
	void f_merge(const ast::t_block& a_junction)
	{
		for (size_t i = 0; i < a_junction.v_privates.size(); ++i) (*v_privates)[i] = a_junction.v_privates[i].v_out;
	}
};

template<>
constexpr t_instruction t_emit::c_instruction_of<intptr_t> = c_instruction__INTEGER;
template<>
constexpr t_instruction t_emit::c_instruction_of<double> = c_instruction__FLOAT;

namespace ast
{

template<typename T>
t_operand t_literal<T>::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return {};
	if (a_tail) {
		a_emit.f_emit_safe_point(this);
		a_emit << static_cast<t_instruction>(t_emit::c_instruction_of<T> + c_instruction__RETURN_NUL - c_instruction__NUL) << v_value;
	} else {
		a_emit << t_emit::c_instruction_of<T> << a_emit.v_stack << v_value;
	}
	a_emit.f_push();
	a_emit.f_at(this);
	return {};
}

}

}

#endif
