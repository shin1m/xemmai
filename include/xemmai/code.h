#ifndef XEMMAI__CODE_H
#define XEMMAI__CODE_H

#include <vector>

#include "scope.h"

namespace xemmai
{

template<typename U>
class t_pointers;

template<typename T>
class t_pointer
{
	template<typename U>
	friend class t_pointer;
	template<typename U>
	friend class t_pointers;

	mutable T* v_p;

	T* f_release() const
	{
		T* p = v_p;
		v_p = 0;
		return p;
	}

public:
	t_pointer(T* a_p = 0) : v_p(a_p)
	{
	}
	t_pointer(const t_pointer& a_p) : v_p(a_p.f_release())
	{
	}
	template<typename U>
	t_pointer(const t_pointer<U>& a_p) : v_p(a_p.f_release())
	{
	}
	~t_pointer()
	{
		delete v_p;
	}
	t_pointer& operator=(const t_pointer& a_p)
	{
		delete v_p;
		v_p = a_p.f_release();
		return *this;
	}
	template<typename U>
	t_pointer& operator=(const t_pointer<U>& a_p)
	{
		delete v_p;
		v_p = a_p.f_release();
		return *this;
	}
	operator T*() const
	{
		return v_p;
	}
	T* operator->() const
	{
		return v_p;
	}
};

template<typename T>
class t_pointers
{
	std::vector<T*> v_ps;

public:
	typedef typename std::vector<T*>::const_iterator t_iterator;

	~t_pointers()
	{
		for (t_iterator i = f_begin(); i != f_end(); ++i) delete *i;
	}
	size_t f_size() const
	{
		return v_ps.size();
	}
	t_iterator f_begin() const
	{
		return v_ps.begin();
	}
	t_iterator f_end() const
	{
		return v_ps.end();
	}
	T* operator[](size_t a_i) const
	{
		return v_ps[a_i];
	}
	void f_add(T* a_p)
	{
		v_ps.push_back(a_p);
	}
	template<typename U>
	void f_add(const t_pointer<U>& a_p)
	{
		v_ps.push_back(a_p.f_release());
	}
};

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
	e_instruction__OBJECT_GET_INDIRECT,
	e_instruction__OBJECT_PUT,
	e_instruction__OBJECT_PUT_INDIRECT,
	e_instruction__OBJECT_HAS,
	e_instruction__OBJECT_HAS_INDIRECT,
	e_instruction__OBJECT_REMOVE,
	e_instruction__OBJECT_REMOVE_INDIRECT,
	e_instruction__GLOBAL_GET,
	e_instruction__STACK_GET,
	e_instruction__STACK_PUT,
	e_instruction__SCOPE_GET0,
	e_instruction__SCOPE_GET1,
	e_instruction__SCOPE_GET2,
	e_instruction__SCOPE_GET,
	e_instruction__SCOPE_GET0_WITHOUT_LOCK,
	e_instruction__SCOPE_GET1_WITHOUT_LOCK,
	e_instruction__SCOPE_GET2_WITHOUT_LOCK,
	e_instruction__SCOPE_GET_WITHOUT_LOCK,
	e_instruction__SCOPE_PUT,
	e_instruction__LAMBDA,
	e_instruction__SELF,
	e_instruction__CLASS,
	e_instruction__SUPER,
	e_instruction__BOOLEAN,
	e_instruction__INTEGER,
	e_instruction__FLOAT,
	e_instruction__INSTANCE,
	e_instruction__RETURN,
	e_instruction__CALL,
	e_instruction__GET_AT,
	e_instruction__SET_AT,
#define XEMMAI__CODE__INSTRUCTION_UNARY(a_name)\
	e_instruction__##a_name##_V,\
	e_instruction__##a_name##_L,\
	e_instruction__##a_name##_T,
	XEMMAI__CODE__INSTRUCTION_UNARY(PLUS)
	XEMMAI__CODE__INSTRUCTION_UNARY(MINUS)
	XEMMAI__CODE__INSTRUCTION_UNARY(NOT)
	XEMMAI__CODE__INSTRUCTION_UNARY(COMPLEMENT)
#define XEMMAI__CODE__INSTRUCTION_BINARY(a_name)\
	e_instruction__##a_name##_VV,\
	e_instruction__##a_name##_LV,\
	e_instruction__##a_name##_TV,\
	e_instruction__##a_name##_VL,\
	e_instruction__##a_name##_LL,\
	e_instruction__##a_name##_TL,\
	e_instruction__##a_name##_VT,\
	e_instruction__##a_name##_LT,\
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
	e_instruction__FIBER_EXIT,
	e_instruction__END
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
		friend class t_code;

		size_t v_target;
	};

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
	static const void** v_labels;

	static void f_loop(const void*** a_labels = 0);
	static const void** f_labels()
	{
		const void** labels;
		f_loop(&labels);
		return labels;
	}
#else
	static void f_loop();
#endif
	static t_transfer f_instantiate(const std::wstring& a_path, bool a_shared, size_t a_privates, size_t a_shareds, size_t a_arguments);

	std::wstring v_path;
	bool v_shared;
	size_t v_size;
	size_t v_privates;
	size_t v_shareds;
	size_t v_arguments;
	std::vector<void*> v_instructions;
	t_pointers<t_slot> v_objects;
	std::vector<t_address_at> v_ats;

	t_code(const std::wstring& a_path, bool a_shared, size_t a_privates, size_t a_shareds, size_t a_arguments) : v_path(a_path), v_shared(a_shared), v_size(a_privates), v_privates(a_privates), v_shareds(a_shareds), v_arguments(a_arguments)
	{
	}
	void f_scan(t_scan a_scan);
	const t_at* f_at(void** a_address) const;
	void f_at(size_t a_address, const t_at& a_at)
	{
		v_ats.push_back(t_address_at(a_address, a_at));
	}
	size_t f_last() const
	{
		return v_instructions.size();
	}
	void f_emit(t_instruction a_instruction)
	{
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
		v_instructions.push_back(const_cast<void*>(v_labels[a_instruction]));
#else
		v_instructions.push_back(reinterpret_cast<void*>(a_instruction));
#endif
	}
	void f_operand(size_t a_operand)
	{
		v_instructions.push_back(reinterpret_cast<void*>(a_operand));
	}
	void f_operand(bool a_operand)
	{
		v_instructions.push_back(reinterpret_cast<void*>(static_cast<int>(a_operand)));
	}
	void f_operand(int a_operand)
	{
		v_instructions.push_back(reinterpret_cast<void*>(a_operand));
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
	void f_operand(t_object* a_operand)
	{
		v_instructions.push_back(a_operand);
		v_objects.f_add(new t_slot(a_operand));
	}
	void f_operand(const t_transfer& a_operand)
	{
		t_slot* p = new t_slot(a_operand);
		v_instructions.push_back(p);
		v_objects.f_add(p);
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
		for (t_label::const_iterator i = a_label.begin(); i != a_label.end(); ++i) v_instructions[*i] = p;
	}
	void f_at(const t_at& a_at)
	{
		f_at(f_last(), a_at);
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
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
