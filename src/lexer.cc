#include <xemmai/global.h>

namespace xemmai
{

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
		v_indent.clear();
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
			size_t n = v_indent.size();
			size_t i = 0;
			for (; i < n && v_c != L'\n' && std::iswspace(v_c); ++i) {
				if (v_c != v_indent[i]) f_throw();
				f_get();
			}
			f_read_indent();
			if (v_c != L'\n' && v_c != L'#') {
				if (i < n) v_indent.erase(v_indent.begin() + i, v_indent.end());
				break;
			}
			if (v_indent.size() > n) v_indent.erase(v_indent.begin() + n, v_indent.end());
		}
	}
	v_at = {v_position, v_line, v_column};
	v_value.clear();
	auto get = [&]
	{
		f_get();
		return v_c;
	};
	switch (v_c) {
	case WEOF:
		v_token = e_token__EOF;
		break;
	case L'!':
		if (get() != L'=') {
			v_token = e_token__EXCLAMATION;
		} else if (get() != L'=') {
			v_token = e_token__NOT_EQUALS;
		} else {
			v_token = e_token__NOT_IDENTICAL;
			f_get();
		}
		break;
	case L'"':
		v_token = e_token__STRING;
		while (get() != WEOF) {
			if (v_c == L'"') {
				f_get();
				break;
			} else if (v_c == L'\\') {
				switch (get()) {
				case L'"':
					v_value.push_back(L'"');
					break;
				case L'0':
					v_value.push_back(L'\0');
					break;
				case L'\\':
					v_value.push_back(L'\\');
					break;
				case L'a':
					v_value.push_back(L'\a');
					break;
				case L'b':
					v_value.push_back(L'\b');
					break;
				case L'f':
					v_value.push_back(L'\f');
					break;
				case L'n':
					v_value.push_back(L'\n');
					break;
				case L'r':
					v_value.push_back(L'\r');
					break;
				case L't':
					v_value.push_back(L'\t');
					break;
				case L'v':
					v_value.push_back(L'\v');
					break;
				default:
					f_throw();
				}
			} else {
				v_value.push_back(v_c);
			}
		}
		break;
	case L'$':
		v_token = e_token__SELF;
		f_get();
		while (v_c == L'@' || v_c == L'^') {
			v_value.push_back(v_c);
			f_get();
		}
		break;
	case L'%':
		v_token = e_token__PERCENT;
		f_get();
		break;
	case L'&':
		if (get() == L'&') {
			v_token = e_token__AND_ALSO;
			f_get();
		} else {
			v_token = e_token__AMPERSAND;
		}
		break;
	case L'\'':
		v_token = e_token__APOSTROPHE;
		f_get();
		break;
	case L'(':
		v_token = e_token__LEFT_PARENTHESIS;
		f_get();
		break;
	case L')':
		v_token = e_token__RIGHT_PARENTHESIS;
		f_get();
		break;
	case L'*':
		v_token = e_token__ASTERISK;
		f_get();
		break;
	case L'+':
		v_token = e_token__PLUS;
		f_get();
		break;
	case L',':
		v_token = e_token__COMMA;
		f_get();
		break;
	case L'-':
		v_token = e_token__HYPHEN;
		f_get();
		break;
	case L'.':
		v_token = e_token__DOT;
		f_get();
		break;
	case L'/':
		v_token = e_token__SLASH;
		f_get();
		break;
	case L':':
		v_token = e_token__COLON;
		do v_value.push_back(v_c); while (get() == L':');
		break;
	case L';':
		v_token = e_token__SEMICOLON;
		f_get();
		break;
	case L'<':
		switch (get()) {
		case L'<':
			v_token = e_token__LEFT_SHIFT;
			f_get();
			break;
		case L'=':
			v_token = e_token__LESS_EQUAL;
			f_get();
			break;
		default:
			v_token = e_token__LESS;
		}
		break;
	case L'=':
		if (get() != L'=') {
			v_token = e_token__EQUAL;
		} else if (get() != L'=') {
			v_token = e_token__EQUALS;
		} else {
			v_token = e_token__IDENTICAL;
			f_get();
		}
		break;
	case L'>':
		switch (get()) {
		case L'=':
			v_token = e_token__GREATER_EQUAL;
			f_get();
			break;
		case L'>':
			v_token = e_token__RIGHT_SHIFT;
			f_get();
			break;
		default:
			v_token = e_token__GREATER;
		}
		break;
	case L'?':
		v_token = e_token__QUESTION;
		f_get();
		break;
	case L'@':
		v_token = e_token__ATMARK;
		f_get();
		break;
	case L'[':
		v_token = e_token__LEFT_BRACKET;
		f_get();
		break;
	case L']':
		v_token = e_token__RIGHT_BRACKET;
		f_get();
		break;
	case L'^':
		v_token = e_token__HAT;
		f_get();
		break;
	case L'{':
		v_token = e_token__LEFT_BRACE;
		f_get();
		break;
	case L'|':
		if (get() == L'|') {
			v_token = e_token__OR_ELSE;
			f_get();
		} else {
			v_token = e_token__BAR;
		}
		break;
	case L'}':
		v_token = e_token__RIGHT_BRACE;
		f_get();
		break;
	case L'~':
		v_token = e_token__TILDE;
		f_get();
		break;
	default:
		if (std::iswalpha(v_c) || v_c == L'_') {
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
				v_value.insert(v_value.end(), a_s.begin(), i);
				return false;
			};
			switch (v_c) {
			case L'b':
				if (match(L"break"sv, 1, e_token__BREAK)) return;
				break;
			case L'c':
				switch (get()) {
				case L'a':
					if (match(L"catch"sv, 2, e_token__CATCH)) return;
					break;
				case L'o':
					if (match(L"continue"sv, 2, e_token__CONTINUE)) return;
					break;
				default:
					v_value.push_back(L'c');
				}
				break;
			case L'e':
				if (match(L"else"sv, 1, e_token__ELSE)) return;
				break;
			case L'f':
				switch (get()) {
				case L'a':
					if (match(L"false"sv, 2, e_token__FALSE)) return;
					break;
				case L'i':
					if (match(L"finally"sv, 2, e_token__FINALLY)) return;
					break;
				case L'o':
					if (match(L"for"sv, 2, e_token__FOR)) return;
					break;
				default:
					v_value.push_back(L'f');
				}
				break;
			case L'i':
				if (match(L"if"sv, 1, e_token__IF)) return;
				break;
			case L'n':
				if (match(L"null"sv, 1, e_token__NULL)) return;
				break;
			case L'r':
				if (match(L"return"sv, 1, e_token__RETURN)) return;
				break;
			case L't':
				switch (get()) {
				case L'h':
					if (match(L"throw"sv, 2, e_token__THROW)) return;
					break;
				case L'r':
					switch (get()) {
					case L'u':
						if (match(L"true"sv, 3, e_token__TRUE)) return;
						break;
					case L'y':
						if (match(L"try"sv, 3, e_token__TRY)) return;
						break;
					default:
						v_value.push_back(L't');
						v_value.push_back(L'r');
					}
					break;
				default:
					v_value.push_back(L't');
				}
				break;
			case L'w':
				if (match(L"while"sv, 1, e_token__WHILE)) return;
				break;
			}
			while (f_is_symbol(v_c)) {
				v_value.push_back(v_c);
				f_get();
			}
			v_token = e_token__SYMBOL;
		} else if (std::iswdigit(v_c)) {
			if (v_c == L'0') {
				v_value.push_back(v_c);
				switch (get()) {
				case L'.':
					break;
				case L'X':
				case L'x':
					v_token = e_token__INTEGER;
					v_value.push_back(v_c);
					if (!std::iswxdigit(get())) f_throw();
					do v_value.push_back(v_c); while (std::iswxdigit(get()));
					v_value.push_back(L'\0');
					return;
				default:
					v_token = e_token__INTEGER;
					while (std::iswdigit(v_c)) {
						if (v_c >= L'8') f_throw();
						v_value.push_back(v_c);
						f_get();
					}
					v_value.push_back(L'\0');
					return;
				}
			}
			while (std::iswdigit(v_c)) {
				v_value.push_back(v_c);
				f_get();
			}
			if (v_c == L'.') {
				v_token = e_token__FLOAT;
				do v_value.push_back(v_c); while (std::iswdigit(get()));
				if (v_c == L'E' || v_c == L'e') {
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'+' || v_c == L'-') {
						v_value.push_back(v_c);
						f_get();
					}
					if (!std::iswdigit(v_c)) f_throw();
					do v_value.push_back(v_c); while (std::iswdigit(get()));
				}
			} else {
				v_token = e_token__INTEGER;
			}
			v_value.push_back(L'\0');
		} else {
			f_throw();
		}
	}
}

t_object* t_lexer::t_error::f_instantiate(t_lexer& a_lexer)
{
	return f_new<t_error>(f_global(), a_lexer);
}

void t_lexer::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%zu:%zu\n", v_path.c_str(), v_at.v_line, v_at.v_column);
	f_print_with_caret(stderr, v_path, v_at.v_position, v_at.v_column);
	t_throwable::f_dump();
}

}
