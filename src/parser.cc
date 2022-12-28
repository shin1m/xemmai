#include <xemmai/global.h>

namespace xemmai
{

void t_parser::f_throw(std::wstring_view a_message)
{
	throw t_rvalue(f_new<t_error>(f_global(), a_message, v_lexer.f_path(), v_lexer.f_at()));
}

t_code::t_variable& t_parser::f_variable(ast::t_scope* a_scope, t_object* a_symbol)
{
	auto i = a_scope->v_variables.lower_bound(a_symbol);
	if (i == a_scope->v_variables.end() || i->first != a_symbol) {
		i = a_scope->v_variables.emplace_hint(i, a_symbol, t_code::t_variable());
		a_scope->v_privates.f_push(&i->second);
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

ast::t_node* t_parser::f_target(bool a_assignable)
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
						return v_arena.f_new<ast::t_scope_put>(at, outer, variable, f_expression());
					}
					if (scope) (outer > 0 ? scope->v_unresolveds : scope->v_references).insert(symbol);
					return v_arena.f_new<ast::t_symbol_get>(at, outer, scope, symbol);
				}
			case t_lexer::c_token__SELF:
				{
					if (!scope) f_throw(L"no more outer scope."sv);
					if (outer > 0) scope->v_self_shared = true;
					ast::t_node* target = v_arena.f_new<ast::t_self>(at, outer);
					for (auto c : v_lexer.f_value()) target = c == L'@' ? v_arena.f_new<ast::t_class>(at, target) : static_cast<ast::t_node*>(v_arena.f_new<ast::t_super>(at, target));
					v_lexer.f_next();
					if (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__SYMBOL) {
						t_at at = v_lexer.f_at();
						auto key = f_symbol();
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
							if (!a_assignable) f_throw(L"can not assign to expression."sv);
							v_lexer.f_next();
							return v_arena.f_new<ast::t_object_put>(at, target, key, f_expression());
						}
						return v_arena.f_new<ast::t_object_get>(at, target, key);
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
				return v_arena.f_new<ast::t_literal<t_object*>>(at, symbol);
			}
		case t_lexer::c_token__LEFT_PARENTHESIS:
			{
				size_t indent = v_lexer.f_indent();
				v_lexer.f_next();
				auto call = v_arena.f_new<ast::t_call>(at, v_arena.f_new<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_tuple>()))));
				if (f_any(indent, t_lexer::c_token__RIGHT_PARENTHESIS)) f_arguments(indent, call);
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
			auto nodes = v_arena.f_new<ast::t_nodes>();
			if (f_any(indent, t_lexer::c_token__RIGHT_PARENTHESIS)) while (f_argument(indent, *nodes));
			f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
			return nodes;
		}
	case t_lexer::c_token__ATMARK:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto lambda = v_arena.f_new<ast::t_lambda>(v_arena, at, v_scope);
			if (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__LEFT_PARENTHESIS) {
				v_lexer.f_next();
				if (v_lexer.f_token() != t_lexer::c_token__RIGHT_PARENTHESIS) {
					while (true) {
						switch (v_lexer.f_token()) {
						case t_lexer::c_token__SYMBOL:
							lambda->v_privates.f_push(&lambda->v_variables.emplace(f_symbol(), t_code::t_variable()).first->second);
							v_lexer.f_next();
							if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
								v_lexer.f_next();
								lambda->v_defaults.f_push(f_expression());
							} else {
								if (lambda->v_defaults.v_size > 0) f_throw(L"expecting '='."sv);
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
							lambda->v_privates.f_push(&lambda->v_variables.emplace(f_symbol(), t_code::t_variable()).first->second);
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
			v_scope = lambda;
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
			for (auto symbol : lambda->v_references) if (!lambda->v_variables.contains(symbol)) lambda->v_outer->v_unresolveds.insert(symbol);
			v_scope = lambda->v_outer;
			v_can_jump = can_jump;
			v_can_return = can_return;
			if (lambda->v_self_shared) ++lambda->v_shareds;
			auto p = lambda->v_privates.v_tail;
			for (size_t i = 0; i < lambda->v_arguments; ++i) {
				p = p->v_next;
				p->v_index = p->v_shared ? lambda->v_shareds++ : i;
			}
			if (p != (lambda->v_arguments > 0 ? lambda->v_privates.v_tail : nullptr)) {
				lambda->v_privates.v_size = lambda->v_arguments;
				lambda->f_classify(p);
			}
			return lambda;
		}
	case t_lexer::c_token__LEFT_BRACKET:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto call = v_arena.f_new<ast::t_call>(at, v_arena.f_new<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_list>()))));
			if (f_any(indent, t_lexer::c_token__RIGHT_BRACKET)) f_arguments(indent, call);
			f_close(indent, t_lexer::c_token__RIGHT_BRACKET);
			return call;
		}
	case t_lexer::c_token__LEFT_BRACE:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto call = v_arena.f_new<ast::t_call>(at, v_arena.f_new<ast::t_literal<t_object*>>(at, v_module.f_slot(t_object::f_of(f_global()->f_type<t_map>()))));
			if (f_any(indent, t_lexer::c_token__RIGHT_BRACE))
				while (true) {
					call->v_arguments.f_push(f_expression());
					if (!f_single_colon()) f_throw(L"expecting ':'."sv);
					v_lexer.f_next();
					call->v_arguments.f_push(f_expression());
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
		return v_arena.f_new<ast::t_null>(at);
	case t_lexer::c_token__TRUE:
		v_lexer.f_next();
		return v_arena.f_new<ast::t_literal<double>>(at, 1.0);
	case t_lexer::c_token__FALSE:
		v_lexer.f_next();
		return v_arena.f_new<ast::t_null>(at);
	case t_lexer::c_token__INTEGER:
		{
			auto value = v_lexer.f_integer();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_literal<intptr_t>>(at, value);
		}
	case t_lexer::c_token__FLOAT:
		{
			auto value = v_lexer.f_float();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_literal<double>>(at, value);
		}
	case t_lexer::c_token__STRING:
		{
			auto value = v_lexer.f_string();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_literal<t_object*>>(at, v_module.f_slot(value));
		}
	case t_lexer::c_token__BREAK:
		{
			if (!v_can_jump) f_throw(L"expecting inside loop."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_break>(f_has_expression() ? f_expression() : v_arena.f_new<ast::t_null>(at));
		}
	case t_lexer::c_token__CONTINUE:
		{
			if (!v_can_jump) f_throw(L"expecting inside loop."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_continue>(at);
		}
	case t_lexer::c_token__RETURN:
		{
			if (!v_can_return) f_throw(L"expecting within lambda."sv);
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_return>(f_has_expression() ? f_expression() : v_arena.f_new<ast::t_null>(at));
		}
	case t_lexer::c_token__THROW:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			return v_arena.f_new<ast::t_throw>(at, f_expression());
		}
	default:
		f_throw(L"unexpected token."sv);
	}
}

ast::t_node* t_parser::f_action(size_t a_indent, ast::t_node* a_target, bool a_assignable)
{
	while (!v_lexer.f_newline() || v_lexer.f_indent() > a_indent && v_lexer.f_token() == t_lexer::c_token__DOT) {
		switch (v_lexer.f_token()) {
		case t_lexer::c_token__LEFT_PARENTHESIS:
			{
				t_at at = v_lexer.f_at();
				size_t indent = v_lexer.f_indent();
				v_lexer.f_next();
				auto call = v_arena.f_new<ast::t_call>(at, a_target);
				if (f_any(indent, t_lexer::c_token__RIGHT_PARENTHESIS)) f_arguments(indent, call);
				f_close(indent, t_lexer::c_token__RIGHT_PARENTHESIS);
				a_target = call;
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
							return v_arena.f_new<ast::t_object_put_indirect>(at, a_target, key, f_expression());
						}
						a_target = v_arena.f_new<ast::t_object_get_indirect>(at, a_target, key);
						continue;
					}
				case t_lexer::c_token__SYMBOL:
					{
						auto key = f_symbol();
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::c_token__EQUAL) {
							if (!a_assignable) f_throw(L"can not assign to expression."sv);
							v_lexer.f_next();
							return v_arena.f_new<ast::t_object_put>(at, a_target, key, f_expression());
						}
						a_target = v_arena.f_new<ast::t_object_get>(at, a_target, key);
						continue;
					}
				case t_lexer::c_token__ATMARK:
					v_lexer.f_next();
					a_target = v_arena.f_new<ast::t_class>(at, a_target);
					continue;
				case t_lexer::c_token__HAT:
					v_lexer.f_next();
					a_target = v_arena.f_new<ast::t_super>(at, a_target);
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
							a_target = v_arena.f_new<ast::t_object_has_indirect>(at, a_target, key);
							continue;
						}
					case t_lexer::c_token__SYMBOL:
						{
							auto key = f_symbol();
							v_lexer.f_next();
							a_target = v_arena.f_new<ast::t_object_has>(at, a_target, key);
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
					return v_arena.f_new<ast::t_set_at>(at, a_target, index, f_expression());
				}
				a_target = v_arena.f_new<ast::t_get_at>(at, a_target, index);
				continue;
			}
		}
		break;
	}
	return a_target;
}

ast::t_node* t_parser::f_unary(bool a_assignable)
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
				return f_action(indent, v_arena.f_new<ast::t_literal<intptr_t>>(at, instruction == c_instruction__MINUS_T ? -value : value), a_assignable);
			}
		case t_lexer::c_token__FLOAT:
			{
				auto value = v_lexer.f_float();
				v_lexer.f_next();
				return f_action(indent, v_arena.f_new<ast::t_literal<double>>(at, instruction == c_instruction__MINUS_T ? -value : value), a_assignable);
			}
		}
		break;
	case c_instruction__COMPLEMENT_T:
		if (v_lexer.f_token() == t_lexer::c_token__INTEGER) {
			auto value = v_lexer.f_integer();
			v_lexer.f_next();
			return f_action(indent, v_arena.f_new<ast::t_literal<intptr_t>>(at, ~value), a_assignable);
		}
		break;
	}
	return v_arena.f_new<ast::t_unary>(at, instruction, f_unary(false));
}

ast::t_node* t_parser::f_multiplicative(bool a_assignable)
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
		node = v_arena.f_new<ast::t_binary>(at, instruction, node, f_unary(false));
	}
	return node;
}

ast::t_node* t_parser::f_additive(bool a_assignable)
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
		node = v_arena.f_new<ast::t_binary>(at, instruction, node, f_multiplicative(false));
	}
	return node;
}

ast::t_node* t_parser::f_shift(bool a_assignable)
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
		node = v_arena.f_new<ast::t_binary>(at, instruction, node, f_additive(false));
	}
	return node;
}

ast::t_node* t_parser::f_relational(bool a_assignable)
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
		node = v_arena.f_new<ast::t_binary>(at, instruction, node, f_shift(false));
	}
	return node;
}

ast::t_node* t_parser::f_equality(bool a_assignable)
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
		node = v_arena.f_new<ast::t_binary>(at, instruction, node, f_relational(false));
	}
	return node;
}

ast::t_node* t_parser::f_and(bool a_assignable)
{
	auto node = f_equality(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__AMPERSAND) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = v_arena.f_new<ast::t_binary>(at, c_instruction__AND_TT, node, f_equality(false));
	}
	return node;
}

ast::t_node* t_parser::f_xor(bool a_assignable)
{
	auto node = f_and(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__HAT) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = v_arena.f_new<ast::t_binary>(at, c_instruction__XOR_TT, node, f_and(false));
	}
	return node;
}

ast::t_node* t_parser::f_or(bool a_assignable)
{
	auto node = f_xor(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__BAR) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = v_arena.f_new<ast::t_binary>(at, c_instruction__OR_TT, node, f_xor(false));
	}
	return node;
}

ast::t_node* t_parser::f_and_also(bool a_assignable)
{
	auto node = f_or(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__AND_ALSO) {
		v_lexer.f_next();
		auto branch = v_arena.f_new<ast::t_if>(v_arena, node);
		branch->v_true = f_or(false);
		branch->v_false = v_arena.f_new<ast::t_preserve>();
		node = branch;
	}
	return node;
}

ast::t_node* t_parser::f_or_else(bool a_assignable)
{
	auto node = f_and_also(a_assignable);
	while (!v_lexer.f_newline() && v_lexer.f_token() == t_lexer::c_token__OR_ELSE) {
		v_lexer.f_next();
		auto branch = v_arena.f_new<ast::t_if>(v_arena, node);
		branch->v_true = v_arena.f_new<ast::t_preserve>();
		branch->v_false = f_and_also(false);
		node = branch;
	}
	return node;
}

ast::t_node* t_parser::f_conditional(bool a_assignable)
{
	auto node = f_or_else(a_assignable);
	if (v_lexer.f_newline() || v_lexer.f_token() != t_lexer::c_token__QUESTION) return node;
	v_lexer.f_next();
	auto branch = v_arena.f_new<ast::t_if>(v_arena, node);
	branch->v_true = f_conditional(false);
	if (!f_single_colon()) f_throw(L"expecting ':'."sv);
	v_lexer.f_next();
	branch->v_false = f_conditional(false);
	return branch;
}

ast::t_node* t_parser::f_expression()
{
	switch (v_lexer.f_token()) {
	case t_lexer::c_token__IF:
		{
			t_at at = v_lexer.f_at();
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = v_arena.f_new<ast::t_if>(v_arena, f_options());
			node->v_true = f_body(indent);
			if ((!v_lexer.f_newline() || v_lexer.f_indent() == indent) && v_lexer.f_token() == t_lexer::c_token__ELSE) {
				v_lexer.f_next();
				node->v_false = f_body(indent);
			} else {
				node->v_false = v_arena.f_new<ast::t_null>(at);
			}
			return node;
		}
	case t_lexer::c_token__WHILE:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto tail = v_scope->v_privates.v_tail;
			auto node = v_arena.f_new<ast::t_while>(v_arena, f_options());
			bool can_jump = v_can_jump;
			v_can_jump = true;
			node->v_body = f_body(indent);
			v_can_jump = can_jump;
			v_scope->f_vary(tail);
			return node;
		}
	case t_lexer::c_token__FOR:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = v_arena.f_new<ast::t_for>(v_arena);
			node->v_initialization = f_options();
			auto tail = v_scope->v_privates.v_tail;
			node->v_condition = f_options();
			node->v_next = f_options();
			bool can_jump = v_can_jump;
			v_can_jump = true;
			node->v_body = f_body(indent);
			v_can_jump = can_jump;
			v_scope->f_vary(tail);
			return node;
		}
	case t_lexer::c_token__TRY:
		{
			size_t indent = v_lexer.f_indent();
			v_lexer.f_next();
			auto node = v_arena.f_new<ast::t_try>(v_arena);
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
					auto c = v_arena.f_new<ast::t_try::t_catch>(v_arena, expression, f_variable(v_scope, symbol));
					c->v_body = f_body(indent);
					node->v_catches.f_push(c);
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

bool t_parser::f_argument(size_t a_indent, ast::t_nodes& a_nodes)
{
	a_nodes.f_push(f_expression());
	if (v_lexer.f_token() != t_lexer::c_token__COMMA) return v_lexer.f_newline() && v_lexer.f_indent() > a_indent;
	if (v_lexer.f_newline() && v_lexer.f_indent() < a_indent) return false;
	v_lexer.f_next();
	return true;
}

void t_parser::f_arguments(size_t a_indent, ast::t_call* a_call)
{
	do if (v_lexer.f_token() == t_lexer::c_token__ASTERISK) {
		a_call->v_expands.f_push(v_arena.f_new<ast::t_call::t_expand>(v_lexer.f_at(), a_call->v_arguments.v_size));
		v_lexer.f_next();
	} while (f_argument(a_indent, a_call->v_arguments));
}

void t_parser::f_expressions(ast::t_nodes& a_nodes)
{
	while (true) {
		a_nodes.f_push(f_expression());
		if (v_lexer.f_newline() || v_lexer.f_token() != t_lexer::c_token__COMMA) break;
		v_lexer.f_next();
	}
}

ast::t_nodes* t_parser::f_options()
{
	if (v_lexer.f_newline()) return nullptr;
	if (v_lexer.f_token() == t_lexer::c_token__SEMICOLON) {
		v_lexer.f_next();
		return nullptr;
	}
	auto nodes = v_arena.f_new<ast::t_nodes>();
	f_expressions(*nodes);
	if (!v_lexer.f_newline()) {
		if (v_lexer.f_token() != t_lexer::c_token__SEMICOLON) f_throw(L"expecting ';'."sv);
		v_lexer.f_next();
	}
	return nodes;
}

ast::t_node* t_parser::f_body(size_t a_indent)
{
	if (!v_lexer.f_newline()) return f_expression();
	auto nodes = v_arena.f_new<ast::t_nodes>();
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
	auto nodes = v_arena.f_new<ast::t_nodes>();
	if (v_lexer.f_token() != t_lexer::c_token__EOF) {
		size_t indent = v_lexer.f_indent();
		while (true) {
			f_expressions(*nodes);
			if (v_lexer.f_token() == t_lexer::c_token__EOF) break;
			if (!v_lexer.f_newline()) f_throw(L"expecting newline."sv);
			if (v_lexer.f_indent() != indent) f_throw(L"unexpected indent."sv);
		}
	}
	v_scope->v_body = nodes;
	for (auto symbol : v_scope->v_unresolveds) {
		auto i = v_scope->v_variables.find(symbol);
		if (i != v_scope->v_variables.end()) i->second.v_shared = true;
	}
	if (v_scope->v_self_shared) ++v_scope->v_shareds;
	if (auto p = v_scope->v_privates.v_tail) {
		v_scope->v_privates.v_size = 0;
		v_scope->f_classify(p);
	}
}

void t_parser::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%zu:%zu\n", v_path.c_str(), v_at.v_line, v_at.v_column);
	f_print_with_caret(stderr, v_path, v_at.v_position, v_at.v_column);
	t_throwable::f_dump();
}

}
