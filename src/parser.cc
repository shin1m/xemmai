#include <xemmai/parser.h>

#include <xemmai/global.h>

namespace xemmai
{

void t_parser::f_throw(const std::wstring& a_message)
{
	throw t_error::f_instantiate(a_message, v_lexer);
}

ast::t_variable& t_parser::f_variable(ast::t_scope* a_scope, const t_value& a_symbol, bool a_loop)
{
	auto i = a_scope->v_variables.find(a_symbol);
	if (i == a_scope->v_variables.end()) {
		i = a_scope->v_variables.emplace_hint(i, a_symbol, ast::t_variable());
		a_scope->v_privates.push_back(&i->second);
		if (a_loop) i->second.v_varies = true;
	} else {
		i->second.v_varies = true;
	}
	return i->second;
}

std::unique_ptr<ast::t_node> t_parser::f_target(bool a_assignable)
{
	t_at at = v_lexer.f_at();
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__COLON:
	case t_lexer::e_token__SYMBOL:
	case t_lexer::e_token__SELF:
		{
			size_t outer = 0;
			ast::t_scope* scope = v_scope;
			if (v_lexer.f_token() == t_lexer::e_token__COLON) {
				outer = v_lexer.f_value().size();
				for (size_t i = 0; i < outer; ++i) {
					if (!scope) t_throwable::f_throw(L"no more outer scope.");
					scope = scope->v_outer;
				}
				v_lexer.f_next();
			}
			switch (v_lexer.f_token()) {
			case t_lexer::e_token__SYMBOL:
				{
					t_scoped symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						if (!a_assignable) t_throwable::f_throw(L"can not assign to expression.");
						if (!scope) t_throwable::f_throw(L"no more outer scope.");
						v_lexer.f_next();
						ast::t_variable& variable = f_variable(scope, symbol, outer > 0 || v_targets->v_loop);
						if (outer > 0) variable.v_shared = true;
						return std::unique_ptr<ast::t_node>(new ast::t_scope_put(at, outer, variable, f_expression()));
					}
					while (scope) {
						auto i = scope->v_variables.find(symbol);
						if (i != scope->v_variables.end()) {
							if (outer > 0) i->second.v_shared = true;
							return std::unique_ptr<ast::t_node>(new ast::t_scope_get(at, outer, i->second));
						}
						++outer;
						scope = scope->v_outer;
					}
					return std::unique_ptr<ast::t_node>(new ast::t_global_get(at, std::move(symbol)));
				}
			case t_lexer::e_token__SELF:
				{
					if (!scope) t_throwable::f_throw(L"no more outer scope.");
					if (outer > 0) scope->v_self_shared = true;
					std::unique_ptr<ast::t_node> target(new ast::t_self(at, outer));
					for (auto c : v_lexer.f_value()) target = std::unique_ptr<ast::t_node>(c == L':' ? static_cast<ast::t_node*>(new ast::t_class(at, std::move(target))) : static_cast<ast::t_node*>(new ast::t_super(at, std::move(target))));
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__SYMBOL) {
						t_at at = v_lexer.f_at();
						t_scoped symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
							if (!a_assignable) t_throwable::f_throw(L"can not assign to expression.");
							v_lexer.f_next();
							return std::unique_ptr<ast::t_node>(new ast::t_object_put(at, std::move(target), std::move(symbol), f_expression()));
						}
						return std::unique_ptr<ast::t_node>(new ast::t_object_get(at, std::move(target), std::move(symbol)));
					}
					return target;
				}
			default:
				f_throw(L"expecting symbol or self.");
			}
		}
	case t_lexer::e_token__APOSTROPHE:
		v_lexer.f_next();
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__SYMBOL:
			{
				t_scoped symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
				v_lexer.f_next();
				return std::unique_ptr<ast::t_node>(new ast::t_instance(at, std::move(symbol)));
			}
		case t_lexer::e_token__LEFT_PARENTHESIS:
			{
				v_lexer.f_next();
				std::unique_ptr<ast::t_call> call(new ast::t_call(at, std::unique_ptr<ast::t_node>(new ast::t_instance(at, f_global()->f_type<t_tuple>()))));
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) {
					call->v_expand = f_expressions(call->v_arguments);
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
				}
				v_lexer.f_next();
				return std::move(call);
			}
		default:
			f_throw(L"expecting symbol or '('.");
		}
	case t_lexer::e_token__LEFT_PARENTHESIS:
		{
			v_lexer.f_next();
			std::unique_ptr<ast::t_node> expression = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			return expression;
		}
	case t_lexer::e_token__ATMARK:
		{
			v_lexer.f_next();
			std::unique_ptr<ast::t_lambda> lambda(new ast::t_lambda(at, v_scope));
			if (v_lexer.f_token() == t_lexer::e_token__LEFT_PARENTHESIS) {
				v_lexer.f_next();
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) {
					while (true) {
						switch (v_lexer.f_token()) {
						case t_lexer::e_token__SYMBOL:
							lambda->v_privates.push_back(&lambda->v_variables.emplace(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end())), ast::t_variable()).first->second);
							v_lexer.f_next();
							if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
								v_lexer.f_next();
								lambda->v_defaults.push_back(f_expression());
							} else {
								if (lambda->v_defaults.size() > 0) f_throw(L"expecting '='.");
							}
							if (v_lexer.f_token() == t_lexer::e_token__COMMA) {
								v_lexer.f_next();
								continue;
							}
							break;
						case t_lexer::e_token__ASTERISK:
							v_lexer.f_next();
							if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
							lambda->v_privates.push_back(&lambda->v_variables.emplace(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end())), ast::t_variable()).first->second);
							lambda->v_variadic = true;
							v_lexer.f_next();
							break;
						default:
							f_throw(L"expecting symbol or '*'.");
						}
						break;
					}
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
				}
				v_lexer.f_next();
			}
			lambda->v_arguments = lambda->v_variables.size();
			v_scope->v_shared = true;
			v_scope = lambda.get();
			t_targets* targets0 = v_targets;
			t_targets targets1(false, true);
			v_targets = &targets1;
			if (v_lexer.f_token() == t_lexer::e_token__LEFT_BRACE)
				f_block(lambda->v_block);
			else
				lambda->v_block.push_back(f_expression());
			v_scope = lambda->v_outer;
			v_targets = targets0;
			std::vector<ast::t_variable*> variables;
			variables.swap(lambda->v_privates);
			if (lambda->v_self_shared) ++lambda->v_shareds;
			auto i = variables.begin();
			for (auto j = i + lambda->v_arguments; i != j; ++i) {
				ast::t_variable* p = *i;
				p->v_index = p->v_shared ? lambda->v_shareds++ : lambda->v_privates.size();
				lambda->v_privates.push_back(p);
			}
			for (; i != variables.end(); ++i) {
				ast::t_variable* p = *i;
				if (p->v_shared) {
					p->v_index = lambda->v_shareds++;
				} else {
					p->v_index = sizeof(t_fiber::t_context) / sizeof(t_slot) + lambda->v_privates.size();
					lambda->v_privates.push_back(p);
				}
			}
			return std::move(lambda);
		}
	case t_lexer::e_token__LEFT_BRACKET:
		{
			v_lexer.f_next();
			std::unique_ptr<ast::t_call> call(new ast::t_call(at, std::unique_ptr<ast::t_node>(new ast::t_instance(at, f_global()->f_type<t_array>()))));
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) {
				call->v_expand = f_expressions(call->v_arguments);
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
			}
			v_lexer.f_next();
			return std::move(call);
		}
	case t_lexer::e_token__LEFT_BRACE:
		{
			v_lexer.f_next();
			std::unique_ptr<ast::t_call> call(new ast::t_call(at, std::unique_ptr<ast::t_node>(new ast::t_instance(at, f_global()->f_type<t_dictionary>()))));
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) {
				call->v_arguments.push_back(f_expression());
				if (!f_single_colon()) f_throw(L"expecting ':'.");
				v_lexer.f_next();
				call->v_arguments.push_back(f_expression());
				while (v_lexer.f_token() == t_lexer::e_token__COMMA) {
					v_lexer.f_next();
					call->v_arguments.push_back(f_expression());
					if (!f_single_colon()) f_throw(L"expecting ':'.");
					v_lexer.f_next();
					call->v_arguments.push_back(f_expression());
				}
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) f_throw(L"expecting '}'.");
			}
			v_lexer.f_next();
			return std::move(call);
		}
	case t_lexer::e_token__NULL:
		v_lexer.f_next();
		return std::unique_ptr<ast::t_node>(new ast::t_null(at));
	case t_lexer::e_token__TRUE:
		v_lexer.f_next();
		return std::unique_ptr<ast::t_node>(new ast::t_boolean(at, true));
	case t_lexer::e_token__FALSE:
		v_lexer.f_next();
		return std::unique_ptr<ast::t_node>(new ast::t_boolean(at, false));
	case t_lexer::e_token__INTEGER:
		{
			ptrdiff_t value = f_integer();
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_integer(at, value));
		}
	case t_lexer::e_token__FLOAT:
		{
			double value = f_float();
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_float(at, value));
		}
	case t_lexer::e_token__STRING:
		{
			std::wstring value(v_lexer.f_value().begin(), v_lexer.f_value().end());
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_instance(at, f_global()->f_as(value)));
		}
	default:
		f_throw(L"unexpected token.");
	}
}

std::unique_ptr<ast::t_node> t_parser::f_action(std::unique_ptr<ast::t_node>&& a_target, bool a_assignable)
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__LEFT_PARENTHESIS:
		{
			t_at at = v_lexer.f_at();
			std::unique_ptr<ast::t_call> call(new ast::t_call(at, std::move(a_target)));
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) {
				call->v_expand = f_expressions(call->v_arguments);
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			}
			v_lexer.f_next();
			return f_action(std::move(call), a_assignable);
		}
	case t_lexer::e_token__DOT:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			t_lexer::t_token token = v_lexer.f_token();
			switch (token) {
			case t_lexer::e_token__LEFT_PARENTHESIS:
				{
					v_lexer.f_next();
					std::unique_ptr<ast::t_node> key = f_expression();
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						if (!a_assignable) t_throwable::f_throw(L"can not assign to expression.");
						v_lexer.f_next();
						return std::unique_ptr<ast::t_node>(new ast::t_object_put_indirect(at, std::move(a_target), std::move(key), f_expression()));
					}
					return f_action(new ast::t_object_get_indirect(at, std::move(a_target), std::move(key)), a_assignable);
				}
			case t_lexer::e_token__SYMBOL:
				{
					t_scoped symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						if (!a_assignable) t_throwable::f_throw(L"can not assign to expression.");
						v_lexer.f_next();
						return std::unique_ptr<ast::t_node>(new ast::t_object_put(at, std::move(a_target), std::move(symbol), f_expression()));
					}
					return f_action(new ast::t_object_get(at, std::move(a_target), std::move(symbol)), a_assignable);
				}
			case t_lexer::e_token__COLON:
				if (v_lexer.f_value().size() != 1) f_throw(L"expecting '(' or ':' or '?' or '^' or '~' or symbol.");
				v_lexer.f_next();
				return f_action(new ast::t_class(at, std::move(a_target)), a_assignable);
			case t_lexer::e_token__HAT:
				v_lexer.f_next();
				return f_action(new ast::t_super(at, std::move(a_target)), a_assignable);
			case t_lexer::e_token__QUESTION:
			case t_lexer::e_token__TILDE:
				v_lexer.f_next();
				switch (v_lexer.f_token()) {
				case t_lexer::e_token__LEFT_PARENTHESIS:
					{
						v_lexer.f_next();
						std::unique_ptr<ast::t_node> key = f_expression();
						if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
						v_lexer.f_next();
						return f_action(token == t_lexer::e_token__QUESTION ? static_cast<ast::t_node*>(new ast::t_object_has_indirect(at, std::move(a_target), std::move(key))) : static_cast<ast::t_node*>(new ast::t_object_remove_indirect(at, std::move(a_target), std::move(key))), a_assignable);
					}
				case t_lexer::e_token__SYMBOL:
					{
						t_scoped key = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
						v_lexer.f_next();
						return f_action(token == t_lexer::e_token__QUESTION ? static_cast<ast::t_node*>(new ast::t_object_has(at, std::move(a_target), std::move(key))) : static_cast<ast::t_node*>(new ast::t_object_remove(at, std::move(a_target), std::move(key))), a_assignable);
					}
				default:
					f_throw(L"expecting '(' or symbol.");
				}
			default:
				f_throw(L"expecting '(' or ':' or '?' or '^' or '~' or symbol.");
			}
		}
	case t_lexer::e_token__LEFT_BRACKET:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			std::unique_ptr<ast::t_node> index = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
			v_lexer.f_next();
			if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
				if (!a_assignable) t_throwable::f_throw(L"can not assign to expression.");
				v_lexer.f_next();
				return std::unique_ptr<ast::t_node>(new ast::t_set_at(at, std::move(a_target), std::move(index), f_expression()));
			}
			return f_action(new ast::t_get_at(at, std::move(a_target), std::move(index)), a_assignable);
		}
	default:
		return std::move(a_target);
	}
}

std::unique_ptr<ast::t_node> t_parser::f_unary(bool a_assignable)
{
	t_instruction instruction;
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__PLUS:
		instruction = e_instruction__PLUS_T;
		break;
	case t_lexer::e_token__HYPHEN:
		instruction = e_instruction__MINUS_T;
		break;
	case t_lexer::e_token__EXCLAMATION:
		instruction = e_instruction__NOT_T;
		break;
	case t_lexer::e_token__TILDE:
		instruction = e_instruction__COMPLEMENT_T;
		break;
	default:
		return f_action(f_target(a_assignable), a_assignable);
	}
	t_at at = v_lexer.f_at();
	v_lexer.f_next();
	switch (instruction) {
	case e_instruction__PLUS_T:
	case e_instruction__MINUS_T:
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__INTEGER:
			{
				ptrdiff_t value = f_integer();
				v_lexer.f_next();
				return f_action(new ast::t_integer(at, instruction == e_instruction__MINUS_T ? -value : value), a_assignable);
			}
		case t_lexer::e_token__FLOAT:
			{
				double value = f_float();
				v_lexer.f_next();
				return f_action(new ast::t_float(at, instruction == e_instruction__MINUS_T ? -value : value), a_assignable);
			}
		}
		break;
	case e_instruction__COMPLEMENT_T:
		if (v_lexer.f_token() == t_lexer::e_token__INTEGER) {
			ptrdiff_t value = f_integer();
			v_lexer.f_next();
			return f_action(new ast::t_integer(at, ~value), a_assignable);
		}
		break;
	}
	return std::unique_ptr<ast::t_node>(new ast::t_unary(at, instruction, f_unary(false)));
}

std::unique_ptr<ast::t_node> t_parser::f_multiplicative(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_unary(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__ASTERISK:
			instruction = e_instruction__MULTIPLY_TT;
			break;
		case t_lexer::e_token__SLASH:
			instruction = e_instruction__DIVIDE_TT;
			break;
		case t_lexer::e_token__PERCENT:
			instruction = e_instruction__MODULUS_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, instruction, std::move(node), f_unary(false)));
	}
}

std::unique_ptr<ast::t_node> t_parser::f_additive(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_multiplicative(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__PLUS:
			instruction = e_instruction__ADD_TT;
			break;
		case t_lexer::e_token__HYPHEN:
			instruction = e_instruction__SUBTRACT_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, instruction, std::move(node), f_multiplicative(false)));
	}
}

std::unique_ptr<ast::t_node> t_parser::f_shift(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_additive(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__LEFT_SHIFT:
			instruction = e_instruction__LEFT_SHIFT_TT;
			break;
		case t_lexer::e_token__RIGHT_SHIFT:
			instruction = e_instruction__RIGHT_SHIFT_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, instruction, std::move(node), f_additive(false)));
	}
}

std::unique_ptr<ast::t_node> t_parser::f_relational(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_shift(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__LESS:
			instruction = e_instruction__LESS_TT;
			break;
		case t_lexer::e_token__LESS_EQUAL:
			instruction = e_instruction__LESS_EQUAL_TT;
			break;
		case t_lexer::e_token__GREATER:
			instruction = e_instruction__GREATER_TT;
			break;
		case t_lexer::e_token__GREATER_EQUAL:
			instruction = e_instruction__GREATER_EQUAL_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, instruction, std::move(node), f_shift(false)));
	}
}

std::unique_ptr<ast::t_node> t_parser::f_equality(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_relational(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__EQUALS:
			instruction = e_instruction__EQUALS_TT;
			break;
		case t_lexer::e_token__NOT_EQUALS:
			instruction = e_instruction__NOT_EQUALS_TT;
			break;
		case t_lexer::e_token__IDENTICAL:
			instruction = e_instruction__IDENTICAL_TT;
			break;
		case t_lexer::e_token__NOT_IDENTICAL:
			instruction = e_instruction__NOT_IDENTICAL_TT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, instruction, std::move(node), f_relational(false)));
	}
}

std::unique_ptr<ast::t_node> t_parser::f_and(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_equality(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__AMPERSAND) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, e_instruction__AND_TT, std::move(node), f_equality(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_xor(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_and(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__HAT) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, e_instruction__XOR_TT, std::move(node), f_and(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_or(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_xor(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__BAR) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, e_instruction__OR_TT, std::move(node), f_xor(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_and_also(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_or(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__AND_ALSO) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		std::unique_ptr<ast::t_if> branch(new ast::t_if(at, std::move(node)));
		branch->v_true.push_back(f_or(false));
		branch->v_false.push_back(std::unique_ptr<ast::t_node>(new ast::t_boolean(at, false)));
		node = std::move(branch);
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_or_else(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_and_also(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__OR_ELSE) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		std::unique_ptr<ast::t_if> branch(new ast::t_if(at, std::move(node)));
		branch->v_true.push_back(std::unique_ptr<ast::t_node>(new ast::t_boolean(at, true)));
		branch->v_false.push_back(f_and_also(false));
		node = std::move(branch);
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_send(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_or_else(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__COLON && v_lexer.f_value().size() == 2) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = std::unique_ptr<ast::t_node>(new ast::t_binary(at, e_instruction__SEND, std::move(node), f_or_else(false)));
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_conditional(bool a_assignable)
{
	std::unique_ptr<ast::t_node> node = f_send(a_assignable);
	if (v_lexer.f_token() != t_lexer::e_token__QUESTION) return node;
	t_at at = v_lexer.f_at();
	v_lexer.f_next();
	std::unique_ptr<ast::t_if> branch(new ast::t_if(at, std::move(node)));
	branch->v_true.push_back(f_conditional(false));
	if (!f_single_colon()) f_throw(L"expecting ':'.");
	v_lexer.f_next();
	branch->v_false.push_back(f_conditional(false));
	return std::move(branch);
}

std::unique_ptr<ast::t_node> t_parser::f_expression()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__IF:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			std::unique_ptr<ast::t_if> node(new ast::t_if(at, f_expression()));
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			f_block_or_statement(node->v_true);
			if (v_lexer.f_token() == t_lexer::e_token__ELSE) {
				v_lexer.f_next();
				f_block_or_statement(node->v_false);
			}
			return std::move(node);
		}
	case t_lexer::e_token__WHILE:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			std::unique_ptr<ast::t_while> node(new ast::t_while(at, f_expression()));
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			t_targets* targets0 = v_targets;
			t_targets targets1(true, targets0->v_return);
			v_targets = &targets1;
			f_block_or_statement(node->v_block);
			v_targets = targets0;
			return std::move(node);
		}
	case t_lexer::e_token__FOR:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			std::unique_ptr<ast::t_for> node(new ast::t_for(at));
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) {
				while (true) {
					node->v_initialization.push_back(f_expression());
					if (v_lexer.f_token() != t_lexer::e_token__COMMA) break;
					v_lexer.f_next();
				}
			}
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) node->v_condition = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) {
				while (true) {
					node->v_next.push_back(f_expression());
					if (v_lexer.f_token() != t_lexer::e_token__COMMA) break;
					v_lexer.f_next();
				}
			}
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			t_targets* targets0 = v_targets;
			t_targets targets1(true, targets0->v_return);
			v_targets = &targets1;
			f_block_or_statement(node->v_block);
			v_targets = targets0;
			return std::move(node);
		}
	case t_lexer::e_token__TRY:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
			std::unique_ptr<ast::t_try> node(new ast::t_try(at));
			t_targets* targets0 = v_targets;
			bool catching = false;
			{
				t_targets targets1(targets0->v_loop, targets0->v_return);
				v_targets = &targets1;
				f_block(node->v_block);
				while (v_lexer.f_token() == t_lexer::e_token__CATCH) {
					catching = true;
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
					v_lexer.f_next();
					std::unique_ptr<ast::t_node> expression = f_expression();
					if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
					t_scoped symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
					std::unique_ptr<ast::t_try::t_catch> c(new ast::t_try::t_catch(std::move(expression), f_variable(v_scope, symbol, v_targets->v_loop)));
					f_block(c->v_block);
					node->v_catches.push_back(std::move(c));
				}
			}
			if (v_lexer.f_token() == t_lexer::e_token__FINALLY) {
				v_lexer.f_next();
				if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
				t_targets targets2(false, false);
				v_targets = &targets2;
				f_block(node->v_finally);
			} else {
				if (!catching) f_throw(L"expecting 'finally'.");
			}
			v_targets = targets0;
			return std::move(node);
		}
	default:
		return f_conditional(true);
	}
}

bool t_parser::f_expressions(std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	while (true) {
		if (v_lexer.f_token() == t_lexer::e_token__ASTERISK) {
			v_lexer.f_next();
			a_nodes.push_back(f_expression());
			return true;
		}
		a_nodes.push_back(f_expression());
		if (v_lexer.f_token() != t_lexer::e_token__COMMA) break;
		v_lexer.f_next();
	}
	return false;
}

std::unique_ptr<ast::t_node> t_parser::f_statement()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__IF:
	case t_lexer::e_token__WHILE:
	case t_lexer::e_token__FOR:
	case t_lexer::e_token__TRY:
		return f_expression();
	case t_lexer::e_token__BREAK:
		{
			if (!v_targets->v_loop) f_throw(L"expecting within loop.");
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			std::unique_ptr<ast::t_node> expression;
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) {
				expression = f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			}
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_break(at, std::move(expression)));
		}
	case t_lexer::e_token__CONTINUE:
		{
			if (!v_targets->v_loop) f_throw(L"expecting within loop.");
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_continue(at));
		}
	case t_lexer::e_token__RETURN:
		{
			if (!v_targets->v_return) f_throw(L"expecting within lambda.");
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			std::unique_ptr<ast::t_node> expression;
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) {
				expression = f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			}
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_return(at, std::move(expression)));
		}
	case t_lexer::e_token__THROW:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			std::unique_ptr<ast::t_node> expression = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			return std::unique_ptr<ast::t_node>(new ast::t_throw(at, std::move(expression)));
		}
	default:
		{
			std::unique_ptr<ast::t_node> expression = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			return expression;
		}
	}
}

void t_parser::f_statements(std::vector<std::unique_ptr<ast::t_node>>& a_nodes, t_lexer::t_token a_token)
{
	while (v_lexer.f_token() != a_token) {
		if (v_lexer.f_token() == t_lexer::e_token__SEMICOLON)
			v_lexer.f_next();
		else
			a_nodes.push_back(f_statement());
	}
}

void t_parser::f_block(std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	v_lexer.f_next();
	f_statements(a_nodes, t_lexer::e_token__RIGHT_BRACE);
	v_lexer.f_next();
}

void t_parser::f_block_or_statement(std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	if (v_lexer.f_token() == t_lexer::e_token__SEMICOLON)
		v_lexer.f_next();
	else if (v_lexer.f_token() == t_lexer::e_token__LEFT_BRACE)
		f_block(a_nodes);
	else
		a_nodes.push_back(f_statement());
}

void t_parser::f_parse(ast::t_module& a_module)
{
	v_scope = &a_module;
	t_targets targets(false, false);
	v_targets = &targets;
	f_statements(a_module.v_block, t_lexer::e_token__EOF);
	std::vector<ast::t_variable*> variables;
	variables.swap(a_module.v_privates);
	if (a_module.v_self_shared) ++a_module.v_shareds;
	for (auto p : variables) {
		if (p->v_shared) {
			p->v_index = a_module.v_shareds++;
		} else {
			p->v_index = sizeof(t_fiber::t_context) / sizeof(t_slot) + a_module.v_privates.size();
			a_module.v_privates.push_back(p);
		}
	}
}

t_scoped t_parser::t_error::f_instantiate(const std::wstring& a_message, t_lexer& a_lexer)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_error>());
	object.f_pointer__(new t_error(a_message, a_lexer));
	return object;
}

void t_parser::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%" XEMMAI__PORTABLE__FORMAT_SIZE_T "d:%" XEMMAI__PORTABLE__FORMAT_SIZE_T "d\n", v_path.c_str(), v_at.f_line(), v_at.f_column());
	f_print_with_caret(v_path.c_str(), v_at.f_position(), v_at.f_column());
	t_throwable::f_dump();
}

t_type* t_type_of<t_parser::t_error>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_parser::t_error>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
