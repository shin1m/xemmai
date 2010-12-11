#include <xemmai/parser.h>

#include <xemmai/engine.h>
#include <xemmai/symbol.h>
#include <xemmai/global.h>

namespace xemmai
{

void t_parser::f_throw(const std::wstring& a_message)
{
	throw t_scoped(t_error::f_instantiate(a_message, v_lexer));
}

ast::t_variable& t_parser::f_variable(ast::t_scope* a_scope, const t_value& a_symbol, bool a_loop)
{
	std::map<t_scoped, ast::t_variable>::iterator i = a_scope->v_variables.find(a_symbol);
	if (i == a_scope->v_variables.end()) {
		i = a_scope->v_variables.insert(i, std::make_pair(a_symbol, ast::t_variable()));
		a_scope->v_privates.push_back(&i->second);
		if (a_loop) i->second.v_varies = true;
	} else {
		i->second.v_varies = true;
	}
	return i->second;
}

ast::t_pointer<ast::t_node> t_parser::f_target(bool a_assignable)
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
					t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						if (!scope) t_throwable::f_throw(L"no more outer scope.");
						v_lexer.f_next();
						ast::t_variable& variable = f_variable(scope, symbol, outer > 0 || v_targets->v_loop);
						if (outer > 0) variable.v_shared = true;
						return new ast::t_scope_put(at, outer, variable, f_expression());
					}
					while (scope) {
						std::map<t_scoped, ast::t_variable>::iterator i = scope->v_variables.find(t_value(symbol));
						if (i != scope->v_variables.end()) {
							if (outer > 0) i->second.v_shared = true;
							return new ast::t_scope_get(at, outer, i->second);
						}
						++outer;
						scope = scope->v_outer;
					}
					return new ast::t_global_get(at, symbol);
				}
			case t_lexer::e_token__SELF:
				{
					if (!scope) t_throwable::f_throw(L"no more outer scope.");
					if (outer > 0) scope->v_self_shared = true;
					ast::t_pointer<ast::t_node> target = new ast::t_self(at, outer);
					std::vector<wchar_t>::const_iterator end = v_lexer.f_value().end();
					for (std::vector<wchar_t>::const_iterator i = v_lexer.f_value().begin(); i != end; ++i) target = *i == L':' ? static_cast<ast::t_node*>(new ast::t_class(at, target)) : static_cast<ast::t_node*>(new ast::t_super(at, target));
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__SYMBOL) {
						t_at at = v_lexer.f_at();
						t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
							v_lexer.f_next();
							return new ast::t_object_put(at, target, symbol, f_expression());
						}
						return new ast::t_object_get(at, target, symbol);
					}
					return target;
				}
			default:
				f_throw(L"expecting symbol or self.");
			}
		}
	case t_lexer::e_token__APOSTROPHE:
		{
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
			t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
			v_lexer.f_next();
			return new ast::t_instance(at, symbol);
		}
	case t_lexer::e_token__LEFT_PARENTHESIS:
		{
			v_lexer.f_next();
			ast::t_pointer<ast::t_node> expression = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			return expression;
		}
	case t_lexer::e_token__ATMARK:
		{
			v_lexer.f_next();
			ast::t_pointer<ast::t_lambda> lambda = new ast::t_lambda(at, v_scope);
			if (v_lexer.f_token() == t_lexer::e_token__LEFT_PARENTHESIS) {
				v_lexer.f_next();
				if (v_lexer.f_token() == t_lexer::e_token__SYMBOL) {
					while (true) {
						lambda->v_privates.push_back(&lambda->v_variables.insert(std::make_pair(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end())), ast::t_variable())).first->second);
						v_lexer.f_next();
						if (v_lexer.f_token() != t_lexer::e_token__COMMA) break;
						v_lexer.f_next();
						if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
					}
				}
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
				v_lexer.f_next();
			}
			lambda->v_arguments = lambda->v_variables.size();
			v_scope->v_shared = true;
			v_scope = lambda;
			t_targets* targets0 = v_targets;
			t_targets targets1(false, true);
			v_targets = &targets1;
			if (v_lexer.f_token() == t_lexer::e_token__LEFT_BRACE)
				f_block(lambda->v_block);
			else
				lambda->v_block.f_add(f_expression());
			v_scope = lambda->v_outer;
			v_targets = targets0;
			std::vector<ast::t_variable*> variables;
			variables.swap(lambda->v_privates);
			if (lambda->v_self_shared) ++lambda->v_shareds;
			std::vector<ast::t_variable*>::const_iterator i = variables.begin();
			for (std::vector<ast::t_variable*>::const_iterator j = i + lambda->v_arguments; i != j; ++i) {
				ast::t_variable* p = *i;
				p->v_index = p->v_shared ? lambda->v_shareds++ : lambda->v_privates.size();
				lambda->v_privates.push_back(p);
			}
			for (; i != variables.end(); ++i) {
				ast::t_variable* p = *i;
				if (p->v_shared) {
					p->v_index = lambda->v_shareds++;
				} else {
					p->v_index = lambda->v_privates.size();
					lambda->v_privates.push_back(p);
				}
			}
			return lambda;
		}
	case t_lexer::e_token__LEFT_BRACKET:
		{
			v_lexer.f_next();
			ast::t_pointer<ast::t_call> call = new ast::t_call(at, new ast::t_instance(at, f_global()->f_type<t_array>()));
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) {
				f_expressions(call->v_arguments);
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
			}
			v_lexer.f_next();
			return call;
		}
	case t_lexer::e_token__LEFT_BRACE:
		{
			v_lexer.f_next();
			ast::t_pointer<ast::t_call> call = new ast::t_call(at, new ast::t_instance(at, f_global()->f_type<t_dictionary>()));
			size_t n = 0;
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) {
				call->v_arguments.f_add(f_expression());
				if (!f_single_colon()) f_throw(L"expecting ':'.");
				v_lexer.f_next();
				call->v_arguments.f_add(f_expression());
				while (v_lexer.f_token() == t_lexer::e_token__COMMA) {
					v_lexer.f_next();
					call->v_arguments.f_add(f_expression());
					if (!f_single_colon()) f_throw(L"expecting ':'.");
					v_lexer.f_next();
					call->v_arguments.f_add(f_expression());
				}
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) f_throw(L"expecting '}'.");
			}
			v_lexer.f_next();
			return call;
		}
	case t_lexer::e_token__NULL:
		v_lexer.f_next();
		return new ast::t_null(at);
	case t_lexer::e_token__TRUE:
		v_lexer.f_next();
		return new ast::t_boolean(at, true);
	case t_lexer::e_token__FALSE:
		v_lexer.f_next();
		return new ast::t_boolean(at, false);
	case t_lexer::e_token__INTEGER:
		{
			int value = f_integer();
			v_lexer.f_next();
			return new ast::t_integer(at, value);
		}
	case t_lexer::e_token__FLOAT:
		{
			double value = f_float();
			v_lexer.f_next();
			return new ast::t_float(at, value);
		}
	case t_lexer::e_token__STRING:
		{
			std::wstring value(v_lexer.f_value().begin(), v_lexer.f_value().end());
			v_lexer.f_next();
			return new ast::t_instance(at, f_global()->f_as(value));
		}
	default:
		f_throw(L"unexpected token.");
	}
}

ast::t_pointer<ast::t_node> t_parser::f_action(const ast::t_pointer<ast::t_node>& a_target, bool a_assignable)
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__LEFT_PARENTHESIS:
		{
			t_at at = v_lexer.f_at();
			ast::t_pointer<ast::t_call> call = new ast::t_call(at, a_target);
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) {
				f_expressions(call->v_arguments);
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			}
			v_lexer.f_next();
			return f_action(call, a_assignable);
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
					ast::t_pointer<ast::t_node> key = f_expression();
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						v_lexer.f_next();
						return new ast::t_object_put_indirect(at, a_target, key, f_expression());
					}
					return f_action(new ast::t_object_get_indirect(at, a_target, key), a_assignable);
				}
			case t_lexer::e_token__SYMBOL:
				{
					t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						v_lexer.f_next();
						return new ast::t_object_put(at, a_target, symbol, f_expression());
					}
					return f_action(new ast::t_object_get(at, a_target, symbol), a_assignable);
				}
			case t_lexer::e_token__COLON:
				if (v_lexer.f_value().size() != 1) f_throw(L"expecting '(' or ':' or '?' or '^' or '~' or symbol.");
				v_lexer.f_next();
				return f_action(new ast::t_class(at, a_target), a_assignable);
			case t_lexer::e_token__HAT:
				v_lexer.f_next();
				return f_action(new ast::t_super(at, a_target), a_assignable);
			case t_lexer::e_token__QUESTION:
			case t_lexer::e_token__TILDE:
				v_lexer.f_next();
				switch (v_lexer.f_token()) {
				case t_lexer::e_token__LEFT_PARENTHESIS:
					{
						v_lexer.f_next();
						ast::t_pointer<ast::t_node> key = f_expression();
						if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
						v_lexer.f_next();
						return f_action(token == t_lexer::e_token__QUESTION ? static_cast<ast::t_node*>(new ast::t_object_has_indirect(at, a_target, key)) : static_cast<ast::t_node*>(new ast::t_object_remove_indirect(at, a_target, key)), a_assignable);
					}
				case t_lexer::e_token__SYMBOL:
					{
						t_transfer key = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
						v_lexer.f_next();
						return f_action(token == t_lexer::e_token__QUESTION ? static_cast<ast::t_node*>(new ast::t_object_has(at, a_target, key)) : static_cast<ast::t_node*>(new ast::t_object_remove(at, a_target, key)), a_assignable);
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
			ast::t_pointer<ast::t_node> index = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
			v_lexer.f_next();
			if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
				v_lexer.f_next();
				return new ast::t_set_at(at, a_target, index, f_expression());
			}
			return f_action(new ast::t_get_at(at, a_target, index), a_assignable);
		}
	default:
		return a_target;
	}
}

ast::t_pointer<ast::t_node> t_parser::f_unary(bool a_assignable)
{
	t_instruction instruction;
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__PLUS:
		instruction = e_instruction__PLUS;
		break;
	case t_lexer::e_token__HYPHEN:
		instruction = e_instruction__MINUS;
		break;
	case t_lexer::e_token__EXCLAMATION:
		instruction = e_instruction__NOT;
		break;
	case t_lexer::e_token__TILDE:
		instruction = e_instruction__COMPLEMENT;
		break;
	default:
		return f_action(f_target(a_assignable), a_assignable);
	}
	t_at at = v_lexer.f_at();
	v_lexer.f_next();
	switch (instruction) {
	case e_instruction__PLUS:
	case e_instruction__MINUS:
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__INTEGER:
			{
				int value = f_integer();
				v_lexer.f_next();
				return f_action(new ast::t_integer(at, instruction == e_instruction__MINUS ? -value : value), a_assignable);
			}
		case t_lexer::e_token__FLOAT:
			{
				double value = f_float();
				v_lexer.f_next();
				return f_action(new ast::t_float(at, instruction == e_instruction__MINUS ? -value : value), a_assignable);
			}
		}
		break;
	case e_instruction__COMPLEMENT:
		if (v_lexer.f_token() == t_lexer::e_token__INTEGER) {
			int value = f_integer();
			v_lexer.f_next();
			return f_action(new ast::t_integer(at, ~value), a_assignable);
		}
		break;
	}
	return new ast::t_unary(at, instruction, f_unary(false));
}

ast::t_pointer<ast::t_node> t_parser::f_multiplicative(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_unary(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__ASTERISK:
			instruction = e_instruction__MULTIPLY;
			break;
		case t_lexer::e_token__SLASH:
			instruction = e_instruction__DIVIDE;
			break;
		case t_lexer::e_token__PERCENT:
			instruction = e_instruction__MODULUS;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, instruction, node, f_unary(false));
	}
}

ast::t_pointer<ast::t_node> t_parser::f_additive(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_multiplicative(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__PLUS:
			instruction = e_instruction__ADD;
			break;
		case t_lexer::e_token__HYPHEN:
			instruction = e_instruction__SUBTRACT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, instruction, node, f_multiplicative(false));
	}
}

ast::t_pointer<ast::t_node> t_parser::f_shift(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_additive(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__LEFT_SHIFT:
			instruction = e_instruction__LEFT_SHIFT;
			break;
		case t_lexer::e_token__RIGHT_SHIFT:
			instruction = e_instruction__RIGHT_SHIFT;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, instruction, node, f_additive(false));
	}
}

ast::t_pointer<ast::t_node> t_parser::f_relational(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_shift(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__LESS:
			instruction = e_instruction__LESS;
			break;
		case t_lexer::e_token__LESS_EQUAL:
			instruction = e_instruction__LESS_EQUAL;
			break;
		case t_lexer::e_token__GREATER:
			instruction = e_instruction__GREATER;
			break;
		case t_lexer::e_token__GREATER_EQUAL:
			instruction = e_instruction__GREATER_EQUAL;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, instruction, node, f_shift(false));
	}
}

ast::t_pointer<ast::t_node> t_parser::f_equality(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_relational(a_assignable);
	while (true) {
		t_instruction instruction;
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__EQUALS:
			instruction = e_instruction__EQUALS;
			break;
		case t_lexer::e_token__NOT_EQUALS:
			instruction = e_instruction__NOT_EQUALS;
			break;
		case t_lexer::e_token__IDENTICAL:
			instruction = e_instruction__IDENTICAL;
			break;
		case t_lexer::e_token__NOT_IDENTICAL:
			instruction = e_instruction__NOT_IDENTICAL;
			break;
		default:
			return node;
		}
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, instruction, node, f_relational(false));
	}
}

ast::t_pointer<ast::t_node> t_parser::f_and(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_equality(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__AMPERSAND) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, e_instruction__AND, node, f_equality(false));
	}
	return node;
}

ast::t_pointer<ast::t_node> t_parser::f_xor(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_and(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__HAT) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, e_instruction__XOR, node, f_and(false));
	}
	return node;
}

ast::t_pointer<ast::t_node> t_parser::f_or(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_xor(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__BAR) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, e_instruction__OR, node, f_xor(false));
	}
	return node;
}

ast::t_pointer<ast::t_node> t_parser::f_and_also(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_or(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__AND_ALSO) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		ast::t_pointer<ast::t_if> branch = new ast::t_if(at, node);
		branch->v_true.f_add(f_or(false));
		branch->v_false.f_add(new ast::t_boolean(at, false));
		node = branch;
	}
	return node;
}

ast::t_pointer<ast::t_node> t_parser::f_or_else(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_and_also(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__OR_ELSE) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		ast::t_pointer<ast::t_if> branch = new ast::t_if(at, node);
		branch->v_true.f_add(new ast::t_boolean(at, true));
		branch->v_false.f_add(f_and_also(false));
		node = branch;
	}
	return node;
}

ast::t_pointer<ast::t_node> t_parser::f_send(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_or_else(a_assignable);
	while (v_lexer.f_token() == t_lexer::e_token__COLON && v_lexer.f_value().size() == 2) {
		t_at at = v_lexer.f_at();
		v_lexer.f_next();
		node = new ast::t_binary(at, e_instruction__SEND, node, f_or_else(false));
	}
	return node;
}

ast::t_pointer<ast::t_node> t_parser::f_conditional(bool a_assignable)
{
	ast::t_pointer<ast::t_node> node = f_send(a_assignable);
	if (v_lexer.f_token() != t_lexer::e_token__QUESTION) return node;
	t_at at = v_lexer.f_at();
	v_lexer.f_next();
	ast::t_pointer<ast::t_if> branch = new ast::t_if(at, node);
	branch->v_true.f_add(f_conditional(false));
	if (!f_single_colon()) f_throw(L"expecting ':'.");
	v_lexer.f_next();
	branch->v_false.f_add(f_conditional(false));
	return branch;
}

ast::t_pointer<ast::t_node> t_parser::f_expression()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__IF:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			ast::t_pointer<ast::t_if> node = new ast::t_if(at, f_expression());
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			f_block_or_statement(node->v_true);
			if (v_lexer.f_token() == t_lexer::e_token__ELSE) {
				v_lexer.f_next();
				f_block_or_statement(node->v_false);
			}
			return node;
		}
	case t_lexer::e_token__WHILE:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			ast::t_pointer<ast::t_while> node = new ast::t_while(at, f_expression());
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			t_targets* targets0 = v_targets;
			t_targets targets1(true, targets0->v_return);
			v_targets = &targets1;
			f_block_or_statement(node->v_block);
			v_targets = targets0;
			return node;
		}
	case t_lexer::e_token__TRY:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
			ast::t_pointer<ast::t_try> node = new ast::t_try(at);
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
					ast::t_pointer<ast::t_node> expression = f_expression();
					if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
					t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end()));
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
					ast::t_pointer<ast::t_try::t_catch> c = new ast::t_try::t_catch(expression, f_variable(v_scope, symbol, v_targets->v_loop));
					f_block(c->v_block);
					node->v_catches.f_add(c);
				}
			}
			if (v_lexer.f_token() == t_lexer::e_token__FINALLY) {
				v_lexer.f_next();
				if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
				v_lexer.f_next();
				t_targets targets2(false, false);
				v_targets = &targets2;
				while (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) node->v_finally.f_add(f_statement());
				v_lexer.f_next();
			} else {
				if (!catching) f_throw(L"expecting 'finally'.");
			}
			v_targets = targets0;
			return node;
		}
	default:
		return f_conditional(true);
	}
}

void t_parser::f_expressions(ast::t_pointers<ast::t_node>& a_nodes)
{
	a_nodes.f_add(f_expression());
	while (v_lexer.f_token() == t_lexer::e_token__COMMA) {
		v_lexer.f_next();
		a_nodes.f_add(f_expression());
	}
}

ast::t_pointer<ast::t_node> t_parser::f_statement()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__IF:
	case t_lexer::e_token__WHILE:
	case t_lexer::e_token__TRY:
		return f_expression();
	case t_lexer::e_token__BREAK:
		{
			if (!v_targets->v_loop) f_throw(L"expecting within loop.");
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			ast::t_pointer<ast::t_node> expression;
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) {
				expression = f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			}
			v_lexer.f_next();
			return new ast::t_break(at, expression);
		}
	case t_lexer::e_token__CONTINUE:
		{
			if (!v_targets->v_loop) f_throw(L"expecting within loop.");
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			return new ast::t_continue(at);
		}
	case t_lexer::e_token__RETURN:
		{
			if (!v_targets->v_return) f_throw(L"expecting within lambda.");
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			ast::t_pointer<ast::t_node> expression;
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) {
				expression = f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			}
			v_lexer.f_next();
			return new ast::t_return(at, expression);
		}
	case t_lexer::e_token__THROW:
		{
			t_at at = v_lexer.f_at();
			v_lexer.f_next();
			ast::t_pointer<ast::t_node> expression = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			return new ast::t_throw(at, expression);
		}
	default:
		{
			ast::t_pointer<ast::t_node> expression = f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			return expression;
		}
	}
}

void t_parser::f_block(ast::t_pointers<ast::t_node>& a_nodes)
{
	v_lexer.f_next();
	while (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) a_nodes.f_add(f_statement());
	v_lexer.f_next();
}

void t_parser::f_block_or_statement(ast::t_pointers<ast::t_node>& a_nodes)
{
	if (v_lexer.f_token() == t_lexer::e_token__LEFT_BRACE)
		f_block(a_nodes);
	else
		a_nodes.f_add(f_statement());
}

void t_parser::f_parse(ast::t_module& a_module)
{
	v_scope = &a_module;
	t_targets targets(false, false);
	v_targets = &targets;
	while (v_lexer.f_token() != t_lexer::e_token__EOF) a_module.v_block.f_add(f_statement());
	std::vector<ast::t_variable*> variables;
	variables.swap(a_module.v_privates);
	if (a_module.v_self_shared) ++a_module.v_shareds;
	for (std::vector<ast::t_variable*>::const_iterator i = variables.begin(); i != variables.end(); ++i) {
		ast::t_variable* p = *i;
		if (p->v_shared) {
			p->v_index = a_module.v_shareds++;
		} else {
			p->v_index = a_module.v_privates.size();
			a_module.v_privates.push_back(p);
		}
	}
}

t_transfer t_parser::t_error::f_instantiate(const std::wstring& a_message, t_lexer& a_lexer)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_error>());
	object.f_pointer__(new t_error(a_message, a_lexer));
	return object;
}

void t_parser::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%d:%d\n", v_path.c_str(), v_at.f_line(), v_at.f_column());
	f_print_with_caret(v_path.c_str(), v_at.f_position(), v_at.f_column());
	t_throwable::f_dump();
}

t_type* t_type_of<t_parser::t_error>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_parser::t_error>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
