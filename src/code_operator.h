#ifdef XEMMAI__CODE__OTHERS
				XEMMAI__CODE__CASE_BEGIN(GET_AT)
						if (stack[0].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_get_at, 1)
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SET_AT)
						if (stack[0].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_set_at, 2)
				XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__UNARY
				XEMMAI__CODE__CASE_BEGIN(PLUS)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer)
							break;
						case t_value::e_tag__FLOAT:
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float)
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_plus, 0)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MINUS)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(-a0.v_integer)
							break;
						case t_value::e_tag__FLOAT:
							XEMMAI__CODE__PRIMITIVE_CALL(-a0.v_float)
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_minus, 0)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT)
						if (a0.f_tag() == t_value::e_tag__BOOLEAN) {
							XEMMAI__CODE__PRIMITIVE_CALL(!a0.v_boolean)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_not, 0)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(COMPLEMENT)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(~a0.v_integer)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_complement, 0)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer * a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer * a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer * p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float * a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float * a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float * p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_multiply, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer / a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer / a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer / p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float / a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float / a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float / p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_divide, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MODULUS)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							f_check<intptr_t>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer % f_as<intptr_t>(a1))
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_modulus, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(ADD)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer + a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer + a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer + p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float + a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float + a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float + p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_add, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer - a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer - a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer - p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float - a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float - a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float - p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_subtract, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							f_check<intptr_t>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer << f_as<intptr_t>(a1))
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_left_shift, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							f_check<intptr_t>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(static_cast<size_t>(a0.v_integer) >> f_as<intptr_t>(a1))
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_right_shift, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer < a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer < a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer < p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float < a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float < a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float < p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_less, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer <= a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer <= a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer <= p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float <= a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float <= a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float <= p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_less_equal, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer > a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer > a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer > p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float > a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float > a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float > p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_greater, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer >= a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer >= a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer >= p->f_integer())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float >= a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float >= a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
									XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float >= p->f_float())
								} else {
									t_throwable::f_throw(L"not supported");
								}
							} else {
								t_throwable::f_throw(L"not supported");
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_greater_equal, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_p == a1.v_p)
							break;
						case t_value::e_tag__BOOLEAN:
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_p == a1.v_p && a0.v_boolean == a1.v_boolean)
							break;
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer == a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer == a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0.v_integer == p->f_integer())
							} else {
								XEMMAI__CODE__PRIMITIVE_CALL(false)
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float == a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float == a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0.v_float == p->f_float())
							} else {
								XEMMAI__CODE__PRIMITIVE_CALL(false)
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_equals, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
						switch (a0.f_tag()) {
						case t_value::e_tag__NULL:
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_p != a1.v_p)
							break;
						case t_value::e_tag__BOOLEAN:
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_p != a1.v_p || a0.v_boolean != a1.v_boolean)
							break;
						case t_value::e_tag__INTEGER:
							if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer != a1.v_integer)
							} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer != a1.v_float)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0.v_integer != p->f_integer())
							} else {
								XEMMAI__CODE__PRIMITIVE_CALL(true)
							}
							break;
						case t_value::e_tag__FLOAT:
							if (a1.f_tag() == t_value::e_tag__FLOAT) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float != a1.v_float)
							} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float != a1.v_integer)
							} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
								t_object* p = a1;
								XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0.v_float != p->f_float())
							} else {
								XEMMAI__CODE__PRIMITIVE_CALL(true)
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_not_equals, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0 == a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0 != a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(AND)
						switch (a0.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							f_check<bool>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0.v_boolean & f_as<bool>(a1)))
							break;
						case t_value::e_tag__INTEGER:
							f_check<intptr_t>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer & f_as<intptr_t>(a1))
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
						default:
							XEMMAI__CODE__OBJECT_CALL(f_and, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(XOR)
						switch (a0.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							f_check<bool>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0.v_boolean ^ f_as<bool>(a1)))
							break;
						case t_value::e_tag__INTEGER:
							f_check<intptr_t>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer ^ f_as<intptr_t>(a1))
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
						default:
							XEMMAI__CODE__OBJECT_CALL(f_xor, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(OR)
						switch (a0.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							f_check<bool>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0.v_boolean | f_as<bool>(a1)))
							break;
						case t_value::e_tag__INTEGER:
							f_check<intptr_t>(a1, L"argument0");
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer | f_as<intptr_t>(a1))
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
						default:
							XEMMAI__CODE__OBJECT_CALL(f_or, 1)
						}
				XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_XI
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer * a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float * a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_multiply, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer / a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float / a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_divide, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MODULUS)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer % a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_modulus, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(ADD)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer + a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float + a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_add, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer - a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float - a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_subtract, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer << a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_left_shift, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(static_cast<size_t>(a0.v_integer) >> a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_right_shift, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer < a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float < a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_less, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer <= a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float <= a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_less_equal, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer > a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float > a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_greater, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer >= a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float >= a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_greater_equal, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer == a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float == a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_equals, 1)
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(false)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer != a1)
						} else if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float != a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_not_equals, 1)
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(true)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0 == t_value(a1)))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0 != t_value(a1)))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(AND)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer & a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_and, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(XOR)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer ^ a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_xor, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(OR)
						if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer | a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_or, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_IX
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 * a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 * a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 * p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 / a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 / a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 / p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MODULUS)
						f_check<intptr_t>(a1, L"argument0");
						XEMMAI__CODE__PRIMITIVE_CALL(a0 % f_as<intptr_t>(a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(ADD)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 + a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 + a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 + p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 - a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 - a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 - p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
						f_check<intptr_t>(a1, L"argument0");
						XEMMAI__CODE__PRIMITIVE_CALL(a0 << f_as<intptr_t>(a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
						f_check<intptr_t>(a1, L"argument0");
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<size_t>(a0) >> f_as<intptr_t>(a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 < a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 < a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 < p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 <= a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 <= a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 <= p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 > a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 > a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 > p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 >= a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 >= a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 >= p->f_integer())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 == a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 == a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0 == p->f_integer())
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(false)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
						if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 != a1.v_integer)
						} else if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 != a1.v_float)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<intptr_t>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0 != p->f_integer())
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(true)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(t_value(a0) == a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(t_value(a0) != a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(AND)
						f_check<intptr_t>(a1, L"argument0");
						XEMMAI__CODE__PRIMITIVE_CALL(a0 & f_as<intptr_t>(a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(XOR)
						f_check<intptr_t>(a1, L"argument0");
						XEMMAI__CODE__PRIMITIVE_CALL(a0 ^ f_as<intptr_t>(a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(OR)
						f_check<intptr_t>(a1, L"argument0");
						XEMMAI__CODE__PRIMITIVE_CALL(a0 | f_as<intptr_t>(a1))
				XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_XF
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float * a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer * a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_multiply, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float / a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer / a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_divide, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MODULUS)
						if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_modulus, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(ADD)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float + a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer + a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_add, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float - a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer - a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_subtract, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
						if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_left_shift, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
						if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_right_shift, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float < a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer < a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_less, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float <= a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer <= a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_less_equal, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float > a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer > a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_greater, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float >= a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer >= a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_greater_equal, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float == a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer == a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_equals, 1)
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(false)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
						if (a0.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float != a1)
						} else if (a0.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer != a1)
						} else if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_not_equals, 1)
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(true)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0 == t_value(a1)))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(a0 != t_value(a1)))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(AND)
						if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_and, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(XOR)
						if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_xor, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(OR)
						if (a0.f_tag() >= t_value::e_tag__OBJECT) {
							XEMMAI__CODE__OBJECT_CALL(f_or, 1)
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_FX
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 * a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 * a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 * p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 / a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 / a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 / p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_NA(MODULUS)
				XEMMAI__CODE__CASE_BEGIN(ADD)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 + a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 + a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 + p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 - a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 - a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 - p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_NA(LEFT_SHIFT)
				XEMMAI__CODE__CASE_NA(RIGHT_SHIFT)
				XEMMAI__CODE__CASE_BEGIN(LESS)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 < a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 < a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 < p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 <= a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 <= a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 <= p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 > a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 > a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 > p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 >= a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 >= a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							if (dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type()))) {
								XEMMAI__CODE__PRIMITIVE_CALL(a0 >= p->f_float())
							} else {
								t_throwable::f_throw(L"not supported");
							}
						} else {
							t_throwable::f_throw(L"not supported");
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 == a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 == a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) != nullptr && a0 == p->f_float())
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(false)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
						if (a1.f_tag() == t_value::e_tag__FLOAT) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 != a1.v_float)
						} else if (a1.f_tag() == t_value::e_tag__INTEGER) {
							XEMMAI__CODE__PRIMITIVE_CALL(a0 != a1.v_integer)
						} else if (a1.f_tag() >= t_value::e_tag__OBJECT) {
							t_object* p = a1;
							XEMMAI__CODE__PRIMITIVE_CALL(dynamic_cast<t_type_of<double>*>(&f_as<t_type&>(p->f_type())) == nullptr || a0 != p->f_float())
						} else {
							XEMMAI__CODE__PRIMITIVE_CALL(true)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(t_value(a0) == a1))
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_IDENTICAL)
						XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(t_value(a0) != a1))
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
						if (stack[0].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_send, 1)
				XEMMAI__CODE__CASE_END
#endif
