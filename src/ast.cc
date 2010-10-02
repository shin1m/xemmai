#include <xemmai/ast.h>

#include <xemmai/engine.h>
#include <xemmai/global.h>

namespace xemmai
{

namespace ast
{

t_node::~t_node()
{
}

void f_generate_block(t_generator& a_generator, const t_pointers<t_node>& a_nodes, bool a_tail)
{
	t_pointers<t_node>::t_iterator i = a_nodes.f_begin();
	t_pointers<t_node>::t_iterator j = a_nodes.f_end();
	if (i == j) {
		a_generator.f_emit(e_instruction__NULL);
	} else {
		for (--j; i != j; ++i) {
			(*i)->f_generate(a_generator, false);
			a_generator.f_emit(e_instruction__POP);
		}
		(*i)->f_generate(a_generator, a_tail);
	}
}

void f_generate_block_without_value(t_generator& a_generator, const t_pointers<t_node>& a_nodes)
{
	for (t_pointers<t_node>::t_iterator i = a_nodes.f_begin(); i != a_nodes.f_end(); ++i) {
		(*i)->f_generate(a_generator, false);
		a_generator.f_emit(e_instruction__POP);
	}
}

void t_lambda::f_generate(t_generator& a_generator, bool a_tail)
{
	t_transfer code = t_code::f_instantiate(a_generator.v_path, v_variables.size(), v_arguments);
	t_scope* scope0 = a_generator.v_scope;
	scope0->v_shared = true;
	a_generator.v_scope = this;
	t_code* code0 = a_generator.v_code;
	a_generator.v_code = &f_as<t_code&>(code);
	t_generator::t_targets* targets0 = a_generator.v_targets;
	std::vector<size_t> return0;
	t_generator::t_targets targets1(0, false, 0, &return0, true);
	a_generator.v_targets = &targets1;
	f_generate_block(a_generator, v_block, true);
	a_generator.f_resolve(return0);
	a_generator.f_emit(e_instruction__RETURN);
	a_generator.v_code->f_estimate(!v_shared);
	a_generator.v_scope = scope0;
	a_generator.v_code = code0;
	a_generator.v_targets = targets0;
	a_generator.f_emit(e_instruction__LAMBDA);
	a_generator.f_operand(code);
}

void t_if::f_generate(t_generator& a_generator, bool a_tail)
{
	v_expression->f_generate(a_generator, false);
	std::vector<size_t> label0;
	std::vector<size_t> label1;
	a_generator.f_emit(e_instruction__BRANCH);
	a_generator.f_operand(label0);
	f_generate_block(a_generator, v_true, a_tail);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(label1);
	a_generator.f_resolve(label0);
	f_generate_block(a_generator, v_false, a_tail);
	a_generator.f_resolve(label1);
}

void t_while::f_generate(t_generator& a_generator, bool a_tail)
{
	size_t loop = a_generator.v_code->f_last();
	v_expression->f_generate(a_generator, false);
	std::vector<size_t> label0;
	a_generator.f_emit(e_instruction__BRANCH);
	a_generator.f_operand(label0);
	t_generator::t_targets* targets0 = a_generator.v_targets;
	std::vector<size_t> break0;
	std::vector<size_t> continue0;
	t_generator::t_targets targets1(&break0, a_tail, &continue0, targets0->v_return, targets0->v_return_is_tail);
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, v_block);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(loop);
	a_generator.f_resolve(label0);
	a_generator.f_emit(e_instruction__NULL);
	a_generator.f_resolve(break0);
	a_generator.f_resolve(continue0, loop);
	a_generator.v_targets = targets0;
}

void t_break::f_generate(t_generator& a_generator, bool a_tail)
{
	if (v_expression)
		v_expression->f_generate(a_generator, a_generator.v_targets->v_break_is_tail);
	else
		a_generator.f_emit(e_instruction__NULL);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(*a_generator.v_targets->v_break);
}

void t_continue::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(*a_generator.v_targets->v_continue);
}

void t_return::f_generate(t_generator& a_generator, bool a_tail)
{
	if (v_expression)
		v_expression->f_generate(a_generator, a_generator.v_targets->v_return_is_tail);
	else
		a_generator.f_emit(e_instruction__NULL);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(*a_generator.v_targets->v_return);
}

void t_try::f_generate(t_generator& a_generator, bool a_tail)
{
	std::vector<size_t> catch0;
	std::vector<size_t> finally0;
	a_generator.f_emit(e_instruction__TRY);
	a_generator.f_operand(catch0);
	a_generator.f_operand(finally0);
	t_generator::t_targets* targets0 = a_generator.v_targets;
	{
		std::vector<size_t> break0;
		std::vector<size_t> continue0;
		std::vector<size_t> return0;
		t_generator::t_targets targets1(targets0->v_break ? &break0 : 0, false, targets0->v_continue ? &continue0 : 0, targets0->v_return ? &return0 : 0, false);
		a_generator.v_targets = &targets1;
		f_generate_block(a_generator, v_block, false);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__STEP);
		a_generator.f_resolve(catch0);
		for (t_pointers<t_catch>::t_iterator i = v_catches.f_begin(); i != v_catches.f_end(); ++i) {
			(*i)->v_expression->f_generate(a_generator, false);
			std::vector<size_t> label0;
			a_generator.f_emit(e_instruction__CATCH);
			a_generator.f_operand(label0);
			a_generator.f_operand((*i)->v_variable.v_index);
			f_generate_block(a_generator, (*i)->v_block, false);
			a_generator.f_emit(e_instruction__FINALLY);
			a_generator.f_operand(t_fiber::t_try::e_state__STEP);
			a_generator.f_resolve(label0);
		}
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__THROW);
		a_generator.f_resolve(break0);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__BREAK);
		a_generator.f_resolve(continue0);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__CONTINUE);
		a_generator.f_resolve(return0);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__RETURN);
	}
	a_generator.f_resolve(finally0);
	{
		t_generator::t_targets targets2(0, false, 0, 0, false);
		a_generator.v_targets = &targets2;
		f_generate_block_without_value(a_generator, v_finally);
	}
	a_generator.f_emit(e_instruction__YRT);
	if (targets0->v_break)
		a_generator.f_operand(*targets0->v_break);
	else
		a_generator.f_operand(size_t(0));
	if (targets0->v_continue)
		a_generator.f_operand(*targets0->v_continue);
	else
		a_generator.f_operand(size_t(0));
	if (targets0->v_return)
		a_generator.f_operand(*targets0->v_return);
	else
		a_generator.f_operand(size_t(0));
	a_generator.v_targets = targets0;
}

void t_throw::f_generate(t_generator& a_generator, bool a_tail)
{
	v_expression->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__THROW);
}

void t_object_get::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_GET);
	a_generator.f_operand(v_key);
	a_generator.f_at(this);
}

void t_object_get_indirect::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_key->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_GET_INDIRECT);
	a_generator.f_at(this);
}

void t_object_put::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_value->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_PUT);
	a_generator.f_operand(v_key);
	a_generator.f_at(this);
}

void t_object_put_indirect::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_key->f_generate(a_generator, false);
	v_value->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_PUT_INDIRECT);
	a_generator.f_at(this);
}

void t_object_has::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_HAS);
	a_generator.f_operand(v_key);
	a_generator.f_at(this);
}

void t_object_has_indirect::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_key->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_HAS_INDIRECT);
	a_generator.f_at(this);
}

void t_object_remove::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_REMOVE);
	a_generator.f_operand(v_key);
	a_generator.f_at(this);
}

void t_object_remove_indirect::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_key->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__OBJECT_REMOVE_INDIRECT);
	a_generator.f_at(this);
}

void t_global_get::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__GLOBAL_GET);
	a_generator.f_operand(v_key);
	a_generator.f_at(this);
}

void t_scope_get::f_generate(t_generator& a_generator, bool a_tail)
{
	if (v_outer > 0) {
		a_generator.f_emit(v_variable.v_varies ? e_instruction__SCOPE_GET : e_instruction__SCOPE_GET_WITHOUT_LOCK);
		a_generator.f_operand(v_outer);
	} else {
		a_generator.f_emit(v_variable.v_shared && v_variable.v_varies ? e_instruction__SCOPE_GET0 : e_instruction__SCOPE_GET0_WITHOUT_LOCK);
	}
	a_generator.f_operand(v_variable.v_index);
	a_generator.f_at(this);
}

void t_scope_put::f_generate(t_generator& a_generator, bool a_tail)
{
	v_value->f_generate(a_generator, false);
	if (v_outer > 0) {
		a_generator.f_emit(e_instruction__SCOPE_PUT);
		a_generator.f_operand(v_outer);
	} else {
		a_generator.f_emit(v_variable.v_shared ? e_instruction__SCOPE_PUT0 : e_instruction__SCOPE_PUT0_WITHOUT_LOCK);
	}
	a_generator.f_operand(v_variable.v_index);
	a_generator.f_at(this);
}

void t_self::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__SELF);
	a_generator.f_operand(v_outer);
	a_generator.f_at(this);
}

void t_class::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__CLASS);
	a_generator.f_at(this);
}

void t_super::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	a_generator.f_emit(e_instruction__SUPER);
	a_generator.f_at(this);
}

void t_null::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__NULL);
	a_generator.f_at(this);
}

void t_boolean::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__BOOLEAN);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
}

void t_integer::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__INTEGER);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
}

void t_float::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__FLOAT);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
}

void t_instance::f_generate(t_generator& a_generator, bool a_tail)
{
	a_generator.f_emit(e_instruction__INSTANCE);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
}

void t_unary::f_generate(t_generator& a_generator, bool a_tail)
{
	v_expression->f_generate(a_generator, false);
	a_generator.f_emit(a_tail ? static_cast<t_instruction>(v_instruction + e_instruction__CALL_TAIL - e_instruction__CALL) : v_instruction);
	a_generator.f_at(this);
}

void t_binary::f_generate(t_generator& a_generator, bool a_tail)
{
	v_left->f_generate(a_generator, false);
	v_right->f_generate(a_generator, false);
	a_generator.f_emit(a_tail ? static_cast<t_instruction>(v_instruction + e_instruction__CALL_TAIL - e_instruction__CALL) : v_instruction);
	a_generator.f_at(this);
}

void t_call::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	for (t_pointers<t_node>::t_iterator i = v_arguments.f_begin(); i != v_arguments.f_end(); ++i) (*i)->f_generate(a_generator, false);
	a_generator.f_emit(a_tail ? e_instruction__CALL_TAIL : e_instruction__CALL);
	a_generator.f_operand(v_arguments.f_size());
	a_generator.f_at(this);
}

void t_get_at::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_index->f_generate(a_generator, false);
	a_generator.f_emit(a_tail ? e_instruction__GET_AT_TAIL : e_instruction__GET_AT);
	a_generator.f_at(this);
}

void t_set_at::f_generate(t_generator& a_generator, bool a_tail)
{
	v_target->f_generate(a_generator, false);
	v_index->f_generate(a_generator, false);
	v_value->f_generate(a_generator, false);
	a_generator.f_emit(a_tail ? e_instruction__SET_AT_TAIL : e_instruction__SET_AT);
	a_generator.f_at(this);
}

}

t_transfer t_generator::f_generate(ast::t_module& a_module)
{
	v_path = a_module.f_path();
	v_scope = &a_module;
	t_transfer code = t_code::f_instantiate(v_path, a_module.v_variables.size(), 0);
	v_code = &f_as<t_code&>(code);
	t_targets targets(0, false, 0, 0, false);
	v_targets = &targets;
	f_generate_block_without_value(*this, a_module.v_block);
	v_code->f_emit(e_instruction__END);
	v_code->f_estimate(false);
	v_code->f_generate();
	return code;
}

}
