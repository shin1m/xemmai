#ifndef XEMMAI__CODE_H
#define XEMMAI__CODE_H

#include "object.h"

namespace xemmai
{

struct t_context;

enum t_instruction
{
	c_instruction__JUMP,
	c_instruction__BRANCH,
	c_instruction__TRY,
	c_instruction__CATCH,
	c_instruction__FINALLY,
	c_instruction__YRT,
	c_instruction__THROW,
	c_instruction__OBJECT_GET,
	c_instruction__OBJECT_GET_INDIRECT,
	c_instruction__OBJECT_PUT,
	c_instruction__OBJECT_PUT_INDIRECT,
	c_instruction__OBJECT_HAS,
	c_instruction__OBJECT_HAS_INDIRECT,
	c_instruction__METHOD_GET,
	c_instruction__METHOD_BIND,
	c_instruction__GLOBAL_GET,
	c_instruction__STACK_GET,
	c_instruction__STACK_PUT,
	c_instruction__SCOPE_GET0,
	c_instruction__SCOPE_GET1,
	c_instruction__SCOPE_GET2,
	c_instruction__SCOPE_GET,
	c_instruction__SCOPE_PUT0,
	c_instruction__SCOPE_PUT,
	c_instruction__LAMBDA,
	c_instruction__ADVANCED_LAMBDA,
	c_instruction__SELF,
	c_instruction__CLASS,
	c_instruction__SUPER,
	c_instruction__NUL,
	c_instruction__INTEGER,
	c_instruction__FLOAT,
	c_instruction__INSTANCE,
	c_instruction__RETURN_NUL,
	c_instruction__RETURN_INTEGER,
	c_instruction__RETURN_FLOAT,
	c_instruction__RETURN_INSTANCE,
	c_instruction__RETURN_V,
	c_instruction__RETURN_T,
#define XEMMAI__CODE__INSTRUCTION_UNARY(a_name)\
	c_instruction__##a_name##_L,\
	c_instruction__##a_name##_V,\
	c_instruction__##a_name##_T,
#define XEMMAI__CODE__INSTRUCTION_BINARY(a_name)\
	c_instruction__##a_name##_II,\
	c_instruction__##a_name##_FI,\
	c_instruction__##a_name##_LI,\
	c_instruction__##a_name##_VI,\
	c_instruction__##a_name##_TI,\
	c_instruction__##a_name##_IF,\
	c_instruction__##a_name##_FF,\
	c_instruction__##a_name##_LF,\
	c_instruction__##a_name##_VF,\
	c_instruction__##a_name##_TF,\
	c_instruction__##a_name##_IL,\
	c_instruction__##a_name##_FL,\
	c_instruction__##a_name##_LL,\
	c_instruction__##a_name##_VL,\
	c_instruction__##a_name##_TL,\
	c_instruction__##a_name##_IV,\
	c_instruction__##a_name##_FV,\
	c_instruction__##a_name##_LV,\
	c_instruction__##a_name##_VV,\
	c_instruction__##a_name##_TV,\
	c_instruction__##a_name##_IT,\
	c_instruction__##a_name##_FT,\
	c_instruction__##a_name##_LT,\
	c_instruction__##a_name##_VT,\
	c_instruction__##a_name##_TT,
#define XEMMAI__CODE__INSTRUCTIONS(a_tail)\
	c_instruction__CALL##a_tail,\
	c_instruction__CALL_WITH_EXPANSION##a_tail,\
	c_instruction__STACK_CALL##a_tail,\
	c_instruction__SCOPE_CALL0##a_tail,\
	c_instruction__SCOPE_CALL1##a_tail,\
	c_instruction__SCOPE_CALL2##a_tail,\
	c_instruction__GET_AT##a_tail,\
	c_instruction__SET_AT##a_tail,\
	XEMMAI__CODE__INSTRUCTION_UNARY(PLUS##a_tail)\
	XEMMAI__CODE__INSTRUCTION_UNARY(MINUS##a_tail)\
	XEMMAI__CODE__INSTRUCTION_UNARY(NOT##a_tail)\
	XEMMAI__CODE__INSTRUCTION_UNARY(COMPLEMENT##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(MULTIPLY##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(DIVIDE##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(MODULUS##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(ADD##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(SUBTRACT##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(LEFT_SHIFT##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(RIGHT_SHIFT##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(LESS##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(LESS_EQUAL##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(GREATER##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(GREATER_EQUAL##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(EQUALS##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(NOT_EQUALS##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(IDENTICAL##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(NOT_IDENTICAL##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(AND##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(XOR##a_tail)\
	XEMMAI__CODE__INSTRUCTION_BINARY(OR##a_tail)
	XEMMAI__CODE__INSTRUCTIONS()
	XEMMAI__CODE__INSTRUCTIONS(_TAIL)
	c_instruction__SAFE_POINT,
	c_instruction__BREAK_POINT
};

struct t_at
{
	long v_position;
	size_t v_line;
	size_t v_column;
};

struct XEMMAI__LOCAL t_code
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
		c_try__CATCH,
		c_try__STEP,
		c_try__BREAK,
		c_try__CONTINUE,
		c_try__RETURN,
		c_try__THROW
	};

	template<t_instruction> static size_t f_do(t_context*, void**, t_pvalue*);
	static size_t(*v_dos[])(t_context*, void**, t_pvalue*);
	static void f_rethrow [[noreturn]] (void** a_pc);
	static void f_try(t_context* a_context);
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
		return reinterpret_cast<void*>(v_dos[a_instruction]);
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
