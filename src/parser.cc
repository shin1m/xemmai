#include <xemmai/global.h>

namespace xemmai
{

t_code::t_variable& t_parser::f_variable(ast::t_scope* a_scope, t_object* a_symbol)
{
	auto i = a_scope->v_variables.find(a_symbol);
	if (i == a_scope->v_variables.end()) {
		i = a_scope->v_variables.emplace_hint(i, a_symbol, t_code::t_variable());
		a_scope->v_privates.push_back(&i->second);
	} else {
		i->second.v_varies = true;
	}
	return i->second;
}

std::unique_ptr<ast::t_node> t_parser::f_target(bool a_assignable)
{
	t_at at = v_lexer.f_at();
	switch (v_lexer.f_token()) {
	case t_lexer::c_token__COLON:
	case t_lexer::c_token__SYMBOL:
	case t_lexer::c_token__SELF:
		{
			size_t outer = 0;
			auto scope = v_scope;
			if (v_lexer.f_token() == t_lexer::c_token__COLON) {
				outer = v_lexer.f_integer();
				for (size_t i = 0; i < outer; ++i) {
					if (!scope) f_throw(L"no more outer scope."sv);
					scope = scope->v_outer;
				}
				v_lexer.f_next();
			}
			switch (v_lexer.f_token()) {
			case t_lexer::c_token__SYMBOL:
				{
					auto symbol = f_symbol();
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
						if (!a_assignable) f_throw(L"can not assign to expression."sv);
						if (!scope) f_throw(L"no more outer scope."sv);
						v_lexer.f_next();
						auto& variable = f_variable(scope, symbol);
						if (outer > 0) variable.v_shared = variable.v_varies = true;
						return std::make_unique<ast::t_scope_put>(at, outer, variable, f_expression());
					}
					if (scope) (outer > 0 ? scope->v_unresolveds : scope->v_references).insert(symbol);
					return std::make_unique<ast::t_symbol_get>(at, outer, scope, symbol);
				}
			case t_lexer::c_token__SELF:
				{
					if (!scope) f_throw(L"no more outer scope."sv);
					if (outer > 0) scope->v_self_shared = true;
					std::unique_ptr<ast::t_node> target(new ast::t_self(at, outer));
					for (auto c : v_lexer.f_value()) target.reset(c == L'@' ? new ast::t_class(at, std::move(target)) : static_cast<ast::t_node*>(new ast::t_super(at, std::move(target))));
					v_lexer.f_next();
					if (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__SYMBOL) {
						t_at at = v_lexer.f_at();
						auto key = f_symbol();
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
							if (!a_assignable) f_throw(L"can not assign to expression."sv);
							v_lexer.f_next();
							return std::make_unique<ast::t_object_put>(at, std::move(target), key, f_expression());
						}
						return std::make_unique<ast::t_object_get>(at, std::move(target), key);
					}
					return target;
				}
			default:
				f_throw(L"expecting symbol or self."sv);
			}
		}
	case t_lexer::c_token__APOSTROPHE:
		v_lexer.f_next();
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__SYMBOL:
			{
				auto symbol = f_symbol();
				v_lexer.f_next();
				return std::make_unique<ast::t_literal<t_object*>>(at, symbol);
			}
		case t_lexer::c_token__LEFT_PARENTHESIS:
			{
				size_t indent = v_lexer.f_indent();
				v_lexer.f_next();
				auto call = std::make_unique<ast::t_call>(at, std::make_unique<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_tuple>()))));
				if ((!v_lexer.f_newline() || v_lexer.f_indent() > indent) && v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) call->v_expand = f_expressions(indent, call->v_arguments);
				if ((!v_lexer.f_newline() || v_lexer.f_indent() >= indent) && v_lexer.f_token() == t_lexer::c_token__RIGHT_PARENTHESIS) v_lexer.f_next();
				return call;
			}
		default:
			f_throw(L"expecting symbol or '('."sv);
		}
	case t_lexer::c_token__LEFT_PARENTHESIS:
		{
			v_lexer.f_next();
			auto expression = f_expression();
			if (v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'."sv);
			v_lexer.f_next();
			return expression;
		}
	case t_lexer::c_token__ATMARK:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto lambda = std::make_unique<ast::t_lambda>(at, v_scope);
			if (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__LEFT_PARENTHESIS) {
				v_lexer.f_next();
				if (v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) {
					while (true) {
						switch (v_lexer.f_token()) {
						case t_lexer::c_token__SYMBOL:
							lambda->v_privates.push_back(&lambda->v_variables.emplace(f_symbol(), t_code::t_variable()).first->second);
							v_lexer.f_next();
							if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
								v_lexer.f_next();
								lambda->v_defaults.push_back(f_expression());
							} else {
								if (lambda->v_defaults.size() > 0) f_throw(L"expecting '='."sv);
							}
							if (v_lexer.f_newline() && v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) continue;
							if (v_lexer.f_token() == t_lexer::c_token__COMMA) {
								v_lexer.f_next();
								continue;
							}
							break;
						case t_lexer::c_token__ASTERISK:
							v_lexer.f_next();
							if (v_lexer.f_token() != t_lexer::c_token__SYMBOL) f_throw(L"expecting symbol."sv);
							lambda->v_privates.push_back(&lambda->v_variables.emplace(f_symbol(), t_code::t_variable()).first->second);
							lambda->v_variadic = true;
							v_lexer.f_next();
							break;
						default:
							f_throw(L"expecting symbol or '*'."sv);
						}
						break;
					}
					if (v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'."sv);
				}
				v_lexer.f_next();
			}
			lambda->v_arguments = lambda->v_variables.size();
			v_scope->v_shared = true;
			v_scope = lambda.get();
			bool can_jump = v_can_jump;
			bool can_return = v_can_return;
			v_can_return = true;
			if (v_lexer.f_newline()) {
				if (v_lexer.f_indent() > indent) f_block(lambda->v_block);
			} else {
				lambda->v_block.push_back(f_expression());
				if (v_lexer.f_newline() && v_lexer.f_indent() > indent) f_throw(L"unexpected indent."sv);
			}
			for (auto symbol : lambda->v_unresolveds) {
				auto i = lambda->v_variables.find(symbol);
				if (i == lambda->v_variables.end())
					lambda->v_outer->v_unresolveds.insert(symbol);
				else
					i->second.v_shared = true;
			}
			for (auto symbol : lambda->v_references) {
				auto i = lambda->v_variables.find(symbol);
				if (i == lambda->v_variables.end()) lambda->v_outer->v_unresolveds.insert(symbol);
			}
			v_scope = lambda->v_outer;
			v_can_jump = can_jump;
			v_can_return = can_return;
			std::vector<t_code::t_variable*> variables;
			variables.swap(lambda->v_privates);
			if (lambda->v_self_shared) ++lambda->v_shareds;
			auto i = variables.begin();
			for (auto j = i + lambda->v_arguments; i != j; ++i) {
				t_code::t_variable* p = *i;
				p->v_index = p->v_shared ? lambda->v_shareds++ : lambda->v_privates.size();
				lambda->v_privates.push_back(p);
			}
			for (; i != variables.end(); ++i) {
				t_code::t_variable* p = *i;
				if (p->v_shared) {
					p->v_index = lambda->v_shareds++;
				} else {
					p->v_index = lambda->v_privates.size();
					lambda->v_privates.push_back(p);
				}
			}
			return lambda;
		}
	case t_lexer::c_token__LEFT_BRACKET:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto call = std::make_unique<ast::t_call>(at, std::make_unique<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_list>()))));
			if ((!v_lexer.f_newline() || v_lexer.f_indent() > indent) && v_lexer.f_token() != t_lexer::c_token__RIGHT_BRACKET) call->v_expand = f_expressions(indent, call->v_arguments);
			if ((!v_lexer.f_newline() || v_lexer.f_indent() >= indent) && v_lexer.f_token() == t_lexer::c_token__RIGHT_BRACKET) v_lexer.f_next();
			return call;
		}
	case t_lexer::c_token__LEFT_BRACE:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto call = std::make_unique<ast::t_call>(at, std::make_unique<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_map>()))));
			if ((!v_lexer.f_newline() || v_lexer.f_indent() > indent) && v_lexer.f_token() != t_lexer::c_token__RIGHT_BRACE)
				while (true) {
					call->v_arguments.push_back(f_expression());
					if (!f_single_colon()) f_throw(L"expecting ':'."sv);
					v_lexer.f_next();
					call->v_arguments.push_back(f_expression());
					if (v_lexer.f_token() == t_lexer::c_token__COMMA) {
						if (v_lexer.f_newline() && v_lexer.f_indent() < indent) break;
						v_lexer.f_next();
						continue;
					}
					if (!v_lexer.f_newline() || v_lexer.f_indent() <= indent) break;
				}
			if ((!v_lexer.f_newline() || v_lexer.f_indent() >= indent) && v_lexer.f_token() == t_lexer::c_token__RIGHT_BRACE) v_lexer.f_next();
			return call;
		}
	case t_lexer::c_token__NULL:
		v_lexer.f_next();
		return std::make_unique<ast::t_null>(at);
	case t_lexer::c_token__TRUE:
		v_lexer.f_next();
		return std::make_unique<ast::t_literal<bool>>(at, true);
	case t_lexer::c_token__FALSE:
		v_lexer.f_next();
		return std::make_unique<ast::t_literal<bool>>(at, false);
	case t_lexer::c_token__INTEGER:
		{
			auto value = v_lexer.f_integer();
			v_lexer.f_next();
			return std::make_unique<ast::t_literal<intptr_t>>(at, value);
		}
	case t_lexer::c_token__FLOAT:
		{
			auto value = v_lexer.f_float();
			v_lexer.f_next();
			return std::make_unique<ast::t_literal<double>>(at, value);
		}
	case t_lexer::c_token__STRING:
		{
			auto value = v_lexer.f_string();
			v_lexer.f_next();
			return std::make_unique<ast::t_literal<t_object*>>(at, v_module.f_slot(value));
		}
	case t_lexer::c_token__BREAK:
		{
			t_at at = v_lexer.f_at();
			if (!v_can_jump) f_throw(L"expecting inside loop."sv, at);
			v_lexer.f_next();
			return std::make_unique<ast::t_break>(at, f_end_of_expression() ? nullptr : f_expression());
		}
	case t_lexer::c_token__CONTINUE:
		{
			t_at at = v_lexer.f_at();
			if (!v_can_jump) f_throw(L"expecting inside loop."sv, at);
			v_lexer.f_next();
			return std::make_unique<ast::t_continue>(at);
		}
	case t_lexer::c_token__RETURN:
		{
			if (!v_can_return) f_throw(L"expecting within lambda."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return std::make_unique<ast::t_return>(at, f_end_of_expression() ? nullptr : f_expression());
		}
	case t_lexer::c_token__THROW:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return std::make_unique<ast::t_throw>(at, f_expression());
		}
	default:
		f_throw(L"unexpected token."sv);
	}
}

std::unique_ptr<ast::t_node> t_parser::f_action(size_t a_indent, std::unique_ptr<ast::t_node>&& a_target, bool a_assignable)
{
	while (!v_lexer.f_newline() || v_lexer.f_indent() >= a_indent && v_lexer.f_token() == t_lexer::c_token__DOT) {
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__LEFT_PARENTHESIS:
			{
				t_at at = v_lexer.f_at();
				size_t indent = v_lexer.f_indent();
				v_lexer.f_next();
				auto call = std::make_unique<ast::t_call>(at, std::move(a_target));
				if ((!v_lexer.f_newline() || v_lexer.f_indent() > indent) && v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) call->v_expand = f_expressions(indent, call->v_arguments);
				if ((!v_lexer.f_newline() || v_lexer.f_indent() >= indent) && v_lexer.f_token() == t_lexer::c_token__RIGHT_PARENTHESIS) v_lexer.f_next();
				a_target = std::move(call);
				continue;
			}
		case t_lexer::c_token__DOT:
			{
				t_at at = v_lexer.f_at();
				v_lexer.f_next();
				switch (v_lexer.f_token()) {
				case t_lexer::c_token__LEFT_PARENTHESIS:
					{
						v_lexer.f_next();
						auto key = f_expression();
						if (v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'."sv);
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
							if (!a_assignable) f_throw(L"can not assign to expression."sv);
							v_lexer.f_next();
							return std::make_unique<ast::t_object_put_indirect>(at, std::move(a_target), std::move(key), f_expression());
						}
						a_target.reset(new ast::t_object_get_indirect(at, std::move(a_target), std::move(key)));
						continue;
					}
				case t_lexer::c_token__SYMBOL:
					{
						auto key = f_symbol();
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
							if (!a_assignable) f_throw(L"can not assign to expression."sv);
							v_lexer.f_next();
							return std::make_unique<ast::t_object_put>(at, std::move(a_target), key, f_expression());
						}
						a_target.reset(new ast::t_object_get(at, std::move(a_target), key));
						continue;
					}
				case t_lexer::c_token__ATMARK:
					v_lexer.f_next();
					a_target.reset(new ast::t_class(at, std::move(a_target)));
					continue;
				case t_lexer::c_token__HAT:
					v_lexer.f_next();
					a_target.reset(new ast::t_super(at, std::move(a_target)));
					continue;
				case t_lexer::c_token__QUESTION:
					v_lexer.f_next();
					switch (v_lexer.f_token()) {
					case t_lexer::c_token__LEFT_PARENTHESIS:
						{
							v_lexer.f_next();
							auto key = f_expression();
							if (v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'."sv);
							v_lexer.f_next();
							a_target.reset(new ast::t_object_has_indirect(at, std::move(a_target), std::move(key)));
							continue;
						}
					case t_lexer::c_token__SYMBOL:
						{
							auto key = f_symbol();
							v_lexer.f_next();
							a_target.reset(new ast::t_object_has(at, std::move(a_target), key));
							continue;
						}
					default:
						f_throw(L"expecting '(' or symbol."sv);
					}
				default:
					f_throw(L"expecting '(' or '?' or '@' or '^' or symbol."sv);
				}
			}
		case t_lexer::c_token__LEFT_BRACKET:
			{
				t_at at = v_lexer.f_at();
				v_lexer.f_next();
				auto index = f_expression();
				if (v_lexer.f_token() != t_lexer::c_token__RIGHT_BRACKET) f_throw(L"expecting ']'."sv);
				v_lexer.f_next();
				if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
					if (!a_assignable) f_throw(L"can not assign to expression."sv);
					v_lexer.f_next();
					return std::make_unique<ast::t_set_at>(at, std::move(a_target), std::move(index), f_expression());
				}
				a_target.reset(new ast::t_get_at(at, std::move(a_target), std::move(index)));
				continue;
			}
		}
		break;
	}
	return a_target;
}

std::unique_ptr<ast::t_node> t_parser::f_unary(bool a_assignable)
{
	size_t indent = v_lexer.f_indent();
	t_instruction instruction;
	switch (v_lexer.f_token()) {
	case t_lexer::c_token__PLUS:
		instruction = c_instruction__PLUS_T;
		break;
	case t_lexer::c_token__HYPHEN:
		instruction = c_instruction__MINUS_T;
		break;
	case t_lexer::c_token__EXCLAMATION:
		instruction = c_instruction__NOT_T;
		break;
	case t_lexer::c_token__TILDE:
		instruction = c_instruction__COMPLEMENT_T;
		break;
	default:
		return f_action(indent, f_target(a_assignable), a_assignable);
	}
	t_at at = v_lexer.f_at();
	v_lexer.f_next();
	switch (instruction) {
	case c_instruction__PLUS_T:
	case c_instruction__MINUS_T:
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__INTEGER:
			{
				auto value = v_lexer.f_integer();
				v_lexer.f_next();
				return f_action(indent, new ast::t_literal(at, instruction == c_instruction__MINUS_T ? -value : value), a_assignable);
			}
		case t_lexer::c_token__FLOAT:
			{
				auto value = v_lexer.f_float();
				v_lexer.f_next();
				return f_action(indent, new ast::t_literal(at, instruction == c_instruction__MINUS_T ? -value : value), a_assignable);
			}
		}
		break;
	case c_instruction__COMPLEMENT_T:
		if (v_lexer.f_token() == t_lexer::c_token__INTEGER) {
			auto value = v_lexer.f_integer();
			v_lexer.f_next();
			return f_action(indent, new ast::t_literal(at, ~value), a_assignable);
		}
		break;
	}
	return std::make_unique<ast::t_unary>(at, instruction, f_unary(false));
}

std::unique_ptr<ast::t_node> t_parser::f_multiplicative(bool a_assignable)
{
	auto node = f_unary(a_assignable);
	while (!v_lexer.f_newline()) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__ASTERISK:
			instruction = c_instruction__MULTIPLY_TT;
			break;
		case t_lexer::c_token__SLASH:
			instruction = c_instruction__DIVIDE_TT;
			break;
		case t_lexer::c_token__PERCENT:
			instruction = c_instruction__MODULUS_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, instruction, std::move(node), f_unary(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_additive(bool a_assignable)
{
	auto node = f_multiplicative(a_assignable);
	while (!v_lexer.f_newline()) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__PLUS:
			instruction = c_instruction__ADD_TT;
			break;
		case t_lexer::c_token__HYPHEN:
			instruction = c_instruction__SUBTRACT_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, instruction, std::move(node), f_multiplicative(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_shift(bool a_assignable)
{
	auto node = f_additive(a_assignable);
	while (!v_lexer.f_newline()) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__LEFT_SHIFT:
			instruction = c_instruction__LEFT_SHIFT_TT;
			break;
		case t_lexer::c_token__RIGHT_SHIFT:
			instruction = c_instruction__RIGHT_SHIFT_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, instruction, std::move(node), f_additive(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_relational(bool a_assignable)
{
	auto node = f_shift(a_assignable);
	while (!v_lexer.f_newline()) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__LESS:
			instruction = c_instruction__LESS_TT;
			break;
		case t_lexer::c_token__LESS_EQUAL:
			instruction = c_instruction__LESS_EQUAL_TT;
			break;
		case t_lexer::c_token__GREATER:
			instruction = c_instruction__GREATER_TT;
			break;
		case t_lexer::c_token__GREATER_EQUAL:
			instruction = c_instruction__GREATER_EQUAL_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, instruction, std::move(node), f_shift(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_equality(bool a_assignable)
{
	auto node = f_relational(a_assignable);
	while (!v_lexer.f_newline()) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__EQUALS:
			instruction = c_instruction__EQUALS_TT;
			break;
		case t_lexer::c_token__NOT_EQUALS:
			instruction = c_instruction__NOT_EQUALS_TT;
			break;
		case t_lexer::c_token__IDENTICAL:
			instruction = c_instruction__IDENTICAL_TT;
			break;
		case t_lexer::c_token__NOT_IDENTICAL:
			instruction = c_instruction__NOT_IDENTICAL_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, instruction, std::move(node), f_relational(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_and(bool a_assignable)
{
	auto node = f_equality(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__AMPERSAND) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, c_instruction__AND_TT, std::move(node), f_equality(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_xor(bool a_assignable)
{
	auto node = f_and(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__HAT) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, c_instruction__XOR_TT, std::move(node), f_and(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_or(bool a_assignable)
{
	auto node = f_xor(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__BAR) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node.reset(new ast::t_binary(at, c_instruction__OR_TT, std::move(node), f_xor(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_and_also(bool a_assignable)
{
	auto node = f_or(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__AND_ALSO) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		auto branch = std::make_unique<ast::t_if>(at, std::move(node), true);
		branch->v_true.push_back(f_or(false));
		node = std::move(branch);
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_or_else(bool a_assignable)
{
	auto node = f_and_also(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__OR_ELSE) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		auto branch = std::make_unique<ast::t_if>(at, std::move(node), true);
		branch->v_false.push_back(f_and_also(false));
		node = std::move(branch);
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_conditional(bool a_assignable)
{
	auto node = f_or_else(a_assignable);
	if (v_lexer.f_newline() || v_lexer.f_token() != t_lexer::c_token__QUESTION) return node;
	t_at at = v_lexer.f_at();
	v_lexer.f_next();
	auto branch = std::make_unique<ast::t_if>(at, std::move(node));
	branch->v_true.push_back(f_conditional(false));
	if (!f_single_colon()) f_throw(L"expecting ':'."sv);
	v_lexer.f_next();
	branch->v_false.push_back(f_conditional(false));
	return branch;
}

std::unique_ptr<ast::t_node> t_parser::f_expression()
{
	switch (v_lexer.f_token()) {
	case t_lexer::c_token__IF:
		{
			t_at at = v_lexer.f_at();
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = std::make_unique<ast::t_if>(at, f_expression());
			if (v_lexer.f_newline()) {
				if (v_lexer.f_indent() > indent) f_block(node->v_true);
			} else if (f_single_colon()) {
				v_lexer.f_next();
				node->v_true.push_back(f_expression());
			} else {
				f_throw(L"expecting newline or ':'."sv);
			}
			if ((!v_lexer.f_newline() || v_lexer.f_indent() == indent) && v_lexer.f_token() == t_lexer::c_token__ELSE) {
				v_lexer.f_next();
				if (v_lexer.f_newline()) {
					if (v_lexer.f_indent() > indent) f_block(node->v_false);
				} else if (f_single_colon()) {
					v_lexer.f_next();
					node->v_false.push_back(f_expression());
				} else if (v_lexer.f_token() == t_lexer::c_token__IF) {
					node->v_false.push_back(f_expression());
				} else {
					f_throw(L"expecting newline or ':' or 'if'."sv);
				}
			}
			return node;
		}
	case t_lexer::c_token__WHILE:
		{
			t_at at = v_lexer.f_at();
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			size_t varies = v_scope->v_privates.size();
			auto node = std::make_unique<ast::t_while>(at, f_expression());
			bool can_jump = v_can_jump;
			v_can_jump = true;
			f_block_or_expression(indent, node->v_block);
			v_can_jump = can_jump;
			while (varies < v_scope->v_privates.size()) v_scope->v_privates[varies++]->v_varies = true;
			return node;
		}
	case t_lexer::c_token__FOR:
		{
			t_at at = v_lexer.f_at();
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = std::make_unique<ast::t_for>(at);
			if (v_lexer.f_token() != t_lexer::c_token__SEMICOLON) f_expressions(node->v_initialization);
			if (v_lexer.f_token() != t_lexer::c_token__SEMICOLON) f_throw(L"expecting ';'."sv);
			v_lexer.f_next();
			size_t varies = v_scope->v_privates.size();
			if (v_lexer.f_token() != t_lexer::c_token__SEMICOLON) node->v_condition = f_expression();
			if (v_lexer.f_token() != t_lexer::c_token__SEMICOLON) f_throw(L"expecting ';'."sv);
			v_lexer.f_next();
			if (!v_lexer.f_newline() && !f_single_colon()) f_expressions(node->v_next);
			bool can_jump = v_can_jump;
			v_can_jump = true;
			f_block_or_expression(indent, node->v_block);
			v_can_jump = can_jump;
			while (varies < v_scope->v_privates.size()) v_scope->v_privates[varies++]->v_varies = true;
			return node;
		}
	case t_lexer::c_token__TRY:
		{
			t_at at = v_lexer.f_at();
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = std::make_unique<ast::t_try>(at);
			bool catching = false;
			{
				f_block_or_expression(indent, node->v_block);
				while (v_lexer.f_indent() == indent && v_lexer.f_token() == t_lexer::c_token__CATCH) {
					catching = true;
					v_lexer.f_next();
					auto expression = f_expression();
					if (v_lexer.f_token() != t_lexer::c_token__SYMBOL) f_throw(L"expecting symbol."sv);
					auto symbol = f_symbol();
					v_lexer.f_next();
					auto c = std::make_unique<ast::t_try::t_catch>(std::move(expression), f_variable(v_scope, symbol));
					f_block_or_expression(indent, c->v_block);
					node->v_catches.push_back(std::move(c));
				}
			}
			if (v_lexer.f_indent() == indent && v_lexer.f_token() == t_lexer::c_token__FINALLY) {
				v_lexer.f_next();
				bool can_jump = v_can_jump;
				v_can_jump = false;
				bool can_return = v_can_return;
				v_can_return = false;
				f_block_or_expression(indent, node->v_finally);
				v_can_jump = can_jump;
				v_can_return = can_return;
			} else {
				if (!catching) f_throw(L"expecting 'finally'."sv);
			}
			return node;
		}
	default:
		return f_conditional(true);
	}
}

bool t_parser::f_expressions(size_t a_indent, std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	while (v_lexer.f_token() != t_lexer::c_token__ASTERISK) {
		a_nodes.push_back(f_expression());
		if (v_lexer.f_token() == t_lexer::c_token__COMMA) {
			if (v_lexer.f_newline() && v_lexer.f_indent() < a_indent) return false;
			v_lexer.f_next();
			continue;
		}
		if (!v_lexer.f_newline() || v_lexer.f_indent() <= a_indent) return false;
	}
	v_lexer.f_next();
	a_nodes.push_back(f_expression());
	return true;
}

void t_parser::f_block(std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	size_t indent = v_lexer.f_indent();
	do {
		a_nodes.push_back(f_expression());
		if (!v_lexer.f_newline()) f_throw(L"expecting newline."sv);
	} while (v_lexer.f_indent() == indent);
	if (v_lexer.f_indent() > indent) f_throw(L"unexpected indent."sv);
}

void t_parser::f_block_or_expression(size_t a_indent, std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	if (v_lexer.f_newline()) {
		if (v_lexer.f_indent() > a_indent) f_block(a_nodes);
	} else if (f_single_colon()) {
		v_lexer.f_next();
		a_nodes.push_back(f_expression());
	} else {
		f_throw(L"expecting newline or ':'."sv);
	}
}

void t_parser::operator()(ast::t_scope& a_scope)
{
	v_scope = &a_scope;
	size_t indent = v_lexer.f_indent();
	while (v_lexer.f_token() != t_lexer::c_token__EOF) {
		v_scope->v_block.push_back(f_expression());
		if (!v_lexer.f_newline()) f_throw(L"expecting newline."sv);
		if (v_lexer.f_indent() != indent) f_throw(L"unexpected indent."sv);
	}
	for (auto symbol : v_scope->v_unresolveds) {
		auto i = v_scope->v_variables.find(symbol);
		if (i != v_scope->v_variables.end()) i->second.v_shared = true;
	}
	std::vector<t_code::t_variable*> variables;
	variables.swap(v_scope->v_privates);
	if (v_scope->v_self_shared) ++v_scope->v_shareds;
	for (auto p : variables) {
		if (p->v_shared) {
			p->v_index = v_scope->v_shareds++;
		} else {
			p->v_index = v_scope->v_privates.size();
			v_scope->v_privates.push_back(p);
		}
	}
}

t_object* t_parser::t_error::f_instantiate(std::wstring_view a_message, std::wstring_view a_path, const t_at& a_at)
{
	return f_new<t_error>(f_global(), a_message, a_path, a_at);
}

void t_parser::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%zu:%zu\n", v_path.c_str(), v_at.v_line, v_at.v_column);
	f_print_with_caret(stderr, v_path, v_at.v_position, v_at.v_column);
	t_throwable::f_dump();
}

}
