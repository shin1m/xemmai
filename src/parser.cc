#include <xemmai/global.h>

namespace xemmai
{

void t_parser::f_throw(std::wstring_view a_message)
{
	throw t_rvalue(f_new<t_error>(f_global(), a_message, v_lexer.f_path(), v_lexer.f_at()));
}

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

bool t_parser::f_has_expression() const
{
	if (v_lexer.f_newline()) return false;
	switch (v_lexer.f_token()) {
	case t_lexer::c_token__EXCLAMATION:
	case t_lexer::c_token__APOSTROPHE:
	case t_lexer::c_token__LEFT_PARENTHESIS:
	case t_lexer::c_token__PLUS:
	case t_lexer::c_token__HYPHEN:
	case t_lexer::c_token__COLON:
	case t_lexer::c_token__ATMARK:
	case t_lexer::c_token__LEFT_BRACKET:
	case t_lexer::c_token__LEFT_BRACE:
	case t_lexer::c_token__TILDE:
	case t_lexer::c_token__NULL:
	case t_lexer::c_token__TRUE:
	case t_lexer::c_token__FALSE:
	case t_lexer::c_token__SYMBOL:
	case t_lexer::c_token__SELF:
	case t_lexer::c_token__INTEGER:
	case t_lexer::c_token__FLOAT:
	case t_lexer::c_token__STRING:
	case t_lexer::c_token__IF:
	case t_lexer::c_token__WHILE:
	case t_lexer::c_token__FOR:
	case t_lexer::c_token__TRY:
		return true;
	default:
		return false;
	}
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
				if (f_any(indent, t_lexer::c_token__RIGHT_PARENTHESIS)) f_arguments(indent, call.get());
				f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
				return call;
			}
		default:
			f_throw(L"expecting symbol or '('."sv);
		}
	case t_lexer::c_token__LEFT_PARENTHESIS:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto nodes = std::make_unique<ast::t_nodes>();
			if (f_any(indent, t_lexer::c_token__RIGHT_PARENTHESIS)) while (f_argument(indent, *nodes));
			f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
			return nodes;
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
								lambda->v_defaults.emplace_back(f_expression());
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
			lambda->v_body = f_body(indent);
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
			if (f_any(indent, t_lexer::c_token__RIGHT_BRACKET)) f_arguments(indent, call.get());
			f_close(indent, t_lexer::c_token__RIGHT_BRACKET);
			return call;
		}
	case t_lexer::c_token__LEFT_BRACE:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto call = std::make_unique<ast::t_call>(at, std::make_unique<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_map>()))));
			if (f_any(indent, t_lexer::c_token__RIGHT_BRACE))
				while (true) {
					call->v_arguments.emplace_back(f_expression());
					if (!f_single_colon()) f_throw(L"expecting ':'."sv);
					v_lexer.f_next();
					call->v_arguments.emplace_back(f_expression());
					if (v_lexer.f_token() == t_lexer::c_token__COMMA) {
						if (v_lexer.f_newline() && v_lexer.f_indent() < indent) break;
						v_lexer.f_next();
						continue;
					}
					if (!v_lexer.f_newline() || v_lexer.f_indent() <= indent) break;
				}
			f_close(indent, t_lexer::c_token__RIGHT_BRACE);
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
			if (!v_can_jump) f_throw(L"expecting inside loop."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return std::make_unique<ast::t_break>(f_has_expression() ? f_expression() : std::make_unique<ast::t_null>(at));
		}
	case t_lexer::c_token__CONTINUE:
		{
			if (!v_can_jump) f_throw(L"expecting inside loop."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return std::make_unique<ast::t_continue>(at);
		}
	case t_lexer::c_token__RETURN:
		{
			if (!v_can_return) f_throw(L"expecting within lambda."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return std::make_unique<ast::t_return>(f_has_expression() ? f_expression() : std::make_unique<ast::t_null>(at));
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
	while (!v_lexer.f_newline() || v_lexer.f_indent() > a_indent && v_lexer.f_token() == t_lexer::c_token__DOT) {
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__LEFT_PARENTHESIS:
			{
				t_at at = v_lexer.f_at();
				size_t indent = v_lexer.f_indent();
				v_lexer.f_next();
				auto call = std::make_unique<ast::t_call>(at, std::move(a_target));
				if (f_any(indent, t_lexer::c_token__RIGHT_PARENTHESIS)) f_arguments(indent, call.get());
				f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
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
						size_t indent = v_lexer.f_indent();
						v_lexer.f_next();
						auto key = f_expression();
						f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
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
							size_t indent = v_lexer.f_indent();
							v_lexer.f_next();
							auto key = f_expression();
							f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
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
				size_t indent = v_lexer.f_indent();
				v_lexer.f_next();
				auto index = f_expression();
				f_close(indent, t_lexer::c_token__RIGHT_BRACKET);
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
		v_lexer.f_next();
		auto branch = std::make_unique<ast::t_if>(std::move(node));
		branch->v_true = f_or(false);
		branch->v_false = std::make_unique<ast::t_preserve>();
		node = std::move(branch);
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_or_else(bool a_assignable)
{
	auto node = f_and_also(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__OR_ELSE) {
		v_lexer.f_next();
		auto branch = std::make_unique<ast::t_if>(std::move(node));
		branch->v_true = std::make_unique<ast::t_preserve>();
		branch->v_false = f_and_also(false);
		node = std::move(branch);
	}
	return node;
}

std::unique_ptr<ast::t_node> t_parser::f_conditional(bool a_assignable)
{
	auto node = f_or_else(a_assignable);
	if (v_lexer.f_newline() || v_lexer.f_token() != t_lexer::c_token__QUESTION) return node;
	v_lexer.f_next();
	auto branch = std::make_unique<ast::t_if>(std::move(node));
	branch->v_true = f_conditional(false);
	if (!f_single_colon()) f_throw(L"expecting ':'."sv);
	v_lexer.f_next();
	branch->v_false = f_conditional(false);
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
			auto node = std::make_unique<ast::t_if>(f_options());
			node->v_true = f_body(indent);
			if ((!v_lexer.f_newline() || v_lexer.f_indent() == indent) && v_lexer.f_token() == t_lexer::c_token__ELSE) {
				v_lexer.f_next();
				node->v_false = f_body(indent);
			} else {
				node->v_false = std::make_unique<ast::t_null>(at);
			}
			return node;
		}
	case t_lexer::c_token__WHILE:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			size_t varies = v_scope->v_privates.size();
			auto node = std::make_unique<ast::t_while>(f_options());
			bool can_jump = v_can_jump;
			v_can_jump = true;
			node->v_body = f_body(indent);
			v_can_jump = can_jump;
			while (varies < v_scope->v_privates.size()) v_scope->v_privates[varies++]->v_varies = true;
			return node;
		}
	case t_lexer::c_token__FOR:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = std::make_unique<ast::t_for>();
			node->v_initialization = f_options();
			size_t varies = v_scope->v_privates.size();
			node->v_condition = f_options();
			node->v_next = f_options();
			bool can_jump = v_can_jump;
			v_can_jump = true;
			node->v_body = f_body(indent);
			v_can_jump = can_jump;
			while (varies < v_scope->v_privates.size()) v_scope->v_privates[varies++]->v_varies = true;
			return node;
		}
	case t_lexer::c_token__TRY:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = std::make_unique<ast::t_try>();
			bool catching = false;
			{
				node->v_body = f_body(indent);
				while (v_lexer.f_indent() == indent && v_lexer.f_token() == t_lexer::c_token__CATCH) {
					catching = true;
					v_lexer.f_next();
					auto expression = f_expression();
					if (v_lexer.f_token() != t_lexer::c_token__SYMBOL) f_throw(L"expecting symbol."sv);
					auto symbol = f_symbol();
					v_lexer.f_next();
					auto c = std::make_unique<ast::t_try::t_catch>(std::move(expression), f_variable(v_scope, symbol));
					c->v_body = f_body(indent);
					node->v_catches.emplace_back(std::move(c));
				}
			}
			if (v_lexer.f_indent() == indent && v_lexer.f_token() == t_lexer::c_token__FINALLY) {
				v_lexer.f_next();
				bool can_jump = v_can_jump;
				v_can_jump = false;
				bool can_return = v_can_return;
				v_can_return = false;
				node->v_finally = f_body(indent);
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

bool t_parser::f_argument(size_t a_indent, std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	a_nodes.emplace_back(f_expression());
	if (v_lexer.f_token() != t_lexer::c_token__COMMA) return v_lexer.f_newline() && v_lexer.f_indent() > a_indent;
	if (v_lexer.f_newline() && v_lexer.f_indent() < a_indent) return false;
	v_lexer.f_next();
	return true;
}

void t_parser::f_arguments(size_t a_indent, ast::t_call* a_call)
{
	do if (v_lexer.f_token() == t_lexer::c_token__ASTERISK) {
		a_call->v_expands.emplace_back(v_lexer.f_at(), a_call->v_arguments.size());
		v_lexer.f_next();
	} while (f_argument(a_indent, a_call->v_arguments));
}

void t_parser::f_expressions(std::vector<std::unique_ptr<ast::t_node>>& a_nodes)
{
	while (true) {
		a_nodes.emplace_back(f_expression());
		if (v_lexer.f_newline() || v_lexer.f_token() != t_lexer::c_token__COMMA) break;
		v_lexer.f_next();
	}
}

std::unique_ptr<ast::t_nodes> t_parser::f_options()
{
	if (v_lexer.f_newline()) return nullptr;
	if (v_lexer.f_token() == t_lexer::c_token__SEMICOLON) {
		v_lexer.f_next();
		return nullptr;
	}
	auto nodes = std::make_unique<ast::t_nodes>();
	f_expressions(*nodes);
	if (!v_lexer.f_newline()) {
		if (v_lexer.f_token() != t_lexer::c_token__SEMICOLON) f_throw(L"expecting ';'."sv);
		v_lexer.f_next();
	}
	return nodes;
}

std::unique_ptr<ast::t_node> t_parser::f_body(size_t a_indent)
{
	if (!v_lexer.f_newline()) return f_expression();
	auto nodes = std::make_unique<ast::t_nodes>();
	size_t indent = v_lexer.f_indent();
	if (indent > a_indent) {
		do f_expressions(*nodes); while (v_lexer.f_newline() && v_lexer.f_indent() == indent);
		if (v_lexer.f_indent() > indent) f_throw(L"unexpected indent."sv);
	}
	return nodes;
}

void t_parser::operator()(ast::t_scope& a_scope)
{
	v_scope = &a_scope;
	auto nodes = std::make_unique<ast::t_nodes>();
	if (v_lexer.f_token() != t_lexer::c_token__EOF) {
		size_t indent = v_lexer.f_indent();
		while (true) {
			f_expressions(*nodes);
			if (v_lexer.f_token() == t_lexer::c_token__EOF) break;
			if (!v_lexer.f_newline()) f_throw(L"expecting newline."sv);
			if (v_lexer.f_indent() != indent) f_throw(L"unexpected indent."sv);
		}
	}
	v_scope->v_body = std::move(nodes);
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

void t_parser::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%zu:%zu\n", v_path.c_str(), v_at.v_line, v_at.v_column);
	f_print_with_caret(stderr, v_path, v_at.v_position, v_at.v_column);
	t_throwable::f_dump();
}

}
