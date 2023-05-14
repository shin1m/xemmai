#ifndef XEMMAI__CODE_H
#define XEMMAI__CODE_H

#include "object.h"

namespace xemmai
{

struct t_context;

enum t_instruction
{
	e_instruction__JUMP,
	e_instruction__BRANCH,
	e_instruction__TRY,
	e_instruction__CATCH,
	e_instruction__FINALLY,
	e_instruction__YRT,
	e_instruction__THROW,
	e_instruction__OBJECT_GET,
	e_instruction__OBJECT_GET_INDIRECT,
	e_instruction__OBJECT_PUT,
	e_instruction__OBJECT_PUT_INDIRECT,
	e_instruction__OBJECT_HAS,
	e_instruction__OBJECT_HAS_INDIRECT,
	e_instruction__METHOD_GET,
	e_instruction__METHOD_BIND,
	e_instruction__GLOBAL_GET,
	e_instruction__STACK_GET,
	e_instruction__STACK_PUT,
	e_instruction__SCOPE_GET0,
	e_instruction__SCOPE_GET1,
	e_instruction__SCOPE_GET2,
	e_instruction__SCOPE_GET,
	e_instruction__SCOPE_PUT0,
	e_instruction__SCOPE_PUT,
	e_instruction__LAMBDA,
	e_instruction__ADVANCED_LAMBDA,
	e_instruction__SELF,
	e_instruction__CLASS,
	e_instruction__SUPER,
	e_instruction__NUL,
	e_instruction__BOOLEAN,
	e_instruction__INTEGER,
	e_instruction__FLOAT,
	e_instruction__INSTANCE,
	e_instruction__RETURN_NUL,
	e_instruction__RETURN_BOOLEAN,
	e_instruction__RETURN_INTEGER,
	e_instruction__RETURN_FLOAT,
	e_instruction__RETURN_INSTANCE,
	e_instruction__RETURN_V,
	e_instruction__RETURN_T,
	e_instruction__CALL,
	e_instruction__CALL_WITH_EXPANSION,
	e_instruction__STACK_CALL,
	e_instruction__SCOPE_CALL0,
	e_instruction__SCOPE_CALL1,
	e_instruction__SCOPE_CALL2,
	e_instruction__GET_AT,
	e_instruction__SET_AT,
#define XEMMAI__CODE__INSTRUCTION_UNARY(a_name)\
	e_instruction__##a_name##_L,\
	e_instruction__##a_name##_V,\
	e_instruction__##a_name##_T,
	XEMMAI__CODE__INSTRUCTION_UNARY(PLUS)
	XEMMAI__CODE__INSTRUCTION_UNARY(MINUS)
	XEMMAI__CODE__INSTRUCTION_UNARY(NOT)
	XEMMAI__CODE__INSTRUCTION_UNARY(COMPLEMENT)
#define XEMMAI__CODE__INSTRUCTION_BINARY(a_name)\
	e_instruction__##a_name##_II,\
	e_instruction__##a_name##_FI,\
	e_instruction__##a_name##_LI,\
	e_instruction__##a_name##_VI,\
	e_instruction__##a_name##_TI,\
	e_instruction__##a_name##_IF,\
	e_instruction__##a_name##_FF,\
	e_instruction__##a_name##_LF,\
	e_instruction__##a_name##_VF,\
	e_instruction__##a_name##_TF,\
	e_instruction__##a_name##_IL,\
	e_instruction__##a_name##_FL,\
	e_instruction__##a_name##_LL,\
	e_instruction__##a_name##_VL,\
	e_instruction__##a_name##_TL,\
	e_instruction__##a_name##_IV,\
	e_instruction__##a_name##_FV,\
	e_instruction__##a_name##_LV,\
	e_instruction__##a_name##_VV,\
	e_instruction__##a_name##_TV,\
	e_instruction__##a_name##_IT,\
	e_instruction__##a_name##_FT,\
	e_instruction__##a_name##_LT,\
	e_instruction__##a_name##_VT,\
	e_instruction__##a_name##_TT,
	XEMMAI__CODE__INSTRUCTION_BINARY(MULTIPLY)
	XEMMAI__CODE__INSTRUCTION_BINARY(DIVIDE)
	XEMMAI__CODE__INSTRUCTION_BINARY(MODULUS)
	XEMMAI__CODE__INSTRUCTION_BINARY(ADD)
	XEMMAI__CODE__INSTRUCTION_BINARY(SUBTRACT)
	XEMMAI__CODE__INSTRUCTION_BINARY(LEFT_SHIFT)
	XEMMAI__CODE__INSTRUCTION_BINARY(RIGHT_SHIFT)
	XEMMAI__CODE__INSTRUCTION_BINARY(LESS)
	XEMMAI__CODE__INSTRUCTION_BINARY(LESS_EQUAL)
	XEMMAI__CODE__INSTRUCTION_BINARY(GREATER)
	XEMMAI__CODE__INSTRUCTION_BINARY(GREATER_EQUAL)
	XEMMAI__CODE__INSTRUCTION_BINARY(EQUALS)
	XEMMAI__CODE__INSTRUCTION_BINARY(NOT_EQUALS)
	XEMMAI__CODE__INSTRUCTION_BINARY(IDENTICAL)
	XEMMAI__CODE__INSTRUCTION_BINARY(NOT_IDENTICAL)
	XEMMAI__CODE__INSTRUCTION_BINARY(AND)
	XEMMAI__CODE__INSTRUCTION_BINARY(XOR)
	XEMMAI__CODE__INSTRUCTION_BINARY(OR)
	e_instruction__CALL_TAIL,
	e_instruction__CALL_WITH_EXPANSION_TAIL,
	e_instruction__STACK_CALL_TAIL,
	e_instruction__SCOPE_CALL0_TAIL,
	e_instruction__SCOPE_CALL1_TAIL,
	e_instruction__SCOPE_CALL2_TAIL,
	e_instruction__GET_AT_TAIL,
	e_instruction__SET_AT_TAIL,
	XEMMAI__CODE__INSTRUCTION_UNARY(PLUS_TAIL)
	XEMMAI__CODE__INSTRUCTION_UNARY(MINUS_TAIL)
	XEMMAI__CODE__INSTRUCTION_UNARY(NOT_TAIL)
	XEMMAI__CODE__INSTRUCTION_UNARY(COMPLEMENT_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(MULTIPLY_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(DIVIDE_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(MODULUS_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(ADD_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(SUBTRACT_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(LEFT_SHIFT_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(RIGHT_SHIFT_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(LESS_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(LESS_EQUAL_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(GREATER_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(GREATER_EQUAL_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(EQUALS_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(NOT_EQUALS_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(IDENTICAL_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(NOT_IDENTICAL_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(AND_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(XOR_TAIL)
	XEMMAI__CODE__INSTRUCTION_BINARY(OR_TAIL)
	e_instruction__SAFE_POINT,
	e_instruction__BREAK_POINT
};

struct t_at
{
	long v_position;
	size_t v_line;
	size_t v_column;
};

struct t_code
{
	struct t_variable
	{
		bool v_shared = false;
		bool v_varies = false;
		size_t v_index;
	};
	struct t_at_address : t_at
	{
		size_t v_address;

		bool operator<(size_t a_address) const
		{
			return v_address < a_address;
		}
	};
	enum t_try
	{
		e_try__CATCH,
		e_try__STEP,
		e_try__BREAK,
		e_try__CONTINUE,
		e_try__RETURN,
		e_try__THROW
	};

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
	static size_t f_loop(t_context* a_context, const void*** a_labels = nullptr);
	static const void** f_labels()
	{
		const void** labels;
		f_loop(nullptr, &labels);
		return labels;
	}

	static inline const void** v_labels = f_labels();
#else
	static XEMMAI__PORTABLE__EXPORT size_t f_loop(t_context* a_context);
#endif
	static void f_rethrow [[noreturn]] ();
	static void f_try(t_context* a_context);
	static size_t f_loop(t_context& a_context);
	static t_object* f_instantiate(t_object* a_module, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum);

	t_slot v_module;
	bool v_shared;
	bool v_variadic;
	size_t v_size;
	size_t v_privates;
	size_t v_shareds;
	size_t v_arguments;
	size_t v_minimum;
	std::vector<void*> v_instructions;
	std::vector<t_at_address> v_ats;
	std::map<std::wstring, t_variable, std::less<>> v_variables;

	t_code(t_object* a_module, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum) : v_module(a_module), v_shared(a_shared), v_variadic(a_variadic), v_size(a_privates), v_privates(a_privates), v_shareds(a_shareds), v_arguments(a_arguments), v_minimum(a_minimum)
	{
	}
	const t_at* f_at(void** a_address) const;
	void* f_p(t_instruction a_instruction) const
	{
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
		return const_cast<void*>(v_labels[a_instruction]);
#else
		return reinterpret_cast<void*>(a_instruction);
#endif
	}
};

template<>
struct t_type_of<t_code> : t_uninstantiatable<t_finalizes<t_derives<t_code>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_code>().v_module);
	}
};

}

#endif
