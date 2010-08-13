#include <xemmai/lexer.h>

#include <cwctype>
#include <xemmai/global.h>

namespace xemmai
{

void t_lexer::f_throw()
{
	throw t_scoped(t_error::f_instantiate(*this));
}

void t_lexer::f_get()
{
	++v_position;
	switch (v_c) {
	case L'\n':
		v_position0 = v_position;
		++v_line0;
		v_column0 = 1;
		break;
	default:
		++v_column0;
	}
	v_c = std::getwc(v_stream);
}

namespace
{

inline bool f_is_symbol(wint_t a_c)
{
	return std::iswalnum(a_c) || a_c == L'_';
}

}

void t_lexer::f_next()
{
	while (true) {
		while (std::iswspace(v_c)) f_get();
		if (v_c != L'#') break;
		f_get();
		while (v_c != WEOF) {
			if (v_c == L'\n') {
				f_get();
				break;
			}
			f_get();
		}
	}
	v_position1 = v_position0;
	v_line1 = v_line0;
	v_column1 = v_column0;
	v_value.clear();
	switch (v_c) {
	case WEOF:
		v_token = e_token__EOF;
		break;
	case L'!':
		f_get();
		if (v_c == L'=') {
			f_get();
			if (v_c == L'=') {
				v_token = e_token__NOT_IDENTICAL;
				f_get();
			} else {
				v_token = e_token__NOT_EQUALS;
			}
		} else {
			v_token = e_token__EXCLAMATION;
		}
		break;
	case L'"':
		v_token = e_token__STRING;
		f_get();
		while (v_c != WEOF) {
			if (v_c == L'"') {
				f_get();
				break;
			} else if (v_c == L'\\') {
				f_get();
				switch (v_c) {
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
			f_get();
		}
		break;
	case L'$':
		v_token = e_token__SELF;
		f_get();
		while (v_c == L':' || v_c == L'^') {
			v_value.push_back(v_c);
			f_get();
		}
		break;
	case L'%':
		v_token = e_token__PERCENT;
		f_get();
		break;
	case L'&':
		f_get();
		if (v_c == L'&') {
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
		do {
			v_value.push_back(v_c);
			f_get();
		} while (v_c == L':');
		break;
	case L';':
		v_token = e_token__SEMICOLON;
		f_get();
		break;
	case L'<':
		f_get();
		switch (v_c) {
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
		f_get();
		if (v_c == L'=') {
			f_get();
			if (v_c == L'=') {
				v_token = e_token__IDENTICAL;
				f_get();
			} else {
				v_token = e_token__EQUALS;
			}
		} else {
			v_token = e_token__EQUAL;
		}
		break;
	case L'>':
		f_get();
		switch (v_c) {
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
		f_get();
		if (v_c == L'|') {
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
			switch (v_c) {
			case L'b':
				v_value.push_back(v_c);
				f_get();
				if (v_c == L'r') {
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'e') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'a') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'k') {
								f_get();
								if (!f_is_symbol(v_c)) {
									v_token = e_token__BREAK;
									return;
								}
							}
						}
					}
				}
				break;
			case L'c':
				v_value.push_back(v_c);
				f_get();
				switch (v_c) {
				case L'a':
					v_value.push_back(v_c);
					f_get();
					if (v_c == L't') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'c') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'h') {
								f_get();
								if (!f_is_symbol(v_c)) {
									v_token = e_token__CATCH;
									return;
								}
							}
						}
					}
					break;
				case L'o':
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'n') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L't') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'i') {
								v_value.push_back(v_c);
								f_get();
								if (v_c == L'n') {
									v_value.push_back(v_c);
									f_get();
									if (v_c == L'u') {
										v_value.push_back(v_c);
										f_get();
										if (v_c == L'e') {
											f_get();
											if (!f_is_symbol(v_c)) {
												v_token = e_token__CONTINUE;
												return;
											}
										}
									}
								}
							}
						}
					}
					break;
				}
				break;
			case L'e':
				v_value.push_back(v_c);
				f_get();
				if (v_c == L'l') {
					v_value.push_back(v_c);
					f_get();
					if (v_c == L's') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'e') {
							f_get();
							if (!f_is_symbol(v_c)) {
								v_token = e_token__ELSE;
								return;
							}
						}
					}
				}
				break;
			case L'f':
				v_value.push_back(v_c);
				f_get();
				switch (v_c) {
				case L'a':
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'l') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L's') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'e') {
								f_get();
								if (!f_is_symbol(v_c)) {
									v_token = e_token__FALSE;
									return;
								}
							}
						}
					}
					break;
				case L'i':
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'n') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'a') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'l') {
								v_value.push_back(v_c);
								f_get();
								if (v_c == L'l') {
									v_value.push_back(v_c);
									f_get();
									if (v_c == L'y') {
										f_get();
										if (!f_is_symbol(v_c)) {
											v_token = e_token__FINALLY;
											return;
										}
									}
								}
							}
						}
					}
					break;
				}
				break;
			case L'i':
				v_value.push_back(v_c);
				f_get();
				if (v_c == L'f') {
					f_get();
					if (!f_is_symbol(v_c)) {
						v_token = e_token__IF;
						return;
					}
				}
				break;
			case L'n':
				v_value.push_back(v_c);
				f_get();
				if (v_c == L'u') {
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'l') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'l') {
							f_get();
							if (!f_is_symbol(v_c)) {
								v_token = e_token__NULL;
								return;
							}
						}
					}
				}
				break;
			case L'r':
				v_value.push_back(v_c);
				f_get();
				if (v_c == L'e') {
					v_value.push_back(v_c);
					f_get();
					if (v_c == L't') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'u') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'r') {
								v_value.push_back(v_c);
								f_get();
								if (v_c == L'n') {
									f_get();
									if (!f_is_symbol(v_c)) {
										v_token = e_token__RETURN;
										return;
									}
								}
							}
						}
					}
				}
				break;
			case L't':
				v_value.push_back(v_c);
				f_get();
				switch (v_c) {
				case L'h':
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'r') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'o') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'w') {
								f_get();
								if (!f_is_symbol(v_c)) {
									v_token = e_token__THROW;
									return;
								}
							}
						}
					}
					break;
				case L'r':
					v_value.push_back(v_c);
					f_get();
					switch (v_c) {
					case L'u':
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'e') {
							f_get();
							if (!f_is_symbol(v_c)) {
								v_token = e_token__TRUE;
								return;
							}
						}
						break;
					case L'y':
						f_get();
						if (!f_is_symbol(v_c)) {
							v_token = e_token__TRY;
							return;
						}
					}
					break;
				}
				break;
			case L'w':
				v_value.push_back(v_c);
				f_get();
				if (v_c == L'h') {
					v_value.push_back(v_c);
					f_get();
					if (v_c == L'i') {
						v_value.push_back(v_c);
						f_get();
						if (v_c == L'l') {
							v_value.push_back(v_c);
							f_get();
							if (v_c == L'e') {
								f_get();
								if (!f_is_symbol(v_c)) {
									v_token = e_token__WHILE;
									return;
								}
							}
						}
					}
				}
				break;
			}
			while (f_is_symbol(v_c)) {
				v_value.push_back(v_c);
				f_get();
			}
			v_token = e_token__SYMBOL;
		} else if (std::iswdigit(v_c)) {
			do {
				v_value.push_back(v_c);
				f_get();
			} while (std::iswdigit(v_c));
			if (v_c == L'.') {
				v_token = e_token__FLOAT;
				do {
					v_value.push_back(v_c);
					f_get();
				} while (std::iswdigit(v_c));
			} else {
				v_token = e_token__INTEGER;
			}
			v_value.push_back(L'\0');
		} else {
			f_throw();
		}
	}
}

t_transfer t_lexer::t_error::f_instantiate(t_lexer& a_lexer)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_error>());
	object.f_pointer__(new t_error(a_lexer));
	return object;
}

void t_lexer::t_error::f_dump() const
{
	std::fprintf(stderr, "at %ls:%d:%d\n", v_path.c_str(), v_line, v_column);
	f_print_with_caret(v_path.c_str(), v_position, v_column);
	t_throwable::f_dump();
}

t_type* t_type_of<t_lexer::t_error>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_lexer::t_error>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
