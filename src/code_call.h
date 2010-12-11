				XEMMAI__CODE__CASE_BEGIN(GET_AT)
						if (x.f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_get_at, 1)
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SET_AT)
						if (x.f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_set_at, 2)
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(PLUS)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer);
							break;
						case t_value::e_tag__FLOAT:
							XEMMAI__CODE__PRIMITIVE_CALL(x.v_float);
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_plus, 0)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MINUS)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(-x.v_integer);
							break;
						case t_value::e_tag__FLOAT:
							XEMMAI__CODE__PRIMITIVE_CALL(-x.v_float);
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_minus, 0)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							XEMMAI__CODE__PRIMITIVE_CALL(!x.v_boolean);
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__INTEGER:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_not, 0)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(COMPLEMENT)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(~x.v_integer);
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_complement, 0)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer * f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float * f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_multiply, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer / f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float / f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_divide, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MODULUS)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer % f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_modulus, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(ADD)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer + f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float + f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_add, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer - f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float - f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_subtract, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer << f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_left_shift, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer >> f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_right_shift, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer < f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float < f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_less, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer <= f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float <= f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_less_equal, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer > f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float > f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_greater, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer >= f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_float >= f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_greater_equal, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_p == a0.v_p);
							}
							break;
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_p == a0.v_p && x.v_boolean == a0.v_boolean);
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(t_type_of<int>::f_equals(x.v_integer, a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(t_type_of<double>::f_equals(x.v_float, a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_equals, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_p != a0.v_p);
							}
							break;
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_p != a0.v_p || x.v_boolean != a0.v_boolean);
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(t_type_of<int>::f_not_equals(x.v_integer, a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack[1].f_transfer();
								XEMMAI__CODE__PRIMITIVE_CALL(t_type_of<double>::f_not_equals(x.v_float, a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_not_equals, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(AND)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<bool>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(x.v_boolean & f_as<bool>(a0)));
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer & f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_and, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(XOR)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<bool>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(x.v_boolean ^ f_as<bool>(a0)));
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer ^ f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_xor, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(OR)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<bool>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(static_cast<bool>(x.v_boolean | f_as<bool>(a0)));
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack[1].f_transfer();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(x.v_integer | f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_or, 1)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SEND)
						if (x.f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_send, 1)
				XEMMAI__CODE__CASE_END
