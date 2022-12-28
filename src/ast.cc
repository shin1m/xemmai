#include <xemmai/global.h>

namespace xemmai
{

namespace ast
{

void* t_arena::t_of::f_allocate(size_t a_n)
{
	auto p = v_p;
	if (a_n > v_q - p) [[unlikely]] {
		auto n = t_object::f_size_to_capacity<t_bytes, unsigned char>(std::max(t_object::f_capacity_to_size<t_bytes, unsigned char>(a_n), sizeof(t_object) << 5));
		auto block = t_bytes::f_instantiate(n);
		v_blocks.emplace_back(block);
		p = &block->f_as<t_bytes>()[0];
		v_q = p + n;
	}
	v_p = p + a_n;
	return p;
}

bool t_block::f_forward(const std::vector<t_private, t_allocator<t_private>>& a_privates)
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

bool t_block::f_backward(const std::vector<t_private, t_allocator<t_private>>& a_privates)
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

void t_nodes::f_flow(t_flow& a_flow)
{
	f_each([&](auto p)
	{
		p->f_flow(a_flow);
	});
}

t_operand t_nodes::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (auto p = v_tail) {
		while (true) {
			p = p->v_next;
			if (p == v_tail) break;
			p->f_emit(a_emit, false, false, true);
		}
		p->f_emit(a_emit, a_tail, false, a_clear);
	} else {
		if (!a_clear) a_emit.f_emit_null();
	}
	return {};
}

void t_scope::f_vary(t_variable* a_p)
{
	if (a_p == v_privates.v_tail) return;
	if (!a_p) a_p = v_privates.v_tail;
	do {
		a_p = a_p->v_next;
		a_p->v_varies = true;
	} while (a_p != v_privates.v_tail);
}

void t_scope::f_classify(t_variable* a_p)
{
	while (true)
		if (auto p = a_p->v_next; p->v_shared) {
			p->v_index = v_shareds++;
			a_p->v_next = p->v_next;
			if (p == v_privates.v_tail) {
				v_privates.v_tail = p == a_p ? nullptr : a_p;
				break;
			}
		} else {
			p->v_index = v_privates.v_size++;
			if (p == v_privates.v_tail) break;
			a_p = p;
		}
}

void t_scope::f_analyze(size_t a_arguments)
{
	t_flow::t_targets targets{nullptr, nullptr, &v_junction};
	t_flow flow{a_arguments, &targets, &v_block_body};
	v_body->f_flow(flow);
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
	v_block_body.f_forward(std::vector<t_block::t_private, t_allocator<t_block::t_private>>(v_privates.v_size - a_arguments, v_block_body.v_privates.get_allocator()));
	loop([&](auto p)
	{
		for (auto q : p->v_nexts) if (q->f_forward(p->v_privates) && !q->v_queue) push(q);
	}, &v_block_body);
	loop([&](auto p)
	{
		for (auto q : p->v_previouses) if (q->f_backward(p->v_privates) && !q->v_queue) push(q);
	}, &v_junction);
}

t_object* t_lambda::f_code(t_object* a_module)
{
	size_t minimum = v_arguments - v_defaults.v_size;
	if (v_variadic) --minimum;
	return a_module->f_as<t_script>().f_slot(t_code::f_instantiate(a_module, v_shared, v_variadic, v_privates.v_size, v_shareds, v_arguments, minimum));
}

void t_lambda::f_safe_points(t_code& a_code, std::map<std::pair<size_t, void**>, size_t>& a_safe_points, const std::vector<std::tuple<size_t, size_t, size_t>>& a_safe_positions)
{
	t_code::t_variable self;
	self.v_shared = v_self_shared;
	self.v_index = v_self_shared ? 0 : -1;
	a_code.v_variables.emplace(L"$"sv, self);
	for (auto& [s, v] : v_variables) a_code.v_variables.emplace(s->f_as<t_symbol>().f_string(), v);
	std::wstring prefix;
	self.v_shared = true;
	self.v_index = 0;
	for (auto scope = v_outer; scope; scope = scope->v_outer) {
		prefix += L':';
		if (scope->v_self_shared) a_code.v_variables.emplace(prefix + L'$', self);
		for (auto& [s, v] : scope->v_variables) if (v.v_shared) a_code.v_variables.emplace(prefix + s->f_as<t_symbol>().f_string(), v);
	}
	for (auto [l, a, c] : a_safe_positions) a_safe_points.emplace(std::make_pair(l, &a_code.v_instructions[a]), c);
}

void t_lambda::f_flow(t_flow& a_flow)
{
	f_analyze(v_arguments);
	v_defaults.f_flow(a_flow);
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
	std::vector<bool> privates1(v_privates.v_size - v_arguments);
	a_emit.v_privates = &privates1;
	auto stack0 = a_emit.v_stack;
	a_emit.v_stack = v_privates.v_size;
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
		<< c_instruction__SELF << a_emit.v_stack
		<< c_instruction__SCOPE_PUT0 << a_emit.v_stack << 0;
	{
		auto p = v_privates.v_tail;
		for (size_t i = 0; i < v_arguments; ++i) if (p = p->v_next; p->v_shared) a_emit
			<< c_instruction__STACK_GET << a_emit.v_stack << i
			<< c_instruction__SCOPE_PUT0 << a_emit.v_stack << p->v_index;
	}
	v_body->f_emit(a_emit, true, false);
	a_emit.f_pop();
	a_emit.f_target(return0);
	assert(a_emit.v_stack == v_privates.v_size);
	a_emit << c_instruction__RETURN_T;
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
	if (v_variadic || v_defaults.v_size > 0) {
		v_defaults.f_each([&](auto p)
		{
			p->f_emit(a_emit, false, false);
		});
		for (size_t i = 0; i < v_defaults.v_size; ++i) a_emit.f_pop();
		a_emit << c_instruction__ADVANCED_LAMBDA;
	} else {
		a_emit << c_instruction__LAMBDA;
	}
	(a_emit << a_emit.v_stack << code).f_push();
	if (a_clear) a_emit.f_pop();
	return {};
}

void t_if::f_flow(t_flow& a_flow)
{
	if (v_condition) v_condition->f_flow(a_flow);
	a_flow.v_current->f_next(&v_block_true);
	a_flow.v_current->f_next(&v_block_false);
	a_flow.v_current = &v_block_true;
	v_true->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction);
	a_flow.v_current = &v_block_false;
	v_false->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction);
	v_junction.f_next(&v_block_exit);
	a_flow.v_current = &v_block_exit;
}

t_operand t_if::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto& label0 = a_emit.f_label();
	if (v_condition) {
		v_condition->f_emit(a_emit, false, false);
		a_emit.f_pop();
		a_emit << c_instruction__BRANCH << a_emit.v_stack << label0;
	}
	auto privates = *a_emit.v_privates;
	auto stack = a_emit.v_stack;
	v_true->f_emit(a_emit, a_tail, false, a_clear);
	a_emit.f_join(v_junction);
	auto& label1 = a_emit.f_label();
	a_emit << c_instruction__JUMP << label1;
	a_emit.f_target(label0);
	*a_emit.v_privates = privates;
	a_emit.v_stack = stack;
	v_false->f_emit(a_emit, a_tail, false, a_clear);
	a_emit.f_join(v_junction);
	a_emit.f_target(label1);
	a_emit.f_merge(v_junction);
	return {};
}

void t_while::f_flow(t_flow& a_flow)
{
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_condition.f_next(&v_block_condition);
	a_flow.v_current = &v_block_condition;
	if (v_condition) v_condition->f_flow(a_flow);
	a_flow.v_current->f_next(&v_block_body);
	a_flow.v_current->f_next(&v_junction_exit);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_exit, &v_junction_condition, targets0->v_return};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_body;
	v_body->f_flow(a_flow);
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
	auto& label0 = a_emit.f_label();
	if (v_condition) {
		v_condition->f_emit(a_emit, false, false);
		a_emit.f_pop();
		a_emit << c_instruction__BRANCH << a_emit.v_stack << label0;
	}
	auto targets0 = a_emit.v_targets;
	auto& break0 = a_emit.f_label();
	t_emit::t_targets targets1{&break0, &v_junction_exit, a_emit.v_stack, a_tail, a_clear, &loop, &v_junction_condition, targets0->v_return, targets0->v_return_junction, targets0->v_return_stack, targets0->v_return_is_tail};
	a_emit.v_targets = &targets1;
	v_body->f_emit(a_emit, false, false, true);
	a_emit.v_targets = targets0;
	a_emit << c_instruction__JUMP << loop;
	a_emit.f_target(label0);
	a_emit.f_join(v_junction_exit);
	if (!a_clear) a_emit.f_emit_null();
	a_emit.f_target(break0);
	a_emit.f_merge(v_junction_exit);
	return {};
}

void t_for::f_flow(t_flow& a_flow)
{
	if (v_initialization) v_initialization->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_condition.f_next(&v_block_condition);
	a_flow.v_current = &v_block_condition;
	if (v_condition) v_condition->f_flow(a_flow);
	a_flow.v_current->f_next(&v_block_body);
	a_flow.v_current->f_next(&v_junction_exit);
	auto targets0 = a_flow.v_targets;
	t_flow::t_targets targets1{&v_junction_exit, &v_junction_next, targets0->v_return};
	a_flow.v_targets = &targets1;
	a_flow.v_current = &v_block_body;
	v_body->f_flow(a_flow);
	a_flow.v_targets = targets0;
	a_flow.v_current->f_next(&v_junction_next);
	v_junction_next.f_next(&v_block_next);
	a_flow.v_current = &v_block_next;
	if (v_next) v_next->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction_condition);
	v_junction_exit.f_next(&v_block_exit);
	a_flow.v_current = &v_block_exit;
}

t_operand t_for::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_initialization) v_initialization->f_emit(a_emit, false, false, true);
	a_emit.f_join(v_junction_condition);
	auto& loop = a_emit.f_label();
	auto& continue0 = v_next ? a_emit.f_label() : loop;
	a_emit.f_target(loop);
	a_emit.f_merge(v_junction_condition);
	auto& label0 = a_emit.f_label();
	if (v_condition) {
		v_condition->f_emit(a_emit, false, false);
		a_emit.f_pop();
		a_emit << c_instruction__BRANCH << a_emit.v_stack << label0;
	}
	auto targets0 = a_emit.v_targets;
	auto& break0 = a_emit.f_label();
	t_emit::t_targets targets1{&break0, &v_junction_exit, a_emit.v_stack, a_tail, a_clear, &continue0, &v_junction_next, targets0->v_return, targets0->v_return_junction, targets0->v_return_stack, targets0->v_return_is_tail};
	a_emit.v_targets = &targets1;
	v_body->f_emit(a_emit, false, false, true);
	a_emit.v_targets = targets0;
	if (v_next) {
		a_emit.f_target(continue0);
		v_next->f_emit(a_emit, false, false, true);
	}
	a_emit << c_instruction__JUMP << loop;
	a_emit.f_target(label0);
	a_emit.f_join(v_junction_exit);
	if (!a_clear) a_emit.f_emit_null();
	a_emit.f_target(break0);
	a_emit.f_merge(v_junction_exit);
	return {};
}

void t_break::f_flow(t_flow& a_flow)
{
	v_expression->f_flow(a_flow);
	a_flow.v_current->f_next(a_flow.v_targets->v_break);
}

t_operand t_break::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto stack = a_emit.v_stack;
	a_emit.v_stack = a_emit.v_targets->v_break_stack;
	v_expression->f_emit(a_emit, a_emit.v_targets->v_break_is_tail, false, a_emit.v_targets->v_break_must_clear);
	a_emit.v_stack = stack;
	if (!a_clear) a_emit.f_push();
	a_emit.f_join(*a_emit.v_targets->v_break_junction);
	a_emit << c_instruction__JUMP << *a_emit.v_targets->v_break;
	return {};
}

void t_continue::f_flow(t_flow& a_flow)
{
	a_flow.v_current->f_next(a_flow.v_targets->v_continue);
}

t_operand t_continue::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (!a_clear) a_emit.f_push();
	a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__JUMP << *a_emit.v_targets->v_continue;
	return {};
}

void t_return::f_flow(t_flow& a_flow)
{
	v_expression->f_flow(a_flow);
	a_flow.v_current->f_next(a_flow.v_targets->v_return);
}

t_operand t_return::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	auto stack = a_emit.v_stack;
	a_emit.v_stack = a_emit.v_targets->v_return_stack;
	v_expression->f_emit(a_emit, a_emit.v_targets->v_return_is_tail, false);
	if (a_emit.v_targets->v_return_is_tail) {
		assert(a_emit.v_stack == a_emit.v_scope->v_privates.v_size + 1);
		a_emit << c_instruction__RETURN_T;
	} else {
		a_emit.f_join(*a_emit.v_targets->v_return_junction);
		a_emit << c_instruction__JUMP << *a_emit.v_targets->v_return;
	}
	a_emit.v_stack = stack;
	if (!a_clear) a_emit.f_push();
	return {};
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
	v_body->f_flow(a_flow);
	a_flow.v_current->f_next(&v_junction_finally);
	v_catches.f_each([&](auto p)
	{
		a_flow.v_current->f_next(&p->v_block_expression);
		a_flow.v_current = &p->v_block_expression;
		p->v_expression->f_flow(a_flow);
		auto block = a_flow.v_current;
		block->f_next(&p->v_block_body);
		a_flow.v_current = &p->v_block_body;
		if (!p->v_variable.v_shared) a_flow.v_current->f_use(p->v_variable.v_index - a_flow.v_arguments, true);
		p->v_body->f_flow(a_flow);
		a_flow.v_current->f_next(&v_junction_finally);
		a_flow.v_current = block;
	});
	v_junction_finally.f_next(&v_junction_try);
	v_junction_finally.f_next(&v_block_finally);
	t_flow::t_targets targets2{nullptr, nullptr, nullptr};
	a_flow.v_targets = &targets2;
	a_flow.v_current = &v_block_finally;
	if (v_finally) v_finally->f_flow(a_flow);
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
	a_emit << c_instruction__TRY << stack << catch0 << finally0;
	auto targets0 = a_emit.v_targets;
	{
		auto& break0 = a_emit.f_label();
		auto& continue0 = a_emit.f_label();
		auto& return0 = a_emit.f_label();
		t_emit::t_targets targets1{targets0->v_break ? &break0 : nullptr, &v_junction_finally, stack, false, targets0->v_break_must_clear, targets0->v_continue ? &continue0 : nullptr, &v_junction_finally, targets0->v_return ? &return0 : nullptr, &v_junction_finally, stack, false};
		a_emit.v_targets = &targets1;
		v_body->f_emit(a_emit, false, false, a_clear);
		a_emit << c_instruction__FINALLY << t_code::c_try__STEP;
		a_emit.f_target(catch0);
		v_catches.f_each([&](auto p)
		{
			if (!a_clear) a_emit.f_pop();
			p->v_expression->f_emit(a_emit, false, false);
			auto& label0 = a_emit.f_label();
			a_emit.f_pop() << c_instruction__CATCH << label0 << (p->v_variable.v_shared ? ~p->v_variable.v_index : p->v_variable.v_index);
			p->v_body->f_emit(a_emit, false, false, a_clear);
			a_emit << c_instruction__FINALLY << t_code::c_try__STEP;
			a_emit.f_target(label0);
		});
		a_emit << c_instruction__FINALLY << t_code::c_try__THROW;
		a_emit.f_target(break0);
		a_emit << c_instruction__FINALLY << t_code::c_try__BREAK;
		a_emit.f_target(continue0);
		a_emit << c_instruction__FINALLY << t_code::c_try__CONTINUE;
		a_emit.f_target(return0);
		a_emit << c_instruction__FINALLY << t_code::c_try__RETURN;
	}
	a_emit.f_target(finally0);
	if (v_finally) {
		t_emit::t_targets targets2{nullptr, nullptr, 0, false, false, nullptr, nullptr, nullptr, nullptr, 0, false};
		a_emit.v_targets = &targets2;
		if (a_clear) a_emit.f_push();
		v_finally->f_emit(a_emit, false, false, true);
		if (a_clear) a_emit.f_pop();
	}
	a_emit << c_instruction__YRT;
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
	if (step) a_emit << c_instruction__JUMP << *step;
	auto join = [&](t_emit::t_label* a_label0, t_emit::t_label* a_label1, t_block* a_junction, size_t a_stack)
	{
		if (!a_label0) return;
		a_emit.f_target(*a_label0);
		if (stack > a_stack) a_emit << c_instruction__STACK_GET << a_stack << stack;
		a_emit.f_join(*a_junction);
		a_emit << c_instruction__JUMP << *a_label1;
	};
	join(break0, targets0->v_break, targets0->v_break_junction, break_stack);
	join(continue0, targets0->v_continue, targets0->v_continue_junction, stack);
	join(return0, targets0->v_return, targets0->v_return_junction, targets0->v_return_stack);
	if (step) a_emit.f_target(*step);
	a_emit.v_targets = targets0;
	return {};
}

void t_throw::f_flow(t_flow& a_flow)
{
	v_expression->f_flow(a_flow);
}

t_operand t_throw::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_expression->f_emit(a_emit, false, false);
	a_emit.f_pop();
	a_emit << c_instruction__THROW << a_emit.v_stack;
	if (!a_clear) a_emit.f_push();
	a_emit.f_at(this);
	return {};
}

void t_object_get::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_object_get::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__OBJECT_GET << a_emit.v_stack - 1 << v_key << 0;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

void t_object_get::f_method(t_emit& a_emit)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__METHOD_GET << a_emit.v_stack - 1 << v_key << 0;
	a_emit.f_push();
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
	a_emit << c_instruction__OBJECT_GET_INDIRECT << a_emit.v_stack - 2 << 0;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
	a_emit << c_instruction__OBJECT_PUT << a_emit.v_stack - 2 << v_key << 0;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
	a_emit << c_instruction__OBJECT_PUT_INDIRECT << a_emit.v_stack - 3 << 0;
	a_emit.f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

void t_object_has::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_object_has::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__OBJECT_HAS << a_emit.v_stack - 1 << v_key << 0;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
	a_emit << c_instruction__OBJECT_HAS_INDIRECT << a_emit.v_stack - 2 << 0;
	a_emit.f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
		a_emit << c_instruction__GLOBAL_GET << a_emit.v_stack << v_symbol << 0;
		a_emit.f_push();
		a_emit.f_at(this);
		if (a_clear) a_emit.f_pop();
		return {};
	}
	if (v_variable->v_shared) {
		size_t instruction = c_instruction__SCOPE_GET0 + (v_resolved < 3 ? v_resolved : 3);
		if (a_tail) a_emit.f_emit_safe_point(this);
		a_emit << static_cast<t_instruction>(instruction) << a_emit.v_stack;
		if (v_resolved >= 3) a_emit << v_resolved;
	} else {
		int i = v_variable->v_index - a_emit.v_arguments;
		if (i >= 0 && !(*a_emit.v_privates)[i]) {
			a_emit << c_instruction__NUL << a_emit.v_arguments + i;
			(*a_emit.v_privates)[i] = true;
		}
		if (a_operand) return t_operand(t_operand::c_tag__VARIABLE, v_variable->v_index);
		if (a_tail) {
			a_emit.f_emit_safe_point(this);
			a_emit << c_instruction__RETURN_V;
		} else {
			a_emit << c_instruction__STACK_GET << a_emit.v_stack;
		}
	}
	a_emit << v_variable->v_index;
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
		case t_operand::c_tag__INTEGER:
			a_emit << c_instruction__INTEGER << v_variable.v_index << operand.v_integer;
			break;
		case t_operand::c_tag__FLOAT:
			a_emit << c_instruction__FLOAT << v_variable.v_index << operand.v_float;
			break;
		case t_operand::c_tag__LITERAL:
			if (operand.v_value)
				a_emit << c_instruction__INSTANCE << v_variable.v_index << operand.v_value;
			else
				a_emit << c_instruction__NUL << v_variable.v_index;
			break;
		case t_operand::c_tag__VARIABLE:
			a_emit << c_instruction__STACK_PUT << operand.v_index << v_variable.v_index;
			break;
		default:
			a_emit << c_instruction__STACK_PUT << a_emit.v_stack - 1 << v_variable.v_index;
			a_emit.f_pop();
		}
		a_emit.f_at(this);
		return {};
	}
	v_value->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	if (v_variable.v_shared) {
		if (v_outer > 0)
			a_emit << c_instruction__SCOPE_PUT << a_emit.v_stack - 1 << v_outer;
		else
			a_emit << c_instruction__SCOPE_PUT0 << a_emit.v_stack - 1;
	} else {
		int i = v_variable.v_index - a_emit.v_arguments;
		if (i >= 0) (*a_emit.v_privates)[i] = true;
		a_emit << c_instruction__STACK_PUT << a_emit.v_stack - 1;
	}
	a_emit << v_variable.v_index;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

t_operand t_self::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear) return {};
	if (a_tail) a_emit.f_emit_safe_point(this);
	if (v_outer > 0) {
		a_emit << static_cast<t_instruction>(c_instruction__SCOPE_GET0 + (v_outer < 3 ? v_outer : 3)) << a_emit.v_stack;
		if (v_outer >= 3) a_emit << v_outer;
		a_emit << 0;
	} else {
		a_emit << c_instruction__SELF << a_emit.v_stack;
	}
	a_emit.f_push();
	a_emit.f_at(this);
	return {};
}

void t_class::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_class::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	if (a_tail) a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__CLASS << a_emit.v_stack - 1;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

void t_super::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
}

t_operand t_super::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_emit(a_emit, false, false);
	if (a_tail) a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__SUPER << a_emit.v_stack - 1;
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

t_operand t_null::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return nullptr;
	if (a_clear) return {};
	if (a_tail) {
		a_emit.f_emit_safe_point(this);
		(a_emit << c_instruction__RETURN_NUL).f_push();
	} else {
		a_emit.f_emit_null();
	}
	a_emit.f_at(this);
	return {};
}

void t_unary::f_flow(t_flow& a_flow)
{
	v_expression->f_flow(a_flow);
}

t_operand t_unary::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	a_emit.f_push();
	auto operand = v_expression->f_emit(a_emit, false, true);
	if (operand.v_tag == t_operand::c_tag__INTEGER) {
		a_emit.f_pop();
		switch (v_instruction) {
		case c_instruction__PLUS_T:
			return t_literal(v_at, operand.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
		case c_instruction__MINUS_T:
			return t_literal(v_at, -operand.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
		case c_instruction__NOT_T:
			return t_null(v_at).f_emit(a_emit, a_tail, a_operand, a_clear);
		case c_instruction__COMPLEMENT_T:
			return t_literal(v_at, ~operand.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
		default:
			assert(false);
		}
	} else if (operand.v_tag == t_operand::c_tag__FLOAT) {
		a_emit.f_pop();
		switch (v_instruction) {
		case c_instruction__PLUS_T:
			return t_literal(v_at, operand.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
		case c_instruction__MINUS_T:
			return t_literal(v_at, -operand.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
		case c_instruction__NOT_T:
			return t_null(v_at).f_emit(a_emit, a_tail, a_operand, a_clear);
		default:
			f_throw(L"not supported."sv);
		}
	}
	size_t instruction = v_instruction;
	if (a_tail) instruction += c_instruction__CALL_TAIL - c_instruction__CALL;
	switch (operand.v_tag) {
	case t_operand::c_tag__LITERAL:
		instruction += c_instruction__PLUS_L - c_instruction__PLUS_T;
		break;
	case t_operand::c_tag__VARIABLE:
		instruction += c_instruction__PLUS_V - c_instruction__PLUS_T;
		break;
	}
	a_emit.f_emit_safe_point(this);
	if (operand.v_tag != t_operand::c_tag__TEMPORARY) a_emit.f_push();
	a_emit.f_pop().f_pop();
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.v_size);
	if (!a_tail) a_emit << a_emit.v_stack;
	switch (operand.v_tag) {
	case t_operand::c_tag__LITERAL:
		a_emit << operand.v_value;
		break;
	case t_operand::c_tag__VARIABLE:
		a_emit << operand.v_index;
		break;
	}
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
	auto boolean = [&](bool a_value)
	{
		return a_value ? t_literal<double>(v_at, 1.0).f_emit(a_emit, a_tail, a_operand, a_clear) : t_null(v_at).f_emit(a_emit, a_tail, a_operand, a_clear);
	};
	if (left.v_tag == t_operand::c_tag__INTEGER) {
		if (right.v_tag == t_operand::c_tag__INTEGER) {
			a_emit.f_pop();
			switch (v_instruction) {
			case c_instruction__MULTIPLY_TT:
				return t_literal(v_at, left.v_integer * right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__DIVIDE_TT:
				return t_literal(v_at, left.v_integer / right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__MODULUS_TT:
				return t_literal(v_at, left.v_integer % right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__ADD_TT:
				return t_literal(v_at, left.v_integer + right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__SUBTRACT_TT:
				return t_literal(v_at, left.v_integer - right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__LEFT_SHIFT_TT:
				return t_literal(v_at, left.v_integer << right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__RIGHT_SHIFT_TT:
				return t_literal<intptr_t>(v_at, static_cast<uintptr_t>(left.v_integer) >> right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__LESS_TT:
				return boolean(left.v_integer < right.v_integer);
			case c_instruction__LESS_EQUAL_TT:
				return boolean(left.v_integer <= right.v_integer);
			case c_instruction__GREATER_TT:
				return boolean(left.v_integer > right.v_integer);
			case c_instruction__GREATER_EQUAL_TT:
				return boolean(left.v_integer >= right.v_integer);
			case c_instruction__EQUALS_TT:
			case c_instruction__IDENTICAL_TT:
				return boolean(left.v_integer == right.v_integer);
			case c_instruction__NOT_EQUALS_TT:
			case c_instruction__NOT_IDENTICAL_TT:
				return boolean(left.v_integer != right.v_integer);
			case c_instruction__AND_TT:
				return t_literal(v_at, left.v_integer & right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__XOR_TT:
				return t_literal(v_at, left.v_integer ^ right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__OR_TT:
				return t_literal(v_at, left.v_integer | right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			default:
				assert(false);
			}
		} else if (right.v_tag == t_operand::c_tag__FLOAT) {
			a_emit.f_pop();
			switch (v_instruction) {
			case c_instruction__MULTIPLY_TT:
				return t_literal(v_at, left.v_integer * right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__DIVIDE_TT:
				return t_literal(v_at, left.v_integer / right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__ADD_TT:
				return t_literal(v_at, left.v_integer + right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__SUBTRACT_TT:
				return t_literal(v_at, left.v_integer - right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__LESS_TT:
				return boolean(left.v_integer < right.v_float);
			case c_instruction__LESS_EQUAL_TT:
				return boolean(left.v_integer <= right.v_float);
			case c_instruction__GREATER_TT:
				return boolean(left.v_integer > right.v_float);
			case c_instruction__GREATER_EQUAL_TT:
				return boolean(left.v_integer >= right.v_float);
			case c_instruction__EQUALS_TT:
				return boolean(left.v_integer == right.v_float);
			case c_instruction__NOT_EQUALS_TT:
				return boolean(left.v_integer != right.v_float);
			case c_instruction__IDENTICAL_TT:
				return boolean(false);
			case c_instruction__NOT_IDENTICAL_TT:
				return boolean(true);
			default:
				f_throw(L"not supported."sv);
			}
		}
	} else if (left.v_tag == t_operand::c_tag__FLOAT) {
		if (right.v_tag == t_operand::c_tag__INTEGER) {
			a_emit.f_pop();
			switch (v_instruction) {
			case c_instruction__MULTIPLY_TT:
				return t_literal(v_at, left.v_float * right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__DIVIDE_TT:
				return t_literal(v_at, left.v_float / right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__ADD_TT:
				return t_literal(v_at, left.v_float + right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__SUBTRACT_TT:
				return t_literal(v_at, left.v_float - right.v_integer).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__LESS_TT:
				return boolean(left.v_float < right.v_integer);
			case c_instruction__LESS_EQUAL_TT:
				return boolean(left.v_float <= right.v_integer);
			case c_instruction__GREATER_TT:
				return boolean(left.v_float > right.v_integer);
			case c_instruction__GREATER_EQUAL_TT:
				return boolean(left.v_float >= right.v_integer);
			case c_instruction__EQUALS_TT:
				return boolean(left.v_float == right.v_integer);
			case c_instruction__NOT_EQUALS_TT:
				return boolean(left.v_float != right.v_integer);
			case c_instruction__IDENTICAL_TT:
				return boolean(false);
			case c_instruction__NOT_IDENTICAL_TT:
				return boolean(true);
			default:
				f_throw(L"not supported."sv);
			}
		} else if (right.v_tag == t_operand::c_tag__FLOAT) {
			a_emit.f_pop();
			switch (v_instruction) {
			case c_instruction__MULTIPLY_TT:
				return t_literal(v_at, left.v_float * right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__DIVIDE_TT:
				return t_literal(v_at, left.v_float / right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__ADD_TT:
				return t_literal(v_at, left.v_float + right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__SUBTRACT_TT:
				return t_literal(v_at, left.v_float - right.v_float).f_emit(a_emit, a_tail, a_operand, a_clear);
			case c_instruction__LESS_TT:
				return boolean(left.v_float < right.v_float);
			case c_instruction__LESS_EQUAL_TT:
				return boolean(left.v_float <= right.v_float);
			case c_instruction__GREATER_TT:
				return boolean(left.v_float > right.v_float);
			case c_instruction__GREATER_EQUAL_TT:
				return boolean(left.v_float >= right.v_float);
			case c_instruction__EQUALS_TT:
			case c_instruction__IDENTICAL_TT:
				return boolean(left.v_float == right.v_float);
			case c_instruction__NOT_EQUALS_TT:
			case c_instruction__NOT_IDENTICAL_TT:
				return boolean(left.v_float != right.v_float);
			case c_instruction__AND_TT:
			case c_instruction__OR_TT:
				return boolean(true);
			case c_instruction__XOR_TT:
				return boolean(false);
			default:
				f_throw(L"not supported."sv);
			}
		}
	}
	size_t instruction = v_instruction;
	if (a_tail) instruction += c_instruction__CALL_TAIL - c_instruction__CALL;
	switch (left.v_tag) {
	case t_operand::c_tag__INTEGER:
		instruction += c_instruction__MULTIPLY_IT - c_instruction__MULTIPLY_TT;
		break;
	case t_operand::c_tag__FLOAT:
		instruction += c_instruction__MULTIPLY_FT - c_instruction__MULTIPLY_TT;
		break;
	case t_operand::c_tag__LITERAL:
		instruction += c_instruction__MULTIPLY_LT - c_instruction__MULTIPLY_TT;
		break;
	case t_operand::c_tag__VARIABLE:
		instruction += c_instruction__MULTIPLY_VT - c_instruction__MULTIPLY_TT;
		break;
	}
	switch (right.v_tag) {
	case t_operand::c_tag__INTEGER:
		instruction += c_instruction__MULTIPLY_TI - c_instruction__MULTIPLY_TT;
		break;
	case t_operand::c_tag__FLOAT:
		instruction += c_instruction__MULTIPLY_TF - c_instruction__MULTIPLY_TT;
		break;
	case t_operand::c_tag__LITERAL:
		instruction += c_instruction__MULTIPLY_TL - c_instruction__MULTIPLY_TT;
		break;
	case t_operand::c_tag__VARIABLE:
		instruction += c_instruction__MULTIPLY_TV - c_instruction__MULTIPLY_TT;
		break;
	}
	a_emit.f_emit_safe_point(this);
	if (left.v_tag != t_operand::c_tag__TEMPORARY) a_emit.f_push();
	if (right.v_tag != t_operand::c_tag__TEMPORARY) a_emit.f_push();
	a_emit.f_pop().f_pop().f_pop();
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.v_size);
	if (!a_tail) a_emit << a_emit.v_stack;
	switch (left.v_tag) {
	case t_operand::c_tag__INTEGER:
		a_emit << left.v_integer;
		break;
	case t_operand::c_tag__FLOAT:
		a_emit << left.v_float;
		break;
	case t_operand::c_tag__LITERAL:
		a_emit << left.v_value;
		break;
	case t_operand::c_tag__VARIABLE:
		a_emit << left.v_index;
		break;
	}
	switch (right.v_tag) {
	case t_operand::c_tag__INTEGER:
		a_emit << right.v_integer;
		break;
	case t_operand::c_tag__FLOAT:
		a_emit << right.v_float;
		break;
	case t_operand::c_tag__LITERAL:
		a_emit << right.v_value;
		break;
	case t_operand::c_tag__VARIABLE:
		a_emit << right.v_index;
		break;
	}
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

t_operand t_preserve::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	if (!a_clear) a_emit.f_push();
	return {};
}

void t_call::f_flow(t_flow& a_flow)
{
	v_target->f_flow(a_flow);
	v_arguments.f_flow(a_flow);
}

t_operand t_call::f_emit(t_emit& a_emit, bool a_tail, bool a_operand, bool a_clear)
{
	size_t instruction = v_expands.v_tail ? c_instruction__CALL_WITH_EXPANSION : c_instruction__CALL;
	t_symbol_get* get = nullptr;
	if (!v_expands.v_tail) {
		auto p = dynamic_cast<t_symbol_get*>(v_target);
		if (p && p->v_variable && (!p->v_variable->v_shared || p->v_resolved < 3 && !p->v_variable->v_varies)) get = p;
	}
	if (get) {
		instruction = get->v_variable->v_shared ? c_instruction__SCOPE_CALL0 + get->v_resolved : c_instruction__STACK_CALL;
		a_emit.f_push().f_push();
	} else if (auto p = dynamic_cast<t_object_get*>(v_target)) {
		p->f_method(a_emit);
	} else if (auto p = dynamic_cast<t_get_at*>(v_target)) {
		p->f_bind(a_emit);
	} else {
		v_target->f_emit(a_emit, false, false);
		a_emit.f_emit_null();
	}
	v_arguments.f_each([&](auto p)
	{
		p->f_emit(a_emit, false, false);
	});
	for (size_t i = 0; i < v_arguments.v_size; ++i) a_emit.f_pop();
	a_emit.f_pop().f_pop();
	if (a_tail) instruction += c_instruction__CALL_TAIL - c_instruction__CALL;
	a_emit.f_emit_safe_point(this);
	a_emit << static_cast<t_instruction>(instruction);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.v_size);
	if (!a_tail) a_emit << a_emit.v_stack;
	if (get) a_emit << get->v_variable->v_index;
	a_emit << v_arguments.v_size;
	if (v_expands.v_tail) {
		size_t i = 0;
		v_expands.f_each([&](auto p)
		{
			a_emit.f_at(p->v_at);
			a_emit << p->v_index - i;
			i = p->v_index + 1;
		});
		a_emit << v_arguments.v_size - i;
	}
	a_emit.f_push();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
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
	a_emit << (a_tail ? c_instruction__GET_AT_TAIL : c_instruction__GET_AT);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.v_size + 3);
	if (!a_tail) a_emit << a_emit.v_stack - 3;
	a_emit.f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

void t_get_at::f_bind(t_emit& a_emit)
{
	v_target->f_emit(a_emit, false, false);
	v_index->f_emit(a_emit, false, false);
	a_emit.f_emit_safe_point(this);
	a_emit << c_instruction__METHOD_BIND << a_emit.v_stack - 2;
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
	a_emit << (a_tail ? c_instruction__SET_AT_TAIL : c_instruction__SET_AT);
	assert(!a_tail || a_emit.v_stack == a_emit.v_scope->v_privates.v_size + 4);
	if (!a_tail) a_emit << a_emit.v_stack - 4;
	a_emit.f_pop().f_pop().f_pop();
	a_emit.f_at(this);
	if (a_clear) a_emit.f_pop();
	return {};
}

}

t_object* t_emit::operator()(ast::t_scope& a_scope)
{
	a_scope.f_analyze(0);
	v_scope = &a_scope;
	v_arguments = 0;
	std::vector<bool> privates(a_scope.v_privates.v_size);
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
		<< c_instruction__STACK_GET << v_stack << 0
		<< c_instruction__SCOPE_PUT0 << v_stack << 0;
	a_scope.v_body->f_emit(*this, false, false, true);
	*this << c_instruction__RETURN_NUL;
	f_resolve();
	if (v_safe_points) {
		t_code::t_variable self;
		self.v_shared = a_scope.v_self_shared;
		self.v_index = a_scope.v_self_shared ? 0 : -1;
		v_code->v_variables.emplace(L"$"sv, self);
		for (auto& [s, v] : a_scope.v_variables) v_code->v_variables.emplace(s->f_as<t_symbol>().f_string(), v);
		for (auto [l, a, c] : safe_positions) v_safe_points->emplace(std::make_pair(l, &v_code->v_instructions[a]), c);
	}
	return code;
}

void t_emit::f_join(const ast::t_block& a_junction)
{
	for (size_t i = 0; i < a_junction.v_privates.size(); ++i) {
		if ((*v_privates)[i]) continue;
		auto& x = a_junction.v_privates[i];
		if (x.v_use && x.v_in) *this << c_instruction__NUL << v_arguments + i;
	}
}

}
