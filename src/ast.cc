#include <xemmai/ast.h>

#include <xemmai/global.h>

namespace xemmai
{

namespace ast
{

namespace
{

void f_emit_block(t_emit& a_emit, const std::vector<std::unique_ptr<t_node>>& a_nodes, bool a_tail, bool a_clear)
{
	auto i = a_nodes.begin();
	auto j = a_nodes.end();
	if (i == j) {
		if (!a_clear) a_emit.f_emit_null();
	} else {
		for (--j; i != j; ++i) (*i)->f_emit(a_emit, false, false, true);
		(*i)->f_emit(a_emit, a_tail, false, a_clear);
	}
}

void f_emit_block_without_value(t_emit& a_emit, const std::vector<std::unique_ptr<t_node>>& a_nodes)
{
	for (auto& p : a_nodes) p->f_emit(a_emit, false, false, true);
}

}

bool t_block::f_merge(const std::vector<bool>& a_uses)
{
	bool b = false;
	if (v_uses.size() < a_uses.size()) {
		v_uses.resize(a_uses.size(), false);
		for (size_t i : v_defines) v_uses[i] = true;
		b = true;
	}
	for (size_t i = 0; i < a_uses.size(); ++i) if (a_uses[i] && !v_uses[i]) v_uses[i] = b = true;
	return b;
}

void t_flow::f_queue(t_block* a_block)
{
	if (v_queue) {
		a_block->v_queue = v_queue->v_queue;
		v_queue->v_queue = a_block;
	} else {
		a_block->v_queue = a_block;
	}
	v_queue = a_block;
}

void t_flow::operator()(t_block* a_block)
{
	f_queue(a_block);
	do {
		auto p = v_queue->v_queue;
		if (p == v_queue)
			v_queue = nullptr;
		else
			v_queue->v_queue = p->v_queue;
		p->v_queue = nullptr;
		for (auto next : p->v_nexts) if (next->f_merge(p->v_uses) && !next->v_queue) f_queue(next);
	} while (v_queue);
}

void t_node::f_flow(t_flow& a_flow)
{
}

t_object* t_lambda::f_code(t_object* a_module)
{
	size_t minimum = v_arguments - v_defaults.size();
	if (v_variadic) --minimum;
	auto code = t_code::f_instantiate(a_module, v_shared, v_variadic, v_privates.size(), v_shareds, v_arguments, minimum);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_module));
	return script.f_slot(std::move(code));
}

void t_lambda::f_safe_points(t_code& a_code, std::map<std::pair<size_t, void**>, size_t>& a_safe_points, const std::vector<std::tuple<size_t, size_t, size_t>>& a_safe_positions)
{
	t_code::t_variable self;
	self.v_shared = v_self_shared;
	self.v_index = v_self_shared ? 0 : -1;
	a_code.v_variables.emplace(L"$"sv, self);
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

void t_lambda::f_flow(t_flow& a_flow)
{
	t_flow::t_targets targets{nullptr, nullptr, &v_junction};
	t_flow flow{v_arguments, &targets, &v_block_block, &v_block_block};
	for (auto& p : v_block) p->f_flow(flow);
	flow.v_current->v_nexts.push_back(&v_junction);
	v_block_block.f_merge(std::vector<bool>(v_privates.size() - v_arguments, false));
	flow(&v_block_block);
	for (auto& p : v_defaults) p->f_flow(a_flow);
}

t_operand t_lambda::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto code = f_code(a_emit.v_module);
	auto& code1 = f_as<t_code&>(code);
	auto scope0 = a_emit.v_scope;
	a_emit.v_scope = this;
	auto code0 = a_emit.v_code;
	a_emit.v_code = &code1;
	auto arguments0 = a_emit.v_arguments;
	a_emit.v_arguments = v_arguments;
	auto stack0 = a_emit.v_stack;
	std::vector<bool> stack1(v_privates.size() - v_arguments, false);
	a_emit.v_stack = &stack1;
	auto labels0 = a_emit.v_labels;
	std::deque<t_code::t_label> labels1;
	a_emit.v_labels = &labels1;
	auto targets0 = a_emit.v_targets;
	auto& return0 = a_emit.f_label();
	t_emit::t_targets targets1{nullptr, nullptr, false, false, nullptr, nullptr, &return0, &v_junction, true};
	a_emit.v_targets = &targets1;
	auto safe_positions0 = a_emit.v_safe_positions;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions1;
	a_emit.v_safe_positions = &safe_positions1;
	if (v_self_shared) a_emit
		<< e_instruction__SELF << a_emit.f_stack()
		<< e_instruction__SCOPE_PUT0_CLEAR << a_emit.f_stack() << 0;
	for (size_t i = 0; i < v_arguments; ++i)
		if (v_privates[i]->v_shared) a_emit
			<< e_instruction__STACK_GET << a_emit.f_stack() << i
			<< e_instruction__SCOPE_PUT0_CLEAR << a_emit.f_stack() << v_privates[i]->v_index;
	f_emit_block(a_emit, v_block, true, false);
	a_emit.f_pop();
	a_emit.f_target(return0);
	a_emit.f_join(v_junction);
	assert(a_emit.f_stack() == v_privates.size());
	a_emit << e_instruction__RETURN_T;
	a_emit.f_resolve();
	a_emit.v_scope = scope0;
	a_emit.v_code = code0;
	a_emit.v_arguments = arguments0;
	a_emit.v_stack = stack0;
	a_emit.v_labels = labels0;
	a_emit.v_targets = targets0;
	a_emit.v_safe_positions = safe_positions0;
	if (a_emit.v_safe_points) f_safe_points(code1, *a_emit.v_safe_points, safe_positions1);
	a_emit.f_emit_safe_point(this);
	if (v_variadic || v_defaults.size() > 0) {
		for (size_t i = 0; i < v_defaults.size(); ++i) v_defaults[i]->f_emit(a_emit, false, false);
		for (size_t i = 0; i < v_defaults.size(); ++i) a_emit.f_pop();
		a_emit << e_instruction__ADVANCED_LAMBDA;
	} else {
		a_emit << e_instruction__LAMBDA;
	}
	(a_emit << a_emit.f_stack() << code).f_push(true);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_if::f_flow(t_flow& a_flow)
{
	v_condition->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_block_true);
	a_flow.v_current->v_nexts.push_back(&v_block_false);
	a_flow.v_current = &v_block_true;
	for (auto& p : v_true) p->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_junction);
	a_flow.v_current = &v_block_false;
	for (auto& p : v_false) p->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_junction);
	v_junction.v_nexts.push_back(&v_block_exit);
	a_flow.v_current = &v_block_exit;
}

t_operand t_if::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_condition->f_emit(a_emit, false, false);
	auto& label0 = a_emit.f_label();
	a_emit.f_pop();
	a_emit << e_instruction__BRANCH << a_emit.f_stack() << label0;
	auto stack = *a_emit.v_stack;
	f_emit_block(a_emit, v_true, a_tail, a_clear);
	a_emit.f_join(v_junction);
	auto& label1 = a_emit.f_label();
	a_emit << e_instruction__JUMP << label1;
	a_emit.f_target(label0);
	*a_emit.v_stack = stack;
	f_emit_block(a_emit, v_false, a_tail, a_clear);
	a_emit.f_join(v_junction);
	a_emit.f_target(label1);
	a_emit.f_merge(v_junction);
	return t_operand();
}

void t_while::f_flow(t_flow& a_flow)
{
	a_flow.v_current->v_nexts.push_back(&v_junction_condition);
	v_junction_condition.v_nexts.push_back(&v_block_condition);
	a_flow.v_current = &v_block_condition;
	v_condition->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_block_block);
	a_flow.v_current->v_nexts.push_back(&v_junction_exit);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_exit, &v_junction_condition, targets0->v_return};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_block;
	for (auto& p : v_block) p->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->v_nexts.push_back(&v_junction_condition);
	v_junction_exit.v_nexts.push_back(&v_block_exit);
	a_flow.v_current = &v_block_exit;
}

t_operand t_while::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_join(v_junction_condition);
	auto& loop = a_emit.f_label();
	a_emit.f_target(loop);
	a_emit.f_merge(v_junction_condition);
	v_condition->f_emit(a_emit, false, false);
	auto& label0 = a_emit.f_label();
	a_emit.f_pop();
	a_emit << e_instruction__BRANCH << a_emit.f_stack() << label0;
	auto targets0 = a_emit.v_targets;
	auto& break0 = a_emit.f_label();
	t_emit::t_targets targets1{&break0, &v_junction_exit, a_tail, a_clear, &loop, &v_junction_condition, targets0->v_return, targets0->v_return_junction, targets0->v_return_is_tail};
	a_emit.v_targets = &targets1;
	f_emit_block_without_value(a_emit, v_block);
	a_emit.v_targets = targets0;
	a_emit << e_instruction__JUMP << loop;
	a_emit.f_target(label0);
	a_emit.f_join(v_junction_exit);
	if (!a_clear) a_emit.f_emit_null();
	a_emit.f_target(break0);
	a_emit.f_merge(v_junction_exit);
	return t_operand();
}

void t_for::f_flow(t_flow& a_flow)
{
	for (auto& p : v_initialization) p->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_junction_condition);
	v_junction_condition.v_nexts.push_back(&v_block_condition);
	a_flow.v_current = &v_block_condition;
	if (v_condition) v_condition->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_block_block);
	a_flow.v_current->v_nexts.push_back(&v_junction_exit);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_exit, &v_junction_next, targets0->v_return};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_block;
	for (auto& p : v_block) p->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->v_nexts.push_back(&v_junction_next);
	v_junction_next.v_nexts.push_back(&v_block_next);
	a_flow.v_current = &v_block_next;
	for (auto& p : v_next) p->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_junction_condition);
	v_junction_exit.v_nexts.push_back(&v_block_exit);
	a_flow.v_current = &v_block_exit;
}

t_operand t_for::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	f_emit_block_without_value(a_emit, v_initialization);
	a_emit.f_join(v_junction_condition);
	auto& loop = a_emit.f_label();
	auto& continue0 = v_next.size() > 0 ? a_emit.f_label() : loop;
	a_emit.f_target(loop);
	a_emit.f_merge(v_junction_condition);
	auto& label0 = a_emit.f_label();
	if (v_condition) {
		v_condition->f_emit(a_emit, false, false);
		a_emit.f_pop();
		a_emit << e_instruction__BRANCH << a_emit.f_stack() << label0;
	}
	auto targets0 = a_emit.v_targets;
	auto& break0 = a_emit.f_label();
	t_emit::t_targets targets1{&break0, &v_junction_exit, a_tail, a_clear, &continue0, &v_junction_next, targets0->v_return, targets0->v_return_junction, targets0->v_return_is_tail};
	a_emit.v_targets = &targets1;
	f_emit_block_without_value(a_emit, v_block);
	a_emit.v_targets = targets0;
	if (v_next.size() > 0) {
		a_emit.f_target(continue0);
		f_emit_block_without_value(a_emit, v_next);
	}
	a_emit << e_instruction__JUMP << loop;
	a_emit.f_target(label0);
	a_emit.f_join(v_junction_exit);
	if (!a_clear) a_emit.f_emit_null();
	a_emit.f_target(break0);
	a_emit.f_merge(v_junction_exit);
	return t_operand();
}

void t_break::f_flow(t_flow& a_flow)
{
	if (v_expression) v_expression->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(a_flow.v_targets->v_break);
}

t_operand t_break::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_expression)
		v_expression->f_emit(a_emit, a_emit.v_targets->v_break_is_tail, false, a_emit.v_targets->v_break_must_clear);
	else if (!a_emit.v_targets->v_break_must_clear)
		t_null(v_at).f_emit(a_emit, a_emit.v_targets->v_break_is_tail, false, false);
	if (!a_emit.v_targets->v_break_must_clear) a_emit.f_pop();
	if (!a_clear) a_emit.f_push(false);
	a_emit.f_join(*a_emit.v_targets->v_break_junction);
	a_emit << e_instruction__JUMP << *a_emit.v_targets->v_break;
	return t_operand();
}

void t_continue::f_flow(t_flow& a_flow)
{
	a_flow.v_current->v_nexts.push_back(a_flow.v_targets->v_continue);
}

t_operand t_continue::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (!a_clear) a_emit.f_push(false);
	a_emit << e_instruction__JUMP << *a_emit.v_targets->v_continue;
	return t_operand();
}

void t_return::f_flow(t_flow& a_flow)
{
	if (v_expression) v_expression->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(a_flow.v_targets->v_return);
}

t_operand t_return::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_expression)
		v_expression->f_emit(a_emit, a_emit.v_targets->v_return_is_tail, false);
	else
		t_null(v_at).f_emit(a_emit, a_emit.v_targets->v_return_is_tail, false, false);
	a_emit.f_join(*a_emit.v_targets->v_return_junction);
	if (a_emit.v_targets->v_return_is_tail) {
		assert(a_emit.f_stack() - 1 == a_emit.v_scope->v_privates.size());
		a_emit << e_instruction__RETURN_T;
	} else {
		a_emit << e_instruction__JUMP << *a_emit.v_targets->v_return;
	}
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_try::f_flow(t_flow& a_flow)
{
	a_flow.v_current->v_nexts.push_back(&v_junction_try);
	v_junction_try.v_nexts.push_back(&v_block_try);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_finally, &v_junction_finally, &v_junction_finally};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_try;
	for (auto& p : v_block) p->f_flow(a_flow);
	a_flow.v_current->v_nexts.push_back(&v_junction_finally);
	for (auto& p : v_catches) {
		a_flow.v_current->v_nexts.push_back(&p->v_block_expression);
		a_flow.v_current = &p->v_block_expression;
		p->v_expression->f_flow(a_flow);
		auto block = a_flow.v_current;
		block->v_nexts.push_back(&p->v_block_block);
		a_flow.v_current = &p->v_block_block;
		if (!p->v_variable.v_shared) a_flow.v_current->v_defines.push_back(p->v_variable.v_index - a_flow.v_arguments);
		for (auto& p : p->v_block) p->f_flow(a_flow);
		a_flow.v_current->v_nexts.push_back(&v_junction_finally);
		a_flow.v_current = block;
	}
	v_junction_finally.v_nexts.push_back(&v_junction_try);
	v_junction_finally.v_nexts.push_back(&v_block_finally);
	t_flow::t_targets targets2{nullptr, nullptr, nullptr};
	a_flow.v_targets = &targets2;
	a_flow.v_current = &v_block_finally;
	for (auto& p : v_finally) p->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->v_nexts.push_back(&v_block_exit);
	if (targets0->v_break) a_flow.v_current->v_nexts.push_back(targets0->v_break);
	if (targets0->v_continue) a_flow.v_current->v_nexts.push_back(targets0->v_continue);
	if (targets0->v_return) a_flow.v_current->v_nexts.push_back(targets0->v_return);
	a_flow.v_current = &v_block_exit;
}

t_operand t_try::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_join(v_junction_try);
	a_emit.f_merge(v_junction_try);
	auto& catch0 = a_emit.f_label();
	auto& finally0 = a_emit.f_label();
	a_emit << e_instruction__TRY << a_emit.f_stack() << catch0 << finally0;
	auto targets0 = a_emit.v_targets;
	{
		auto& break0 = a_emit.f_label();
		auto& continue0 = a_emit.f_label();
		auto& return0 = a_emit.f_label();
		t_emit::t_targets targets1{targets0->v_break ? &break0 : nullptr, &v_junction_finally, false, targets0->v_break_must_clear, targets0->v_continue ? &continue0 : nullptr, &v_junction_finally, targets0->v_return ? &return0 : nullptr, &v_junction_finally, false};
		a_emit.v_targets = &targets1;
		f_emit_block(a_emit, v_block, false, a_clear);
		a_emit << e_instruction__FINALLY << t_code::e_try__STEP;
		a_emit.f_target(catch0);
		for (auto& p : v_catches) {
			if (!a_clear) a_emit.f_pop();
			p->v_expression->f_emit(a_emit, false, false);
			auto& label0 = a_emit.f_label();
			a_emit.f_pop() << e_instruction__CATCH << label0 << (p->v_variable.v_shared ? ~p->v_variable.v_index : p->v_variable.v_index);
			f_emit_block(a_emit, p->v_block, false, a_clear);
			a_emit << e_instruction__FINALLY << t_code::e_try__STEP;
			a_emit.f_target(label0);
		}
		a_emit << e_instruction__FINALLY << t_code::e_try__THROW;
		a_emit.f_target(break0);
		a_emit << e_instruction__FINALLY << t_code::e_try__BREAK;
		a_emit.f_target(continue0);
		a_emit << e_instruction__FINALLY << t_code::e_try__CONTINUE;
		a_emit.f_target(return0);
		a_emit << e_instruction__FINALLY << t_code::e_try__RETURN;
	}
	a_emit.f_target(finally0);
	{
		t_emit::t_targets targets2{nullptr, nullptr, false, false, nullptr, nullptr, nullptr, nullptr, false};
		a_emit.v_targets = &targets2;
		f_emit_block_without_value(a_emit, v_finally);
	}
	if (a_clear) {
		a_emit.f_stack_map();
	} else {
		bool live = a_emit.v_stack->back();
		a_emit.f_pop().f_stack_map().f_push(live);
	}
	a_emit << e_instruction__YRT;
	auto operand = [&](t_code::t_label* a_label, t_block* a_junction) -> t_code::t_label*
	{
		if (!a_label) {
			a_emit << size_t(0);
			return nullptr;
		}
		for (size_t i = 0; i < a_junction->v_uses.size(); ++i) {
			if ((*a_emit.v_stack)[i] || !a_junction->v_uses[i]) continue;
			auto label = &a_emit.f_label();
			a_emit << *label;
			return label;
		}
		a_emit << *a_label;
		return nullptr;
	};
	auto break0 = operand(targets0->v_break, targets0->v_break_junction);
	auto continue0 = operand(targets0->v_continue, targets0->v_continue_junction);
	auto return0 = operand(targets0->v_return, targets0->v_return_junction);
	auto step = break0 || continue0 || return0 ? &a_emit.f_label() : nullptr;
	if (step) a_emit << e_instruction__JUMP << *step;
	auto join = [&](t_code::t_label* a_label0, t_code::t_label* a_label1, t_block* a_junction)
	{
		if (!a_label0) return;
		a_emit.f_target(*a_label0);
		a_emit.f_join(*a_junction);
		a_emit << e_instruction__JUMP << *a_label1;
	};
	join(break0, targets0->v_break, targets0->v_break_junction);
	join(continue0, targets0->v_continue, targets0->v_continue_junction);
	join(return0, targets0->v_return, targets0->v_return_junction);
	if (step) a_emit.f_target(*step);
	a_emit.v_targets = targets0;
	return t_operand();
}

void t_throw::f_flow(t_flow& a_flow)
{
	v_expression->f_flow(a_flow);
}

t_operand t_throw::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_expression->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit.f_pop();
	a_emit << e_instruction__THROW << a_emit.f_stack();
	a_emit.f_stack_map();
	if (!a_clear) a_emit.f_push(false);
	a_emit.f_at(this);
	return t_operand();
}

void t_object_get::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_object_get::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_emit.v_module));
	a_emit << e_instruction__OBJECT_GET << a_emit.f_stack() - 1 << v_key << 0 << script.f_slot({}) << 0;
	a_emit.f_stack_map();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_object_get::f_method(t_emit& a_emit)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_emit.v_module));
	a_emit.f_pop();
	a_emit << e_instruction__METHOD_GET << a_emit.f_stack() << v_key << 0 << script.f_slot({}) << 0;
	a_emit.f_stack_map().f_push(true).f_push(true);
	a_emit.f_at(this);
}

void t_object_get_indirect::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_key->f_flow(a_flow);
}

t_operand t_object_get_indirect::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	v_key->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__OBJECT_GET_INDIRECT << a_emit.f_stack() - 2;
	a_emit.f_stack_map().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_object_put::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_value->f_flow(a_flow);
}

t_operand t_object_put::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	v_value->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	auto& script = static_cast<t_script&>(f_as<t_module&>(a_emit.v_module));
	a_emit << (a_clear ? e_instruction__OBJECT_PUT_CLEAR : e_instruction__OBJECT_PUT) << a_emit.f_stack() - 2 << v_key << 0 << script.f_slot({}) << 0;
	a_emit.f_stack_map().f_pop();
	if (a_clear) a_emit.f_pop();
	a_emit.f_at(this);
	return t_operand();
}

void t_object_put_indirect::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_key->f_flow(a_flow);
	v_value->f_flow(a_flow);
}

t_operand t_object_put_indirect::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	v_key->f_emit(a_emit, false, false);
	v_value->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__OBJECT_PUT_INDIRECT << a_emit.f_stack() - 3;
	a_emit.f_stack_map().f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_object_has::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_object_has::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__OBJECT_HAS << a_emit.f_stack() - 1 << v_key;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_object_has_indirect::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_key->f_flow(a_flow);
}

t_operand t_object_has_indirect::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	v_key->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__OBJECT_HAS_INDIRECT << a_emit.f_stack() - 2;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_object_remove::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_object_remove::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__OBJECT_REMOVE << a_emit.f_stack() - 1 << v_key;
	a_emit.f_stack_map();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_object_remove_indirect::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_key->f_flow(a_flow);
}

t_operand t_object_remove_indirect::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	v_key->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__OBJECT_REMOVE_INDIRECT << a_emit.f_stack() - 2;
	a_emit.f_stack_map().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
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

t_operand t_symbol_get::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	f_resolve();
	if (!v_variable) {
		if (a_tail) a_emit.f_emit_safe_point(this);
		a_emit << e_instruction__GLOBAL_GET << a_emit.f_stack() << v_symbol;
		a_emit.f_stack_map().f_push(true);
		a_emit.f_at(this);
		if (a_clear) a_emit.f_emit_clear();
		return t_operand();
	}
	if (v_variable->v_shared) {
		size_t instruction = (v_variable->v_varies ? e_instruction__SCOPE_GET0 : e_instruction__SCOPE_GET0_WITHOUT_LOCK) + (v_resolved < 3 ? v_resolved : 3);
		if (a_tail) a_emit.f_emit_safe_point(this);
		a_emit << static_cast<t_instruction>(instruction) << a_emit.f_stack();
		if (v_resolved >= 3) a_emit << v_resolved;
		a_emit.f_push(true);
	} else {
		if (a_operand) return t_operand(t_operand::e_tag__VARIABLE, v_variable->v_index);
		if (a_tail) {
			a_emit.f_emit_safe_point(this);
			a_emit.f_join(*a_emit.v_targets->v_return_junction);
			a_emit << e_instruction__RETURN_V;
			a_emit.f_push(false);
		} else {
			a_emit << e_instruction__STACK_GET << a_emit.f_stack();
			a_emit.f_push(true);
		}
	}
	a_emit << v_variable->v_index;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_scope_put::f_flow(t_flow& a_flow)
{
	v_value->f_flow(a_flow);
	if (v_variable.v_shared) return;
	int i = v_variable.v_index - a_flow.v_arguments;
	if (i >= 0) a_flow.v_current->v_defines.push_back(i);
}

t_operand t_scope_put::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear && !v_variable.v_shared) {
		int i = v_variable.v_index - a_emit.v_arguments;
		if (i >= 0 && !(*a_emit.v_stack)[i]) {
			auto operand = v_value->f_emit(a_emit, false, true);
			a_emit.f_emit_safe_point(this);
			(*a_emit.v_stack)[i] = true;
			switch (operand.v_tag) {
			case t_operand::e_tag__INTEGER:
				a_emit << e_instruction__INTEGER << v_variable.v_index << operand.v_integer;
				break;
			case t_operand::e_tag__FLOAT:
				a_emit << e_instruction__FLOAT << v_variable.v_index << operand.v_float;
				break;
			case t_operand::e_tag__LITERAL:
				switch (operand.v_value->f_tag()) {
				case t_value::e_tag__NULL:
					a_emit << e_instruction__NUL << v_variable.v_index;
					break;
				case t_value::e_tag__BOOLEAN:
					a_emit << e_instruction__BOOLEAN << v_variable.v_index << operand.v_value->f_boolean();
					break;
				default:
					a_emit << e_instruction__INSTANCE << v_variable.v_index << *operand.v_value;
				}
				break;
			case t_operand::e_tag__VARIABLE:
				a_emit << e_instruction__STACK_LET << operand.v_index << v_variable.v_index;
				break;
			default:
				a_emit << e_instruction__STACK_LET_CLEAR << a_emit.f_stack() - 1 << v_variable.v_index;
				a_emit.f_pop();
			}
			a_emit.f_at(this);
			return t_operand();
		}
	}
	v_value->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	if (v_variable.v_shared) {
		if (v_outer > 0)
			a_emit << (a_clear ? e_instruction__SCOPE_PUT_CLEAR : e_instruction__SCOPE_PUT) << a_emit.f_stack() - 1 << v_outer;
		else
			a_emit << (a_clear ? e_instruction__SCOPE_PUT0_CLEAR : e_instruction__SCOPE_PUT0) << a_emit.f_stack() - 1;
	} else {
		int i = v_variable.v_index - a_emit.v_arguments;
		if (i < 0 || (*a_emit.v_stack)[i]) {
			a_emit << (a_clear ? e_instruction__STACK_PUT_CLEAR : e_instruction__STACK_PUT);
		} else {
			(*a_emit.v_stack)[i] = true;
			a_emit << e_instruction__STACK_LET;
		}
		a_emit << a_emit.f_stack() - 1;
	}
	a_emit << v_variable.v_index;
	if (a_clear) a_emit.f_pop();
	a_emit.f_at(this);
	return t_operand();
}

t_operand t_self::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear) return t_operand();
	if (a_tail) a_emit.f_emit_safe_point(this);
	if (v_outer > 0) {
		a_emit << static_cast<t_instruction>(e_instruction__SCOPE_GET0_WITHOUT_LOCK + (v_outer < 3 ? v_outer : 3)) << a_emit.f_stack();
		if (v_outer >= 3) a_emit << v_outer;
		a_emit << 0;
	} else {
		a_emit << e_instruction__SELF << a_emit.f_stack();
	}
	a_emit.f_push(true);
	a_emit.f_at(this);
	return t_operand();
}

void t_class::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_class::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	if (a_tail) a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__CLASS << a_emit.f_stack() - 1;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_super::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_super::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	if (a_tail) a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__SUPER << a_emit.f_stack() - 1;
	a_emit.f_stack_map();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

t_operand t_null::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return t_value::v_null;
	if (a_clear) return t_operand();
	if (a_tail) {
		a_emit.f_emit_safe_point(this);
		a_emit.f_join(*a_emit.v_targets->v_return_junction);
		(a_emit << e_instruction__RETURN_N).f_push(false);
	} else {
		a_emit.f_emit_null();
	}
	a_emit.f_at(this);
	return t_operand();
}

void t_unary::f_flow(t_flow& a_flow)
{
	v_expression->f_flow(a_flow);
}

t_operand t_unary::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push(false);
	auto operand = v_expression->f_emit(a_emit, false, true);
	a_emit.f_pop();
	if (operand.v_tag == t_operand::e_tag__INTEGER) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_literal<intptr_t>(v_at, operand.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
		case e_instruction__MINUS_T:
			return t_literal<intptr_t>(v_at, -operand.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
		case e_instruction__COMPLEMENT_T:
			return t_literal<intptr_t>(v_at, ~operand.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
		default:
			f_throw(L"not supported."sv);
		}
	} else if (operand.v_tag == t_operand::e_tag__FLOAT) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_literal<double>(v_at, operand.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
		case e_instruction__MINUS_T:
			return t_literal<double>(v_at, -operand.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
		default:
			f_throw(L"not supported."sv);
		}
	}
	size_t instruction = v_instruction;
	if (a_tail) {
		a_emit.f_join(*a_emit.v_targets->v_return_junction);
		instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	}
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		instruction += e_instruction__PLUS_L - e_instruction__PLUS_T;
		break;
	case t_operand::e_tag__VARIABLE:
		instruction += e_instruction__PLUS_V - e_instruction__PLUS_T;
		break;
	}
	a_emit.f_emit_safe_point(this);
	if (operand.v_tag == t_operand::e_tag__TEMPORARY) a_emit.f_pop();
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.f_stack() == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.f_stack();
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		a_emit << *operand.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_emit << operand.v_index;
		break;
	}
	a_emit.f_stack_map().f_push(true);
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_binary::f_flow(t_flow& a_flow)
{
	v_left->f_flow(a_flow);
	v_right->f_flow(a_flow);
}

t_operand t_binary::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push(false);
	size_t instruction = v_instruction;
	bool operand = instruction != e_instruction__SEND;
	auto left = v_left->f_emit(a_emit, false, operand);
	auto right = v_right->f_emit(a_emit, false, operand);
	if (left.v_tag == t_operand::e_tag__INTEGER) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			a_emit.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_literal<intptr_t>(v_at, left.v_integer * right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_literal<intptr_t>(v_at, left.v_integer / right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__MODULUS_TT:
				return t_literal<intptr_t>(v_at, left.v_integer % right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_literal<intptr_t>(v_at, left.v_integer + right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_literal<intptr_t>(v_at, left.v_integer - right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LEFT_SHIFT_TT:
				return t_literal<intptr_t>(v_at, left.v_integer << right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__RIGHT_SHIFT_TT:
				return t_literal<intptr_t>(v_at, static_cast<size_t>(left.v_integer) >> right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_literal<bool>(v_at, left.v_integer < right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_integer <= right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_literal<bool>(v_at, left.v_integer > right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_integer >= right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_literal<bool>(v_at, left.v_integer == right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_literal<bool>(v_at, left.v_integer != right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__AND_TT:
				return t_literal<intptr_t>(v_at, left.v_integer & right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__XOR_TT:
				return t_literal<intptr_t>(v_at, left.v_integer ^ right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__OR_TT:
				return t_literal<intptr_t>(v_at, left.v_integer | right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			default:
				f_throw(L"not supported."sv);
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			a_emit.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_literal<double>(v_at, left.v_integer * right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_literal<double>(v_at, left.v_integer / right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_literal<double>(v_at, left.v_integer + right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_literal<double>(v_at, left.v_integer - right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_literal<bool>(v_at, left.v_integer < right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_integer <= right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_literal<bool>(v_at, left.v_integer > right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_integer >= right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
				return t_literal<bool>(v_at, left.v_integer == right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
				return t_literal<bool>(v_at, left.v_integer != right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__IDENTICAL_TT:
				return t_literal<bool>(v_at, false).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_literal<bool>(v_at, true).f_emit(a_emit, a_tail, a_operand, a_clear);
			default:
				f_throw(L"not supported."sv);
			}
		}
	} else if (left.v_tag == t_operand::e_tag__FLOAT) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			a_emit.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_literal<double>(v_at, left.v_float * right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_literal<double>(v_at, left.v_float / right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_literal<double>(v_at, left.v_float + right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_literal<double>(v_at, left.v_float - right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_literal<bool>(v_at, left.v_float < right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_float <= right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_literal<bool>(v_at, left.v_float > right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_float >= right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
				return t_literal<bool>(v_at, left.v_float == right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
				return t_literal<bool>(v_at, left.v_float != right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__IDENTICAL_TT:
				return t_literal<bool>(v_at, false).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_literal<bool>(v_at, true).f_emit(a_emit, a_tail, a_operand, a_clear);
			default:
				f_throw(L"not supported."sv);
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			a_emit.f_pop();
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_literal<double>(v_at, left.v_float * right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_literal<double>(v_at, left.v_float / right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_literal<double>(v_at, left.v_float + right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_literal<double>(v_at, left.v_float - right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_literal<bool>(v_at, left.v_float < right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_float <= right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_literal<bool>(v_at, left.v_float > right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_literal<bool>(v_at, left.v_float >= right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_literal<bool>(v_at, left.v_float == right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_literal<bool>(v_at, left.v_float != right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			default:
				f_throw(L"not supported."sv);
			}
		}
	}
	if (a_tail) {
		a_emit.f_join(*a_emit.v_targets->v_return_junction);
		instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	}
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
	a_emit.f_emit_safe_point(this);
	size_t stack = a_emit.f_stack() - 1;
	if (left.v_tag == t_operand::e_tag__TEMPORARY) --stack;
	if (right.v_tag == t_operand::e_tag__TEMPORARY) --stack;
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || stack == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << stack;
	switch (left.v_tag) {
	case t_operand::e_tag__INTEGER:
		a_emit << left.v_integer;
		break;
	case t_operand::e_tag__FLOAT:
		a_emit << left.v_float;
		break;
	case t_operand::e_tag__LITERAL:
		a_emit << *left.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_emit << left.v_index;
		break;
	}
	switch (right.v_tag) {
	case t_operand::e_tag__INTEGER:
		a_emit << right.v_integer;
		break;
	case t_operand::e_tag__FLOAT:
		a_emit << right.v_float;
		break;
	case t_operand::e_tag__LITERAL:
		a_emit << *right.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_emit << right.v_index;
		break;
	}
	a_emit.f_stack_map(-1);
	if (left.v_tag == t_operand::e_tag__TEMPORARY) a_emit.f_pop();
	if (right.v_tag == t_operand::e_tag__TEMPORARY) a_emit.f_pop();
	a_emit.f_pop().f_stack_map().f_push(true);
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_call::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	for (auto& p : v_arguments) p->f_flow(a_flow);
}

t_operand t_call::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	size_t instruction = v_expand ? e_instruction__CALL_WITH_EXPANSION : e_instruction__CALL;
	t_symbol_get* get = nullptr;
	if (auto p = v_expand ? nullptr : dynamic_cast<t_symbol_get*>(v_target.get())) {
		p->f_resolve();
		if (p->v_variable && (!p->v_variable->v_shared || p->v_resolved < 3 && !p->v_variable->v_varies)) get = p;
	}
	if (get) {
		instruction = get->v_variable->v_shared ? e_instruction__SCOPE_CALL0 + get->v_resolved : e_instruction__STACK_CALL;
		a_emit.f_push(false).f_push(false);
	} else if (auto p = dynamic_cast<t_object_get*>(v_target.get())) {
		p->f_method(a_emit);
	} else if (auto p = dynamic_cast<t_get_at*>(v_target.get())) {
		p->f_bind(a_emit);
	} else {
		v_target->f_emit(a_emit, false, false);
		a_emit.f_emit_null();
	}
	for (auto& p : v_arguments) p->f_emit(a_emit, false, false);
	for (size_t i = 0; i < v_arguments.size(); ++i) a_emit.f_pop();
	a_emit.f_pop().f_pop();
	if (a_tail) {
		a_emit.f_join(*a_emit.v_targets->v_return_junction);
		instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	}
	a_emit.f_emit_safe_point(this);
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.f_stack() == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.f_stack();
	if (get) a_emit << get->v_variable->v_index;
	a_emit << v_arguments.size();
	a_emit.f_stack_map().f_push(true);
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_get_at::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_index->f_flow(a_flow);
}

t_operand t_get_at::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push(false);
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	if (a_tail) a_emit.f_join(*a_emit.v_targets->v_return_junction);
	a_emit.f_emit_safe_point(this);
	a_emit << (a_tail ? e_instruction__GET_AT_TAIL : e_instruction__GET_AT);
	assert(!a_tail || a_emit.f_stack() - 3 == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.f_stack() - 3;
	a_emit.f_stack_map(-1).f_pop().f_pop().f_pop().f_stack_map().f_push(true);
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

void t_get_at::f_bind(t_emit& a_emit)
{
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__METHOD_BIND << a_emit.f_stack() - 2;
	a_emit.f_stack_map(-1).f_push(true).f_pop().f_pop().f_pop().f_stack_map().f_push(true).f_push(true);
	a_emit.f_at(this);
}

void t_set_at::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_index->f_flow(a_flow);
	v_value->f_flow(a_flow);
}

t_operand t_set_at::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push(false);
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	v_value->f_emit(a_emit, false, false);
	if (a_tail) a_emit.f_join(*a_emit.v_targets->v_return_junction);
	a_emit.f_emit_safe_point(this);
	a_emit << (a_tail ? e_instruction__SET_AT_TAIL : e_instruction__SET_AT);
	assert(!a_tail || a_emit.f_stack() - 4 == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.f_stack() - 4;
	a_emit.f_stack_map(-1).f_pop().f_pop().f_pop().f_pop().f_stack_map().f_push(true);
	a_emit.f_at(this);
	if (a_clear) a_emit.f_emit_clear();
	return t_operand();
}

}

t_scoped t_emit::operator()(ast::t_scope& a_scope)
{
	{
		ast::t_flow::t_targets targets{nullptr, nullptr, nullptr};
		ast::t_flow flow{0, &targets, &a_scope.v_block_block, &a_scope.v_block_block};
		for (auto& p : a_scope.v_block) p->f_flow(flow);
		flow.v_current->v_nexts.push_back(&a_scope.v_junction);
		a_scope.v_block_block.f_merge(std::vector<bool>(a_scope.v_privates.size(), false));
		flow(&a_scope.v_block_block);
	}
	v_scope = &a_scope;
	v_arguments = 0;
	std::vector<bool> stack(a_scope.v_privates.size(), false);
	v_stack = &stack;
	t_scoped code = t_code::f_instantiate(v_module, true, false, f_stack(), a_scope.v_shareds, 0, 0);
	v_code = &f_as<t_code&>(code);
	std::deque<t_code::t_label> labels;
	v_labels = &labels;
	t_targets targets{nullptr, nullptr, false, false, nullptr, nullptr, nullptr, nullptr, false};
	v_targets = &targets;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions;
	v_safe_positions = &safe_positions;
	if (a_scope.v_self_shared) *this
		<< e_instruction__STACK_GET << f_stack() << 0
		<< e_instruction__SCOPE_PUT0_CLEAR << f_stack() << 0;
	ast::f_emit_block_without_value(*this, a_scope.v_block);
	*this << e_instruction__END;
	f_resolve();
	if (v_safe_points) {
		t_code::t_variable self;
		self.v_shared = a_scope.v_self_shared;
		self.v_index = a_scope.v_self_shared ? 0 : -1;
		v_code->v_variables.emplace(L"$"sv, self);
		for (auto& pair : a_scope.v_variables) v_code->v_variables.emplace(f_as<t_symbol&>(pair.first).f_string(), pair.second);
		for (auto& x : safe_positions) v_safe_points->emplace(std::make_pair(std::get<0>(x), &v_code->v_instructions[std::get<1>(x)]), std::get<2>(x));
	}
	return code;
}

void t_emit::f_join(const ast::t_block& a_junction)
{
	for (size_t i = 0; i < a_junction.v_uses.size(); ++i)
		if (!(*v_stack)[i] && a_junction.v_uses[i]) *this << e_instruction__NUL << v_arguments + i;
}

}
