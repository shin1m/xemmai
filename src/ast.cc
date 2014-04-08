#include <xemmai/ast.h>

#include <xemmai/global.h>

namespace xemmai
{

namespace ast
{

void f_generate_block(t_generator& a_generator, size_t a_stack, const std::vector<std::unique_ptr<t_node>>& a_nodes, bool a_tail)
{
	auto i = a_nodes.begin();
	auto j = a_nodes.end();
	if (i == j) {
		a_generator.f_reserve(a_stack + 1);
	} else {
		for (--j; i != j; ++i) {
			(*i)->f_generate(a_generator, a_stack, false, false);
			a_generator.f_emit(e_instruction__CLEAR);
			a_generator.f_operand(a_stack);
		}
		(*i)->f_generate(a_generator, a_stack, a_tail, false);
	}
}

void f_generate_block_without_value(t_generator& a_generator, size_t a_stack, const std::vector<std::unique_ptr<t_node>>& a_nodes)
{
	for (auto& p : a_nodes) {
		p->f_generate(a_generator, a_stack, false, false);
		a_generator.f_emit(e_instruction__CLEAR);
		a_generator.f_operand(a_stack);
	}
}

t_operand t_lambda::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	size_t stack = v_privates.size() + sizeof(t_fiber::t_context) / sizeof(t_slot);
	size_t minimum = v_arguments - v_defaults.size();
	if (v_variadic) --minimum;
	t_scoped code = t_code::f_instantiate(a_generator.v_path, v_shared, v_variadic, stack, v_shareds, v_arguments, minimum);
	t_scope* scope0 = a_generator.v_scope;
	a_generator.v_scope = this;
	t_code* code0 = a_generator.v_code;
	a_generator.v_code = &f_as<t_code&>(code);
	std::deque<t_code::t_label>* labels0 = a_generator.v_labels;
	std::deque<t_code::t_label> labels1;
	a_generator.v_labels = &labels1;
	t_generator::t_targets* targets0 = a_generator.v_targets;
	t_code::t_label& return0 = a_generator.f_label();
	t_generator::t_targets targets1(nullptr, false, nullptr, &return0, true);
	a_generator.v_targets = &targets1;
	auto safe_positions0 = a_generator.v_safe_positions;
	std::map<std::pair<size_t, size_t>, size_t> safe_positions1;
	a_generator.v_safe_positions = &safe_positions1;
	if (v_self_shared) {
		a_generator.f_reserve(stack + 1);
		a_generator.f_emit(e_instruction__SELF);
		a_generator.f_operand(stack);
		a_generator.f_operand(0);
		a_generator.f_emit(e_instruction__SCOPE_PUT);
		a_generator.f_operand(stack);
		a_generator.f_operand(0);
		a_generator.f_operand(0);
		a_generator.f_emit(e_instruction__CLEAR);
		a_generator.f_operand(stack);
	}
	for (size_t i = 0; i < v_arguments; ++i) {
		if (!v_privates[i]->v_shared) continue;
		a_generator.f_reserve(stack + 1);
		a_generator.f_emit(e_instruction__STACK_GET);
		a_generator.f_operand(stack);
		a_generator.f_operand(i);
		a_generator.f_emit(e_instruction__SCOPE_PUT);
		a_generator.f_operand(stack);
		a_generator.f_operand(0);
		a_generator.f_operand(v_privates[i]->v_index);
		a_generator.f_emit(e_instruction__CLEAR);
		a_generator.f_operand(stack);
	}
	f_generate_block(a_generator, stack, v_block, true);
	a_generator.f_target(return0);
	a_generator.f_emit(e_instruction__RETURN);
	a_generator.f_operand(stack);
	a_generator.f_resolve();
	a_generator.v_scope = scope0;
	a_generator.v_code = code0;
	a_generator.v_labels = labels0;
	a_generator.v_targets = targets0;
	a_generator.v_safe_positions = safe_positions0;
	if (a_generator.v_safe_points) {
		t_code& p = f_as<t_code&>(code);
		for (auto& pair : v_variables) p.v_variables.emplace(f_as<t_symbol&>(pair.first).f_string(), pair.second);
		for (auto& pair : safe_positions1) a_generator.v_safe_points->emplace(pair.first, &p.v_instructions[pair.second]);
	}
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit_safe_point(this);
	if (v_variadic || v_defaults.size() > 0) {
		for (size_t i = 0; i < v_defaults.size(); ++i) v_defaults[i]->f_generate(a_generator, a_stack + i, false, false);
		a_generator.f_emit(e_instruction__ADVANCED_LAMBDA);
	} else {
		a_generator.f_emit(e_instruction__LAMBDA);
	}
	a_generator.f_operand(a_stack);
	a_generator.f_operand(static_cast<t_object*>(code));
	return a_stack;
}

t_operand t_if::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_expression->f_generate(a_generator, a_stack, false, false);
	t_code::t_label& label0 = a_generator.f_label();
	t_code::t_label& label1 = a_generator.f_label();
	a_generator.f_emit(e_instruction__BRANCH);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(label0);
	f_generate_block(a_generator, a_stack, v_true, a_tail);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(label1);
	a_generator.f_target(label0);
	f_generate_block(a_generator, a_stack, v_false, a_tail);
	a_generator.f_target(label1);
	return a_stack;
}

t_operand t_while::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	t_code::t_label& continue0 = a_generator.f_label();
	a_generator.f_target(continue0);
	v_expression->f_generate(a_generator, a_stack, false, false);
	t_code::t_label& label0 = a_generator.f_label();
	a_generator.f_emit(e_instruction__BRANCH);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(label0);
	t_generator::t_targets* targets0 = a_generator.v_targets;
	t_code::t_label& break0 = a_generator.f_label();
	t_generator::t_targets targets1(&break0, a_tail, &continue0, targets0->v_return, targets0->v_return_is_tail);
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, a_stack, v_block);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(continue0);
	a_generator.f_target(label0);
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_target(break0);
	a_generator.v_targets = targets0;
	return a_stack;
}

t_operand t_for::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	f_generate_block_without_value(a_generator, a_stack, v_initialization);
	t_code::t_label& continue0 = a_generator.f_label();
	t_code::t_label& continue1 = v_next.size() > 0 ? a_generator.f_label() : continue0;
	a_generator.f_target(continue0);
	t_code::t_label& label0 = a_generator.f_label();
	if (v_condition) {
		v_condition->f_generate(a_generator, a_stack, false, false);
		a_generator.f_emit(e_instruction__BRANCH);
		a_generator.f_operand(a_stack);
		a_generator.f_operand(label0);
	}
	t_generator::t_targets* targets0 = a_generator.v_targets;
	t_code::t_label& break0 = a_generator.f_label();
	t_generator::t_targets targets1(&break0, a_tail, &continue1, targets0->v_return, targets0->v_return_is_tail);
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, a_stack, v_block);
	if (v_next.size() > 0) {
		a_generator.f_target(continue1);
		f_generate_block_without_value(a_generator, a_stack, v_next);
	}
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(continue0);
	a_generator.f_target(label0);
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_target(break0);
	a_generator.v_targets = targets0;
	return a_stack;
}

t_operand t_break::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (v_expression)
		v_expression->f_generate(a_generator, a_stack, a_generator.v_targets->v_break_is_tail, false);
	else
		a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(*a_generator.v_targets->v_break);
	return a_stack;
}

t_operand t_continue::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(*a_generator.v_targets->v_continue);
	return a_stack;
}

t_operand t_return::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (v_expression)
		v_expression->f_generate(a_generator, a_stack, a_generator.v_targets->v_return_is_tail, false);
	else
		a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit(e_instruction__JUMP);
	a_generator.f_operand(*a_generator.v_targets->v_return);
	return a_stack;
}

t_operand t_try::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	t_code::t_label& catch0 = a_generator.f_label();
	t_code::t_label& finally0 = a_generator.f_label();
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit(e_instruction__TRY);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(catch0);
	a_generator.f_operand(finally0);
	t_generator::t_targets* targets0 = a_generator.v_targets;
	{
		t_code::t_label& break0 = a_generator.f_label();
		t_code::t_label& continue0 = a_generator.f_label();
		t_code::t_label& return0 = a_generator.f_label();
		t_generator::t_targets targets1(targets0->v_break ? &break0 : nullptr, false, targets0->v_continue ? &continue0 : nullptr, targets0->v_return ? &return0 : nullptr, false);
		a_generator.v_targets = &targets1;
		f_generate_block(a_generator, a_stack, v_block, false);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__STEP);
		a_generator.f_target(catch0);
		for (auto& p : v_catches) {
			p->v_expression->f_generate(a_generator, a_stack + 1, false, false);
			t_code::t_label& label0 = a_generator.f_label();
			a_generator.f_emit(e_instruction__CATCH);
			a_generator.f_operand(a_stack);
			a_generator.f_operand(label0);
			a_generator.f_operand(p->v_variable.v_shared ? ~p->v_variable.v_index : p->v_variable.v_index);
			f_generate_block(a_generator, a_stack, p->v_block, false);
			a_generator.f_emit(e_instruction__FINALLY);
			a_generator.f_operand(t_fiber::t_try::e_state__STEP);
			a_generator.f_target(label0);
		}
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__THROW);
		a_generator.f_target(break0);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__BREAK);
		a_generator.f_target(continue0);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__CONTINUE);
		a_generator.f_target(return0);
		a_generator.f_emit(e_instruction__FINALLY);
		a_generator.f_operand(t_fiber::t_try::e_state__RETURN);
	}
	a_generator.f_target(finally0);
	{
		t_generator::t_targets targets2(nullptr, false, nullptr, nullptr, false);
		a_generator.v_targets = &targets2;
		f_generate_block_without_value(a_generator, a_stack + 1, v_finally);
	}
	a_generator.f_emit(e_instruction__YRT);
	a_generator.f_operand(a_stack);
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
	return a_stack;
}

t_operand t_throw::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_expression->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__THROW);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_get::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__OBJECT_GET);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(static_cast<t_object*>(v_key));
	a_generator.f_operand(0);
	a_generator.f_operand(0);
	a_generator.f_operand(t_scoped());
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_get_indirect::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__OBJECT_GET_INDIRECT);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_put::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_emit_safe_point(this);
	v_target->f_generate(a_generator, a_stack, false, false);
	v_value->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit(e_instruction__OBJECT_PUT);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(static_cast<t_object*>(v_key));
	a_generator.f_operand(0);
	a_generator.f_operand(0);
	a_generator.f_operand(t_scoped());
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_put_indirect::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_emit_safe_point(this);
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	v_value->f_generate(a_generator, a_stack + 2, false, false);
	a_generator.f_emit(e_instruction__OBJECT_PUT_INDIRECT);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_has::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__OBJECT_HAS);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(static_cast<t_object*>(v_key));
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_has_indirect::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__OBJECT_HAS_INDIRECT);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_remove::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__OBJECT_REMOVE);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(static_cast<t_object*>(v_key));
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_object_remove_indirect::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__OBJECT_REMOVE_INDIRECT);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_global_get::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__GLOBAL_GET);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(static_cast<t_object*>(v_key));
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_scope_get::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (v_variable.v_shared) {
		size_t instruction = (v_variable.v_varies ? e_instruction__SCOPE_GET0 : e_instruction__SCOPE_GET0_WITHOUT_LOCK) + (v_outer < 3 ? v_outer : 3);
		a_generator.f_reserve(a_stack + 1);
		if (a_tail) a_generator.f_emit_safe_point(this);
		a_generator.f_emit(static_cast<t_instruction>(instruction));
		a_generator.f_operand(a_stack);
		if (v_outer >= 3) a_generator.f_operand(v_outer);
	} else {
		if (a_operand) return t_operand(t_operand::e_tag__VARIABLE, v_variable.v_index);
		if (a_tail) {
			a_generator.f_emit_safe_point(this);
			a_generator.f_emit(e_instruction__RETURN);
		} else {
			a_generator.f_reserve(a_stack + 1);
			a_generator.f_emit(e_instruction__STACK_GET);
			a_generator.f_operand(a_stack);
		}
	}
	a_generator.f_operand(v_variable.v_index);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_scope_put::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_emit_safe_point(this);
	v_value->f_generate(a_generator, a_stack, false, false);
	if (v_variable.v_shared) {
		a_generator.f_emit(e_instruction__SCOPE_PUT);
		a_generator.f_operand(a_stack);
		a_generator.f_operand(v_outer);
	} else {
		a_generator.f_emit(e_instruction__STACK_PUT);
		a_generator.f_operand(a_stack);
	}
	a_generator.f_operand(v_variable.v_index);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_self::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__SELF);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(v_outer);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_class::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__CLASS);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_super::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__SUPER);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_null::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (a_operand) return t_scoped();
	a_generator.f_reserve(a_stack + 1);
	return a_stack;
}

t_operand t_boolean::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (a_operand) return t_scoped(v_value);
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__BOOLEAN);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_integer::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (a_operand) return v_value;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__INTEGER);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_float::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (a_operand) return v_value;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__FLOAT);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(v_value);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_instance::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	if (a_operand) return t_scoped(v_value);
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator.f_emit(e_instruction__INSTANCE);
	a_generator.f_operand(a_stack);
	a_generator.f_operand(t_scoped(v_value));
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_unary::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	t_operand operand = v_expression->f_generate(a_generator, a_stack, false, true);
	if (operand.v_tag == t_operand::e_tag__INTEGER) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_integer(v_at, operand.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
		case e_instruction__MINUS_T:
			return t_integer(v_at, -operand.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
		case e_instruction__COMPLEMENT_T:
			return t_integer(v_at, ~operand.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
		default:
			t_throwable::f_throw(L"not supported");
		}
	} else if (operand.v_tag == t_operand::e_tag__FLOAT) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_float(v_at, operand.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
		case e_instruction__MINUS_T:
			return t_float(v_at, -operand.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
		default:
			t_throwable::f_throw(L"not supported");
		}
	}
	size_t instruction = v_instruction;
	if (a_tail) instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		instruction += e_instruction__PLUS_L - e_instruction__PLUS_T;
		break;
	case t_operand::e_tag__VARIABLE:
		instruction += e_instruction__PLUS_V - e_instruction__PLUS_T;
		break;
	}
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(static_cast<t_instruction>(instruction));
	a_generator.f_operand(a_stack);
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		a_generator.f_operand(std::move(operand.v_value));
		break;
	case t_operand::e_tag__VARIABLE:
		a_generator.f_operand(operand.v_value.f_integer());
		break;
	}
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_binary::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	size_t instruction = v_instruction;
	bool operand = instruction != e_instruction__SEND;
	t_operand left = v_left->f_generate(a_generator, a_stack, false, operand);
	t_operand right = v_right->f_generate(a_generator, left.v_tag == t_operand::e_tag__TEMPORARY ? a_stack + 1 : a_stack, false, operand);
	if (left.v_tag == t_operand::e_tag__INTEGER) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_integer(v_at, left.v_value.f_integer() * right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__DIVIDE_TT:
				return t_integer(v_at, left.v_value.f_integer() / right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__MODULUS_TT:
				return t_integer(v_at, left.v_value.f_integer() % right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__ADD_TT:
				return t_integer(v_at, left.v_value.f_integer() + right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__SUBTRACT_TT:
				return t_integer(v_at, left.v_value.f_integer() - right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LEFT_SHIFT_TT:
				return t_integer(v_at, left.v_value.f_integer() << right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__RIGHT_SHIFT_TT:
				return t_integer(v_at, static_cast<size_t>(left.v_value.f_integer()) >> right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_value.f_integer() < right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_integer() <= right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_value.f_integer() > right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_integer() >= right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, left.v_value.f_integer() == right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, left.v_value.f_integer() != right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__AND_TT:
				return t_integer(v_at, left.v_value.f_integer() & right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__XOR_TT:
				return t_integer(v_at, left.v_value.f_integer() ^ right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__OR_TT:
				return t_integer(v_at, left.v_value.f_integer() | right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			default:
				t_throwable::f_throw(L"not supported");
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_value.f_integer() * right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_value.f_integer() / right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_value.f_integer() + right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_value.f_integer() - right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_value.f_integer() < right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_integer() <= right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_value.f_integer() > right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_integer() >= right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__EQUALS_TT:
				return t_boolean(v_at, left.v_value.f_integer() == right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__NOT_EQUALS_TT:
				return t_boolean(v_at, left.v_value.f_integer() != right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, false).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, true).f_generate(a_generator, a_stack, a_tail, a_operand);
			default:
				t_throwable::f_throw(L"not supported");
			}
		}
	} else if (left.v_tag == t_operand::e_tag__FLOAT) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_value.f_float() * right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_value.f_float() / right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_value.f_float() + right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_value.f_float() - right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_value.f_float() < right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_float() <= right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_value.f_float() > right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_float() >= right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__EQUALS_TT:
				return t_boolean(v_at, left.v_value.f_float() == right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__NOT_EQUALS_TT:
				return t_boolean(v_at, left.v_value.f_float() != right.v_value.f_integer()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, false).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, true).f_generate(a_generator, a_stack, a_tail, a_operand);
			default:
				t_throwable::f_throw(L"not supported");
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_value.f_float() * right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_value.f_float() / right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_value.f_float() + right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_value.f_float() - right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_value.f_float() < right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_float() <= right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_value.f_float() > right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_value.f_float() >= right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, left.v_value.f_float() == right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, left.v_value.f_float() != right.v_value.f_float()).f_generate(a_generator, a_stack, a_tail, a_operand);
			default:
				t_throwable::f_throw(L"not supported");
			}
		}
	}
	if (a_tail) instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	switch (left.v_tag) {
	case t_operand::e_tag__INTEGER:
		instruction += e_instruction__MULTIPLY_IT - e_instruction__MULTIPLY_TT;
		break;
	case t_operand::e_tag__FLOAT:
		instruction += e_instruction__MULTIPLY_FT - e_instruction__MULTIPLY_TT;
		break;
	case t_operand::e_tag__LITERAL:
		instruction += e_instruction__MULTIPLY_LT - e_instruction__MULTIPLY_TT;
		break;
	case t_operand::e_tag__VARIABLE:
		instruction += e_instruction__MULTIPLY_VT - e_instruction__MULTIPLY_TT;
		break;
	}
	switch (right.v_tag) {
	case t_operand::e_tag__INTEGER:
		instruction += e_instruction__MULTIPLY_TI - e_instruction__MULTIPLY_TT;
		break;
	case t_operand::e_tag__FLOAT:
		instruction += e_instruction__MULTIPLY_TF - e_instruction__MULTIPLY_TT;
		break;
	case t_operand::e_tag__LITERAL:
		instruction += e_instruction__MULTIPLY_TL - e_instruction__MULTIPLY_TT;
		break;
	case t_operand::e_tag__VARIABLE:
		instruction += e_instruction__MULTIPLY_TV - e_instruction__MULTIPLY_TT;
		break;
	}
	a_generator.f_reserve(a_stack + 2);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(static_cast<t_instruction>(instruction));
	a_generator.f_operand(a_stack);
	switch (left.v_tag) {
	case t_operand::e_tag__INTEGER:
		a_generator.f_operand(left.v_value.f_integer());
		break;
	case t_operand::e_tag__FLOAT:
		a_generator.f_operand(left.v_value.f_float());
		break;
	case t_operand::e_tag__LITERAL:
		a_generator.f_operand(std::move(left.v_value));
		break;
	case t_operand::e_tag__VARIABLE:
		a_generator.f_operand(left.v_value.f_integer());
		break;
	}
	switch (right.v_tag) {
	case t_operand::e_tag__INTEGER:
		a_generator.f_operand(right.v_value.f_integer());
		break;
	case t_operand::e_tag__FLOAT:
		a_generator.f_operand(right.v_value.f_float());
		break;
	case t_operand::e_tag__LITERAL:
		a_generator.f_operand(std::move(right.v_value));
		break;
	case t_operand::e_tag__VARIABLE:
		a_generator.f_operand(right.v_value.f_integer());
		break;
	}
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_call::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	a_generator.f_emit_safe_point(this);
	size_t instruction = v_expand ? e_instruction__CALL_WITH_EXPANSION : e_instruction__CALL;
	t_scope_get* get = v_expand ? nullptr : dynamic_cast<t_scope_get*>(v_target.get());
	if (get && get->v_outer == 1 && !get->v_variable.v_varies) {
		instruction = e_instruction__CALL_OUTER;
		a_generator.f_reserve(a_stack + 1);
	} else {
		get = nullptr;
		v_target->f_generate(a_generator, a_stack, false, false);
	}
	for (size_t i = 0; i < v_arguments.size(); ++i) v_arguments[i]->f_generate(a_generator, a_stack + 1 + i, false, false);
	if (a_tail) instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	a_generator.f_emit(static_cast<t_instruction>(instruction));
	a_generator.f_operand(a_stack);
	if (get) a_generator.f_operand(get->v_variable.v_index);
	a_generator.f_operand(v_arguments.size());
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_get_at::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_index->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(a_tail ? e_instruction__GET_AT_TAIL : e_instruction__GET_AT);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

t_operand t_set_at::f_generate(t_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_index->f_generate(a_generator, a_stack + 1, false, false);
	v_value->f_generate(a_generator, a_stack + 2, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator.f_emit(a_tail ? e_instruction__SET_AT_TAIL : e_instruction__SET_AT);
	a_generator.f_operand(a_stack);
	a_generator.f_at(this);
	return a_stack;
}

}

t_scoped t_generator::f_generate(ast::t_module& a_module)
{
	v_path = a_module.f_path();
	v_scope = &a_module;
	size_t stack = a_module.v_privates.size() + sizeof(t_fiber::t_context) / sizeof(t_slot);
	t_scoped code = t_code::f_instantiate(v_path, true, false, stack, a_module.v_shareds, 0, 0);
	v_code = &f_as<t_code&>(code);
	std::deque<t_code::t_label> labels;
	v_labels = &labels;
	t_targets targets(nullptr, false, nullptr, nullptr, false);
	v_targets = &targets;
	std::map<std::pair<size_t, size_t>, size_t> safe_positions;
	v_safe_positions = &safe_positions;
	f_reserve(stack + 1);
	if (a_module.v_self_shared) {
		f_emit(e_instruction__STACK_GET);
		f_operand(stack);
		f_operand(0);
		f_emit(e_instruction__SCOPE_PUT);
		f_operand(stack);
		f_operand(0);
		f_operand(0);
		f_emit(e_instruction__CLEAR);
		f_operand(stack);
	}
	f_generate_block_without_value(*this, stack, a_module.v_block);
	f_emit(e_instruction__END);
	f_resolve();
	if (v_safe_points) {
		for (auto& pair : a_module.v_variables) v_code->v_variables.emplace(f_as<t_symbol&>(pair.first).f_string(), pair.second);
		for (auto& pair : safe_positions) v_safe_points->emplace(pair.first, &v_code->v_instructions[pair.second]);
	}
	return code;
}

}
