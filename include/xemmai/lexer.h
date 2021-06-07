#ifndef XEMMAI__LEXER_H
#define XEMMAI__LEXER_H

#include "throwable.h"
#include "code.h"
#include <cwchar>
#include <cwctype>

namespace xemmai
{

struct t_lexer
{
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
		e_token__BREAK,
		e_token__CONTINUE,
		e_token__RETURN,
		e_token__THROW,
		e_token__IF,
		e_token__ELSE,
		e_token__WHILE,
		e_token__FOR,
		e_token__TRY,
		e_token__CATCH,
		e_token__FINALLY
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
	t_at v_at{0, 0, 0};
	bool v_newline;
	std::vector<wchar_t> v_indent;
	t_token v_token;
	std::vector<wchar_t> v_value;

	void f_throw [[noreturn]] ();
	void f_get();
	void f_read_indent()
	{
		while (v_c != L'\n' && std::iswspace(v_c)) {
			v_indent.push_back(v_c);
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
		static t_object* f_instantiate(t_lexer& a_lexer);

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
		return v_indent.size();
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
struct t_type_of<t_lexer::t_error> : t_uninstantiatable<t_bears<t_lexer::t_error, t_type_of<t_throwable>>>
{
	using t_base::t_base;
};

}

#endif
