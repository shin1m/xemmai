#include <xemmai/ast.h>

#include <xemmai/global.h>

namespace xemmai
{

namespace ast
{

namespace
{

void f_generate_block(t_generator& a_generator, const std::vector<std::unique_ptr<t_node>>& a_nodes, bool a_tail, bool a_clear)
{
	auto i = a_nodes.begin();
	auto j = a_nodes.end();
	if (i == j) {
		if (!a_clear) (a_generator << e_instruction__NUL << a_generator.f_stack()).f_push(false);
	} else {
		for (--j; i != j; ++i) (*i)->f_generate(a_generator, false, false, true);
		(*i)->f_generate(a_generator, a_tail, false, a_clear);
	}
}

void f_generate_block_without_value(t_generator& a_generator, const std::vector<std::unique_ptr<t_node>>& a_nodes)
{
	for (auto& p : a_nodes) p->f_generate(a_generator, false, false, true);
}

}

t_object* t_lambda::f_code(t_object* a_module)
{
	size_t minimum = v_arguments - v_defaults.size();
	if (v_variadic) --minimum;
	t_scoped code = t_code::f_instantiate(a_module, v_shared, v_variadic, v_privates.size(), v_shareds, v_arguments, minimum);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_module));
	return script.f_slot(std::move(code));
}

void t_lambda::f_safe_points(t_code& a_code, std::map<std::pair<size_t, void**>, size_t>& a_safe_points, const std::vector<std::tuple<size_t, size_t, size_t>>& a_safe_positions)
{
	t_code::t_variable self;
	self.v_shared = v_self_shared;
	self.v_index = v_self_shared ? 0 : -1;
	a_code.v_variables.emplace(L"$", self);
	for (auto& pair : v_variables) a_code.v_variables.emplace(f_as<t_symbol&>(pair.first).f_string(), pair.second);
	std::wstring prefix;
	self.v_shared = true;
	self.v_index = 0;
	for (auto scope = v_outer; scope; scope = scope->v_outer) {
		prefix += L':';
		if (scope->v_self_shared) a_code.v_variables.emplace(prefix + L'$', self);
		for (auto& pair : scope->v_variables) if (pair.second.v_shared) a_code.v_variables.emplace(prefix + f_as<t_symbol&>(pair.first).f_string(), pair.second);
	}
	for (auto& x : a_safe_positions) a_safe_points.emplace(std::make_pair(std::get<0>(x), &a_code.v_instructions[std::get<1>(x)]), std::get<2>(x));
}

t_operand t_lambda::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	auto code = f_code(a_generator.v_module);
	auto& code1 = f_as<t_code&>(code);
#ifdef XEMMAI_ENABLE_JIT
	f_jit_generate_with_lock(a_generator, code1);
#else
	auto scope0 = a_generator.v_scope;
	a_generator.v_scope = this;
	auto code0 = a_generator.v_code;
	a_generator.v_code = &code1;
	auto stack0 = a_generator.v_stack;
	std::vector<bool> stack1;
	a_generator.v_stack = &stack1;
	auto labels0 = a_generator.v_labels;
	std::deque<t_code::t_label> labels1;
	a_generator.v_labels = &labels1;
	auto targets0 = a_generator.v_targets;
	auto& return0 = a_generator.f_label();
	t_generator::t_targets targets1{nullptr, false, false, nullptr, &return0, true};
	a_generator.v_targets = &targets1;
	auto safe_positions0 = a_generator.v_safe_positions;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions1;
	a_generator.v_safe_positions = &safe_positions1;
	if (v_self_shared) a_generator
		<< e_instruction__SELF << a_generator.f_stack() << 0
		<< e_instruction__SCOPE_PUT_CLEAR << a_generator.f_stack() << 0 << 0;
	for (size_t i = 0; i < v_arguments; ++i)
		if (v_privates[i]->v_shared) a_generator
			<< e_instruction__STACK_GET << a_generator.f_stack() << i
			<< e_instruction__SCOPE_PUT_CLEAR << a_generator.f_stack() << 0 << v_privates[i]->v_index;
	for (size_t i = v_arguments; i < v_privates.size(); ++i) a_generator << e_instruction__NUL << i;
	f_generate_block(a_generator, v_block, true, false);
	a_generator.f_pop();
	a_generator.f_target(return0);
	a_generator << e_instruction__RETURN << a_generator.f_stack();
	a_generator.f_resolve();
	a_generator.v_scope = scope0;
	a_generator.v_code = code0;
	a_generator.v_stack = stack0;
	a_generator.v_labels = labels0;
	a_generator.v_targets = targets0;
	a_generator.v_safe_positions = safe_positions0;
	if (a_generator.v_safe_points) f_safe_points(code1, *a_generator.v_safe_points, safe_positions1);
#endif
	a_generator.f_emit_safe_point(this);
	if (v_variadic || v_defaults.size() > 0) {
		for (size_t i = 0; i < v_defaults.size(); ++i) v_defaults[i]->f_generate(a_generator, false, false);
		for (size_t i = 0; i < v_defaults.size(); ++i) a_generator.f_pop();
		a_generator << e_instruction__ADVANCED_LAMBDA;
	} else {
		a_generator << e_instruction__LAMBDA;
	}
	(a_generator << a_generator.f_stack() << code).f_push(true);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_if::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_expression->f_generate(a_generator, false, false);
	a_generator.f_pop();
	auto& label0 = a_generator.f_label();
	auto& label1 = a_generator.f_label();
	a_generator << e_instruction__BRANCH << a_generator.f_stack() << label0;
	f_generate_block(a_generator, v_true, a_tail, a_clear);
	if (!a_clear) a_generator.f_pop();
	a_generator << e_instruction__JUMP << label1;
	a_generator.f_target(label0);
	f_generate_block(a_generator, v_false, a_tail, a_clear);
	a_generator.f_target(label1);
	return t_operand();
}

t_operand t_while::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	auto& loop = a_generator.f_label();
	a_generator.f_target(loop);
	v_expression->f_generate(a_generator, false, false);
	a_generator.f_pop();
	auto& label0 = a_generator.f_label();
	a_generator << e_instruction__BRANCH << a_generator.f_stack() << label0;
	auto targets0 = a_generator.v_targets;
	auto& break0 = a_generator.f_label();
	t_generator::t_targets targets1{&break0, a_tail, a_clear, &loop, targets0->v_return, targets0->v_return_is_tail};
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, v_block);
	a_generator.v_targets = targets0;
	a_generator << e_instruction__JUMP << loop;
	a_generator.f_target(label0);
	if (!a_clear) (a_generator << e_instruction__NUL << a_generator.f_stack()).f_push(false);
	a_generator.f_target(break0);
	return t_operand();
}

t_operand t_for::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	f_generate_block_without_value(a_generator, v_initialization);
	auto& loop = a_generator.f_label();
	auto& continue0 = v_next.size() > 0 ? a_generator.f_label() : loop;
	a_generator.f_target(loop);
	auto& label0 = a_generator.f_label();
	if (v_condition) {
		v_condition->f_generate(a_generator, false, false);
		a_generator.f_pop();
		a_generator << e_instruction__BRANCH << a_generator.f_stack() << label0;
	}
	auto targets0 = a_generator.v_targets;
	auto& break0 = a_generator.f_label();
	t_generator::t_targets targets1{&break0, a_tail, a_clear, &continue0, targets0->v_return, targets0->v_return_is_tail};
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, v_block);
	a_generator.v_targets = targets0;
	if (v_next.size() > 0) {
		a_generator.f_target(continue0);
		f_generate_block_without_value(a_generator, v_next);
	}
	a_generator << e_instruction__JUMP << loop;
	a_generator.f_target(label0);
	if (!a_clear) (a_generator << e_instruction__NUL << a_generator.f_stack()).f_push(false);
	a_generator.f_target(break0);
	return t_operand();
}

t_operand t_break::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_expression)
		v_expression->f_generate(a_generator, a_generator.v_targets->v_break_is_tail, false, a_generator.v_targets->v_break_must_clear);
	else if (!a_generator.v_targets->v_break_must_clear)
		(a_generator << e_instruction__NUL << a_generator.f_stack()).f_push(false);
	if (!a_generator.v_targets->v_break_must_clear) a_generator.f_pop();
	if (!a_clear) a_generator.f_push(false);
	a_generator << e_instruction__JUMP << *a_generator.v_targets->v_break;
	return t_operand();
}

t_operand t_continue::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (!a_clear) a_generator.f_push(false);
	a_generator << e_instruction__JUMP << *a_generator.v_targets->v_continue;
	return t_operand();
}

t_operand t_return::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_expression)
		v_expression->f_generate(a_generator, a_generator.v_targets->v_return_is_tail, false);
	else
		(a_generator << e_instruction__NUL << a_generator.f_stack()).f_push(false);
	a_generator << e_instruction__JUMP << *a_generator.v_targets->v_return;
	if (a_clear) a_generator.f_pop();
	return t_operand();
}

t_operand t_try::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	auto& catch0 = a_generator.f_label();
	auto& finally0 = a_generator.f_label();
	a_generator << e_instruction__TRY << a_generator.f_stack() << catch0 << finally0;
	auto targets0 = a_generator.v_targets;
	{
		auto& break0 = a_generator.f_label();
		auto& continue0 = a_generator.f_label();
		auto& return0 = a_generator.f_label();
		t_generator::t_targets targets1{targets0->v_break ? &break0 : nullptr, false, targets0->v_break_must_clear, targets0->v_continue ? &continue0 : nullptr, targets0->v_return ? &return0 : nullptr, false};
		a_generator.v_targets = &targets1;
		f_generate_block(a_generator, v_block, false, a_clear);
		a_generator << e_instruction__FINALLY << t_code::e_try__STEP;
		a_generator.f_target(catch0);
		for (auto& p : v_catches) {
			if (!a_clear) a_generator.f_pop();
			p->v_expression->f_generate(a_generator, false, false);
			auto& label0 = a_generator.f_label();
			a_generator.f_pop() << e_instruction__CATCH << label0 << (p->v_variable.v_shared ? ~p->v_variable.v_index : p->v_variable.v_index);
			f_generate_block(a_generator, p->v_block, false, a_clear);
			a_generator << e_instruction__FINALLY << t_code::e_try__STEP;
			a_generator.f_target(label0);
		}
		a_generator << e_instruction__FINALLY << t_code::e_try__THROW;
		a_generator.f_target(break0);
		a_generator << e_instruction__FINALLY << t_code::e_try__BREAK;
		a_generator.f_target(continue0);
		a_generator << e_instruction__FINALLY << t_code::e_try__CONTINUE;
		a_generator.f_target(return0);
		a_generator << e_instruction__FINALLY << t_code::e_try__RETURN;
	}
	a_generator.f_target(finally0);
	{
		t_generator::t_targets targets2{nullptr, false, false, nullptr, nullptr, false};
		a_generator.v_targets = &targets2;
		f_generate_block_without_value(a_generator, v_finally);
		a_generator.f_stack_map();
	}
	a_generator << e_instruction__YRT;
	if (targets0->v_break)
		a_generator << *targets0->v_break;
	else
		a_generator << size_t(0);
	if (targets0->v_continue)
		a_generator << *targets0->v_continue;
	else
		a_generator << size_t(0);
	if (targets0->v_return)
		a_generator << *targets0->v_return;
	else
		a_generator << size_t(0);
	a_generator.v_targets = targets0;
	return t_operand();
}

t_operand t_throw::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_expression->f_generate(a_generator, false, false);
	a_generator.f_pop();
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__THROW << a_generator.f_stack();
	a_generator.f_stack_map();
	if (!a_clear) a_generator.f_push(false);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_object_get::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_generator.v_module));
	a_generator << e_instruction__OBJECT_GET << a_generator.f_stack() - 1 << v_key << 0 << script.f_slot(t_scoped()) << 0;
	a_generator.f_stack_map();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

void t_object_get::f_method(t_generator& a_generator)
{
	v_target->f_generate(a_generator, false, false);
	a_generator.f_pop();
	a_generator.f_emit_safe_point(this);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_generator.v_module));
	a_generator << e_instruction__METHOD_GET << a_generator.f_stack() << v_key << 0 << script.f_slot(t_scoped()) << 0;
	a_generator.f_stack_map().f_push(true).f_push(true);
	a_generator.f_at(this);
}

t_operand t_object_get_indirect::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	v_key->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__OBJECT_GET_INDIRECT << a_generator.f_stack() - 2;
	a_generator.f_stack_map().f_pop();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_object_put::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	v_value->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_generator.v_module));
	a_generator << (a_clear ? e_instruction__OBJECT_PUT_CLEAR : e_instruction__OBJECT_PUT) << a_generator.f_stack() - 2 << v_key << 0 << script.f_slot(t_scoped()) << 0;
	a_generator.f_stack_map().f_pop();
	if (a_clear) a_generator.f_pop();
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_object_put_indirect::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	v_key->f_generate(a_generator, false, false);
	v_value->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__OBJECT_PUT_INDIRECT << a_generator.f_stack() - 3;
	a_generator.f_stack_map().f_pop().f_pop();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_object_has::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__OBJECT_HAS << a_generator.f_stack() - 1 << v_key;
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_object_has_indirect::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	v_key->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__OBJECT_HAS_INDIRECT << a_generator.f_stack() - 2;
	a_generator.f_pop();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_object_remove::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__OBJECT_REMOVE << a_generator.f_stack() - 1 << v_key;
	a_generator.f_stack_map();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_object_remove_indirect::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	v_key->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__OBJECT_REMOVE_INDIRECT << a_generator.f_stack() - 2;
	a_generator.f_stack_map().f_pop();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

void t_symbol_get::f_resolve()
{
	if (v_resolved != size_t(-1)) return;
	v_resolved = v_outer;
	for (auto scope = v_scope; scope; scope = scope->v_outer) {
		auto i = scope->v_variables.find(v_symbol);
		if (i != scope->v_variables.end()) {
			v_variable = &i->second;
			break;
		}
		++v_resolved;
	}
}

t_operand t_symbol_get::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	f_resolve();
	if (!v_variable) {
		if (a_tail) a_generator.f_emit_safe_point(this);
		a_generator << e_instruction__GLOBAL_GET << a_generator.f_stack() << v_symbol;
		a_generator.f_stack_map().f_push(true);
		a_generator.f_at(this);
		if (a_clear) a_generator.f_emit_clear();
		return t_operand();
	}
	if (v_variable->v_shared) {
		size_t instruction = (v_variable->v_varies ? e_instruction__SCOPE_GET0 : e_instruction__SCOPE_GET0_WITHOUT_LOCK) + (v_resolved < 3 ? v_resolved : 3);
		if (a_tail) a_generator.f_emit_safe_point(this);
		a_generator << static_cast<t_instruction>(instruction) << a_generator.f_stack();
		if (v_resolved >= 3) a_generator << v_resolved;
		a_generator.f_push(true);
	} else {
		if (a_operand) return t_operand(t_operand::e_tag__VARIABLE, v_variable->v_index);
		if (a_tail) {
			a_generator.f_emit_safe_point(this);
			a_generator << e_instruction__RETURN;
			a_generator.f_push(false);
		} else {
			a_generator << e_instruction__STACK_GET << a_generator.f_stack();
			a_generator.f_push(true);
		}
	}
	a_generator << v_variable->v_index;
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_scope_put::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_value->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	if (v_variable.v_shared)
		a_generator << (a_clear ? e_instruction__SCOPE_PUT_CLEAR : e_instruction__SCOPE_PUT) << a_generator.f_stack() - 1 << v_outer;
	else
		a_generator << (v_let ? (a_clear ? e_instruction__STACK_LET_CLEAR : e_instruction__STACK_LET) : (a_clear ? e_instruction__STACK_PUT_CLEAR : e_instruction__STACK_PUT)) << a_generator.f_stack() - 1;
	a_generator << v_variable.v_index;
	if (a_clear) a_generator.f_pop();
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_self::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear) return t_operand();
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__SELF << a_generator.f_stack() << v_outer;
	a_generator.f_push(true);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_class::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__CLASS << a_generator.f_stack() - 1;
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_super::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, false, false);
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__SUPER << a_generator.f_stack() - 1;
	a_generator.f_stack_map();
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_null::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return t_value::v_null;
	if (a_clear) return t_operand();
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__NUL << a_generator.f_stack();
	a_generator.f_push(false);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_boolean::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value ? t_value::v_true : t_value::v_false;
	if (a_clear) return t_operand();
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__BOOLEAN << a_generator.f_stack() << v_value;
	a_generator.f_push(false);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_integer::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return t_operand();
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__INTEGER << a_generator.f_stack() << v_value;
	a_generator.f_push(false);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_float::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return t_operand();
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__FLOAT << a_generator.f_stack() << v_value;
	a_generator.f_push(false);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_instance::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return t_operand();
	if (a_tail) a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__INSTANCE << a_generator.f_stack() << v_value;
	a_generator.f_push(true);
	a_generator.f_at(this);
	return t_operand();
}

t_operand t_unary::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	a_generator.f_push(false);
	auto operand = v_expression->f_generate(a_generator, false, true);
	a_generator.f_pop();
	if (operand.v_tag == t_operand::e_tag__INTEGER) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_integer(v_at, operand.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
		case e_instruction__MINUS_T:
			return t_integer(v_at, -operand.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
		case e_instruction__COMPLEMENT_T:
			return t_integer(v_at, ~operand.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
		default:
			t_throwable::f_throw(L"not supported");
		}
	} else if (operand.v_tag == t_operand::e_tag__FLOAT) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_float(v_at, operand.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
		case e_instruction__MINUS_T:
			return t_float(v_at, -operand.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
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
	a_generator.f_emit_safe_point(this);
	a_generator << static_cast<t_instruction>(instruction);
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		a_generator << a_generator.f_stack() << operand.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_generator << a_generator.f_stack() << operand.v_index;
		break;
	default:
		a_generator.f_pop();
		a_generator << a_generator.f_stack();
	}
	a_generator.f_stack_map().f_push(true);
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_binary::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	a_generator.f_push(false);
	size_t instruction = v_instruction;
	bool operand = instruction != e_instruction__SEND;
	auto left = v_left->f_generate(a_generator, false, operand);
	auto right = v_right->f_generate(a_generator, false, operand);
	if (left.v_tag == t_operand::e_tag__INTEGER) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			a_generator.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_integer(v_at, left.v_integer * right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_integer(v_at, left.v_integer / right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__MODULUS_TT:
				return t_integer(v_at, left.v_integer % right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_integer(v_at, left.v_integer + right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_integer(v_at, left.v_integer - right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LEFT_SHIFT_TT:
				return t_integer(v_at, left.v_integer << right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__RIGHT_SHIFT_TT:
				return t_integer(v_at, static_cast<size_t>(left.v_integer) >> right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_integer < right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_integer <= right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_integer > right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_integer >= right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, left.v_integer == right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, left.v_integer != right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__AND_TT:
				return t_integer(v_at, left.v_integer & right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__XOR_TT:
				return t_integer(v_at, left.v_integer ^ right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__OR_TT:
				return t_integer(v_at, left.v_integer | right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			a_generator.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_integer * right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_integer / right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_integer + right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_integer - right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_integer < right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_integer <= right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_integer > right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_integer >= right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
				return t_boolean(v_at, left.v_integer == right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
				return t_boolean(v_at, left.v_integer != right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, false).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, true).f_generate(a_generator, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		}
	} else if (left.v_tag == t_operand::e_tag__FLOAT) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			a_generator.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_float * right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_float / right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_float + right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_float - right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_float < right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_float <= right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_float > right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_float >= right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
				return t_boolean(v_at, left.v_float == right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
				return t_boolean(v_at, left.v_float != right.v_integer).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, false).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, true).f_generate(a_generator, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			a_generator.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_float * right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_float / right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_float + right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_float - right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_float < right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_float <= right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_float > right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_float >= right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, left.v_float == right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, left.v_float != right.v_float).f_generate(a_generator, a_tail, a_operand, a_clear);
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
	a_generator.f_emit_safe_point(this);
	size_t stack = a_generator.f_stack() - 1;
	if (left.v_tag == t_operand::e_tag__TEMPORARY) --stack;
	if (right.v_tag == t_operand::e_tag__TEMPORARY) --stack;
	a_generator << static_cast<t_instruction>(instruction) << stack;
	switch (left.v_tag) {
	case t_operand::e_tag__INTEGER:
		a_generator << left.v_integer;
		break;
	case t_operand::e_tag__FLOAT:
		a_generator << left.v_float;
		break;
	case t_operand::e_tag__LITERAL:
		a_generator << left.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_generator << left.v_index;
		break;
	}
	switch (right.v_tag) {
	case t_operand::e_tag__INTEGER:
		a_generator << right.v_integer;
		break;
	case t_operand::e_tag__FLOAT:
		a_generator << right.v_float;
		break;
	case t_operand::e_tag__LITERAL:
		a_generator << right.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_generator << right.v_index;
		break;
	}
	a_generator.f_stack_map(-1);
	if (left.v_tag == t_operand::e_tag__TEMPORARY) a_generator.f_pop();
	if (right.v_tag == t_operand::e_tag__TEMPORARY) a_generator.f_pop();
	a_generator.f_pop().f_stack_map().f_push(true);
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_call::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	size_t instruction = v_expand ? e_instruction__CALL_WITH_EXPANSION : e_instruction__CALL;
	const t_code::t_variable* variable = nullptr;
	if (auto get = v_expand ? nullptr : dynamic_cast<t_symbol_get*>(v_target.get())) {
		get->f_resolve();
		if (get->v_resolved == 1 && get->v_variable && !get->v_variable->v_varies) variable = get->v_variable;
	}
	if (variable) {
		instruction = e_instruction__CALL_OUTER;
		a_generator.f_push(false).f_push(false);
	} else if (auto p = dynamic_cast<t_object_get*>(v_target.get())) {
		p->f_method(a_generator);
	} else if (auto p = dynamic_cast<t_get_at*>(v_target.get())) {
		p->f_bind(a_generator);
	} else {
		v_target->f_generate(a_generator, false, false);
		a_generator << e_instruction__NUL << a_generator.f_stack();
		a_generator.f_push(false);
	}
	for (size_t i = 0; i < v_arguments.size(); ++i) v_arguments[i]->f_generate(a_generator, false, false);
	for (size_t i = 0; i < v_arguments.size(); ++i) a_generator.f_pop();
	a_generator.f_pop().f_pop();
	if (a_tail) instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	a_generator.f_emit_safe_point(this);
	a_generator << static_cast<t_instruction>(instruction) << a_generator.f_stack();
	if (variable) a_generator << variable->v_index;
	a_generator << v_arguments.size();
	a_generator.f_stack_map().f_push(true);
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

t_operand t_get_at::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	a_generator.f_push(false);
	v_target->f_generate(a_generator, false, false);
	v_index->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << (a_tail ? e_instruction__GET_AT_TAIL : e_instruction__GET_AT) << a_generator.f_stack() - 3;
	a_generator.f_stack_map(-1).f_pop().f_pop().f_pop().f_stack_map().f_push(true);
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

void t_get_at::f_bind(t_generator& a_generator)
{
	v_target->f_generate(a_generator, false, false);
	v_index->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << e_instruction__METHOD_BIND << a_generator.f_stack() - 2;
	a_generator.f_stack_map(-1).f_pop().f_pop().f_stack_map().f_push(true).f_push(true);
	a_generator.f_at(this);
}

t_operand t_set_at::f_generate(t_generator& a_generator, bool a_tail, bool a_operand, bool a_clear)
{
	a_generator.f_push(false);
	v_target->f_generate(a_generator, false, false);
	v_index->f_generate(a_generator, false, false);
	v_value->f_generate(a_generator, false, false);
	a_generator.f_emit_safe_point(this);
	a_generator << (a_tail ? e_instruction__SET_AT_TAIL : e_instruction__SET_AT) << a_generator.f_stack() - 4;
	a_generator.f_stack_map(-1).f_pop().f_pop().f_pop().f_pop().f_stack_map().f_push(true);
	a_generator.f_at(this);
	if (a_clear) a_generator.f_emit_clear();
	return t_operand();
}

}

t_scoped t_generator::operator()(ast::t_scope& a_scope)
{
	v_scope = &a_scope;
	std::vector<bool> stack;
	v_stack = &stack;
	t_scoped code = t_code::f_instantiate(v_module, true, false, f_stack(), a_scope.v_shareds, 0, 0);
	v_code = &f_as<t_code&>(code);
	std::deque<t_code::t_label> labels;
	v_labels = &labels;
	t_targets targets{nullptr, false, false, nullptr, nullptr, false};
	v_targets = &targets;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions;
	v_safe_positions = &safe_positions;
	if (a_scope.v_self_shared) *this
		<< e_instruction__STACK_GET << f_stack() << 0
		<< e_instruction__SCOPE_PUT_CLEAR << f_stack() << 0 << 0;
	for (size_t i = 0; i < a_scope.v_privates.size(); ++i) *this << e_instruction__NUL << i;
	ast::f_generate_block_without_value(*this, a_scope.v_block);
	*this << e_instruction__END;
	f_resolve();
	if (v_safe_points) {
		t_code::t_variable self;
		self.v_shared = a_scope.v_self_shared;
		self.v_index = a_scope.v_self_shared ? 0 : -1;
		v_code->v_variables.emplace(L"$", self);
		for (auto& pair : a_scope.v_variables) v_code->v_variables.emplace(f_as<t_symbol&>(pair.first).f_string(), pair.second);
		for (auto& x : safe_positions) v_safe_points->emplace(std::make_pair(std::get<0>(x), &v_code->v_instructions[std::get<1>(x)]), std::get<2>(x));
	}
#ifdef XEMMAI_ENABLE_JIT
	f_jit_install();
	v_code->v_jit_loop = t_code::f_jit_loop_nojit;
#endif
	return code;
}

}
