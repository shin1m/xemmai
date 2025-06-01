#include <xemmai/global.h>
#include <charconv>

namespace xemmai
{

void t_lexer::f_throw()
{
	throw t_rvalue(f_new<t_error>(f_global(), *this));
}

void t_lexer::f_get()
{
	switch (v_c) {
	case L'\n':
		v_position = std::ftell(v_stream);
		++v_line;
		v_column = 1;
		break;
	default:
		++v_column;
	}
	v_c = std::getwc(v_stream);
}

t_lexer::t_lexer(std::wstring_view a_path, std::FILE* a_stream) : v_path(a_path), v_stream(a_stream), v_c(std::getwc(v_stream))
{
	while (true) {
		f_read_indent();
		if (v_c != L'\n' && v_c != L'#') break;
		v_indent.f_rewind(0);
		f_skip_line();
	}
	f_next();
}

void t_lexer::f_next()
{
	while (v_c != L'\n' && std::iswspace(v_c)) f_get();
	v_newline = v_c == L'\n' || v_c == L'#';
	if (v_newline) {
		while (true) {
			f_skip_line();
			std::wstring_view s = v_indent;
			size_t n = s.size();
			size_t i = 0;
			for (; i < n && v_c != L'\n' && std::iswspace(v_c); ++i) {
				if (v_c != s[i]) f_throw();
				f_get();
			}
			f_read_indent();
			if (v_c != L'\n' && v_c != L'#') {
				if (i < n) v_indent.f_rewind(i);
				break;
			}
			if (f_indent() > n) v_indent.f_rewind(n);
		}
	}
	v_at = {v_position, v_line, v_column};
	auto get = [&]
	{
		f_get();
		return v_c;
	};
	auto set = [&](auto a_token)
	{
		v_token = a_token;
	};
	auto next = [&](auto a_token)
	{
		v_token = a_token;
		f_get();
	};
	switch (v_c) {
	case WEOF:
		return set(c_token__EOF);
	case L'!':
		if (get() != L'=') return set(c_token__EXCLAMATION);
		if (get() != L'=') return set(c_token__NOT_EQUALS);
		return next(c_token__NOT_IDENTICAL);
	case L'"':
		for (t_stringer s;;)
			switch (get()) {
			case WEOF:
				f_throw();
			case L'"':
				v_string = s;
				return next(c_token__STRING);
			case L'\\':
				switch (get()) {
				case L'"':
					s << L'"';
					continue;
				case L'0':
					s << L'\0';
					continue;
				case L'\\':
					s << L'\\';
					continue;
				case L'a':
					s << L'\a';
					continue;
				case L'b':
					s << L'\b';
					continue;
				case L'f':
					s << L'\f';
					continue;
				case L'n':
					s << L'\n';
					continue;
				case L'r':
					s << L'\r';
					continue;
				case L't':
					s << L'\t';
					continue;
				case L'v':
					s << L'\v';
					continue;
				default:
					f_throw();
				}
			default:
				s << v_c;
			}
	case L'$':
		f_get();
		v_value.f_rewind(0);
		while (v_c == L'@' || v_c == L'^') {
			v_value << v_c;
			f_get();
		}
		return set(c_token__SELF);
	case L'%':
		return next(c_token__PERCENT);
	case L'&':
		if (get() == L'&') return next(c_token__AND_ALSO);
		return set(c_token__AMPERSAND);
	case L'\'':
		return next(c_token__APOSTROPHE);
	case L'(':
		return next(c_token__LEFT_PARENTHESIS);
	case L')':
		return next(c_token__RIGHT_PARENTHESIS);
	case L'*':
		return next(c_token__ASTERISK);
	case L'+':
		return next(c_token__PLUS);
	case L',':
		return next(c_token__COMMA);
	case L'-':
		return next(c_token__HYPHEN);
	case L'.':
		return next(c_token__DOT);
	case L'/':
		return next(c_token__SLASH);
	case L':':
		v_integer = 1;
		while (get() == L':') ++v_integer;
		return set(c_token__COLON);
	case L';':
		return next(c_token__SEMICOLON);
	case L'<':
		switch (get()) {
		case L'<':
			return next(c_token__LEFT_SHIFT);
		case L'=':
			return next(c_token__LESS_EQUAL);
		}
		return set(c_token__LESS);
	case L'=':
		if (get() != L'=') return set(c_token__EQUAL);
		if (get() != L'=') return set(c_token__EQUALS);
		return next(c_token__IDENTICAL);
	case L'>':
		switch (get()) {
		case L'=':
			return next(c_token__GREATER_EQUAL);
		case L'>':
			return next(c_token__RIGHT_SHIFT);
		}
		return set(c_token__GREATER);
	case L'?':
		return next(c_token__QUESTION);
	case L'@':
		return next(c_token__ATMARK);
	case L'[':
		return next(c_token__LEFT_BRACKET);
	case L']':
		return next(c_token__RIGHT_BRACKET);
	case L'^':
		return next(c_token__HAT);
	case L'{':
		return next(c_token__LEFT_BRACE);
	case L'|':
		if (get() == L'|') return next(c_token__OR_ELSE);
		return set(c_token__BAR);
	case L'}':
		return next(c_token__RIGHT_BRACE);
	case L'~':
		return next(c_token__TILDE);
	default:
		if (std::iswalpha(v_c) || v_c == L'_') {
			v_value.f_rewind(0);
			auto match = [&](auto a_s, auto a_i, auto a_token)
			{
				auto i = a_s.begin() + a_i;
				for (;; ++i) {
					if (i == a_s.end()) {
						if (!f_is_symbol(get())) {
							v_token = a_token;
							return true;
						}
						break;
					}
					if (get() != *i) break;
				}
				v_value << std::wstring_view{a_s.begin(), i};
				return false;
			};
			switch (v_c) {
			case L'b':
				if (match(L"break"sv, 1, c_token__BREAK)) return;
				break;
			case L'c':
				switch (get()) {
				case L'a':
					if (match(L"catch"sv, 2, c_token__CATCH)) return;
					break;
				case L'o':
					if (match(L"continue"sv, 2, c_token__CONTINUE)) return;
					break;
				default:
					v_value << L'c';
				}
				break;
			case L'e':
				if (match(L"else"sv, 1, c_token__ELSE)) return;
				break;
			case L'f':
				switch (get()) {
				case L'a':
					if (match(L"false"sv, 2, c_token__FALSE)) return;
					break;
				case L'i':
					if (match(L"finally"sv, 2, c_token__FINALLY)) return;
					break;
				case L'o':
					if (match(L"for"sv, 2, c_token__FOR)) return;
					break;
				default:
					v_value << L'f';
				}
				break;
			case L'i':
				if (match(L"if"sv, 1, c_token__IF)) return;
				break;
			case L'n':
				if (match(L"null"sv, 1, c_token__NULL)) return;
				break;
			case L'r':
				if (match(L"return"sv, 1, c_token__RETURN)) return;
				break;
			case L't':
				switch (get()) {
				case L'h':
					if (match(L"throw"sv, 2, c_token__THROW)) return;
					break;
				case L'r':
					switch (get()) {
					case L'u':
						if (match(L"true"sv, 3, c_token__TRUE)) return;
						break;
					case L'y':
						if (match(L"try"sv, 3, c_token__TRY)) return;
						break;
					default:
						v_value << L't' << L'r';
					}
					break;
				default:
					v_value << L't';
				}
				break;
			case L'w':
				if (match(L"while"sv, 1, c_token__WHILE)) return;
				break;
			}
			while (f_is_symbol(v_c)) {
				v_value << v_c;
				f_get();
			}
			v_token = c_token__SYMBOL;
		} else if (std::iswdigit(v_c)) {
			if (v_c == L'0' && get() != L'.') {
				v_integer = 0;
				if (v_c == L'X' || v_c == L'x') {
					if (!std::iswxdigit(get())) f_throw();
					do {
						auto i = v_c - L'0';
						v_integer = v_integer * 16 + (i < 10 ? i : i - 7 & 0xf);
					} while (std::iswxdigit(get()));
				} else {
					while (std::iswdigit(v_c)) {
						if (v_c >= L'8') f_throw();
						v_integer = v_integer * 8 + (v_c - L'0');
						f_get();
					}
				}
				v_integer &= ~uintptr_t{} >> 1;
				return set(c_token__INTEGER);
			}
			char cs[32];
			auto p = cs;
			auto push = [&]
			{
				if (p >= cs + sizeof(cs)) f_throw();
				*p++ = v_c;
			};
			while (std::iswdigit(v_c)) {
				push();
				f_get();
			}
			if (v_c == L'.') {
				v_token = c_token__FLOAT;
				do push(); while (std::iswdigit(get()));
				if (v_c == L'E' || v_c == L'e') {
					push();
					f_get();
					if (v_c == L'+' || v_c == L'-') {
						push();
						f_get();
					}
					if (!std::iswdigit(v_c)) f_throw();
					do push(); while (std::iswdigit(get()));
				}
				if (std::from_chars(cs, p, v_float).ec != std::errc()) f_throw();
			} else {
				v_token = c_token__INTEGER;
				if (std::from_chars(cs, p, v_integer).ec != std::errc()) f_throw();
			}
		} else {
			f_throw();
		}
	}
}

void t_lexer::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%zu:%zu\n", v_path.c_str(), v_at.v_line, v_at.v_column);
	f_print_with_caret(stderr, v_path, v_at.v_position, v_at.v_column);
	t_throwable::f_dump();
}

}
