#include <xemmai/parser.h>

#include <xemmai/engine.h>
#include <xemmai/symbol.h>
#include <xemmai/integer.h>
#include <xemmai/float.h>
#include <xemmai/string.h>

namespace xemmai
{

void t_parser::f_throw(const std::wstring& a_message)
{
	throw t_scoped(t_error::f_instantiate(a_message, v_lexer));
}

void t_parser::f_get(long a_position, size_t a_line, size_t a_column, size_t a_outer, t_scope* a_scope, const t_transfer& a_symbol)
{
	while (a_scope) {
		t_hash::t_entry* field = a_scope->v_code->v_fields.f_find<t_object::t_hash_traits>(a_symbol);
		if (field) {
			if (a_outer > 0) {
				a_scope->v_shared = true;
				f_emit(e_instruction__SCOPE_GET);
				f_operand(a_outer);
			} else {
				f_emit(e_instruction__SCOPE_GET0);
			}
			f_operand(field->v_value->v_integer);
			f_at(a_position, a_line, a_column);
			return;
		}
		++a_outer;
		a_scope = a_scope->v_outer;
	}
	f_emit(e_instruction__GLOBAL_GET);
	f_operand(a_symbol);
	f_at(a_position, a_line, a_column);
}

size_t t_parser::f_index(t_scope* a_scope, const t_transfer& a_symbol)
{
	t_hash::t_entry* field = a_scope->v_code->v_fields.f_find<t_object::t_hash_traits>(a_symbol);
	if (field) return field->v_value->v_integer;
	t_code* p = f_as<t_code*>(a_scope->v_code);
	size_t index = p->v_size;
	a_scope->v_code->v_fields.f_put<t_object::t_hash_traits>(a_symbol, f_global()->f_as(index));
	++p->v_size;
	return index;
}

void t_parser::f_number(long a_position, size_t a_line, size_t a_column, t_lexer::t_token a_token)
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__INTEGER:
		{
			wchar_t *p;
			int value = std::wcstol(&v_lexer.f_value()[0], &p, 10);
			switch (a_token) {
			case t_lexer::e_token__HYPHEN:
				value = -value;
				break;
			case t_lexer::e_token__TILDE:
				value = ~value;
				break;
			}
			v_lexer.f_next();
			f_emit(e_instruction__INSTANCE);
			f_operand(f_global()->f_as(value));
		}
		break;
	case t_lexer::e_token__FLOAT:
		{
			wchar_t *p;
			double value = std::wcstod(&v_lexer.f_value()[0], &p);
			if (a_token == t_lexer::e_token__HYPHEN) value = -value;
			v_lexer.f_next();
			f_emit(e_instruction__INSTANCE);
			f_operand(f_global()->f_as(value));
		}
		break;
	}
	f_at(a_position, a_line, a_column);
}

void t_parser::f_target()
{
	long position = v_lexer.f_position();
	size_t line = v_lexer.f_line();
	size_t column = v_lexer.f_column();
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__LEFT_PARENTHESIS:
		v_lexer.f_next();
		f_expression();
		if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
		v_lexer.f_next();
		break;
	case t_lexer::e_token__ATMARK:
		{
			v_lexer.f_next();
			std::vector<std::wstring> symbols;
			if (v_lexer.f_token() == t_lexer::e_token__LEFT_PARENTHESIS) {
				v_lexer.f_next();
				if (v_lexer.f_token() == t_lexer::e_token__SYMBOL) {
					symbols.push_back(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1));
					v_lexer.f_next();
					while (v_lexer.f_token() == t_lexer::e_token__COMMA) {
						v_lexer.f_next();
						if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
						symbols.push_back(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1));
						v_lexer.f_next();
					}
				}
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
				v_lexer.f_next();
			}
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
			t_transfer code = t_code::f_instantiate(v_lexer.f_path(), symbols.size());
			for (size_t i = 0; i < symbols.size(); ++i) code->v_fields.f_put<t_object::t_hash_traits>(t_symbol::f_instantiate(symbols[i]), f_global()->f_as(i));
			t_scope scope(v_scope, code);
			v_scope = &scope;
			std::vector<void*>* instructions = v_instructions;
			v_instructions = &f_as<t_code*>(code)->v_instructions;
			std::vector<void*>* objects = v_objects;
			v_objects = &f_as<t_code*>(code)->v_objects;
			t_targets* targets0 = v_targets;
			std::vector<size_t> return0;
			t_targets targets1(0, 0, &return0);
			v_targets = &targets1;
			f_block();
			f_resolve(return0);
			f_emit(e_instruction__RETURN);
			f_at(position, line, column);
			f_as<t_code*>(code)->f_estimate();
			f_as<t_code*>(code)->f_tail();
			f_as<t_code*>(code)->f_generate(!scope.v_shared);
			v_scope = scope.v_outer;
			v_instructions = instructions;
			v_objects = objects;
			v_targets = targets0;
			f_emit(e_instruction__LAMBDA);
			f_operand(code);
			f_at(position, line, column);
		}
		break;
	case t_lexer::e_token__NULL:
		v_lexer.f_next();
		f_emit(e_instruction__INSTANCE);
		f_operand(f_global()->f_null());
		f_at(position, line, column);
		break;
	case t_lexer::e_token__TRUE:
		v_lexer.f_next();
		f_emit(e_instruction__INSTANCE);
		f_operand(f_global()->f_true());
		f_at(position, line, column);
		break;
	case t_lexer::e_token__FALSE:
		v_lexer.f_next();
		f_emit(e_instruction__INSTANCE);
		f_operand(f_global()->f_false());
		f_at(position, line, column);
		break;
	case t_lexer::e_token__INTEGER:
	case t_lexer::e_token__FLOAT:
		f_number(position, line, column, t_lexer::e_token__PLUS);
		break;
	case t_lexer::e_token__STRING:
		{
			std::wstring value(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1);
			v_lexer.f_next();
			f_emit(e_instruction__INSTANCE);
			f_operand(f_global()->f_as(value));
			f_at(position, line, column);
		}
		break;
	case t_lexer::e_token__LEFT_BRACKET:
		{
			v_lexer.f_next();
			f_emit(e_instruction__INSTANCE);
			f_operand(f_global()->f_type<t_array>());
			f_at(position, line, column);
			size_t n = 0;
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) {
				n = f_expressions();
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
			}
			v_lexer.f_next();
			f_emit(e_instruction__CALL);
			f_operand(n);
			f_at(position, line, column);
		}
		break;
	case t_lexer::e_token__LEFT_BRACE:
		{
			v_lexer.f_next();
			f_emit(e_instruction__INSTANCE);
			f_operand(f_global()->f_type<t_dictionary>());
			f_at(position, line, column);
			size_t n = 0;
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) {
				f_expression();
				++n;
				if (v_lexer.f_token() != t_lexer::e_token__COLON) f_throw(L"expecting ':'.");
				v_lexer.f_next();
				f_expression();
				++n;
				while (v_lexer.f_token() == t_lexer::e_token__COMMA) {
					v_lexer.f_next();
					f_expression();
					++n;
					if (v_lexer.f_token() != t_lexer::e_token__COLON) f_throw(L"expecting ':'.");
					v_lexer.f_next();
					f_expression();
					++n;
				}
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) f_throw(L"expecting '}'.");
			}
			v_lexer.f_next();
			f_emit(e_instruction__CALL);
			f_operand(n);
			f_at(position, line, column);
		}
		break;
	default:
		f_throw(L"unexpected token.");
	}
}

void t_parser::f_call()
{
	long position = v_lexer.f_position();
	size_t line = v_lexer.f_line();
	size_t column = v_lexer.f_column();
	v_lexer.f_next();
	size_t n = 0;
	if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) {
		n = f_expressions();
		if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
	}
	v_lexer.f_next();
	f_emit(e_instruction__CALL);
	f_operand(n);
	f_at(position, line, column);
}

void t_parser::f_getter()
{
	while (true) {
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__LEFT_PARENTHESIS:
			f_call();
			break;
		case t_lexer::e_token__DOT:
			{
				long position = v_lexer.f_position();
				size_t line = v_lexer.f_line();
				size_t column = v_lexer.f_column();
				v_lexer.f_next();
				switch (v_lexer.f_token()) {
				case t_lexer::e_token__COLON:
					f_emit(e_instruction__CLASS);
					break;
				case t_lexer::e_token__HAT:
					f_emit(e_instruction__SUPER);
					break;
				case t_lexer::e_token__TILDE:
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
					f_emit(e_instruction__OBJECT_REMOVE);
					f_operand(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1)));
					break;
				case t_lexer::e_token__SYMBOL:
					f_emit(e_instruction__OBJECT_GET);
					f_operand(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1)));
					break;
				default:
					f_throw(L"expecting ':' or '^' or '~' or symbol.");
				}
				f_at(position, line, column);
				v_lexer.f_next();
			}
			break;
		case t_lexer::e_token__LEFT_BRACKET:
			{
				long position = v_lexer.f_position();
				size_t line = v_lexer.f_line();
				size_t column = v_lexer.f_column();
				v_lexer.f_next();
				f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
				f_emit(e_instruction__GET_AT);
				f_at(position, line, column);
				v_lexer.f_next();
			}
			break;
		default:
			return;
		}
	}
}

void t_parser::f_primary()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__COLON:
	case t_lexer::e_token__SYMBOL:
	case t_lexer::e_token__SELF:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			size_t outer = 0;
			while (v_lexer.f_token() == t_lexer::e_token__COLON) {
				++outer;
				v_lexer.f_next();
			}
			switch (v_lexer.f_token()) {
			case t_lexer::e_token__SYMBOL:
				{
					t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1));
					v_lexer.f_next();
					t_scope* scope = v_scope;
					for (size_t i = 0; i < outer; ++i) {
						if (!scope) t_throwable::f_throw(L"no more outer scope.");
						scope = scope->v_outer;
					}
					f_get(position, line, column, outer, scope, symbol);
				}
				break;
			case t_lexer::e_token__SELF:
				{
					f_emit(e_instruction__SELF);
					f_operand(outer);
					f_at(position, line, column);
					std::vector<wchar_t>::const_iterator end = v_lexer.f_value().end() - 1;
					for (std::vector<wchar_t>::const_iterator i = v_lexer.f_value().begin(); i != end; ++i) {
						f_emit(*i == L':' ? e_instruction__CLASS : e_instruction__SUPER);
						f_at(position, line, column);
					}
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__SYMBOL) {
						f_emit(e_instruction__OBJECT_GET);
						f_operand(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1)));
						f_at(v_lexer.f_position(), v_lexer.f_line(), v_lexer.f_column());
						v_lexer.f_next();
					}
				}
				break;
			default:
				f_throw(L"expecting symbol or self.");
			}
		}
		break;
	default:
		f_target();
	}
	f_getter();
}

void t_parser::f_unary()
{
	while (true) {
		t_lexer::t_token token = v_lexer.f_token();
		switch (token) {
		case t_lexer::e_token__PLUS:
		case t_lexer::e_token__HYPHEN:
		case t_lexer::e_token__EXCLAMATION:
		case t_lexer::e_token__TILDE:
			break;
		case t_lexer::e_token__COLON:
		case t_lexer::e_token__SYMBOL:
		case t_lexer::e_token__SELF:
		case t_lexer::e_token__LEFT_PARENTHESIS:
		case t_lexer::e_token__ATMARK:
		case t_lexer::e_token__NULL:
		case t_lexer::e_token__TRUE:
		case t_lexer::e_token__FALSE:
		case t_lexer::e_token__INTEGER:
		case t_lexer::e_token__FLOAT:
		case t_lexer::e_token__STRING:
			f_primary();
		default:
			return;
		}
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		switch (token) {
		case t_lexer::e_token__PLUS:
		case t_lexer::e_token__HYPHEN:
			switch (v_lexer.f_token()) {
			case t_lexer::e_token__INTEGER:
			case t_lexer::e_token__FLOAT:
				f_number(position, line, column, token);
				f_getter();
				return;
			}
			f_unary();
			f_emit(token == t_lexer::e_token__PLUS ? e_instruction__PLUS : e_instruction__MINUS);
			break;
		case t_lexer::e_token__EXCLAMATION:
			f_unary();
			f_emit(e_instruction__NOT);
			break;
		case t_lexer::e_token__TILDE:
			if (v_lexer.f_token() == t_lexer::e_token__INTEGER) {
				f_number(position, line, column, token);
				f_getter();
				return;
			}
			f_unary();
			f_emit(e_instruction__COMPLEMENT);
			break;
		}
		f_at(position, line, column);
	}
}

void t_parser::f_multiplicative()
{
	while (true) {
		t_lexer::t_token token = v_lexer.f_token();
		switch (token) {
		case t_lexer::e_token__ASTERISK:
		case t_lexer::e_token__SLASH:
		case t_lexer::e_token__PERCENT:
			break;
		default:
			return;
		}
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		switch (token) {
		case t_lexer::e_token__ASTERISK:
			f_emit(e_instruction__MULTIPLY);
			break;
		case t_lexer::e_token__SLASH:
			f_emit(e_instruction__DIVIDE);
			break;
		case t_lexer::e_token__PERCENT:
			f_emit(e_instruction__MODULUS);
			break;
		}
		f_at(position, line, column);
	}
}

void t_parser::f_additive()
{
	while (true) {
		t_lexer::t_token token = v_lexer.f_token();
		switch (token) {
		case t_lexer::e_token__PLUS:
		case t_lexer::e_token__HYPHEN:
			break;
		default:
			return;
		}
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		switch (token) {
		case t_lexer::e_token__PLUS:
			f_emit(e_instruction__ADD);
			break;
		case t_lexer::e_token__HYPHEN:
			f_emit(e_instruction__SUBTRACT);
			break;
		}
		f_at(position, line, column);
	}
}

void t_parser::f_shift()
{
	while (true) {
		t_lexer::t_token token = v_lexer.f_token();
		switch (token) {
		case t_lexer::e_token__LEFT_SHIFT:
		case t_lexer::e_token__RIGHT_SHIFT:
			break;
		default:
			return;
		}
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		switch (token) {
		case t_lexer::e_token__LEFT_SHIFT:
			f_emit(e_instruction__LEFT_SHIFT);
			break;
		case t_lexer::e_token__RIGHT_SHIFT:
			f_emit(e_instruction__RIGHT_SHIFT);
			break;
		}
		f_at(position, line, column);
	}
}

void t_parser::f_relational()
{
	while (true) {
		t_lexer::t_token token = v_lexer.f_token();
		switch (token) {
		case t_lexer::e_token__LESS:
		case t_lexer::e_token__LESS_EQUAL:
		case t_lexer::e_token__GREATER:
		case t_lexer::e_token__GREATER_EQUAL:
			break;
		default:
			return;
		}
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		switch (token) {
		case t_lexer::e_token__LESS:
			f_emit(e_instruction__LESS);
			break;
		case t_lexer::e_token__LESS_EQUAL:
			f_emit(e_instruction__LESS_EQUAL);
			break;
		case t_lexer::e_token__GREATER:
			f_emit(e_instruction__GREATER);
			break;
		case t_lexer::e_token__GREATER_EQUAL:
			f_emit(e_instruction__GREATER_EQUAL);
			break;
		}
		f_at(position, line, column);
	}
}

void t_parser::f_equality()
{
	while (true) {
		t_lexer::t_token token = v_lexer.f_token();
		switch (token) {
		case t_lexer::e_token__EQUALS:
		case t_lexer::e_token__NOT_EQUALS:
		case t_lexer::e_token__IDENTICAL:
		case t_lexer::e_token__NOT_IDENTICAL:
			break;
		default:
			return;
		}
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		switch (token) {
		case t_lexer::e_token__EQUALS:
			f_emit(e_instruction__EQUALS);
			break;
		case t_lexer::e_token__NOT_EQUALS:
			f_emit(e_instruction__NOT_EQUALS);
			break;
		case t_lexer::e_token__IDENTICAL:
			f_emit(e_instruction__IDENTICAL);
			break;
		case t_lexer::e_token__NOT_IDENTICAL:
			f_emit(e_instruction__NOT_IDENTICAL);
			break;
		}
		f_at(position, line, column);
	}
}

void t_parser::f_and()
{
	while (v_lexer.f_token() == t_lexer::e_token__AMPERSAND) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		f_equality();
		f_emit(e_instruction__AND);
		f_at(position, line, column);
	}
}

void t_parser::f_xor()
{
	while (v_lexer.f_token() == t_lexer::e_token__HAT) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		f_equality();
		f_and();
		f_emit(e_instruction__XOR);
		f_at(position, line, column);
	}
}

void t_parser::f_or()
{
	while (v_lexer.f_token() == t_lexer::e_token__BAR) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		f_equality();
		f_and();
		f_xor();
		f_emit(e_instruction__OR);
		f_at(position, line, column);
	}
}

void t_parser::f_and_also()
{
	while (v_lexer.f_token() == t_lexer::e_token__AND_ALSO) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		std::vector<size_t> label0;
		std::vector<size_t> label1;
		f_emit(e_instruction__BRANCH);
		f_operand(label0);
		f_at(position, line, column);
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		f_equality();
		f_and();
		f_xor();
		f_or();
		f_emit(e_instruction__JUMP);
		f_operand(label1);
		f_resolve(label0);
		f_emit(e_instruction__INSTANCE);
		f_operand(f_global()->f_false());
		f_at(position, line, column);
		f_resolve(label1);
	}
}

void t_parser::f_or_else()
{
	while (v_lexer.f_token() == t_lexer::e_token__OR_ELSE) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		std::vector<size_t> label0;
		std::vector<size_t> label1;
		f_emit(e_instruction__BRANCH);
		f_operand(label0);
		f_emit(e_instruction__INSTANCE);
		f_operand(f_global()->f_true());
		f_emit(e_instruction__JUMP);
		f_operand(label1);
		f_at(position, line, column);
		f_resolve(label0);
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		f_equality();
		f_and();
		f_xor();
		f_or();
		f_and_also();
		f_resolve(label1);
	}
}

void t_parser::f_extension()
{
	while (v_lexer.f_token() == t_lexer::e_token__EXTEND) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		v_lexer.f_next();
		f_unary();
		f_multiplicative();
		f_additive();
		f_shift();
		f_relational();
		f_equality();
		f_and();
		f_xor();
		f_or();
		f_and_also();
		f_or_else();
		f_emit(e_instruction__EXTEND);
		f_emit(e_instruction__POP);
		f_at(position, line, column);
	}
}

void t_parser::f_expression()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__PLUS:
	case t_lexer::e_token__HYPHEN:
	case t_lexer::e_token__EXCLAMATION:
	case t_lexer::e_token__TILDE:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			t_lexer::t_token token = v_lexer.f_token();
			bool done = false;
			v_lexer.f_next();
			switch (token) {
			case t_lexer::e_token__PLUS:
			case t_lexer::e_token__HYPHEN:
				switch (v_lexer.f_token()) {
				case t_lexer::e_token__INTEGER:
				case t_lexer::e_token__FLOAT:
					f_number(position, line, column, token);
					break;
				default:
					f_unary();
					f_emit(token == t_lexer::e_token__PLUS ? e_instruction__PLUS : e_instruction__MINUS);
					f_at(position, line, column);
					done = true;
				}
				break;
			case t_lexer::e_token__EXCLAMATION:
				f_unary();
				f_emit(e_instruction__NOT);
				f_at(position, line, column);
				done = true;
				break;
			case t_lexer::e_token__TILDE:
				if (v_lexer.f_token() == t_lexer::e_token__INTEGER) {
					f_number(position, line, column, t_lexer::e_token__TILDE);
				} else {
					f_unary();
					f_emit(e_instruction__COMPLEMENT);
					f_at(position, line, column);
					done = true;
				}
				break;
			}
			if (done) {
				f_multiplicative();
				f_additive();
				f_shift();
				f_relational();
				f_equality();
				f_and();
				f_xor();
				f_or();
				f_and_also();
				f_or_else();
				f_extension();
				return;
			}
		}
		break;
	case t_lexer::e_token__COLON:
	case t_lexer::e_token__SYMBOL:
	case t_lexer::e_token__SELF:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			size_t outer = 0;
			while (v_lexer.f_token() == t_lexer::e_token__COLON) {
				++outer;
				v_lexer.f_next();
			}
			switch (v_lexer.f_token()) {
			case t_lexer::e_token__SYMBOL:
				{
					t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1));
					v_lexer.f_next();
					t_scope* scope = v_scope;
					for (size_t i = 0; i < outer; ++i) {
						if (!scope) t_throwable::f_throw(L"no more outer scope.");
						scope = scope->v_outer;
					}
					if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
						v_lexer.f_next();
						if (scope) {
							size_t index = f_index(scope, symbol);
							f_expression();
							if (outer > 0) {
								scope->v_shared = true;
								f_emit(e_instruction__SCOPE_PUT);
								f_operand(outer);
							} else {
								f_emit(e_instruction__SCOPE_PUT0);
							}
							f_operand(index);
						} else {
							f_emit(e_instruction__INSTANCE);
							f_operand(f_engine()->f_module_global());
							f_at(position, line, column);
							f_expression();
							f_emit(e_instruction__OBJECT_PUT);
							f_operand(symbol);
						}
						f_at(position, line, column);
						return;
					}
					f_get(position, line, column, outer, scope, symbol);
				}
				break;
			case t_lexer::e_token__SELF:
				{
					f_emit(e_instruction__SELF);
					f_operand(outer);
					f_at(position, line, column);
					std::vector<wchar_t>::const_iterator end = v_lexer.f_value().end() - 1;
					for (std::vector<wchar_t>::const_iterator i = v_lexer.f_value().begin(); i != end; ++i) {
						f_emit(*i == L':' ? e_instruction__CLASS : e_instruction__SUPER);
						f_at(position, line, column);
					}
					v_lexer.f_next();
					if (v_lexer.f_token() == t_lexer::e_token__SYMBOL) {
						long position = v_lexer.f_position();
						size_t line = v_lexer.f_line();
						size_t column = v_lexer.f_column();
						std::wstring symbol(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1);
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
							v_lexer.f_next();
							f_expression();
							f_emit(e_instruction__OBJECT_PUT);
							f_operand(t_symbol::f_instantiate(symbol));
							f_at(position, line, column);
							return;
						}
						f_emit(e_instruction__OBJECT_GET);
						f_operand(t_symbol::f_instantiate(symbol));
						f_at(position, line, column);
					}
				}
				break;
			default:
				f_throw(L"expecting symbol or self.");
			}
		}
		break;
	case t_lexer::e_token__IF:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			std::vector<size_t> label0;
			std::vector<size_t> label1;
			f_emit(e_instruction__BRANCH);
			f_operand(label0);
			f_at(position, line, column);
			f_block_or_statement();
			f_emit(e_instruction__JUMP);
			f_operand(label1);
			f_at(position, line, column);
			f_resolve(label0);
			if (v_lexer.f_token() == t_lexer::e_token__ELSE) {
				v_lexer.f_next();
				f_block_or_statement();
			} else {
				f_emit(e_instruction__INSTANCE);
				f_operand(f_global()->f_null());
				f_at(position, line, column);
			}
			f_resolve(label1);
		}
		return;
	case t_lexer::e_token__WHILE:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
			v_lexer.f_next();
			size_t loop = v_instructions->size();
			f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
			v_lexer.f_next();
			std::vector<size_t> label0;
			f_emit(e_instruction__BRANCH);
			f_operand(label0);
			f_at(position, line, column);
			t_targets* targets0 = v_targets;
			std::vector<size_t> break0;
			std::vector<size_t> continue0;
			t_targets targets1(&break0, &continue0, targets0->v_return);
			v_targets = &targets1;
			f_block_or_statement();
			position = v_lexer.f_position();
			line = v_lexer.f_line();
			column = v_lexer.f_column();
			f_emit(e_instruction__POP);
			f_emit(e_instruction__JUMP);
			f_operand(loop);
			f_resolve(label0);
			f_emit(e_instruction__INSTANCE);
			f_operand(f_global()->f_null());
			f_at(position, line, column);
			f_resolve(break0);
			f_resolve(continue0, loop);
			v_targets = targets0;
		}
		return;
	case t_lexer::e_token__TRY:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
			std::vector<size_t> catch0;
			std::vector<size_t> finally0;
			f_emit(e_instruction__TRY);
			f_operand(catch0);
			f_operand(finally0);
			f_at(position, line, column);
			t_targets* targets0 = v_targets;
			bool catching;
			{
				std::vector<size_t> break0;
				std::vector<size_t> continue0;
				std::vector<size_t> return0;
				t_targets targets1(targets0->v_break ? &break0 : 0, targets0->v_continue ? &continue0 : 0, targets0->v_return ? &return0 : 0);
				v_targets = &targets1;
				f_block();
				f_emit(e_instruction__FINALLY);
				f_operand(t_fiber::t_try::e_state__STEP);
				f_at(position, line, column);
				f_resolve(catch0);
				catching = v_lexer.f_token() == t_lexer::e_token__CATCH;
				if (catching) {
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__LEFT_PARENTHESIS) f_throw(L"expecting '('.");
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
					t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1));
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__RIGHT_PARENTHESIS) f_throw(L"expecting ')'.");
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
					f_emit(e_instruction__CATCH);
					f_operand(f_index(v_scope, symbol));
					f_at(position, line, column);
					f_block();
					f_emit(e_instruction__FINALLY);
					f_operand(t_fiber::t_try::e_state__STEP);
				} else {
					f_emit(e_instruction__FINALLY);
					f_operand(t_fiber::t_try::e_state__THROW);
				}
				f_resolve(break0);
				f_emit(e_instruction__FINALLY);
				f_operand(t_fiber::t_try::e_state__BREAK);
				f_resolve(continue0);
				f_emit(e_instruction__FINALLY);
				f_operand(t_fiber::t_try::e_state__CONTINUE);
				f_resolve(return0);
				f_emit(e_instruction__FINALLY);
				f_operand(t_fiber::t_try::e_state__RETURN);
				f_at(position, line, column);
			}
			f_resolve(finally0);
			if (v_lexer.f_token() == t_lexer::e_token__FINALLY) {
				v_lexer.f_next();
				if (v_lexer.f_token() != t_lexer::e_token__LEFT_BRACE) f_throw(L"expecting '{'.");
				v_lexer.f_next();
				t_targets targets2(0, 0, 0);
				v_targets = &targets2;
				while (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) {
					position = v_lexer.f_position();
					line = v_lexer.f_line();
					column = v_lexer.f_column();
					f_statement();
					f_emit(e_instruction__POP);
					f_at(position, line, column);
				}
				v_lexer.f_next();
			} else {
				if (!catching) f_throw(L"expecting 'finally'.");
			}
			f_emit(e_instruction__YRT);
			if (targets0->v_break)
				f_operand(*targets0->v_break);
			else
				f_operand(0);
			if (targets0->v_continue)
				f_operand(*targets0->v_continue);
			else
				f_operand(0);
			if (targets0->v_return)
				f_operand(*targets0->v_return);
			else
				f_operand(0);
			f_at(position, line, column);
			v_targets = targets0;
		}
		return;
	default:
		f_target();
	}
	while (true) {
		switch (v_lexer.f_token()) {
		case t_lexer::e_token__LEFT_PARENTHESIS:
			f_call();
			break;
		case t_lexer::e_token__DOT:
			{
				long position = v_lexer.f_position();
				size_t line = v_lexer.f_line();
				size_t column = v_lexer.f_column();
				v_lexer.f_next();
				switch (v_lexer.f_token()) {
				case t_lexer::e_token__COLON:
					f_emit(e_instruction__CLASS);
					v_lexer.f_next();
					break;
				case t_lexer::e_token__HAT:
					f_emit(e_instruction__SUPER);
					v_lexer.f_next();
					break;
				case t_lexer::e_token__TILDE:
					v_lexer.f_next();
					if (v_lexer.f_token() != t_lexer::e_token__SYMBOL) f_throw(L"expecting symbol.");
					f_emit(e_instruction__OBJECT_REMOVE);
					f_operand(t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1)));
					v_lexer.f_next();
					break;
				case t_lexer::e_token__SYMBOL:
					{
						t_transfer symbol = t_symbol::f_instantiate(std::wstring(v_lexer.f_value().begin(), v_lexer.f_value().end() - 1));
						v_lexer.f_next();
						if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
							v_lexer.f_next();
							f_expression();
							f_emit(e_instruction__OBJECT_PUT);
							f_operand(symbol);
							f_at(position, line, column);
							return;
						}
						f_emit(e_instruction__OBJECT_GET);
						f_operand(symbol);
					}
					break;
				default:
					f_throw(L"expecting ':' or '^' or '~' or symbol.");
				}
				f_at(position, line, column);
			}
			break;
		case t_lexer::e_token__LEFT_BRACKET:
			{
				long position = v_lexer.f_position();
				size_t line = v_lexer.f_line();
				size_t column = v_lexer.f_column();
				v_lexer.f_next();
				f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACKET) f_throw(L"expecting ']'.");
				v_lexer.f_next();
				if (v_lexer.f_token() == t_lexer::e_token__EQUAL) {
					v_lexer.f_next();
					f_expression();
					f_emit(e_instruction__SET_AT);
					f_at(position, line, column);
					return;
				}
				f_emit(e_instruction__GET_AT);
				f_at(position, line, column);
			}
			break;
		case t_lexer::e_token__ASTERISK:
		case t_lexer::e_token__SLASH:
		case t_lexer::e_token__PERCENT:
			f_multiplicative();
		case t_lexer::e_token__PLUS:
		case t_lexer::e_token__HYPHEN:
			f_additive();
		case t_lexer::e_token__LEFT_SHIFT:
		case t_lexer::e_token__RIGHT_SHIFT:
			f_shift();
		case t_lexer::e_token__LESS:
		case t_lexer::e_token__LESS_EQUAL:
		case t_lexer::e_token__GREATER:
		case t_lexer::e_token__GREATER_EQUAL:
			f_relational();
		case t_lexer::e_token__EQUALS:
		case t_lexer::e_token__NOT_EQUALS:
		case t_lexer::e_token__IDENTICAL:
		case t_lexer::e_token__NOT_IDENTICAL:
			f_equality();
		case t_lexer::e_token__AMPERSAND:
			f_and();
		case t_lexer::e_token__HAT:
			f_xor();
		case t_lexer::e_token__BAR:
			f_or();
		case t_lexer::e_token__AND_ALSO:
			f_and_also();
		case t_lexer::e_token__OR_ELSE:
			f_or_else();
		case t_lexer::e_token__EXTEND:
			f_extension();
		default:
			return;
		}
	}
}

size_t t_parser::f_expressions()
{
	f_expression();
	size_t n = 1;
	while (v_lexer.f_token() == t_lexer::e_token__COMMA) {
		v_lexer.f_next();
		f_expression();
		++n;
	}
	return n;
}

void t_parser::f_statement()
{
	switch (v_lexer.f_token()) {
	case t_lexer::e_token__IF:
	case t_lexer::e_token__WHILE:
	case t_lexer::e_token__TRY:
		f_expression();
		break;
	case t_lexer::e_token__BREAK:
		{
			if (!v_targets->v_break) f_throw(L"expecting within loop.");
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			if (v_lexer.f_token() == t_lexer::e_token__SEMICOLON) {
				f_emit(e_instruction__INSTANCE);
				f_operand(f_global()->f_null());
			} else {
				f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			}
			v_lexer.f_next();
			f_emit(e_instruction__JUMP);
			f_operand(*v_targets->v_break);
			f_at(position, line, column);
		}
		break;
	case t_lexer::e_token__CONTINUE:
		{
			if (!v_targets->v_continue) f_throw(L"expecting within loop.");
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			f_emit(e_instruction__JUMP);
			f_operand(*v_targets->v_continue);
			f_at(position, line, column);
		}
		break;
	case t_lexer::e_token__RETURN:
		{
			if (!v_targets->v_return) f_throw(L"expecting within lambda.");
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			if (v_lexer.f_token() == t_lexer::e_token__SEMICOLON) {
				f_emit(e_instruction__INSTANCE);
				f_operand(f_global()->f_null());
			} else {
				f_expression();
				if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			}
			v_lexer.f_next();
			f_emit(e_instruction__JUMP);
			f_operand(*v_targets->v_return);
			f_at(position, line, column);
		}
		break;
	case t_lexer::e_token__THROW:
		{
			long position = v_lexer.f_position();
			size_t line = v_lexer.f_line();
			size_t column = v_lexer.f_column();
			v_lexer.f_next();
			f_expression();
			if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
			v_lexer.f_next();
			f_emit(e_instruction__THROW);
			f_at(position, line, column);
		}
		break;
	default:
		f_expression();
		if (v_lexer.f_token() != t_lexer::e_token__SEMICOLON) f_throw(L"expecting ';'.");
		v_lexer.f_next();
	}
}

void t_parser::f_block()
{
	v_lexer.f_next();
	long position = v_lexer.f_position();
	size_t line = v_lexer.f_line();
	size_t column = v_lexer.f_column();
	if (v_lexer.f_token() == t_lexer::e_token__RIGHT_BRACE) {
		f_emit(e_instruction__INSTANCE);
		f_operand(f_global()->f_null());
		f_at(position, line, column);
	} else {
		f_statement();
		while (v_lexer.f_token() != t_lexer::e_token__RIGHT_BRACE) {
			f_emit(e_instruction__POP);
			f_at(position, line, column);
			position = v_lexer.f_position();
			line = v_lexer.f_line();
			column = v_lexer.f_column();
			f_statement();
		}
	}
	v_lexer.f_next();
}

void t_parser::f_block_or_statement()
{
	if (v_lexer.f_token() == t_lexer::e_token__LEFT_BRACE)
		f_block();
	else
		f_statement();
}

t_transfer t_parser::f_parse()
{
	t_transfer code = t_code::f_instantiate(v_lexer.f_path(), 0);
	t_scope scope(0, code);
	v_scope = &scope;
	v_instructions = &f_as<t_code*>(code)->v_instructions;
	v_objects = &f_as<t_code*>(code)->v_objects;
	t_targets targets(0, 0, 0);
	v_targets = &targets;
	while (v_lexer.f_token() != t_lexer::e_token__EOF) {
		long position = v_lexer.f_position();
		size_t line = v_lexer.f_line();
		size_t column = v_lexer.f_column();
		f_statement();
		f_emit(e_instruction__POP);
		f_at(position, line, column);
	}
	f_emit(e_instruction__END);
	f_as<t_code*>(code)->f_estimate();
	f_as<t_code*>(code)->f_generate(!scope.v_shared);
	return code;
}

t_transfer t_parser::t_error::f_instantiate(const std::wstring& a_message, t_lexer& a_lexer)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_error>());
	object->v_pointer = new t_error(a_message, a_lexer);
	return object;
}

void t_parser::t_error::f_dump() const
{
	std::fwprintf(stderr, L"at %ls:%d:%d\n", v_path.c_str(), v_line, v_column);
	f_print_with_caret(v_path.c_str(), v_position, v_column);
	t_throwable::f_dump();
}

t_type* t_type_of<t_parser::t_error>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_parser::t_error>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
