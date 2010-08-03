				XEMMAI__CODE__CASE_BEGIN(GET_AT, 1)
						if (x.f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_get_at)
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SET_AT, 2)
						if (x.f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_set_at)
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(PLUS, 0)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer);
							break;
						case t_value::e_tag__FLOAT:
							XEMMAI__CODE__PRIMITIVE_CALL(double, x.v_float);
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_plus)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MINUS, 0)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(int, -x.v_integer);
							break;
						case t_value::e_tag__FLOAT:
							XEMMAI__CODE__PRIMITIVE_CALL(double, -x.v_float);
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_minus)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT, 0)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							XEMMAI__CODE__PRIMITIVE_CALL(bool, !x.v_boolean);
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__INTEGER:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_not)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(COMPLEMENT, 0)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							XEMMAI__CODE__PRIMITIVE_CALL(int, ~x.v_integer);
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_complement)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MULTIPLY, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer * f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(double, x.v_float * f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_multiply)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(DIVIDE, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer / f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(double, x.v_float / f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_divide)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(MODULUS, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer % f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_modulus)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(ADD, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer + f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(double, x.v_float + f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_add)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SUBTRACT, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer - f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(double, x.v_float - f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_subtract)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LEFT_SHIFT, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer << f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_left_shift)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(RIGHT_SHIFT, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer >> f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_right_shift)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_integer < f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_float < f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_less)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(LESS_EQUAL, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_integer <= f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_float <= f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_less_equal)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_integer > f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_float > f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_greater)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(GREATER_EQUAL, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
						case t_value::e_tag__BOOLEAN:
							t_throwable::f_throw(L"not supported");
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_integer >= f_as<int>(a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								f_check<double>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_float >= f_as<double>(a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_greater_equal)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(EQUALS, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_p == a0.v_p);
							}
							break;
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_p == a0.v_p && x.v_boolean == a0.v_boolean);
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, t_type_of<int>::f_equals(x.v_integer, a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, t_type_of<double>::f_equals(x.v_float, a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_equals)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(NOT_EQUALS, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__NULL:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_p != a0.v_p);
							}
							break;
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, x.v_p != a0.v_p || x.v_boolean != a0.v_boolean);
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, t_type_of<int>::f_not_equals(x.v_integer, a0));
							}
							break;
						case t_value::e_tag__FLOAT:
							{
								t_transfer a0 = stack->f_pop();
								XEMMAI__CODE__PRIMITIVE_CALL(bool, t_type_of<double>::f_not_equals(x.v_float, a0));
							}
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_not_equals)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(AND, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack->f_pop();
								f_check<bool>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, static_cast<bool>(x.v_boolean & f_as<bool>(a0)));
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer & f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_and)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(XOR, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack->f_pop();
								f_check<bool>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, static_cast<bool>(x.v_boolean ^ f_as<bool>(a0)));
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer ^ f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_xor)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(OR, 1)
						switch (x.f_tag()) {
						case t_value::e_tag__BOOLEAN:
							{
								t_transfer a0 = stack->f_pop();
								f_check<bool>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(bool, static_cast<bool>(x.v_boolean | f_as<bool>(a0)));
							}
							break;
						case t_value::e_tag__INTEGER:
							{
								t_transfer a0 = stack->f_pop();
								f_check<int>(a0, L"argument0");
								XEMMAI__CODE__PRIMITIVE_CALL(int, x.v_integer | f_as<int>(a0));
							}
							break;
						case t_value::e_tag__NULL:
						case t_value::e_tag__FLOAT:
							t_throwable::f_throw(L"not supported");
							break;
						default:
							XEMMAI__CODE__OBJECT_CALL(f_or)
						}
				XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE_BEGIN(SEND, 1)
						if (x.f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
						XEMMAI__CODE__OBJECT_CALL(f_send)
				XEMMAI__CODE__CASE_END
