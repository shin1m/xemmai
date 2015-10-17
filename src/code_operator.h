#ifdef XEMMAI__CODE__OTHERS
		XEMMAI__CODE__CASE_BEGIN(GET_AT)
				if (stack[1].f_tag() < t_value::e_tag__OBJECT) goto label__THROW_NOT_SUPPORTED;
				XEMMAI__CODE__OBJECT_CALL(f_get_at)
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(SET_AT)
				if (stack[1].f_tag() < t_value::e_tag__OBJECT) goto label__THROW_NOT_SUPPORTED;
				XEMMAI__CODE__OBJECT_CALL(f_set_at)
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__UNARY
		XEMMAI__CODE__CASE_BEGIN(PLUS)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					XEMMAI__CODE__PRIMITIVE_CALL(_P, a0.v_integer)
					break;
				case t_value::e_tag__FLOAT:
					XEMMAI__CODE__PRIMITIVE_CALL(_P, a0.v_float)
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_plus)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(MINUS)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					XEMMAI__CODE__PRIMITIVE_CALL(_P, -a0.v_integer)
					break;
				case t_value::e_tag__FLOAT:
					XEMMAI__CODE__PRIMITIVE_CALL(_P, -a0.v_float)
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_minus)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT)
				if (a0.f_tag() == t_value::e_tag__BOOLEAN) {
					XEMMAI__CODE__PRIMITIVE_CALL(_P, !a0.v_boolean)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_not)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(COMPLEMENT)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_P, ~a0.v_integer)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_complement)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY
		XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer * a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer * a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer * p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float * a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float * a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float * p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_multiply)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(DIVIDE)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer / a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer / a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer / p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float / a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float / a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float / p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_divide)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(MODULUS)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					f_check<intptr_t>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer % f_as<intptr_t>(a1))
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_modulus)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(ADD)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer + a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer + a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer + p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float + a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float + a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float + p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_add)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer - a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer - a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer - p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float - a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float - a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float - p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_subtract)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					f_check<intptr_t>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer << f_as<intptr_t>(a1))
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_left_shift)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					f_check<intptr_t>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<size_t>(a0.v_integer) >> f_as<intptr_t>(a1))
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_right_shift)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer < a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer < a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer < p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float < a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float < a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float < p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_less)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer <= a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer <= a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer <= p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float <= a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float <= a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float <= p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_less_equal)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer > a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer > a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer > p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float > a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float > a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float > p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_greater)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
				case t_value::e_tag__BOOLEAN:
					goto label__THROW_NOT_SUPPORTED;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer >= a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer >= a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer >= p->f_integer())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float >= a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float >= a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
							XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_float >= p->f_float())
						} else {
							goto label__THROW_NOT_SUPPORTED;
						}
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_greater_equal)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(EQUALS)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_p == a1.v_p)
					break;
				case t_value::e_tag__BOOLEAN:
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_p == a1.v_p && a0.v_boolean == a1.v_boolean)
					break;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer == a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer == a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0.v_integer == p->f_integer())
					} else {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, false)
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float == a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float == a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0.v_float == p->f_float())
					} else {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, false)
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_equals)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
				switch (a0.f_tag()) {
				case t_value::e_tag__NULL:
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_p != a1.v_p)
					break;
				case t_value::e_tag__BOOLEAN:
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_p != a1.v_p || a0.v_boolean != a1.v_boolean)
					break;
				case t_value::e_tag__INTEGER:
					if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer != a1.v_integer)
					} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer != a1.v_float)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0.v_integer != p->f_integer())
					} else {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, true)
					}
					break;
				case t_value::e_tag__FLOAT:
					if (a1.f_tag() == t_value::e_tag__FLOAT) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float != a1.v_float)
					} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float != a1.v_integer)
					} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
						t_object* p = a1;
						XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0.v_float != p->f_float())
					} else {
						XEMMAI__CODE__PRIMITIVE_CALL(_PP, true)
					}
					break;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_not_equals)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0 == a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0 != a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(AND)
				switch (a0.f_tag()) {
				case t_value::e_tag__BOOLEAN:
					f_check<bool>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0.v_boolean & f_as<bool>(a1)))
					break;
				case t_value::e_tag__INTEGER:
					f_check<intptr_t>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer & f_as<intptr_t>(a1))
					break;
				case t_value::e_tag__NULL:
				case t_value::e_tag__FLOAT:
					goto label__THROW_NOT_SUPPORTED;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_and)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(XOR)
				switch (a0.f_tag()) {
				case t_value::e_tag__BOOLEAN:
					f_check<bool>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0.v_boolean ^ f_as<bool>(a1)))
					break;
				case t_value::e_tag__INTEGER:
					f_check<intptr_t>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer ^ f_as<intptr_t>(a1))
					break;
				case t_value::e_tag__NULL:
				case t_value::e_tag__FLOAT:
					goto label__THROW_NOT_SUPPORTED;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_xor)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(OR)
				switch (a0.f_tag()) {
				case t_value::e_tag__BOOLEAN:
					f_check<bool>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0.v_boolean | f_as<bool>(a1)))
					break;
				case t_value::e_tag__INTEGER:
					f_check<intptr_t>(a1, L"argument0");
					XEMMAI__CODE__PRIMITIVE_CALL(, a0.v_integer | f_as<intptr_t>(a1))
					break;
				case t_value::e_tag__NULL:
				case t_value::e_tag__FLOAT:
					goto label__THROW_NOT_SUPPORTED;
				default:
					XEMMAI__CODE__OBJECT_CALL(f_or)
				}
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_XI
		XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer * a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float * a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_multiply)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(DIVIDE)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer / a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float / a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_divide)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(MODULUS)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer % a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_modulus)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(ADD)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer + a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float + a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_add)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer - a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float - a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_subtract)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer << a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_left_shift)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, static_cast<size_t>(a0.v_integer) >> a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_right_shift)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer < a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float < a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_less)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer <= a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float <= a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_less_equal)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer > a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float > a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_greater)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer >= a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float >= a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_greater_equal)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(EQUALS)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer == a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float == a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_equals)
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, false)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer != a1)
				} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float != a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_not_equals)
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, true)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0 == t_value(a1)))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0 != t_value(a1)))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(AND)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer & a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_and)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(XOR)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer ^ a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_xor)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(OR)
				if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer | a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_or)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_IX
		XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 * a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 * a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 * p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(DIVIDE)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 / a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 / a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 / p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(MODULUS)
				f_check<intptr_t>(a1, L"argument0");
				XEMMAI__CODE__PRIMITIVE_CALL(, a0 % f_as<intptr_t>(a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(ADD)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 + a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 + a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 + p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 - a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 - a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 - p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
				f_check<intptr_t>(a1, L"argument0");
				XEMMAI__CODE__PRIMITIVE_CALL(, a0 << f_as<intptr_t>(a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
				f_check<intptr_t>(a1, L"argument0");
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<size_t>(a0) >> f_as<intptr_t>(a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 < a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 < a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 < p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 <= a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 <= a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 <= p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 > a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 > a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 > p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 >= a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 >= a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 >= p->f_integer())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(EQUALS)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 == a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 == a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0 == p->f_integer())
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, false)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
				if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 != a1.v_integer)
				} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 != a1.v_float)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0 != p->f_integer())
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, true)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(t_value(a0) == a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(t_value(a0) != a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(AND)
				f_check<intptr_t>(a1, L"argument0");
				XEMMAI__CODE__PRIMITIVE_CALL(, a0 & f_as<intptr_t>(a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(XOR)
				f_check<intptr_t>(a1, L"argument0");
				XEMMAI__CODE__PRIMITIVE_CALL(, a0 ^ f_as<intptr_t>(a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(OR)
				f_check<intptr_t>(a1, L"argument0");
				XEMMAI__CODE__PRIMITIVE_CALL(, a0 | f_as<intptr_t>(a1))
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_XF
		XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float * a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer * a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_multiply)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(DIVIDE)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float / a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer / a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_divide)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(MODULUS)
				if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_modulus)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(ADD)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float + a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer + a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_add)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float - a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer - a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_subtract)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
				if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_left_shift)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
				if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_right_shift)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float < a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer < a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_less)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float <= a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer <= a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_less_equal)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float > a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer > a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_greater)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float >= a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer >= a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_greater_equal)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(EQUALS)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float == a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer == a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_equals)
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, false)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
				if (a0.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_float != a1)
				} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0.v_integer != a1)
				} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_not_equals)
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, true)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0 == t_value(a1)))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(a0 != t_value(a1)))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(AND)
				if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_and)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(XOR)
				if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_xor)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(OR)
				if (a0.f_tag() >= t_value::e_tag__OBJECT) {
					XEMMAI__CODE__OBJECT_CALL(f_or)
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_FX
		XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 * a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 * a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 * p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(DIVIDE)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 / a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 / a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 / p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_NA(MODULUS)
		XEMMAI__CODE__CASE_BEGIN(ADD)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 + a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 + a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 + p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 - a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 - a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 - p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_NA(LEFT_SHIFT)
		XEMMAI__CODE__CASE_NA(RIGHT_SHIFT)
		XEMMAI__CODE__CASE_BEGIN(LESS)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 < a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 < a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 < p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 <= a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 <= a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 <= p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 > a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 > a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 > p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 >= a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 >= a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
						XEMMAI__CODE__PRIMITIVE_CALL(, a0 >= p->f_float())
					} else {
						goto label__THROW_NOT_SUPPORTED;
					}
				} else {
					goto label__THROW_NOT_SUPPORTED;
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(EQUALS)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 == a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 == a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0 == p->f_float())
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, false)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
				if (a1.f_tag() == t_value::e_tag__FLOAT) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 != a1.v_float)
				} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, a0 != a1.v_integer)
				} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
					t_object* p = a1;
					XEMMAI__CODE__PRIMITIVE_CALL(, dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0 != p->f_float())
				} else {
					XEMMAI__CODE__PRIMITIVE_CALL(_PP, true)
				}
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(t_value(a0) == a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
				XEMMAI__CODE__PRIMITIVE_CALL(, static_cast<bool>(t_value(a0) != a1))
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_NA(AND)
		XEMMAI__CODE__CASE_NA(XOR)
		XEMMAI__CODE__CASE_NA(OR)
#endif
#ifdef XEMMAI__CODE__BINARY_NA
		XEMMAI__CODE__CASE_NA(MULTIPLY)
		XEMMAI__CODE__CASE_NA(DIVIDE)
		XEMMAI__CODE__CASE_NA(MODULUS)
		XEMMAI__CODE__CASE_NA(ADD)
		XEMMAI__CODE__CASE_NA(SUBTRACT)
		XEMMAI__CODE__CASE_NA(LEFT_SHIFT)
		XEMMAI__CODE__CASE_NA(RIGHT_SHIFT)
		XEMMAI__CODE__CASE_NA(LESS)
		XEMMAI__CODE__CASE_NA(LESS_EQUAL)
		XEMMAI__CODE__CASE_NA(GREATER)
		XEMMAI__CODE__CASE_NA(GREATER_EQUAL)
		XEMMAI__CODE__CASE_NA(EQUALS)
		XEMMAI__CODE__CASE_NA(NOT_EQUALS)
		XEMMAI__CODE__CASE_NA(IDENTICAL)
		XEMMAI__CODE__CASE_NA(NOT_IDENTICAL)
		XEMMAI__CODE__CASE_NA(AND)
		XEMMAI__CODE__CASE_NA(XOR)
		XEMMAI__CODE__CASE_NA(OR)
#endif
#ifdef XEMMAI__CODE__OTHERS
		XEMMAI__CODE__CASE_BEGIN(SEND)
				if (stack[1].f_tag() < t_value::e_tag__OBJECT) goto label__THROW_NOT_SUPPORTED;
				XEMMAI__CODE__OBJECT_CALL(f_send)
		XEMMAI__CODE__CASE_END
#endif
