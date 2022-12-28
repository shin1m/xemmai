#define XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
			if (a0.f_tag() >= c_tag__OBJECT) {\
				XEMMAI__CODE__OBJECT_CALL(a_method)\
			} else {\
				goto label__THROW_NOT_SUPPORTED;\
			}
#define XEMMAI__CODE__OBJECT_OR_FALSE(a_operator, a_method)\
			if (a0.f_tag() >= c_tag__OBJECT) {\
				XEMMAI__CODE__OBJECT_CALL(a_method)\
			} else {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(false))\
			}
#define XEMMAI__CODE__OTHERS_OBJECT(a_name, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
		XEMMAI__CODE__CASE_END
#ifdef XEMMAI__CODE__OTHERS
		XEMMAI__CODE__OTHERS_OBJECT(GET_AT, f_get_at)
		XEMMAI__CODE__OTHERS_OBJECT(SET_AT, f_set_at)
#endif
#ifdef XEMMAI__CODE__UNARY
#define XEMMAI__CODE__UNARY_ARITHMETIC(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			switch (a0.f_tag()) {\
			case c_tag__NULL:\
				goto label__THROW_NOT_SUPPORTED;\
			case c_tag__INTEGER:\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_integer))\
				break;\
			case c_tag__FLOAT:\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_float))\
				break;\
			default:\
				XEMMAI__CODE__OBJECT_CALL(a_method)\
			}\
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__UNARY_ARITHMETIC(PLUS, , f_plus)
		XEMMAI__CODE__UNARY_ARITHMETIC(MINUS, -, f_minus)
		XEMMAI__CODE__CASE_BEGIN(NOT)
			XEMMAI__CODE__PRIMITIVE_CALL(!a0)
		XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE_BEGIN(COMPLEMENT)
			if (a0.f_tag() == c_tag__INTEGER) {
				XEMMAI__CODE__PRIMITIVE_CALL(~a0.v_integer)
			} else XEMMAI__CODE__OBJECT_OR_THROW(f_complement)
		XEMMAI__CODE__CASE_END
#endif
#if defined(XEMMAI__CODE__BINARY_LX) || defined(XEMMAI__CODE__BINARY_XL) || defined(XEMMAI__CODE__BINARY_XX) || defined(XEMMAI__CODE__BINARY_XI) || defined(XEMMAI__CODE__BINARY_IX) || defined(XEMMAI__CODE__BINARY_XF) || defined(XEMMAI__CODE__BINARY_FX) || defined(XEMMAI__CODE__BINARY_LI) || defined(XEMMAI__CODE__BINARY_IL) || defined(XEMMAI__CODE__BINARY_LF) || defined(XEMMAI__CODE__BINARY_FL)
#if defined(XEMMAI__CODE__BINARY_LX) || defined(XEMMAI__CODE__BINARY_LI) || defined(XEMMAI__CODE__BINARY_LF)
#define XEMMAI__CODE__BINARY_ARITHMETIC(a_name, a_operator, a_method) XEMMAI__CODE__OTHERS_OBJECT(a_name, a_method)
#endif
#if defined(XEMMAI__CODE__BINARY_LX) || defined(XEMMAI__CODE__BINARY_XF) || defined(XEMMAI__CODE__BINARY_LI) || defined(XEMMAI__CODE__BINARY_LF)
#define XEMMAI__CODE__BINARY_INTEGRAL(a_name, a_operator, a_method) XEMMAI__CODE__OTHERS_OBJECT(a_name, a_method)
#endif
#if defined(XEMMAI__CODE__BINARY_LI) || defined(XEMMAI__CODE__BINARY_LF)
#define XEMMAI__CODE__BINARY_EQUALITY(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__OBJECT_OR_FALSE(a_operator, a_method)\
		XEMMAI__CODE__CASE_END
#endif
#if defined(XEMMAI__CODE__BINARY_LI) || defined(XEMMAI__CODE__BINARY_IL) || defined(XEMMAI__CODE__BINARY_LF) || defined(XEMMAI__CODE__BINARY_FL)
#define XEMMAI__CODE__BINARY_IDENTITY_VALUE false
#else
#define XEMMAI__CODE__BINARY_IDENTITY_VALUE a0 == a1
#endif
#define XEMMAI__CODE__BINARY_IDENTITY(a_name, a_operator)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__PRIMITIVE_CALL(a_operator(XEMMAI__CODE__BINARY_IDENTITY_VALUE))\
		XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, a_type, a_cast, a_field)\
				if (a1.f_tag() >= c_tag__OBJECT) {\
					if (a1->f_type()->f_derives<a_type>()) {\
						XEMMAI__CODE__PRIMITIVE_CALL(a_cast(a0 a_field) a_operator a1->f_as<a_type>())\
					} else {\
						goto label__THROW_NOT_SUPPORTED;\
					}\
				} else {\
					goto label__THROW_NOT_SUPPORTED;\
				}
#define XEMMAI__CODE__BINARY_DERIVED_OR_FALSE(a_operator, a_type, a_field)\
				if (a1.f_tag() >= c_tag__OBJECT) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a1->f_type()->f_derives<a_type>() && a0 a_field == a1->f_as<a_type>()))\
				} else {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(false))\
				}
#if defined(XEMMAI__CODE__BINARY_LX) || defined(XEMMAI__CODE__BINARY_XL) || defined(XEMMAI__CODE__BINARY_XX)
#ifdef XEMMAI__CODE__BINARY_LX
#define XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)
#else
#ifdef XEMMAI__CODE__BINARY_XL
#define XEMMAI__CODE__BINARY_INTEGER(a_operator, a_cast)
#define XEMMAI__CODE__BINARY_ARITHMETIC_INTEGER(a_operator)
#define XEMMAI__CODE__BINARY_ARITHMETIC_FLOAT(a_operator)
#define XEMMAI__CODE__BINARY_EQUALITY_INTEGER(a_operator)
#define XEMMAI__CODE__BINARY_EQUALITY_FLOAT(a_operator)
#else
#define XEMMAI__CODE__BINARY_INTEGER(a_operator, a_cast)\
				if (a1.f_tag() == c_tag__INTEGER) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_cast(a0.v_integer) a_operator a1.v_integer)\
				} else
#define XEMMAI__CODE__BINARY_ARITHMETIC_INTEGER(a_operator)\
				XEMMAI__CODE__BINARY_INTEGER(a_operator, )\
				if (a1.f_tag() == c_tag__FLOAT) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer a_operator a1.v_float)\
				} else
#define XEMMAI__CODE__BINARY_ARITHMETIC_FLOAT(a_operator)\
				if (a1.f_tag() == c_tag__FLOAT) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float a_operator a1.v_float)\
				} else if (a1.f_tag() == c_tag__INTEGER) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float a_operator a1.v_integer)\
				} else
#define XEMMAI__CODE__BINARY_EQUALITY_INTEGER(a_operator)\
				if (a1.f_tag() == c_tag__INTEGER) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_integer == a1.v_integer))\
				} else if (a1.f_tag() == c_tag__FLOAT) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_integer == a1.v_float))\
				} else
#define XEMMAI__CODE__BINARY_EQUALITY_FLOAT(a_operator)\
				if (a1.f_tag() == c_tag__FLOAT) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_float == a1.v_float))\
				} else if (a1.f_tag() == c_tag__INTEGER) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_float == a1.v_integer))\
				} else
#endif
#define XEMMAI__CODE__BINARY_ARITHMETIC(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			switch (a0.f_tag()) {\
			case c_tag__NULL:\
				goto label__THROW_NOT_SUPPORTED;\
			case c_tag__INTEGER:\
				XEMMAI__CODE__BINARY_ARITHMETIC_INTEGER(a_operator)\
				XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, intptr_t, , .v_integer)\
				break;\
			case c_tag__FLOAT:\
				XEMMAI__CODE__BINARY_ARITHMETIC_FLOAT(a_operator)\
				XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, double, , .v_float)\
				break;\
			default:\
				XEMMAI__CODE__OBJECT_CALL(a_method)\
			}\
		XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__BINARY_INTEGRAL(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__BINARY_INTEGER(a_operator, static_cast<uintptr_t>)\
				XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, intptr_t, static_cast<uintptr_t>, .v_integer)\
			} else XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
		XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)\
			case c_tag__INTEGER:\
				XEMMAI__CODE__BINARY_EQUALITY_INTEGER(a_operator)\
				XEMMAI__CODE__BINARY_DERIVED_OR_FALSE(a_operator, intptr_t, .v_integer)\
				break;\
			case c_tag__FLOAT:\
				XEMMAI__CODE__BINARY_EQUALITY_FLOAT(a_operator)\
				XEMMAI__CODE__BINARY_DERIVED_OR_FALSE(a_operator, double, .v_float)\
				break;
#endif
#define XEMMAI__CODE__BINARY_EQUALITY(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			switch (a0.f_tag()) {\
			case c_tag__NULL:\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a1.f_tag() == c_tag__NULL))\
				break;\
			XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)\
			default:\
				if (a0.v_p == a1.v_p) {\
					XEMMAI__CODE__PRIMITIVE_CALL(a_operator(true))\
				} else {\
					XEMMAI__CODE__OBJECT_CALL(a_method)\
				}\
			}\
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__BINARY_ARITHMETIC(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer a_operator a1)\
			} else if (a0.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float a_operator a1)\
			} else XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
		XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__BINARY_INTEGRAL(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(static_cast<uintptr_t>(a0.v_integer) a_operator a1)\
			} else XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
		XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__BINARY_EQUALITY(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_integer == a1))\
			} else if (a0.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_float == a1))\
			} else XEMMAI__CODE__OBJECT_OR_FALSE(a_operator, a_method)\
		XEMMAI__CODE__CASE_END
#endif
#ifdef XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__BINARY_ARITHMETIC(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0.v_float a_operator a1)\
			} else if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer a_operator a1)\
			} else XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
		XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__BINARY_EQUALITY(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_float == a1))\
			} else if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0.v_integer == a1))\
			} else XEMMAI__CODE__OBJECT_OR_FALSE(a_operator, a_method)\
		XEMMAI__CODE__CASE_END
#endif
#if defined(XEMMAI__CODE__BINARY_IL) || defined(XEMMAI__CODE__BINARY_IX) || defined(XEMMAI__CODE__BINARY_FL) || defined(XEMMAI__CODE__BINARY_FX)
#if defined(XEMMAI__CODE__BINARY_IL) || defined(XEMMAI__CODE__BINARY_IX)
#define XEMMAI__CODE__BINARY_TYPE intptr_t
#else
#define XEMMAI__CODE__BINARY_TYPE double
#endif
#if defined(XEMMAI__CODE__BINARY_IX)
#define XEMMAI__CODE__BINARY_INTEGER(a_operator, a_cast)\
			if (a1.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_cast(a0) a_operator a1.v_integer)\
			} else
#define XEMMAI__CODE__BINARY_ARITHMETIC_PRIMITIVE(a_operator)\
			XEMMAI__CODE__BINARY_INTEGER(a_operator, )\
			if (a1.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0 a_operator a1.v_float)\
			} else
#elif defined(XEMMAI__CODE__BINARY_FX)
#define XEMMAI__CODE__BINARY_ARITHMETIC_PRIMITIVE(a_operator)\
			if (a1.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0 a_operator a1.v_float)\
			} else if (a1.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0 a_operator a1.v_integer)\
			} else
#else
#define XEMMAI__CODE__BINARY_INTEGER(a_operator, a_cast)
#define XEMMAI__CODE__BINARY_ARITHMETIC_PRIMITIVE(a_operator)
#endif
#define XEMMAI__CODE__BINARY_ARITHMETIC(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__BINARY_ARITHMETIC_PRIMITIVE(a_operator)\
			XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, XEMMAI__CODE__BINARY_TYPE, , )\
		XEMMAI__CODE__CASE_END
#if defined(XEMMAI__CODE__BINARY_IL) || defined(XEMMAI__CODE__BINARY_IX)
#define XEMMAI__CODE__BINARY_INTEGRAL(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__BINARY_INTEGER(a_operator, static_cast<uintptr_t>)\
			XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, intptr_t, static_cast<uintptr_t>, )\
		XEMMAI__CODE__CASE_END
#else
#define XEMMAI__CODE__BINARY_INTEGRAL(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_NAME(a_name)\
			goto label__THROW_NOT_SUPPORTED;\
		XEMMAI__CODE__CLOSE
#endif
#if defined(XEMMAI__CODE__BINARY_IX)
#define XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)\
			if (a1.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0 == a1.v_integer))\
			} else if (a1.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0 == a1.v_float))\
			} else
#elif defined(XEMMAI__CODE__BINARY_FX)
#define XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)\
			if (a1.f_tag() == c_tag__FLOAT) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0 == a1.v_float))\
			} else if (a1.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_operator(a0 == a1.v_integer))\
			} else
#else
#define XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)
#endif
#define XEMMAI__CODE__BINARY_EQUALITY(a_name, a_operator, a_method)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE(a_operator)\
			XEMMAI__CODE__BINARY_DERIVED_OR_FALSE(a_operator, XEMMAI__CODE__BINARY_TYPE, )\
		XEMMAI__CODE__CASE_END
#endif
#if defined(XEMMAI__CODE__BINARY_LX) || defined(XEMMAI__CODE__BINARY_XL) || defined(XEMMAI__CODE__BINARY_XX) || defined(XEMMAI__CODE__BINARY_FX)
#define XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER if (a1.f_tag() == c_tag__INTEGER) goto label__THROW_NOT_SUPPORTED;
#else
#define XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER
#endif
#if defined(XEMMAI__CODE__BINARY_XL) || defined(XEMMAI__CODE__BINARY_XX) || defined(XEMMAI__CODE__BINARY_XF)
#if defined(XEMMAI__CODE__BINARY_XF)
#define XEMMAI__CODE__BINARY_BITWISE_INTEGER(a_operator) goto label__THROW_NOT_SUPPORTED;
#else
#define XEMMAI__CODE__BINARY_BITWISE_INTEGER(a_operator)\
				XEMMAI__CODE__BINARY_INTEGER(a_operator, )\
				XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, intptr_t, , .v_integer)\
				break;
#endif
#define XEMMAI__CODE__BINARY_BITWISE(a_name, a_operator, a_method, a_null, a_true)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			switch (a0.f_tag()) {\
			case c_tag__NULL:\
				XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER\
				XEMMAI__CODE__PRIMITIVE_CALL(a_null)\
				break;\
			case c_tag__INTEGER:\
				XEMMAI__CODE__BINARY_BITWISE_INTEGER(a_operator)\
			case c_tag__FLOAT:\
				XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER\
				XEMMAI__CODE__PRIMITIVE_CALL(a_true)\
				break;\
			default:\
				XEMMAI__CODE__OBJECT_CALL(a_method)\
			}\
		XEMMAI__CODE__CASE_END
#elif defined(XEMMAI__CODE__BINARY_LF) || defined(XEMMAI__CODE__BINARY_LX)
#define XEMMAI__CODE__BINARY_BITWISE(a_name, a_operator, a_method, a_null, a_true)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER\
			if (a0.f_tag() == c_tag__NULL) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a_null)\
			} else {\
				XEMMAI__CODE__OBJECT_CALL(a_method)\
			}\
		XEMMAI__CODE__CASE_END
#elif defined(XEMMAI__CODE__BINARY_XI)
#define XEMMAI__CODE__BINARY_BITWISE(a_name, a_operator, a_method, a_null, a_true)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			if (a0.f_tag() == c_tag__INTEGER) {\
				XEMMAI__CODE__PRIMITIVE_CALL(a0.v_integer a_operator a1)\
			} else XEMMAI__CODE__OBJECT_OR_THROW(a_method)\
		XEMMAI__CODE__CASE_END
#elif defined(XEMMAI__CODE__BINARY_LI)
#define XEMMAI__CODE__BINARY_BITWISE(a_name, a_operator, a_method, a_null, a_true) XEMMAI__CODE__OTHERS_OBJECT(a_name, a_method)
#elif defined(XEMMAI__CODE__BINARY_IL) || defined(XEMMAI__CODE__BINARY_IX)
#define XEMMAI__CODE__BINARY_BITWISE(a_name, a_operator, a_method, a_null, a_true)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__BINARY_INTEGER(a_operator, )\
			XEMMAI__CODE__BINARY_DERIVED_OR_THROW(a_operator, intptr_t, , )\
		XEMMAI__CODE__CASE_END
#elif defined(XEMMAI__CODE__BINARY_FL) || defined(XEMMAI__CODE__BINARY_FX)
#define XEMMAI__CODE__BINARY_BITWISE(a_name, a_operator, a_method, a_null, a_true)\
		XEMMAI__CODE__CASE_BEGIN(a_name)\
			XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER\
			XEMMAI__CODE__PRIMITIVE_CALL(a_true)\
		XEMMAI__CODE__CASE_END
#endif
		XEMMAI__CODE__BINARY_ARITHMETIC(MULTIPLY, *, f_multiply)
		XEMMAI__CODE__BINARY_ARITHMETIC(DIVIDE, /, f_divide)
		XEMMAI__CODE__BINARY_INTEGRAL(MODULUS, %, f_modulus)
		XEMMAI__CODE__BINARY_ARITHMETIC(ADD, +, f_add)
		XEMMAI__CODE__BINARY_ARITHMETIC(SUBTRACT, -, f_subtract)
		XEMMAI__CODE__BINARY_INTEGRAL(LEFT_SHIFT, <<, f_left_shift)
		XEMMAI__CODE__BINARY_INTEGRAL(RIGHT_SHIFT, >>, f_right_shift)
		XEMMAI__CODE__BINARY_ARITHMETIC(LESS, <, f_less)
		XEMMAI__CODE__BINARY_ARITHMETIC(LESS_EQUAL, <=, f_less_equal)
		XEMMAI__CODE__BINARY_ARITHMETIC(GREATER, >, f_greater)
		XEMMAI__CODE__BINARY_ARITHMETIC(GREATER_EQUAL, >=, f_greater_equal)
		XEMMAI__CODE__BINARY_EQUALITY(EQUALS, , f_equals)
		XEMMAI__CODE__BINARY_EQUALITY(NOT_EQUALS, !, f_not_equals)
		XEMMAI__CODE__BINARY_IDENTITY(IDENTICAL, )
		XEMMAI__CODE__BINARY_IDENTITY(NOT_IDENTICAL, !)
		XEMMAI__CODE__BINARY_BITWISE(AND, &, f_and, false, a1)
		XEMMAI__CODE__BINARY_BITWISE(XOR, ^, f_xor, a1, a1 ? t_pvalue(false) : a0)
		XEMMAI__CODE__BINARY_BITWISE(OR, |, f_or, a1, a0)
#undef XEMMAI__CODE__BINARY_INTEGER
#undef XEMMAI__CODE__BINARY_ARITHMETIC_INTEGER
#undef XEMMAI__CODE__BINARY_ARITHMETIC_FLOAT
#undef XEMMAI__CODE__BINARY_ARITHMETIC_PRIMITIVE
#undef XEMMAI__CODE__BINARY_ARITHMETIC
#undef XEMMAI__CODE__BINARY_INTEGRAL
#undef XEMMAI__CODE__BINARY_EQUALITY_INTEGER
#undef XEMMAI__CODE__BINARY_EQUALITY_FLOAT
#undef XEMMAI__CODE__BINARY_EQUALITY_PRIMITIVE
#undef XEMMAI__CODE__BINARY_EQUALITY
#undef XEMMAI__CODE__BINARY_IDENTITY_VALUE
#undef XEMMAI__CODE__BINARY_BITWISE_NO_INTEGER
#undef XEMMAI__CODE__BINARY_BITWISE_INTEGER
#undef XEMMAI__CODE__BINARY_BITWISE
#undef XEMMAI__CODE__BINARY_TYPE
#endif
