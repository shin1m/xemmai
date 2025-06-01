#ifndef XEMMAI__LEXER_H
#define XEMMAI__LEXER_H

#include "throwable.h"
#include "code.h"
#include <cwchar>
#include <cwctype>

namespace xemmai
{

struct XEMMAI__LOCAL t_lexer
{
	enum t_token
	{
		c_token__EOF,
		c_token__EXCLAMATION,
		c_token__PERCENT,
		c_token__AMPERSAND,
		c_token__APOSTROPHE,
		c_token__LEFT_PARENTHESIS,
		c_token__RIGHT_PARENTHESIS,
		c_token__ASTERISK,
		c_token__PLUS,
		c_token__COMMA,
		c_token__HYPHEN,
		c_token__DOT,
		c_token__SLASH,
		c_token__COLON,
		c_token__SEMICOLON,
		c_token__LESS,
		c_token__EQUAL,
		c_token__GREATER,
		c_token__QUESTION,
		c_token__ATMARK,
		c_token__LEFT_BRACKET,
		c_token__RIGHT_BRACKET,
		c_token__HAT,
		c_token__LEFT_BRACE,
		c_token__BAR,
		c_token__RIGHT_BRACE,
		c_token__TILDE,
		c_token__LEFT_SHIFT,
		c_token__RIGHT_SHIFT,
		c_token__EQUALS,
		c_token__NOT_EQUALS,
		c_token__IDENTICAL,
		c_token__NOT_IDENTICAL,
		c_token__LESS_EQUAL,
		c_token__GREATER_EQUAL,
		c_token__AND_ALSO,
		c_token__OR_ELSE,
		c_token__NULL,
		c_token__TRUE,
		c_token__FALSE,
		c_token__SYMBOL,
		c_token__SELF,
		c_token__INTEGER,
		c_token__FLOAT,
		c_token__STRING,
		c_token__BREAK,
		c_token__CONTINUE,
		c_token__RETURN,
		c_token__THROW,
		c_token__IF,
		c_token__ELSE,
		c_token__WHILE,
		c_token__FOR,
		c_token__TRY,
		c_token__CATCH,
		c_token__FINALLY
	};

private:
	static bool f_is_symbol(wint_t a_c)
	{
		return std::iswalnum(a_c) || a_c == L'_';
	}

	std::wstring v_path;
	std::FILE* v_stream;
	long v_position = 0;
	size_t v_line = 1;
	size_t v_column = 1;
	wint_t v_c;
	t_at v_at;
	bool v_newline;
	t_stringer v_indent;
	t_token v_token;
	t_stringer v_value;
	union
	{
		t_object* v_string;
		intptr_t v_integer;
		double v_float;
	};

	void f_throw [[noreturn]] ();
	void f_get();
	void f_read_indent()
	{
		while (v_c != L'\n' && std::iswspace(v_c)) {
			v_indent << v_c;
			f_get();
		}
	}
	void f_skip_line()
	{
		do {
			if (v_c == L'\n') {
				f_get();
				break;
			}
			f_get();
		} while (v_c != WEOF);
	}

public:
	struct t_error : t_throwable
	{
		std::wstring v_path;
		const t_at v_at;

		t_error(t_lexer& a_lexer) : t_throwable(L"lexical error."sv), v_path(a_lexer.f_path()), v_at(a_lexer.f_at())
		{
		}
		virtual void f_dump() const;
	};

	t_lexer(std::wstring_view a_path, std::FILE* a_stream);
	const std::wstring& f_path() const
	{
		return v_path;
	}
	const t_at& f_at() const
	{
		return v_at;
	}
	bool f_newline() const
	{
		return v_newline;
	}
	size_t f_indent() const
	{
		return static_cast<std::wstring_view>(v_indent).size();
	}
	t_token f_token() const
	{
		return v_token;
	}
	std::wstring_view f_value() const
	{
		return v_value;
	}
	t_object* f_string() const
	{
		return v_string;
	}
	intptr_t f_integer() const
	{
		return v_integer;
	}
	double f_float() const
	{
		return v_float;
	}
	void f_next();
};

template<>
struct t_type_of<t_lexer::t_error> : t_uninstantiatable<t_bears<t_lexer::t_error, t_type_of<t_throwable>>>
{
	using t_base::t_base;
};

}

#endif
