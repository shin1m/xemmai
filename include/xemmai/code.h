#ifndef XEMMAI__CODE_H
#define XEMMAI__CODE_H

#include "object.h"
#include <set>
#include <vector>

namespace xemmai
{

class t_lambda;

enum t_instruction
{
	e_instruction__JUMP,
	e_instruction__BRANCH,
	e_instruction__TRY,
	e_instruction__CATCH,
	e_instruction__FINALLY,
	e_instruction__YRT,
	e_instruction__THROW,
	e_instruction__CLEAR,
	e_instruction__OBJECT_GET,
	e_instruction__OBJECT_GET_MONOMORPHIC_CLASS,
	e_instruction__OBJECT_GET_MONOMORPHIC_INSTANCE,
	e_instruction__OBJECT_GET_MEGAMORPHIC,
	e_instruction__OBJECT_GET_INDIRECT,
	e_instruction__OBJECT_PUT,
	e_instruction__OBJECT_PUT_MONOMORPHIC_ADD,
	e_instruction__OBJECT_PUT_MONOMORPHIC_SET,
	e_instruction__OBJECT_PUT_MEGAMORPHIC,
	e_instruction__OBJECT_PUT_CLEAR,
	e_instruction__OBJECT_PUT_CLEAR_MONOMORPHIC_ADD,
	e_instruction__OBJECT_PUT_CLEAR_MONOMORPHIC_SET,
	e_instruction__OBJECT_PUT_CLEAR_MEGAMORPHIC,
	e_instruction__OBJECT_PUT_INDIRECT,
	e_instruction__OBJECT_HAS,
	e_instruction__OBJECT_HAS_INDIRECT,
	e_instruction__OBJECT_REMOVE,
	e_instruction__OBJECT_REMOVE_INDIRECT,
	e_instruction__METHOD_GET,
	e_instruction__METHOD_GET_MONOMORPHIC_CLASS,
	e_instruction__METHOD_GET_MONOMORPHIC_INSTANCE,
	e_instruction__METHOD_GET_MEGAMORPHIC,
	e_instruction__METHOD_BIND,
	e_instruction__GLOBAL_GET,
	e_instruction__STACK_LET,
	e_instruction__STACK_LET_CLEAR,
	e_instruction__STACK_GET,
	e_instruction__STACK_PUT,
	e_instruction__STACK_PUT_CLEAR,
	e_instruction__SCOPE_GET0,
	e_instruction__SCOPE_GET1,
	e_instruction__SCOPE_GET2,
	e_instruction__SCOPE_GET,
	e_instruction__SCOPE_GET0_WITHOUT_LOCK,
	e_instruction__SCOPE_GET1_WITHOUT_LOCK,
	e_instruction__SCOPE_GET2_WITHOUT_LOCK,
	e_instruction__SCOPE_GET_WITHOUT_LOCK,
	e_instruction__SCOPE_PUT0,
	e_instruction__SCOPE_PUT,
	e_instruction__SCOPE_PUT0_CLEAR,
	e_instruction__SCOPE_PUT_CLEAR,
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
	e_instruction__RETURN_N,
	e_instruction__RETURN_B,
	e_instruction__RETURN_I,
	e_instruction__RETURN_F,
	e_instruction__RETURN_L,
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
	e_instruction__SEND,
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
	e_instruction__SEND_TAIL,
	e_instruction__END,
	e_instruction__SAFE_POINT,
	e_instruction__BREAK_POINT
};

class t_at
{
	long v_position;
	size_t v_line;
	size_t v_column;

public:
	t_at(long a_position, size_t a_line, size_t a_column) : v_position(a_position), v_line(a_line), v_column(a_column)
	{
	}
	long f_position() const
	{
		return v_position;
	}
	size_t f_line() const
	{
		return v_line;
	}
	size_t f_column() const
	{
		return v_column;
	}
};

struct t_code
{
	struct t_variable
	{
		bool v_shared = false;
		bool v_varies = false;
		size_t v_index;
	};
	class t_address_at : public t_at
	{
		size_t v_address;

	public:
		t_address_at(size_t a_address, const t_at& a_at) : t_at(a_at), v_address(a_address)
		{
		}
		bool operator<(const t_address_at& a_other) const
		{
			return v_address < a_other.v_address;
		}
	};
	class t_label : std::vector<size_t>
	{
		friend struct t_code;

		size_t v_target;
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
	struct t_stack_map
	{
		size_t v_address;
		const std::vector<bool>* v_pattern;

		bool operator<(const t_stack_map& a_other) const
		{
			return v_address < a_other.v_address;
		}
	};

	static void f_object_get(t_stacked* a_base, void**& a_pc, void* a_class, void* a_instance, void* a_megamorphic);
	static void f_object_put(t_stacked* a_base, void**& a_pc, void* a_add, void* a_set, void* a_megamorphic);
	static void f_object_put_clear(t_stacked* a_base, void**& a_pc, void* a_add, void* a_set, void* a_megamorphic);
	static void f_method_get(t_stacked* a_base, void**& a_pc, void* a_class, void* a_instance, void* a_megamorphic);

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
	static const void** v_labels;

	static size_t f_loop(t_context* a_context, const void*** a_labels = nullptr);
	static const void** f_labels()
	{
		const void** labels;
		f_loop(nullptr, &labels);
		return labels;
	}
#else
	static XEMMAI__PORTABLE__EXPORT size_t f_loop(t_context* a_context);
#endif
	static void f_try(t_context* a_context);
	static size_t f_loop(t_context& a_context);
	static t_scoped f_instantiate(t_object* a_module, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum);

	t_slot v_module;
	bool v_shared;
	bool v_variadic;
	size_t v_size;
	size_t v_privates;
	size_t v_shareds;
	size_t v_arguments;
	size_t v_minimum;
	std::vector<void*> v_instructions;
	std::vector<t_address_at> v_ats;
	std::set<std::vector<bool>> v_stack_patterns;
	std::vector<t_stack_map> v_stack_map;
	std::map<std::wstring, t_variable, std::less<>> v_variables;

	t_code(t_object* a_module, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum) : v_module(a_module), v_shared(a_shared), v_variadic(a_variadic), v_size(a_privates), v_privates(a_privates), v_shareds(a_shareds), v_arguments(a_arguments), v_minimum(a_minimum)
	{
	}
	const t_at* f_at(void** a_address) const;
	void f_at(size_t a_address, const t_at& a_at)
	{
		v_ats.push_back(t_address_at(a_address, a_at));
	}
	const std::vector<bool>& f_stack_map(void** a_address) const;
	void f_stack_map(int a_offset, const std::vector<bool>& a_pattern)
	{
		v_stack_map.push_back(t_stack_map{f_last() + a_offset, &*v_stack_patterns.insert(a_pattern).first});
	}
	void f_stack_clear(void** a_address, t_stacked* a_base) const;
	void f_stack_clear(void** a_address, t_stacked* a_base, t_stacked* a_stack) const;
	size_t f_last() const
	{
		return v_instructions.size();
	}
	void* f_p(t_instruction a_instruction) const
	{
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
		return const_cast<void*>(v_labels[a_instruction]);
#else
		return reinterpret_cast<void*>(a_instruction);
#endif
	}
	void f_emit(t_instruction a_instruction)
	{
		v_instructions.push_back(f_p(a_instruction));
	}
	template<typename T>
	void f_operand(T a_operand)
	{
		v_instructions.push_back(reinterpret_cast<void*>(a_operand));
	}
	void f_operand(bool a_operand)
	{
		v_instructions.push_back(reinterpret_cast<void*>(a_operand ? 1 : 0));
	}
	void f_operand(double a_operand)
	{
		union
		{
			double v0;
			void* v1[sizeof(double) / sizeof(void*)];
		};
		v0 = a_operand;
		for (size_t i = 0; i < sizeof(double) / sizeof(void*); ++i) v_instructions.push_back(v1[i]);
	}
	void f_operand(const t_value& a_operand)
	{
		v_instructions.push_back(const_cast<t_value*>(&a_operand));
	}
	void f_operand(t_slot& a_operand)
	{
		v_instructions.push_back(&a_operand);
	}
	void f_operand(t_label& a_label)
	{
		a_label.push_back(f_last());
		f_operand(size_t(0));
	}
	void f_target(t_label& a_label)
	{
		a_label.v_target = f_last();
	}
	void f_resolve(const t_label& a_label)
	{
		void* p = &v_instructions[a_label.v_target];
		for (auto i : a_label) v_instructions[i] = p;
	}
	void f_at(const t_at& a_at)
	{
		f_at(f_last(), a_at);
	}
};

template<>
struct t_type_of<t_code> : t_uninstantiatable<t_underivable<t_finalizes<t_derives<t_code>>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_code>().v_module);
	}
};

}

#endif
