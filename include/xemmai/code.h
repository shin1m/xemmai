#ifndef XEMMAI__CODE_H
#define XEMMAI__CODE_H

#include <vector>

#include "object.h"

namespace xemmai
{

enum t_instruction
{
	e_instruction__JUMP,
	e_instruction__BRANCH,
	e_instruction__TRY,
	e_instruction__CATCH,
	e_instruction__FINALLY,
	e_instruction__YRT,
	e_instruction__THROW,
	e_instruction__POP,
	e_instruction__OBJECT_GET,
	e_instruction__OBJECT_GET_INDIRECT,
	e_instruction__OBJECT_PUT,
	e_instruction__OBJECT_PUT_INDIRECT,
	e_instruction__OBJECT_HAS,
	e_instruction__OBJECT_HAS_INDIRECT,
	e_instruction__OBJECT_REMOVE,
	e_instruction__OBJECT_REMOVE_INDIRECT,
	e_instruction__GLOBAL_GET,
	e_instruction__SCOPE_GET,
	e_instruction__SCOPE_GET_WITHOUT_LOCK,
	e_instruction__SCOPE_GET0,
	e_instruction__SCOPE_GET0_WITHOUT_LOCK,
	e_instruction__SCOPE_PUT,
	e_instruction__SCOPE_PUT0,
	e_instruction__SCOPE_PUT0_WITHOUT_LOCK,
	e_instruction__LAMBDA,
	e_instruction__SELF,
	e_instruction__CLASS,
	e_instruction__SUPER,
	e_instruction__INSTANCE,
	e_instruction__IDENTICAL,
	e_instruction__NOT_IDENTICAL,
	e_instruction__RETURN,
	e_instruction__CALL,
	e_instruction__GET_AT,
	e_instruction__SET_AT,
	e_instruction__PLUS,
	e_instruction__MINUS,
	e_instruction__NOT,
	e_instruction__COMPLEMENT,
	e_instruction__MULTIPLY,
	e_instruction__DIVIDE,
	e_instruction__MODULUS,
	e_instruction__ADD,
	e_instruction__SUBTRACT,
	e_instruction__LEFT_SHIFT,
	e_instruction__RIGHT_SHIFT,
	e_instruction__LESS,
	e_instruction__LESS_EQUAL,
	e_instruction__GREATER,
	e_instruction__GREATER_EQUAL,
	e_instruction__EQUALS,
	e_instruction__NOT_EQUALS,
	e_instruction__AND,
	e_instruction__XOR,
	e_instruction__OR,
	e_instruction__SEND,
	e_instruction__CALL_TAIL,
	e_instruction__GET_AT_TAIL,
	e_instruction__SET_AT_TAIL,
	e_instruction__PLUS_TAIL,
	e_instruction__MINUS_TAIL,
	e_instruction__NOT_TAIL,
	e_instruction__COMPLEMENT_TAIL,
	e_instruction__MULTIPLY_TAIL,
	e_instruction__DIVIDE_TAIL,
	e_instruction__MODULUS_TAIL,
	e_instruction__ADD_TAIL,
	e_instruction__SUBTRACT_TAIL,
	e_instruction__LEFT_SHIFT_TAIL,
	e_instruction__RIGHT_SHIFT_TAIL,
	e_instruction__LESS_TAIL,
	e_instruction__LESS_EQUAL_TAIL,
	e_instruction__GREATER_TAIL,
	e_instruction__GREATER_EQUAL_TAIL,
	e_instruction__EQUALS_TAIL,
	e_instruction__NOT_EQUALS_TAIL,
	e_instruction__AND_TAIL,
	e_instruction__XOR_TAIL,
	e_instruction__OR_TAIL,
	e_instruction__SEND_TAIL,
	e_instruction__FIBER_EXIT,
	e_instruction__END,
	e_instruction__DEAD = 0x80
};

struct t_code
{
	enum t_access
	{
		e_access__SHARED = ~(~0 >> 1),
		e_access__VARIES = ~(~0 >> 1) >> 1,
		e_access__INDEX = ~0 >> 2
	};
	class t_at
	{
		size_t v_address;
		long v_position;
		size_t v_line;
		size_t v_column;

	public:
		t_at(size_t a_address, long a_position, size_t a_line, size_t a_column) :
		v_address(a_address), v_position(a_position), v_line(a_line), v_column(a_column)
		{
		}
		bool operator<(const t_at& a_other) const
		{
			return v_address < a_other.v_address;
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

	static t_transfer f_instantiate(const std::wstring& a_path, size_t a_arguments);
	static void f_generate(void** a_p);
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
	static t_transfer f_loop(const void*** a_labels = 0);
#else
	static t_transfer f_loop();
#endif

	std::wstring v_path;
	size_t v_size;
	size_t v_arguments;
	std::vector<void*> v_instructions;
	std::vector<void*> v_objects;
	std::vector<t_at> v_ats;

	t_code(const std::wstring& a_path, size_t a_arguments) : v_path(a_path), v_size(a_arguments), v_arguments(a_arguments)
	{
	}
	void f_scan(t_scan a_scan);
	const t_at* f_at(void** a_address) const;
	void f_at(size_t a_address, long a_position, size_t a_line, size_t a_column)
	{
		v_ats.push_back(t_at(a_address, a_position, a_line, a_column));
	}
	void f_resolve(size_t a_n, void** a_p)
	{
		*a_p = reinterpret_cast<void*>(&v_instructions[reinterpret_cast<size_t>(*a_p)]);
		f_estimate(a_n, reinterpret_cast<void**>(*a_p));
	}
	void f_estimate(size_t a_n, void** a_p);
	void f_estimate()
	{
		f_estimate(v_size, &v_instructions[0]);
	}
	bool f_tail(void** a_p);
	void f_tail();
	void f_generate()
	{
		f_generate(&v_instructions[0]);
	}
};

template<>
struct t_type_of<t_code> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

#endif
