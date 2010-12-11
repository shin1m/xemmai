#ifndef XEMMAI__LEXER_H
#define XEMMAI__LEXER_H

#include <vector>
#include <cwchar>
#include <cwctype>

#include "throwable.h"
#include "code.h"

namespace xemmai
{

class t_lexer
{
public:
	enum t_token
	{
		e_token__EOF,
		e_token__EXCLAMATION,
		e_token__PERCENT,
		e_token__AMPERSAND,
		e_token__APOSTROPHE,
		e_token__LEFT_PARENTHESIS,
		e_token__RIGHT_PARENTHESIS,
		e_token__ASTERISK,
		e_token__PLUS,
		e_token__COMMA,
		e_token__HYPHEN,
		e_token__DOT,
		e_token__SLASH,
		e_token__COLON,
		e_token__SEMICOLON,
		e_token__LESS,
		e_token__EQUAL,
		e_token__GREATER,
		e_token__QUESTION,
		e_token__ATMARK,
		e_token__LEFT_BRACKET,
		e_token__RIGHT_BRACKET,
		e_token__HAT,
		e_token__LEFT_BRACE,
		e_token__BAR,
		e_token__RIGHT_BRACE,
		e_token__TILDE,
		e_token__LEFT_SHIFT,
		e_token__RIGHT_SHIFT,
		e_token__EQUALS,
		e_token__NOT_EQUALS,
		e_token__IDENTICAL,
		e_token__NOT_IDENTICAL,
		e_token__LESS_EQUAL,
		e_token__GREATER_EQUAL,
		e_token__AND_ALSO,
		e_token__OR_ELSE,
		e_token__NULL,
		e_token__TRUE,
		e_token__FALSE,
		e_token__SYMBOL,
		e_token__SELF,
		e_token__INTEGER,
		e_token__FLOAT,
		e_token__STRING,
		e_token__IF,
		e_token__ELSE,
		e_token__WHILE,
		e_token__BREAK,
		e_token__CONTINUE,
		e_token__RETURN,
		e_token__TRY,
		e_token__CATCH,
		e_token__FINALLY,
		e_token__THROW,
	};

private:
	static bool f_is_symbol(wint_t a_c)
	{
		return std::iswalnum(a_c) || a_c == L'_';
	}

	std::wstring v_path;
	FILE* v_stream;
	long v_p;
	long v_position;
	size_t v_line;
	size_t v_column;
	wint_t v_c;
	t_at v_at;
	t_token v_token;
	std::vector<wchar_t> v_value;

	void f_throw();
	void f_get();

public:
	struct t_error : t_throwable
	{
		static t_transfer f_instantiate(t_lexer& a_lexer);

		std::wstring v_path;
		t_at v_at;

		t_error(t_lexer& a_lexer) : t_throwable(L"lexical error."), v_path(a_lexer.f_path()), v_at(a_lexer.f_at())
		{
		}
		virtual void f_dump() const;
	};

	t_lexer(const std::wstring& a_path, FILE* a_stream) :
	v_path(a_path), v_stream(a_stream), v_p(0), v_position(0), v_line(1), v_column(1), v_c(std::getwc(v_stream)), v_at(0, 0, 0)
	{
		f_next();
	}
	const std::wstring& f_path() const
	{
		return v_path;
	}
	const t_at& f_at() const
	{
		return v_at;
	}
	t_token f_token() const
	{
		return v_token;
	}
	const std::vector<wchar_t>& f_value() const
	{
		return v_value;
	}
	void f_next();
};

template<>
struct t_type_of<t_lexer::t_error> : t_type_of<t_throwable>
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<t_throwable>(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
