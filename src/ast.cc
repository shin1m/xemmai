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

bool t_block::f_forward(const std::vector<t_private>& a_privates)
{
	bool b = false;
	if (!v_forward) {
		v_forward = b = true;
		v_privates.resize(a_privates.size());
	}
	for (size_t i = 0; i < a_privates.size(); ++i) {
		if (!a_privates[i].v_out) continue;
		auto& x = v_privates[i];
		if (!x.v_in) x.v_in = x.v_out = b = true;
	}
	return b;
}

bool t_block::f_backward(const std::vector<t_private>& a_privates)
{
	bool b = !v_backward;
	v_backward = true;
	for (size_t i = 0; i < a_privates.size(); ++i) {
		if (!a_privates[i].v_use) continue;
		auto& x = v_privates[i];
		if (!x.v_define && !x.v_use) x.v_use = b = true;
	}
	return b;
}

void t_node::f_flow(t_flow& a_flow)
{
}

void t_scope::f_analyze(size_t a_arguments)
{
	t_flow::t_targets targets{nullptr, nullptr, &v_junction};
	t_flow flow{a_arguments, &targets, &v_block_block};
	for (auto& p : v_block) p->f_flow(flow);
	flow.v_current->f_next(&v_junction);
	t_block* queue = nullptr;
	auto push = [&](t_block* a_block)
	{
		if (queue) {
			a_block->v_queue = queue->v_queue;
			queue->v_queue = a_block;
		} else {
			a_block->v_queue = a_block;
		}
		queue = a_block;
	};
	auto loop = [&](auto a_do, t_block* a_block)
	{
		while (true) {
			a_do(a_block);
			if (!queue) break;
			a_block = queue->v_queue;
			if (a_block == queue)
				queue = nullptr;
			else
				queue->v_queue = a_block->v_queue;
			a_block->v_queue = nullptr;
		}
	};
	v_block_block.f_forward(std::vector<t_block::t_private>(v_privates.size() - a_arguments));
	loop([&](auto p)
	{
		for (auto q : p->v_nexts) if (q->f_forward(p->v_privates) && !q->v_queue) push(q);
	}, &v_block_block);
	loop([&](auto p)
	{
		for (auto q : p->v_previouses) if (q->f_backward(p->v_privates) && !q->v_queue) push(q);
	}, &v_junction);
}

t_object* t_lambda::f_code(t_object* a_module)
{
	size_t minimum = v_arguments - v_defaults.size();
	if (v_variadic) --minimum;
	return a_module->f_as<t_script>().f_slot(t_code::f_instantiate(a_module, v_shared, v_variadic, v_privates.size(), v_shareds, v_arguments, minimum));
}

void t_lambda::f_safe_points(t_code& a_code, std::map<std::pair<size_t, void**>, size_t>& a_safe_points, const std::vector<std::tuple<size_t, size_t, size_t>>& a_safe_positions)
{
	t_code::t_variable self;
	self.v_shared = v_self_shared;
	self.v_index = v_self_shared ? 0 : -1;
	a_code.v_variables.emplace(L"$"sv, self);
	for (auto& pair : v_variables) a_code.v_variables.emplace(pair.first->f_as<t_symbol>().f_string(), pair.second);
	std::wstring prefix;
	self.v_shared = true;
	self.v_index = 0;
	for (auto scope = v_outer; scope; scope = scope->v_outer) {
		prefix += L':';
		if (scope->v_self_shared) a_code.v_variables.emplace(prefix + L'$', self);
		for (auto& pair : scope->v_variables) if (pair.second.v_shared) a_code.v_variables.emplace(prefix + pair.first->f_as<t_symbol>().f_string(), pair.second);
	}
	for (auto& x : a_safe_positions) a_safe_points.emplace(std::make_pair(std::get<0>(x), &a_code.v_instructions[std::get<1>(x)]), std::get<2>(x));
}

void t_lambda::f_flow(t_flow& a_flow)
{
	f_analyze(v_arguments);
	for (auto& p : v_defaults) p->f_flow(a_flow);
}

t_operand t_lambda::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto code = f_code(a_emit.v_module);
	auto& code1 = code->f_as<t_code>();
	auto scope0 = a_emit.v_scope;
	a_emit.v_scope = this;
	auto code0 = a_emit.v_code;
	a_emit.v_code = &code1;
	auto arguments0 = a_emit.v_arguments;
	a_emit.v_arguments = v_arguments;
	auto privates0 = a_emit.v_privates;
	std::vector<bool> privates1(v_privates.size() - v_arguments);
	a_emit.v_privates = &privates1;
	auto stack0 = a_emit.v_stack;
	a_emit.v_stack = v_privates.size();
	auto labels0 = a_emit.v_labels;
	std::deque<t_emit::t_label> labels1;
	a_emit.v_labels = &labels1;
	auto targets0 = a_emit.v_targets;
	auto& return0 = a_emit.f_label();
	t_emit::t_targets targets1{nullptr, nullptr, a_emit.v_stack, false, false, nullptr, nullptr, &return0, &v_junction, a_emit.v_stack, true};
	a_emit.v_targets = &targets1;
	auto safe_positions0 = a_emit.v_safe_positions;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions1;
	a_emit.v_safe_positions = &safe_positions1;
	if (v_self_shared) a_emit
		<< e_instruction__SELF << a_emit.v_stack
		<< e_instruction__SCOPE_PUT0 << a_emit.v_stack << 0;
	for (size_t i = 0; i < v_arguments; ++i)
		if (v_privates[i]->v_shared) a_emit
			<< e_instruction__STACK_GET << a_emit.v_stack << i
			<< e_instruction__SCOPE_PUT0 << a_emit.v_stack << v_privates[i]->v_index;
	f_emit_block(a_emit, v_block, true, false);
	a_emit.f_pop();
	a_emit.f_target(return0);
	assert(a_emit.v_stack == v_privates.size());
	a_emit << e_instruction__RETURN_T;
	a_emit.f_resolve();
	a_emit.v_scope = scope0;
	a_emit.v_code = code0;
	a_emit.v_arguments = arguments0;
	a_emit.v_privates = privates0;
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
	(a_emit << a_emit.v_stack << code).f_push();
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_if::f_flow(t_flow& a_flow)
{
	v_condition->f_flow(a_flow);
	a_flow.v_current->f_next(&v_block_true);
	a_flow.v_current->f_next(&v_block_false);
	a_flow.v_current = &v_block_true;
	for (auto& p : v_true) p->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction);
	a_flow.v_current = &v_block_false;
	for (auto& p : v_false) p->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction);
	v_junction.f_next(&v_block_exit);
	a_flow.v_current = &v_block_exit;
}

t_operand t_if::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_condition->f_emit(a_emit, false, false);
	auto& label0 = a_emit.f_label();
	a_emit.f_pop();
	a_emit << e_instruction__BRANCH << a_emit.v_stack << label0;
	auto privates = *a_emit.v_privates;
	auto stack = a_emit.v_stack;
	f_emit_block(a_emit, v_true, a_tail, a_clear);
	a_emit.f_join(v_junction);
	auto& label1 = a_emit.f_label();
	a_emit << e_instruction__JUMP << label1;
	a_emit.f_target(label0);
	*a_emit.v_privates = privates;
	a_emit.v_stack = stack;
	f_emit_block(a_emit, v_false, a_tail, a_clear);
	a_emit.f_join(v_junction);
	a_emit.f_target(label1);
	a_emit.f_merge(v_junction);
	return t_operand();
}

void t_while::f_flow(t_flow& a_flow)
{
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_condition.f_next(&v_block_condition);
	a_flow.v_current = &v_block_condition;
	v_condition->f_flow(a_flow);
	a_flow.v_current->f_next(&v_block_block);
	a_flow.v_current->f_next(&v_junction_exit);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_exit, &v_junction_condition, targets0->v_return};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_block;
	for (auto& p : v_block) p->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_exit.f_next(&v_block_exit);
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
	a_emit << e_instruction__BRANCH << a_emit.v_stack << label0;
	auto targets0 = a_emit.v_targets;
	auto& break0 = a_emit.f_label();
	t_emit::t_targets targets1{&break0, &v_junction_exit, a_emit.v_stack, a_tail, a_clear, &loop, &v_junction_condition, targets0->v_return, targets0->v_return_junction, targets0->v_return_stack, targets0->v_return_is_tail};
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
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_condition.f_next(&v_block_condition);
	a_flow.v_current = &v_block_condition;
	if (v_condition) v_condition->f_flow(a_flow);
	a_flow.v_current->f_next(&v_block_block);
	a_flow.v_current->f_next(&v_junction_exit);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_exit, &v_junction_next, targets0->v_return};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_block;
	for (auto& p : v_block) p->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->f_next(&v_junction_next);
	v_junction_next.f_next(&v_block_next);
	a_flow.v_current = &v_block_next;
	for (auto& p : v_next) p->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_exit.f_next(&v_block_exit);
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
		a_emit << e_instruction__BRANCH << a_emit.v_stack << label0;
	}
	auto targets0 = a_emit.v_targets;
	auto& break0 = a_emit.f_label();
	t_emit::t_targets targets1{&break0, &v_junction_exit, a_emit.v_stack, a_tail, a_clear, &continue0, &v_junction_next, targets0->v_return, targets0->v_return_junction, targets0->v_return_stack, targets0->v_return_is_tail};
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
	a_flow.v_current->f_next(a_flow.v_targets->v_break);
}

t_operand t_break::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto stack = a_emit.v_stack;
	a_emit.v_stack = a_emit.v_targets->v_break_stack;
	if (v_expression)
		v_expression->f_emit(a_emit, a_emit.v_targets->v_break_is_tail, false, a_emit.v_targets->v_break_must_clear);
	else if (!a_emit.v_targets->v_break_must_clear)
		t_null(v_at).f_emit(a_emit, a_emit.v_targets->v_break_is_tail, false, false);
	a_emit.v_stack = stack;
	if (!a_clear) a_emit.f_push();
	a_emit.f_join(*a_emit.v_targets->v_break_junction);
	a_emit << e_instruction__JUMP << *a_emit.v_targets->v_break;
	return t_operand();
}

void t_continue::f_flow(t_flow& a_flow)
{
	a_flow.v_current->f_next(a_flow.v_targets->v_continue);
}

t_operand t_continue::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (!a_clear) a_emit.f_push();
	a_emit << e_instruction__JUMP << *a_emit.v_targets->v_continue;
	return t_operand();
}

void t_return::f_flow(t_flow& a_flow)
{
	if (v_expression) v_expression->f_flow(a_flow);
	a_flow.v_current->f_next(a_flow.v_targets->v_return);
}

t_operand t_return::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto stack = a_emit.v_stack;
	a_emit.v_stack = a_emit.v_targets->v_return_stack;
	if (v_expression)
		v_expression->f_emit(a_emit, a_emit.v_targets->v_return_is_tail, false);
	else
		t_null(v_at).f_emit(a_emit, a_emit.v_targets->v_return_is_tail, false, false);
	if (a_emit.v_targets->v_return_is_tail) {
		assert(a_emit.v_stack == a_emit.v_scope->v_privates.size() + 1);
		a_emit << e_instruction__RETURN_T;
	} else {
		a_emit.f_join(*a_emit.v_targets->v_return_junction);
		a_emit << e_instruction__JUMP << *a_emit.v_targets->v_return;
	}
	a_emit.v_stack = stack;
	if (!a_clear) a_emit.f_push();
	return t_operand();
}

void t_try::f_flow(t_flow& a_flow)
{
	a_flow.v_current->f_next(&v_junction_try);
	v_junction_try.f_next(&v_junction_finally);
	v_junction_try.f_next(&v_block_try);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_finally, &v_junction_finally, &v_junction_finally};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_try;
	for (auto& p : v_block) p->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction_finally);
	for (auto& p : v_catches) {
		a_flow.v_current->f_next(&p->v_block_expression);
		a_flow.v_current = &p->v_block_expression;
		p->v_expression->f_flow(a_flow);
		auto block = a_flow.v_current;
		block->f_next(&p->v_block_block);
		a_flow.v_current = &p->v_block_block;
		if (!p->v_variable.v_shared) a_flow.v_current->f_use(p->v_variable.v_index - a_flow.v_arguments, true);
		for (auto& p : p->v_block) p->f_flow(a_flow);
		a_flow.v_current->f_next(&v_junction_finally);
		a_flow.v_current = block;
	}
	v_junction_finally.f_next(&v_junction_try);
	v_junction_finally.f_next(&v_block_finally);
	t_flow::t_targets targets2{nullptr, nullptr, nullptr};
	a_flow.v_targets = &targets2;
	a_flow.v_current = &v_block_finally;
	for (auto& p : v_finally) p->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->f_next(&v_block_exit);
	if (targets0->v_break) a_flow.v_current->f_next(targets0->v_break);
	if (targets0->v_continue) a_flow.v_current->f_next(targets0->v_continue);
	if (targets0->v_return) a_flow.v_current->f_next(targets0->v_return);
	a_flow.v_current = &v_block_exit;
}

t_operand t_try::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_join(v_junction_try);
	a_emit.f_merge(v_junction_try);
	auto stack = a_emit.v_stack;
	auto& catch0 = a_emit.f_label();
	auto& finally0 = a_emit.f_label();
	a_emit << e_instruction__TRY << stack << catch0 << finally0;
	auto targets0 = a_emit.v_targets;
	{
		auto& break0 = a_emit.f_label();
		auto& continue0 = a_emit.f_label();
		auto& return0 = a_emit.f_label();
		t_emit::t_targets targets1{targets0->v_break ? &break0 : nullptr, &v_junction_finally, stack, false, targets0->v_break_must_clear, targets0->v_continue ? &continue0 : nullptr, &v_junction_finally, targets0->v_return ? &return0 : nullptr, &v_junction_finally, stack, false};
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
		t_emit::t_targets targets2{nullptr, nullptr, 0, false, false, nullptr, nullptr, nullptr, nullptr, 0, false};
		a_emit.v_targets = &targets2;
		if (a_clear) a_emit.f_push();
		f_emit_block_without_value(a_emit, v_finally);
		if (a_clear) a_emit.f_pop();
	}
	a_emit << e_instruction__YRT;
	auto operand = [&](t_emit::t_label* a_label, t_block* a_junction, size_t a_stack) -> t_emit::t_label*
	{
		if (!a_label) {
			a_emit << size_t(0);
			return nullptr;
		}
		auto jump = [&]
		{
			auto label = &a_emit.f_label();
			a_emit << *label;
			return label;
		};
		if (stack > a_stack) return jump();
		for (size_t i = 0; i < a_junction->v_privates.size(); ++i) {
			if ((*a_emit.v_privates)[i]) continue;
			auto& x = a_junction->v_privates[i];
			if (x.v_use && x.v_in) return jump();
		}
		a_emit << *a_label;
		return nullptr;
	};
	auto break_stack = targets0->v_break_must_clear ? stack : targets0->v_break_stack;
	auto break0 = operand(targets0->v_break, targets0->v_break_junction, break_stack);
	auto continue0 = operand(targets0->v_continue, targets0->v_continue_junction, stack);
	auto return0 = operand(targets0->v_return, targets0->v_return_junction, targets0->v_return_stack);
	auto step = break0 || continue0 || return0 ? &a_emit.f_label() : nullptr;
	if (step) a_emit << e_instruction__JUMP << *step;
	auto join = [&](t_emit::t_label* a_label0, t_emit::t_label* a_label1, t_block* a_junction, size_t a_stack)
	{
		if (!a_label0) return;
		a_emit.f_target(*a_label0);
		if (stack > a_stack) a_emit << e_instruction__STACK_GET << a_stack << stack;
		a_emit.f_join(*a_junction);
		a_emit << e_instruction__JUMP << *a_label1;
	};
	join(break0, targets0->v_break, targets0->v_break_junction, break_stack);
	join(continue0, targets0->v_continue, targets0->v_continue_junction, stack);
	join(return0, targets0->v_return, targets0->v_return_junction, targets0->v_return_stack);
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
	a_emit << e_instruction__THROW << a_emit.v_stack;
	if (!a_clear) a_emit.f_push();
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
	a_emit << e_instruction__OBJECT_GET << a_emit.v_stack - 1 << v_key << 0;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_object_get::f_method(t_emit& a_emit)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit.f_pop();
	a_emit << e_instruction__METHOD_GET << a_emit.v_stack << v_key << 0;
	a_emit.f_push().f_push();
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
	a_emit << e_instruction__OBJECT_GET_INDIRECT << a_emit.v_stack - 2 << 0;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
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
	a_emit << e_instruction__OBJECT_PUT << a_emit.v_stack - 2 << v_key << 0;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
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
	a_emit << e_instruction__OBJECT_PUT_INDIRECT << a_emit.v_stack - 3 << 0;
	a_emit.f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
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
	a_emit << e_instruction__OBJECT_HAS << a_emit.v_stack - 1 << v_key << 0;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
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
	a_emit << e_instruction__OBJECT_HAS_INDIRECT << a_emit.v_stack - 2 << 0;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_symbol_get::f_flow(t_flow& a_flow)
{
	v_resolved = v_outer;
	for (auto scope = v_scope; scope; scope = scope->v_outer) {
		auto i = scope->v_variables.find(v_symbol);
		if (i != scope->v_variables.end()) {
			v_variable = &i->second;
			break;
		}
		++v_resolved;
	}
	if (!v_variable || v_variable->v_shared) return;
	int i = v_variable->v_index - a_flow.v_arguments;
	if (i >= 0) a_flow.v_current->f_use(i, false);
}

t_operand t_symbol_get::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (!v_variable) {
		if (a_tail) a_emit.f_emit_safe_point(this);
		a_emit << e_instruction__GLOBAL_GET << a_emit.v_stack << v_symbol << 0;
		a_emit.f_push();
		a_emit.f_at(this);
		if (a_clear) a_emit.f_pop();
		return t_operand();
	}
	if (v_variable->v_shared) {
		size_t instruction = e_instruction__SCOPE_GET0 + (v_resolved < 3 ? v_resolved : 3);
		if (a_tail) a_emit.f_emit_safe_point(this);
		a_emit << static_cast<t_instruction>(instruction) << a_emit.v_stack;
		if (v_resolved >= 3) a_emit << v_resolved;
	} else {
		int i = v_variable->v_index - a_emit.v_arguments;
		if (i >= 0 && !(*a_emit.v_privates)[i]) {
			a_emit << e_instruction__NUL << a_emit.v_arguments + i;
			(*a_emit.v_privates)[i] = true;
		}
		if (a_operand) return t_operand(t_operand::e_tag__VARIABLE, v_variable->v_index);
		if (a_tail) {
			a_emit.f_emit_safe_point(this);
			a_emit << e_instruction__RETURN_V;
		} else {
			a_emit << e_instruction__STACK_GET << a_emit.v_stack;
		}
	}
	a_emit << v_variable->v_index;
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_scope_put::f_flow(t_flow& a_flow)
{
	v_value->f_flow(a_flow);
	if (v_variable.v_shared) return;
	int i = v_variable.v_index - a_flow.v_arguments;
	if (i >= 0) a_flow.v_current->f_use(i, true);
}

t_operand t_scope_put::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear && !v_variable.v_shared) {
		auto operand = v_value->f_emit(a_emit, false, true);
		a_emit.f_emit_safe_point(this);
		int i = v_variable.v_index - a_emit.v_arguments;
		if (i >= 0) (*a_emit.v_privates)[i] = true;
		switch (operand.v_tag) {
		case t_operand::e_tag__INTEGER:
			a_emit << e_instruction__INTEGER << v_variable.v_index << operand.v_integer;
			break;
		case t_operand::e_tag__FLOAT:
			a_emit << e_instruction__FLOAT << v_variable.v_index << operand.v_float;
			break;
		case t_operand::e_tag__LITERAL:
			switch (operand.v_value->f_tag()) {
			case e_tag__NULL:
				a_emit << e_instruction__NUL << v_variable.v_index;
				break;
			case e_tag__BOOLEAN:
				a_emit << e_instruction__BOOLEAN << v_variable.v_index << operand.v_value->f_boolean();
				break;
			default:
				a_emit << e_instruction__INSTANCE << v_variable.v_index << *operand.v_value;
			}
			break;
		case t_operand::e_tag__VARIABLE:
			a_emit << e_instruction__STACK_PUT << operand.v_index << v_variable.v_index;
			break;
		default:
			a_emit << e_instruction__STACK_PUT << a_emit.v_stack - 1 << v_variable.v_index;
			a_emit.f_pop();
		}
		a_emit.f_at(this);
		return t_operand();
	}
	v_value->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	if (v_variable.v_shared) {
		if (v_outer > 0)
			a_emit << e_instruction__SCOPE_PUT << a_emit.v_stack - 1 << v_outer;
		else
			a_emit << e_instruction__SCOPE_PUT0 << a_emit.v_stack - 1;
	} else {
		int i = v_variable.v_index - a_emit.v_arguments;
		if (i >= 0) (*a_emit.v_privates)[i] = true;
		a_emit << e_instruction__STACK_PUT << a_emit.v_stack - 1;
	}
	a_emit << v_variable.v_index;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

t_operand t_self::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear) return t_operand();
	if (a_tail) a_emit.f_emit_safe_point(this);
	if (v_outer > 0) {
		a_emit << static_cast<t_instruction>(e_instruction__SCOPE_GET0 + (v_outer < 3 ? v_outer : 3)) << a_emit.v_stack;
		if (v_outer >= 3) a_emit << v_outer;
		a_emit << 0;
	} else {
		a_emit << e_instruction__SELF << a_emit.v_stack;
	}
	a_emit.f_push();
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
	a_emit << e_instruction__CLASS << a_emit.v_stack - 1;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
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
	a_emit << e_instruction__SUPER << a_emit.v_stack - 1;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

t_operand t_null::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return nullptr;
	if (a_clear) return t_operand();
	if (a_tail) {
		a_emit.f_emit_safe_point(this);
		(a_emit << e_instruction__RETURN_N).f_push();
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
	a_emit.f_push();
	auto operand = v_expression->f_emit(a_emit, false, true);
	if (operand.v_tag == t_operand::e_tag__INTEGER) {
		a_emit.f_pop();
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
		a_emit.f_pop();
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
	if (a_tail) instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		instruction += e_instruction__PLUS_L - e_instruction__PLUS_T;
		break;
	case t_operand::e_tag__VARIABLE:
		instruction += e_instruction__PLUS_V - e_instruction__PLUS_T;
		break;
	}
	a_emit.f_emit_safe_point(this);
	if (operand.v_tag != t_operand::e_tag__TEMPORARY) a_emit.f_push();
	a_emit.f_pop().f_pop();
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.v_stack;
	switch (operand.v_tag) {
	case t_operand::e_tag__LITERAL:
		a_emit << *operand.v_value;
		break;
	case t_operand::e_tag__VARIABLE:
		a_emit << operand.v_index;
		break;
	}
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_binary::f_flow(t_flow& a_flow)
{
	v_left->f_flow(a_flow);
	v_right->f_flow(a_flow);
}

t_operand t_binary::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push();
	auto left = v_left->f_emit(a_emit, false, true);
	auto right = v_right->f_emit(a_emit, false, true);
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
				return t_literal<intptr_t>(v_at, static_cast<uintptr_t>(left.v_integer) >> right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
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
	size_t instruction = v_instruction;
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
	a_emit.f_emit_safe_point(this);
	if (left.v_tag != t_operand::e_tag__TEMPORARY) a_emit.f_push();
	if (right.v_tag != t_operand::e_tag__TEMPORARY) a_emit.f_push();
	a_emit.f_pop().f_pop().f_pop();
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.v_stack;
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
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
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
	if (!v_expand) {
		auto p = dynamic_cast<t_symbol_get*>(v_target.get());
		if (p && p->v_variable && (!p->v_variable->v_shared || p->v_resolved < 3 && !p->v_variable->v_varies)) get = p;
	}
	if (get) {
		instruction = get->v_variable->v_shared ? e_instruction__SCOPE_CALL0 + get->v_resolved : e_instruction__STACK_CALL;
		a_emit.f_push().f_push();
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
	if (a_tail) instruction += e_instruction__CALL_TAIL - e_instruction__CALL;
	a_emit.f_emit_safe_point(this);
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.size());
	if (!a_tail) a_emit << a_emit.v_stack;
	if (get) a_emit << get->v_variable->v_index;
	a_emit << v_arguments.size();
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_get_at::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_index->f_flow(a_flow);
}

t_operand t_get_at::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push();
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << (a_tail ? e_instruction__GET_AT_TAIL : e_instruction__GET_AT);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.size() + 3);
	if (!a_tail) a_emit << a_emit.v_stack - 3;
	a_emit.f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

void t_get_at::f_bind(t_emit& a_emit)
{
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << e_instruction__METHOD_BIND << a_emit.v_stack - 2;
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
	a_emit.f_push();
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	v_value->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << (a_tail ? e_instruction__SET_AT_TAIL : e_instruction__SET_AT);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.size() + 4);
	if (!a_tail) a_emit << a_emit.v_stack - 4;
	a_emit.f_pop().f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return t_operand();
}

}

t_object* t_emit::operator()(ast::t_scope& a_scope)
{
	a_scope.f_analyze(0);
	v_scope = &a_scope;
	v_arguments = 0;
	std::vector<bool> privates(a_scope.v_privates.size());
	v_privates = &privates;
	v_stack = privates.size();
	auto code = t_code::f_instantiate(v_module, true, false, privates.size(), a_scope.v_shareds, 0, 0);
	v_code = &code->f_as<t_code>();
	std::deque<t_label> labels;
	v_labels = &labels;
	t_targets targets{nullptr, nullptr, v_stack, false, false, nullptr, nullptr, nullptr, nullptr, v_stack, false};
	v_targets = &targets;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions;
	v_safe_positions = &safe_positions;
	if (a_scope.v_self_shared) *this
		<< e_instruction__STACK_GET << v_stack << 0
		<< e_instruction__SCOPE_PUT0 << v_stack << 0;
	ast::f_emit_block_without_value(*this, a_scope.v_block);
	*this << e_instruction__RETURN_N;
	f_resolve();
	if (v_safe_points) {
		t_code::t_variable self;
		self.v_shared = a_scope.v_self_shared;
		self.v_index = a_scope.v_self_shared ? 0 : -1;
		v_code->v_variables.emplace(L"$"sv, self);
		for (auto& pair : a_scope.v_variables) v_code->v_variables.emplace(pair.first->f_as<t_symbol>().f_string(), pair.second);
		for (auto& x : safe_positions) v_safe_points->emplace(std::make_pair(std::get<0>(x), &v_code->v_instructions[std::get<1>(x)]), std::get<2>(x));
	}
	return code;
}

void t_emit::f_join(const ast::t_block& a_junction)
{
	for (size_t i = 0; i < a_junction.v_privates.size(); ++i) {
		if ((*v_privates)[i]) continue;
		auto& x = a_junction.v_privates[i];
		if (x.v_use && x.v_in) *this << e_instruction__NUL << v_arguments + i;
	}
}

}
