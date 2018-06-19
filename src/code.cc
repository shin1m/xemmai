#include <xemmai/code.h>

#include <xemmai/structure.h>
#include <xemmai/array.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

inline void f_put_or_clear(t_stacked& a_top, t_object* a_key, t_stacked& a_value)
{
	try {
		a_top.f_put(a_key, std::move(a_value));
	} catch (...) {
		a_value.f_construct();
		throw;
	}
}

void f_method_bind(t_stacked* a_stack)
{
	t_scoped x = std::move(a_stack[0]);
	a_stack[2].f_construct(std::move(a_stack[1]));
	auto p = static_cast<t_object*>(x);
	size_t n = p->f_type()->f_get_at(p, a_stack);
	if (n != size_t(-1)) t_value::f_loop(a_stack, n);
	a_stack[1].f_construct();
}

inline void f_allocate(t_stacked* a_stack, size_t a_n)
{
	t_stack* stack = f_stack();
	t_stacked* used = a_stack + a_n;
	if (used > stack->v_used) stack->v_used = used;
}

size_t f_expand(void**& a_pc, t_stacked* a_stack, size_t a_n)
{
	assert(a_n > 0);
	a_stack += a_n + 1;
	t_scoped x = std::move(a_stack[0]);
	size_t n;
	if (f_is<t_tuple>(x)) {
		auto& tuple = f_as<const t_tuple&>(x);
		n = tuple.f_size();
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) a_stack[i].f_construct(tuple[i]);
	} else if (f_is<t_array>(x)) {
		t_with_lock_for_read lock(x);
		auto& array = f_as<const t_array&>(x);
		n = array.f_size();
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) a_stack[i].f_construct(array[i]);
	} else {
		size_t i = 0;
		try {
			t_scoped size = x.f_invoke(f_global()->f_symbol_size());
			f_check<size_t>(size, L"size");
			n = f_as<size_t>(size);
			f_allocate(a_stack, n);
			for (; i < n; ++i) a_stack[i].f_construct(x.f_get_at(t_value(i)));
		} catch (...) {
			a_stack -= a_n + 1;
			i += a_n + 1;
			while (i > 0) a_stack[--i].f_destruct();
			throw;
		}
	}
	return a_n - 1 + n;
}

template<size_t (t_type::*A_function)(t_object*, t_stacked*)>
void f_operator(t_object* a_this, t_stacked* a_stack)
{
	size_t n = (a_this->f_type()->*A_function)(a_this, a_stack);
	if (n != size_t(-1)) t_value::f_loop(a_stack, n);
}

}

void t_code::f_object_get(t_stacked* a_base, void**& a_pc, void* a_class, void* a_instance, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 6;
	t_stacked* stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	t_stacked& top = stack[0];
	size_t& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto p = static_cast<t_object*>(top);
		if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
			*static_cast<t_slot*>(pc0[4]) = p->v_structure->v_this;
			pc0[4] = p->v_structure;
			intptr_t index = p->f_field_index(key);
			if (index < 0) {
				f_engine()->f_synchronize();
				pc0[0] = a_class;
				t_scoped value = static_cast<t_object*>(p->f_type()->v_this)->f_get(key);
				top = value.f_type() == f_global()->f_type<t_method>() ? f_as<t_method&>(value).f_bind(p) : std::move(value);
			} else {
				*reinterpret_cast<size_t*>(pc0 + 5) = index;
				f_engine()->f_synchronize();
				pc0[0] = a_instance;
				top = p->f_field_get(index);
			}
		} else {
			pc0[0] = a_megamorphic;
			top = top.f_get(key);
		}
	} else {
		top = top.f_get(key);
	}
}

void t_code::f_object_put(t_stacked* a_base, void**& a_pc, void* a_add, void* a_set, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 6;
	t_stacked* stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	t_stacked& top = stack[0];
	t_stacked& value = stack[1];
	size_t& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto p = static_cast<t_object*>(top);
		if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
			intptr_t index = p->f_field_index(key);
			if (index < 0) {
				t_scoped structure = p->v_structure->f_append(key);
				*static_cast<t_slot*>(pc0[4]) = static_cast<t_object*>(structure);
				pc0[4] = &f_as<t_structure&>(structure);
				f_engine()->f_synchronize();
				pc0[0] = a_add;
				p->f_field_add(std::move(structure), t_scoped(value));
			} else {
				*static_cast<t_slot*>(pc0[4]) = p->v_structure->v_this;
				pc0[4] = p->v_structure;
				*reinterpret_cast<size_t*>(pc0 + 5) = index;
				f_engine()->f_synchronize();
				pc0[0] = a_set;
				p->f_field_get(index) = value;
			}
		} else {
			pc0[0] = a_megamorphic;
			top.f_put(key, t_scoped(value));
		}
	} else {
		top.f_put(key, t_scoped(value));
	}
	top = std::move(value);
}

void t_code::f_object_put_clear(t_stacked* a_base, void**& a_pc, void* a_add, void* a_set, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 6;
	t_stacked* stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	t_stacked& top = stack[0];
	t_stacked& value = stack[1];
	size_t& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto p = static_cast<t_object*>(top);
		if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
			intptr_t index = p->f_field_index(key);
			if (index < 0) {
				t_scoped structure = p->v_structure->f_append(key);
				*static_cast<t_slot*>(pc0[4]) = static_cast<t_object*>(structure);
				pc0[4] = &f_as<t_structure&>(structure);
				f_engine()->f_synchronize();
				pc0[0] = a_add;
				p->f_field_add(std::move(structure), std::move(value));
			} else {
				*static_cast<t_slot*>(pc0[4]) = p->v_structure->v_this;
				pc0[4] = p->v_structure;
				*reinterpret_cast<size_t*>(pc0 + 5) = index;
				f_engine()->f_synchronize();
				pc0[0] = a_set;
				p->f_field_get(index) = std::move(value);
			}
		} else {
			pc0[0] = a_megamorphic;
			f_put_or_clear(top, key, value);
		}
	} else {
		f_put_or_clear(top, key, value);
	}
	top.f_destruct();
}

void t_code::f_method_get(t_stacked* a_base, void**& a_pc, void* a_class, void* a_instance, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 6;
	t_stacked* stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	t_scoped top = std::move(stack[0]);
	size_t& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto p = static_cast<t_object*>(top);
		if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
			*static_cast<t_slot*>(pc0[4]) = p->v_structure->v_this;
			pc0[4] = p->v_structure;
			intptr_t index = p->f_field_index(key);
			if (index < 0) {
				f_engine()->f_synchronize();
				pc0[0] = a_class;
				f_get_of_type(*p, key, stack);
			} else {
				*reinterpret_cast<size_t*>(pc0 + 5) = index;
				f_engine()->f_synchronize();
				pc0[0] = a_instance;
				stack[0].f_construct(p->f_field_get(index));
				stack[1].f_construct();
			}
		} else {
			pc0[0] = a_megamorphic;
			top.f_get(key, stack);
		}
	} else {
		top.f_get(key, stack);
	}
}

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
const void** t_code::v_labels = t_code::f_labels();

size_t t_code::f_loop(t_context* a_context, const void*** a_labels)
{
	if (a_labels) {
		static const void* labels[] = {
			&&label__JUMP,
			&&label__BRANCH,
			&&label__TRY,
			&&label__CATCH,
			&&label__FINALLY,
			&&label__YRT,
			&&label__THROW,
			&&label__CLEAR,
			&&label__OBJECT_GET,
			&&label__OBJECT_GET_MONOMORPHIC_CLASS,
			&&label__OBJECT_GET_MONOMORPHIC_INSTANCE,
			&&label__OBJECT_GET_MEGAMORPHIC,
			&&label__OBJECT_GET_INDIRECT,
			&&label__OBJECT_PUT,
			&&label__OBJECT_PUT_MONOMORPHIC_ADD,
			&&label__OBJECT_PUT_MONOMORPHIC_SET,
			&&label__OBJECT_PUT_MEGAMORPHIC,
			&&label__OBJECT_PUT_CLEAR,
			&&label__OBJECT_PUT_CLEAR_MONOMORPHIC_ADD,
			&&label__OBJECT_PUT_CLEAR_MONOMORPHIC_SET,
			&&label__OBJECT_PUT_CLEAR_MEGAMORPHIC,
			&&label__OBJECT_PUT_INDIRECT,
			&&label__OBJECT_HAS,
			&&label__OBJECT_HAS_INDIRECT,
			&&label__OBJECT_REMOVE,
			&&label__OBJECT_REMOVE_INDIRECT,
			&&label__METHOD_GET,
			&&label__METHOD_GET_MONOMORPHIC_CLASS,
			&&label__METHOD_GET_MONOMORPHIC_INSTANCE,
			&&label__METHOD_GET_MEGAMORPHIC,
			&&label__METHOD_BIND,
			&&label__GLOBAL_GET,
			&&label__STACK_LET,
			&&label__STACK_LET_CLEAR,
			&&label__STACK_GET,
			&&label__STACK_PUT,
			&&label__STACK_PUT_CLEAR,
			&&label__SCOPE_GET0,
			&&label__SCOPE_GET1,
			&&label__SCOPE_GET2,
			&&label__SCOPE_GET,
			&&label__SCOPE_GET0_WITHOUT_LOCK,
			&&label__SCOPE_GET1_WITHOUT_LOCK,
			&&label__SCOPE_GET2_WITHOUT_LOCK,
			&&label__SCOPE_GET_WITHOUT_LOCK,
			&&label__SCOPE_PUT,
			&&label__SCOPE_PUT_CLEAR,
			&&label__LAMBDA,
			&&label__ADVANCED_LAMBDA,
			&&label__SELF,
			&&label__CLASS,
			&&label__SUPER,
			&&label__NUL,
			&&label__BOOLEAN,
			&&label__INTEGER,
			&&label__FLOAT,
			&&label__INSTANCE,
			&&label__RETURN_N,
			&&label__RETURN_B,
			&&label__RETURN_I,
			&&label__RETURN_F,
			&&label__RETURN_L,
			&&label__RETURN_V,
			&&label__RETURN_T,
			&&label__CALL,
			&&label__CALL_WITH_EXPANSION,
			&&label__CALL_OUTER,
			&&label__GET_AT,
			&&label__SET_AT,
#define XEMMAI__CODE__LABEL_UNARY(a_name)\
			&&label__##a_name##_L,\
			&&label__##a_name##_V,\
			&&label__##a_name##_T,
			XEMMAI__CODE__LABEL_UNARY(PLUS)
			XEMMAI__CODE__LABEL_UNARY(MINUS)
			XEMMAI__CODE__LABEL_UNARY(NOT)
			XEMMAI__CODE__LABEL_UNARY(COMPLEMENT)
#define XEMMAI__CODE__LABEL_BINARY(a_name)\
			&&label__##a_name##_II,\
			&&label__##a_name##_FI,\
			&&label__##a_name##_LI,\
			&&label__##a_name##_VI,\
			&&label__##a_name##_TI,\
			&&label__##a_name##_IF,\
			&&label__##a_name##_FF,\
			&&label__##a_name##_LF,\
			&&label__##a_name##_VF,\
			&&label__##a_name##_TF,\
			&&label__##a_name##_IL,\
			&&label__##a_name##_FL,\
			&&label__##a_name##_LL,\
			&&label__##a_name##_VL,\
			&&label__##a_name##_TL,\
			&&label__##a_name##_IV,\
			&&label__##a_name##_FV,\
			&&label__##a_name##_LV,\
			&&label__##a_name##_VV,\
			&&label__##a_name##_TV,\
			&&label__##a_name##_IT,\
			&&label__##a_name##_FT,\
			&&label__##a_name##_LT,\
			&&label__##a_name##_VT,\
			&&label__##a_name##_TT,
			XEMMAI__CODE__LABEL_BINARY(MULTIPLY)
			XEMMAI__CODE__LABEL_BINARY(DIVIDE)
			XEMMAI__CODE__LABEL_BINARY(MODULUS)
			XEMMAI__CODE__LABEL_BINARY(ADD)
			XEMMAI__CODE__LABEL_BINARY(SUBTRACT)
			XEMMAI__CODE__LABEL_BINARY(LEFT_SHIFT)
			XEMMAI__CODE__LABEL_BINARY(RIGHT_SHIFT)
			XEMMAI__CODE__LABEL_BINARY(LESS)
			XEMMAI__CODE__LABEL_BINARY(LESS_EQUAL)
			XEMMAI__CODE__LABEL_BINARY(GREATER)
			XEMMAI__CODE__LABEL_BINARY(GREATER_EQUAL)
			XEMMAI__CODE__LABEL_BINARY(EQUALS)
			XEMMAI__CODE__LABEL_BINARY(NOT_EQUALS)
			XEMMAI__CODE__LABEL_BINARY(IDENTICAL)
			XEMMAI__CODE__LABEL_BINARY(NOT_IDENTICAL)
			XEMMAI__CODE__LABEL_BINARY(AND)
			XEMMAI__CODE__LABEL_BINARY(XOR)
			XEMMAI__CODE__LABEL_BINARY(OR)
			&&label__SEND,
			&&label__CALL_TAIL,
			&&label__CALL_WITH_EXPANSION_TAIL,
			&&label__CALL_OUTER_TAIL,
			&&label__GET_AT_TAIL,
			&&label__SET_AT_TAIL,
			XEMMAI__CODE__LABEL_UNARY(PLUS_TAIL)
			XEMMAI__CODE__LABEL_UNARY(MINUS_TAIL)
			XEMMAI__CODE__LABEL_UNARY(NOT_TAIL)
			XEMMAI__CODE__LABEL_UNARY(COMPLEMENT_TAIL)
			XEMMAI__CODE__LABEL_BINARY(MULTIPLY_TAIL)
			XEMMAI__CODE__LABEL_BINARY(DIVIDE_TAIL)
			XEMMAI__CODE__LABEL_BINARY(MODULUS_TAIL)
			XEMMAI__CODE__LABEL_BINARY(ADD_TAIL)
			XEMMAI__CODE__LABEL_BINARY(SUBTRACT_TAIL)
			XEMMAI__CODE__LABEL_BINARY(LEFT_SHIFT_TAIL)
			XEMMAI__CODE__LABEL_BINARY(RIGHT_SHIFT_TAIL)
			XEMMAI__CODE__LABEL_BINARY(LESS_TAIL)
			XEMMAI__CODE__LABEL_BINARY(LESS_EQUAL_TAIL)
			XEMMAI__CODE__LABEL_BINARY(GREATER_TAIL)
			XEMMAI__CODE__LABEL_BINARY(GREATER_EQUAL_TAIL)
			XEMMAI__CODE__LABEL_BINARY(EQUALS_TAIL)
			XEMMAI__CODE__LABEL_BINARY(NOT_EQUALS_TAIL)
			XEMMAI__CODE__LABEL_BINARY(IDENTICAL_TAIL)
			XEMMAI__CODE__LABEL_BINARY(NOT_IDENTICAL_TAIL)
			XEMMAI__CODE__LABEL_BINARY(AND_TAIL)
			XEMMAI__CODE__LABEL_BINARY(XOR_TAIL)
			XEMMAI__CODE__LABEL_BINARY(OR_TAIL)
			&&label__SEND_TAIL,
			&&label__END,
			&&label__SAFE_POINT,
			&&label__BREAK_POINT
		};
		*a_labels = labels;
		return -1;
	}
#else
size_t t_code::f_loop(t_context* a_context)
{
#endif
	auto base = a_context->v_base;
	auto& pc = a_context->f_pc();
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__CODE__CASE(a_name) XEMMAI__MACRO__CONCATENATE(label__, a_name):
#define XEMMAI__CODE__BREAK goto **pc;
	XEMMAI__CODE__BREAK
#else
#define XEMMAI__CODE__CASE(a_name) case XEMMAI__MACRO__CONCATENATE(e_instruction__, a_name):
#define XEMMAI__CODE__BREAK break;
	while (true) {
		switch (static_cast<t_instruction>(reinterpret_cast<intptr_t>(*pc))) {
#endif
		XEMMAI__CODE__CASE(JUMP)
			pc = static_cast<void**>(*++pc);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BRANCH)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				if (f_as<bool>(stack[0]))
					++pc;
				else
					pc = static_cast<void**>(*pc);
				stack[0].f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(TRY)
			f_try(a_context);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CATCH)
			return e_try__CATCH;
		XEMMAI__CODE__CASE(FINALLY)
			return static_cast<t_try>(reinterpret_cast<intptr_t>(*++pc));
		XEMMAI__CODE__CASE(YRT)
			return 0;
		XEMMAI__CODE__CASE(THROW)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				throw t_scoped(std::move(stack[0]));
			}
		XEMMAI__CODE__CASE(CLEAR)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_destruct();
			}
			XEMMAI__CODE__BREAK
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__CODE__INSTRUCTION(a_name) &&XEMMAI__MACRO__CONCATENATE(label__, a_name)
#else
#define XEMMAI__CODE__INSTRUCTION(a_name) reinterpret_cast<void*>(XEMMAI__MACRO__CONCATENATE(e_instruction__, a_name))
#endif
		XEMMAI__CODE__CASE(OBJECT_GET)
			f_object_get(base, pc, XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MONOMORPHIC_CLASS), XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MONOMORPHIC_INSTANCE), XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_MONOMORPHIC_CLASS)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				t_stacked& top = stack[0];
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned() && p->v_structure == pc0[4]) {
					t_scoped value = static_cast<t_object*>(p->f_type()->v_this)->f_get(key);
					top = value.f_type() == f_global()->f_type<t_method>() ? f_as<t_method&>(value).f_bind(p) : value;
				} else {
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC);
					top = top.f_get(key);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_MONOMORPHIC_INSTANCE)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				t_stacked& top = stack[0];
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
					size_t index = reinterpret_cast<size_t>(pc0[5]);
					if (p->v_structure == pc0[4]) {
						top = p->f_field_get(index);
					} else {
						auto key = static_cast<t_object*>(pc0[2]);
						if (index < p->v_structure->f_size() && static_cast<t_object*>(p->v_structure->f_fields()[index]) == key) {
							top = p->f_field_get(index);
						} else {
							pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC);
							top = top.f_get(key);
						}
					}
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC);
					top = top.f_get(key);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				t_stacked& top = stack[0];
				top = top.f_get(key);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_INDIRECT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				t_stacked& key = stack[1];
				top = top.f_get(key);
				key.f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT)
			f_object_put(base, pc, XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MONOMORPHIC_ADD), XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MONOMORPHIC_SET), XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_MONOMORPHIC_ADD)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				t_stacked& top = stack[0];
				t_stacked& value = stack[1];
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
					auto structure = static_cast<t_structure*>(pc0[4]);
					if (p->v_structure == structure->v_parent1) {
						p->f_field_add(t_scoped(structure->v_this), t_scoped(value));
					} else {
						auto key = static_cast<t_object*>(pc0[2]);
						pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC);
						p->f_field_put(key, t_scoped(value));
					}
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC);
					top.f_put(key, t_scoped(value));
				}
				top = std::move(value);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_MONOMORPHIC_SET)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				t_stacked& top = stack[0];
				t_stacked& value = stack[1];
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
					size_t index = reinterpret_cast<size_t>(pc0[5]);
					if (p->v_structure == pc0[4]) {
						p->f_field_get(index) = value;
					} else {
						auto key = static_cast<t_object*>(pc0[2]);
						if (index < p->v_structure->f_size() && static_cast<t_object*>(p->v_structure->f_fields()[index]) == key) {
							p->f_field_get(index) = value;
						} else {
							pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC);
							p->f_field_put(key, t_scoped(value));
						}
					}
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC);
					top.f_put(key, t_scoped(value));
				}
				top = std::move(value);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				t_stacked& top = stack[0];
				t_stacked& value = stack[1];
				top.f_put(key, t_scoped(value));
				top = std::move(value);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_CLEAR)
			f_object_put_clear(base, pc, XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MONOMORPHIC_ADD), XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MONOMORPHIC_SET), XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_CLEAR_MONOMORPHIC_ADD)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				t_stacked& top = stack[0];
				t_stacked& value = stack[1];
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
					auto structure = static_cast<t_structure*>(pc0[4]);
					if (p->v_structure == structure->v_parent1) {
						p->f_field_add(t_scoped(structure->v_this), std::move(value));
					} else {
						auto key = static_cast<t_object*>(pc0[2]);
						pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MEGAMORPHIC);
						p->f_field_put(key, std::move(value));
					}
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MEGAMORPHIC);
					f_put_or_clear(top, key, value);
				}
				top.f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_CLEAR_MONOMORPHIC_SET)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				t_stacked& top = stack[0];
				t_stacked& value = stack[1];
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
					size_t index = reinterpret_cast<size_t>(pc0[5]);
					if (p->v_structure == pc0[4]) {
						p->f_field_get(index) = std::move(value);
					} else {
						auto key = static_cast<t_object*>(pc0[2]);
						if (index < p->v_structure->f_size() && static_cast<t_object*>(p->v_structure->f_fields()[index]) == key) {
							p->f_field_get(index) = std::move(value);
						} else {
							pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MEGAMORPHIC);
							p->f_field_put(key, std::move(value));
						}
					}
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_CLEAR_MEGAMORPHIC);
					f_put_or_clear(top, key, value);
				}
				top.f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_CLEAR_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				t_stacked& top = stack[0];
				t_stacked& value = stack[1];
				f_put_or_clear(top, key, value);
				top.f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_INDIRECT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				t_stacked& key = stack[1];
				t_stacked& value = stack[2];
				top.f_put(key, t_scoped(value));
				key.f_destruct();
				top = std::move(value);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_HAS)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				auto key = static_cast<t_object*>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				top = top.f_has(key);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_HAS_INDIRECT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				t_stacked& key = stack[1];
				top = top.f_has(key);
				key.f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_REMOVE)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				auto key = static_cast<t_object*>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				top = top.f_remove(key);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_REMOVE_INDIRECT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				t_stacked& key = stack[1];
				top = top.f_remove(key);
				key.f_destruct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET)
			f_method_get(base, pc, XEMMAI__CODE__INSTRUCTION(METHOD_GET_MONOMORPHIC_CLASS), XEMMAI__CODE__INSTRUCTION(METHOD_GET_MONOMORPHIC_INSTANCE), XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET_MONOMORPHIC_CLASS)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				t_scoped top = std::move(stack[0]);
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned() && p->v_structure == pc0[4]) {
					f_get_of_type(*p, key, stack);
				} else {
					pc0[0] = XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC);
					top.f_get(key, stack);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET_MONOMORPHIC_INSTANCE)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				t_scoped top = std::move(stack[0]);
				auto p = static_cast<t_object*>(top);
				if (top.f_tag() >= t_value::e_tag__OBJECT && p->f_owned()) {
					size_t index = reinterpret_cast<size_t>(pc0[5]);
					if (p->v_structure == pc0[4]) {
						stack[0].f_construct(p->f_field_get(index));
						stack[1].f_construct();
					} else {
						auto key = static_cast<t_object*>(pc0[2]);
						if (index < p->v_structure->f_size() && static_cast<t_object*>(p->v_structure->f_fields()[index]) == key) {
							stack[0].f_construct(p->f_field_get(index));
							stack[1].f_construct();
						} else {
							pc0[0] = XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC);
							top.f_get(key, stack);
						}
					}
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC);
					top.f_get(key, stack);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 6;
				t_stacked* stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				t_scoped top = std::move(stack[0]);
				top.f_get(key, stack);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_BIND)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				if (stack[0].f_tag() < t_value::e_tag__OBJECT) goto label__THROW_NOT_SUPPORTED_M1;
				t_object* p = stack[0];
				if (p->f_type() == f_global()->f_type<t_method>())
					stack[0] = f_as<t_method&>(p).f_function();
				else if (!f_is<t_lambda>(p) && p->f_type() != f_global()->f_type<t_native>())
					f_method_bind(stack);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(GLOBAL_GET)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				auto key = static_cast<t_object*>(*++pc);
				++pc;
				stack[0].f_construct(f_engine()->f_module_global()->f_get(key));
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_LET)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				base[index].f_construct(stack[0]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_LET_CLEAR)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				base[index].f_construct(std::move(stack[0]));
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_GET)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_construct(base[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_PUT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				base[index] = stack[0];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_PUT_CLEAR)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				base[index] = std::move(stack[0]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET0)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				t_object* scope = a_context->v_scope;
				t_with_lock_for_read lock(scope);
				stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET1)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				auto& lambda = f_as<t_lambda&>(a_context->v_lambda);
				t_with_lock_for_read lock(lambda.v_scope);
				stack[0].f_construct(lambda.v_as_scope[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET2)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				t_object* scope = f_as<t_lambda&>(a_context->v_lambda).v_as_scope.v_outer;
				t_with_lock_for_read lock(scope);
				stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t outer = reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				assert(outer >= 3);
				t_object* scope = f_as<const t_scope&>(f_as<t_lambda&>(a_context->v_lambda).v_as_scope.v_outer).v_outer;
				for (size_t i = 3; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
				t_with_lock_for_read lock(scope);
				stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET0_WITHOUT_LOCK)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_construct(f_as<const t_scope&>(a_context->v_scope)[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET1_WITHOUT_LOCK)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_construct(f_as<t_lambda&>(a_context->v_lambda).v_as_scope[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET2_WITHOUT_LOCK)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_construct(f_as<const t_scope&>(f_as<t_lambda&>(a_context->v_lambda).v_as_scope.v_outer)[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET_WITHOUT_LOCK)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t outer = reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				assert(outer >= 3);
				t_object* scope = f_as<const t_scope&>(f_as<t_lambda&>(a_context->v_lambda).v_as_scope.v_outer).v_outer;
				for (size_t i = 3; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
				stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_PUT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t outer = reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				t_object* scope;
				if (outer > 0) {
					scope = f_as<t_lambda&>(a_context->v_lambda).v_scope;
					for (size_t i = 1; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
				} else {
					scope = a_context->v_scope;
				}
				t_with_lock_for_write lock(scope);
				f_as<t_scope&>(scope)[index] = stack[0];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_PUT_CLEAR)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t outer = reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				++pc;
				t_object* scope;
				if (outer > 0) {
					scope = f_as<t_lambda&>(a_context->v_lambda).v_scope;
					for (size_t i = 1; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
				} else {
					scope = a_context->v_scope;
				}
				t_with_lock_for_write lock(scope);
				f_as<t_scope&>(scope)[index] = std::move(stack[0]);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(LAMBDA)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				auto code = static_cast<t_object*>(*++pc);
				++pc;
				stack[0].f_construct(t_lambda::f_instantiate(t_scoped(a_context->v_scope), code));
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(ADVANCED_LAMBDA)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				auto code = static_cast<t_object*>(*++pc);
				++pc;
				stack[0].f_construct(t_advanced_lambda::f_instantiate(t_scoped(a_context->v_scope), code, stack));
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SELF)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t outer = reinterpret_cast<size_t>(*++pc);
				++pc;
				if (outer > 0) {
					t_object* scope = f_as<t_lambda&>(a_context->v_lambda).v_scope;
					for (size_t i = 1; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
					stack[0].f_construct(f_as<const t_scope&>(scope)[0]);
				} else {
					stack[0].f_construct(base[-1]);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CLASS)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				top = top.f_type()->v_this;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SUPER)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				t_stacked& top = stack[0];
				if (top.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"not class.");
				top = static_cast<t_slot&>(f_as<t_type&>(top).v_super);
				if (!top) t_throwable::f_throw(L"no more super class.");
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(NUL)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_construct();
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BOOLEAN)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				bool value = reinterpret_cast<intptr_t>(*++pc) != 0;
				++pc;
				stack[0].f_construct(value);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(INTEGER)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				intptr_t value = reinterpret_cast<intptr_t>(*++pc);
				++pc;
				stack[0].f_construct(value);
			}
			XEMMAI__CODE__BREAK
#define XEMMAI__CODE__FLOAT(a_x, a_y)\
				union\
				{\
					double a_x;\
					void* a_y[sizeof(double) / sizeof(void*)];\
				};\
				for (size_t i = 0; i < sizeof(double) / sizeof(void*); ++i) a_y[i] = *++pc;
		XEMMAI__CODE__CASE(FLOAT)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				XEMMAI__CODE__FLOAT(v0, v1)
				++pc;
				stack[0].f_construct(v0);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(INSTANCE)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				auto& value = *static_cast<const t_value*>(*++pc);
				++pc;
				stack[0].f_construct(value);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(RETURN_N)
			a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, nullptr);
			return -1;
		XEMMAI__CODE__CASE(RETURN_B)
			{
				bool value = reinterpret_cast<intptr_t>(*++pc) != 0;
				a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, value);
			}
			return -1;
		XEMMAI__CODE__CASE(RETURN_I)
			{
				intptr_t value = reinterpret_cast<intptr_t>(*++pc);
				a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, value);
			}
			return -1;
		XEMMAI__CODE__CASE(RETURN_F)
			{
				XEMMAI__CODE__FLOAT(v0, v1)
				a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, v0);
			}
			return -1;
		XEMMAI__CODE__CASE(RETURN_L)
			{
				auto& value = *static_cast<const t_value*>(*++pc);
				a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, value);
			}
			return -1;
		XEMMAI__CODE__CASE(RETURN_V)
			{
				size_t index = reinterpret_cast<size_t>(*++pc);
				a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, index);
				return -1;
			}
		XEMMAI__CODE__CASE(RETURN_T)
			a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates);
			return -1;
		XEMMAI__CODE__CASE(CALL)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t n = reinterpret_cast<size_t>(*++pc);
				++pc;
				t_scoped x = std::move(stack[0]);
				x.f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CALL_WITH_EXPANSION)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t n = reinterpret_cast<size_t>(*++pc);
				++pc;
				n = f_expand(pc, stack, n);
				t_scoped x = std::move(stack[0]);
				x.f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CALL_OUTER)
			{
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
				size_t index = reinterpret_cast<size_t>(*++pc);
				size_t n = reinterpret_cast<size_t>(*++pc);
				++pc;
				t_slot& x = f_as<t_lambda&>(a_context->v_lambda).v_as_scope[index];
				stack[1].f_construct();
				x.f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
#define XEMMAI__CODE__FETCH()
#define XEMMAI__CODE__PRIMITIVE(a_x)\
						stack[0].f_construct(a_x);
#define XEMMAI__CODE__PREPARE()\
						t_scoped x = std::move(stack[1]);
#define XEMMAI__CODE__FETCH_L()\
				auto& a0 = *static_cast<const t_value*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_P_L(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_L()\
						auto& x = a0;
#define XEMMAI__CODE__FETCH_V()\
				t_stacked& a0 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_P_V(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_V() XEMMAI__CODE__PREPARE_L()
#define XEMMAI__CODE__FETCH_T()\
				t_stacked& a0 = stack[1];
#define XEMMAI__CODE__PRIMITIVE_T(a_x)\
						stack[0].f_construct(a_x);\
						stack[1].f_destruct();
#define XEMMAI__CODE__PRIMITIVE_P_T(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_T() XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__PREPARE_LL()\
						auto& x = a0;\
						stack[2].f_construct(a1);
#define XEMMAI__CODE__PREPARE_TL()\
						t_scoped x = std::move(stack[1]);\
						stack[2].f_construct(a1);
#define XEMMAI__CODE__FETCH_LI()\
				auto& a0 = *static_cast<const t_value*>(*++pc);\
				intptr_t a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_LI(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_LI(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_LI() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VI()\
				t_stacked& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				intptr_t a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_VI(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_VI(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_VI() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TI()\
				t_stacked& a0 = stack[1];\
				intptr_t a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_TI(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_TI(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PREPARE_TI() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_LF()\
				auto& a0 = *static_cast<const t_value*>(*++pc);\
				XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PRIMITIVE_LF(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_LF(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_LF() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VF()\
				t_stacked& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PRIMITIVE_VF(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_VF(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_VF() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TF()\
				t_stacked& a0 = stack[1];\
				XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PRIMITIVE_TF(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_TF(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PREPARE_TF() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_IL()\
				intptr_t a0 = reinterpret_cast<intptr_t>(*++pc);\
				auto& a1 = *static_cast<const t_value*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_IL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_IL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_IL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__FETCH_FL()\
				XEMMAI__CODE__FLOAT(a0, v0)\
				auto& a1 = *static_cast<const t_value*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_FL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_FL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_FL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__FETCH_LL()\
				auto& a0 = *static_cast<const t_value*>(*++pc);\
				auto& a1 = *static_cast<const t_value*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_LL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_LL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_LL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__FETCH_VL()\
				t_stacked& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				auto& a1 = *static_cast<const t_value*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_VL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_VL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_VL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_VL() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TL()\
				t_stacked& a0 = stack[1];\
				auto& a1 = *static_cast<const t_value*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_TL(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_TL(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_TL(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__FETCH_IV()\
				intptr_t a0 = reinterpret_cast<intptr_t>(*++pc);\
				t_stacked& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_IV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_IV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_IV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__FETCH_FV()\
				XEMMAI__CODE__FLOAT(a0, v0)\
				t_stacked& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_FV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_FV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_FV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__FETCH_LV()\
				auto& a0 = *static_cast<const t_value*>(*++pc);\
				t_stacked& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_LV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_LV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_LV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_LV() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VV()\
				t_stacked& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				t_stacked& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_VV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_VV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_VV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TV()\
				t_stacked& a0 = stack[1];\
				t_stacked& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_TV(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_TV(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_TV(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PREPARE_TV() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_IT()\
				intptr_t a0 = reinterpret_cast<intptr_t>(*++pc);\
				t_stacked& a1 = stack[1];
#define XEMMAI__CODE__PRIMITIVE_IT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_IT(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_IT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__FETCH_FT()\
				XEMMAI__CODE__FLOAT(a0, v0)\
				t_stacked& a1 = stack[1];
#define XEMMAI__CODE__PRIMITIVE_FT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_FT(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_FT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__FETCH_LT()\
				auto& a0 = *static_cast<const t_value*>(*++pc);\
				t_stacked& a1 = stack[1];
#define XEMMAI__CODE__PRIMITIVE_LT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_LT(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_LT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PREPARE_LT()\
						auto& x = a0;\
						stack[2].f_construct(std::move(stack[1]));
#define XEMMAI__CODE__FETCH_VT()\
				t_stacked& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				t_stacked& a1 = stack[1];
#define XEMMAI__CODE__PRIMITIVE_VT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PP_VT(a_x) XEMMAI__CODE__PRIMITIVE_P_T(a_x)
#define XEMMAI__CODE__PRIMITIVE_PO_VT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PREPARE_VT() XEMMAI__CODE__PREPARE_LT()
#define XEMMAI__CODE__FETCH_TT()\
				t_stacked& a0 = stack[1];\
				t_stacked& a1 = stack[2];
#define XEMMAI__CODE__PRIMITIVE_TT(a_x)\
						stack[0].f_construct(a_x);\
						stack[1].f_destruct();\
						stack[2].f_destruct();
#define XEMMAI__CODE__PRIMITIVE_PP_TT(a_x)\
						stack[0].f_construct(a_x);
#define XEMMAI__CODE__PRIMITIVE_PO_TT(a_x)\
						stack[0].f_construct(a_x);\
						stack[2].f_destruct();
#define XEMMAI__CODE__PREPARE_TT() XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name, XEMMAI__CODE__OPERANDS))\
			{\
				t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
				++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_p, a_x)\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PRIMITIVE, a_p), XEMMAI__CODE__OPERANDS)(a_x)
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
				{\
					XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
					f_operator<&t_type::a_method>(x, stack);\
				}
#define XEMMAI__CODE__CASE_END\
			}\
			XEMMAI__CODE__BREAK
#define XEMMAI__CODE__CASE_NA(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name, XEMMAI__CODE__OPERANDS))\
			goto label__THROW_NOT_SUPPORTED;
#define XEMMAI__CODE__OTHERS
#define XEMMAI__CODE__OPERANDS
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__OTHERS
#define XEMMAI__CODE__UNARY
#define XEMMAI__CODE__OPERANDS _L
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _V
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _T
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__UNARY
#define XEMMAI__CODE__BINARY
#define XEMMAI__CODE__OPERANDS _LL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY
#define XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__OPERANDS _LI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__OPERANDS _IL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__OPERANDS _LF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__BINARY_FX
#define XEMMAI__CODE__OPERANDS _FL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_FX
#define XEMMAI__CODE__BINARY_NA
#define XEMMAI__CODE__OPERANDS _II
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_NA
#undef XEMMAI__CODE__CASE_BEGIN
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
#undef XEMMAI__CODE__CASE_NA
		XEMMAI__CODE__CASE(CALL_TAIL)
			{
				size_t n = reinterpret_cast<size_t>(*++pc);
				a_context->f_tail(f_as<t_lambda&>(a_context->v_lambda).v_privates, n);
				return n;
			}
		XEMMAI__CODE__CASE(CALL_WITH_EXPANSION_TAIL)
			{
				size_t n = reinterpret_cast<size_t>(*++pc);
				size_t privates = f_as<t_lambda&>(a_context->v_lambda).v_privates;
				n = f_expand(pc, base + privates, n);
				a_context->f_tail(privates, n);
				return n;
			}
		XEMMAI__CODE__CASE(CALL_OUTER_TAIL)
			{
				size_t index = reinterpret_cast<size_t>(*++pc);
				size_t n = reinterpret_cast<size_t>(*++pc);
				auto& lambda = f_as<t_lambda&>(a_context->v_lambda);
				t_stacked* stack = base + lambda.v_privates;
				stack[0].f_construct(lambda.v_as_scope[index]);
				stack[1].f_construct();
				a_context->f_tail(lambda.v_privates, n);
				return n;
			}
#undef XEMMAI__CODE__PRIMITIVE
#define XEMMAI__CODE__PRIMITIVE(a_x)
#undef XEMMAI__CODE__PREPARE_V
#define XEMMAI__CODE__PREPARE_V()\
						t_scoped x = std::move(static_cast<t_value&>(a0));
#undef XEMMAI__CODE__PRIMITIVE_T
#define XEMMAI__CODE__PRIMITIVE_T(a_x)\
						stack[1].f_destruct();
#undef XEMMAI__CODE__PREPARE_VL
#define XEMMAI__CODE__PREPARE_VL()\
						stack[2].f_construct(a1);\
						t_scoped x = std::move(static_cast<t_value&>(a0));
#undef XEMMAI__CODE__PREPARE_VI
#define XEMMAI__CODE__PREPARE_VI() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_VF
#define XEMMAI__CODE__PREPARE_VF() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_LV
#define XEMMAI__CODE__PREPARE_LV()\
						auto& x = a0;\
						stack[2].f_construct(std::move(static_cast<t_value&>(a1)));
#undef XEMMAI__CODE__PREPARE_VV
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_TV
#define XEMMAI__CODE__PREPARE_TV()\
						t_scoped x = std::move(stack[1]);\
						stack[2].f_construct(std::move(static_cast<t_value&>(a1)));
#undef XEMMAI__CODE__PREPARE_VT
#define XEMMAI__CODE__PREPARE_VT()\
						t_scoped x = std::move(static_cast<t_value&>(a0));\
						stack[2].f_construct(std::move(stack[1]));
#undef XEMMAI__CODE__PRIMITIVE_TT
#define XEMMAI__CODE__PRIMITIVE_TT(a_x)\
						stack[1].f_destruct();\
						stack[2].f_destruct();
#undef XEMMAI__CODE__PRIMITIVE_PP_TT
#define XEMMAI__CODE__PRIMITIVE_PP_TT(a_x)
#undef XEMMAI__CODE__PRIMITIVE_PO_TT
#define XEMMAI__CODE__PRIMITIVE_PO_TT(a_x)\
						stack[2].f_destruct();
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name##_TAIL, XEMMAI__CODE__OPERANDS))\
			{\
				size_t privates = f_as<t_lambda&>(a_context->v_lambda).v_privates;\
				t_stacked* stack = base + privates;\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
				++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_p, a_x)\
				a_context->f_return(privates, a_x);\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PRIMITIVE, a_p), XEMMAI__CODE__OPERANDS)(a_x)\
				return -1;
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
				{\
					XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
					return a_context->f_tail<&t_type::a_method>(privates, x);\
				}
#define XEMMAI__CODE__CASE_END\
			}
#define XEMMAI__CODE__CASE_NA(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name##_TAIL, XEMMAI__CODE__OPERANDS))\
			goto label__THROW_NOT_SUPPORTED;
#define XEMMAI__CODE__OTHERS
#define XEMMAI__CODE__OPERANDS
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__OTHERS
#define XEMMAI__CODE__UNARY
#define XEMMAI__CODE__OPERANDS _L
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _V
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _T
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__UNARY
#define XEMMAI__CODE__BINARY
#define XEMMAI__CODE__OPERANDS _LL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY
#define XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__OPERANDS _LI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__OPERANDS _IL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__OPERANDS _LF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__BINARY_FX
#define XEMMAI__CODE__OPERANDS _FL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_FX
#define XEMMAI__CODE__BINARY_NA
#define XEMMAI__CODE__OPERANDS _II
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_NA
#undef XEMMAI__CODE__CASE_BEGIN
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
#undef XEMMAI__CODE__CASE_NA
		XEMMAI__CODE__CASE(END)
			a_context->f_return(f_as<t_lambda&>(a_context->v_lambda).v_privates, nullptr);
			return -1;
		XEMMAI__CODE__CASE(SAFE_POINT)
			++pc;
			f_as<t_fiber&>(t_fiber::f_current()).v_context = a_context;
			f_engine()->f_debug_safe_point();
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BREAK_POINT)
			++pc;
			f_as<t_fiber&>(t_fiber::f_current()).v_context = a_context;
			f_engine()->f_debug_break_point();
			XEMMAI__CODE__BREAK
#ifndef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
		}
	}
#endif
label__THROW_NOT_SUPPORTED_M1:
	--pc;
label__THROW_NOT_SUPPORTED:
	t_throwable::f_throw(L"not supported.");
}

void t_code::f_try(t_context* a_context)
{
	auto base = a_context->v_base;
	auto& pc = a_context->f_pc();
	t_stacked* stack = base + reinterpret_cast<size_t>(*++pc);
	auto catch0 = static_cast<void**>(*++pc);
	auto finally0 = static_cast<void**>(*++pc);
	++pc;
	t_try try0;
	try {
		try {
			try0 = static_cast<t_try>(f_loop(a_context));
		} catch (const t_scoped&) {
			throw;
		} catch (...) {
			throw t_throwable::f_instantiate(L"<unknown>.");
		}
	} catch (const t_scoped& thrown) {
		auto& code = f_as<t_code&>(f_as<t_lambda&>(a_context->v_lambda).v_code);
		code.f_stack_clear(pc, base, stack);
		auto& p = f_as<t_fiber&>(t_fiber::f_current());
		void** caught = p.v_caught == nullptr ? pc : p.v_caught;
		p.v_caught = nullptr;
		pc = catch0;
		while (true) {
			try {
				try {
					try0 = static_cast<t_try>(f_loop(a_context));
					if (try0 != e_try__CATCH) break;
					++pc;
					t_scoped type = std::move(stack[0]);
					if (thrown != f_engine()->v_fiber_exit && thrown.f_is(&f_as<t_type&>(type))) {
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						p.f_caught(thrown, caught);
						if ((index & ~(~0 >> 1)) != 0) {
							t_scoped& scope = a_context->v_scope;
							t_with_lock_for_write lock(scope);
							f_as<t_scope&>(scope)[~index] = thrown;
						} else {
							base[index] = thrown;
						}
					} else {
						pc = static_cast<void**>(*pc);
					}
				} catch (const t_scoped&) {
					throw;
				} catch (...) {
					throw t_throwable::f_instantiate(L"<unknown>.");
				}
			} catch (const t_scoped& thrown) {
				code.f_stack_clear(pc, base, stack);
				caught = p.v_caught == nullptr ? pc : p.v_caught;
				p.v_caught = nullptr;
				pc = finally0;
				f_loop(a_context);
				p.v_caught = caught;
				throw thrown;
			}
		}
		if (try0 == e_try__THROW) {
			pc = finally0;
			f_loop(a_context);
			p.v_caught = caught;
			throw thrown;
		}
	}
	pc = finally0;
	f_loop(a_context);
	auto break0 = static_cast<void**>(*++pc);
	auto continue0 = static_cast<void**>(*++pc);
	auto return0 = static_cast<void**>(*++pc);
	switch (try0) {
	case e_try__BREAK:
		pc = break0;
		break;
	case e_try__CONTINUE:
		pc = continue0;
		break;
	case e_try__RETURN:
		pc = return0;
		break;
	default:
		++pc;
	}
}

t_scoped t_code::f_instantiate(t_object* a_module, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_code>());
	object.f_pointer__(new t_code(a_module, a_shared, a_variadic, a_privates, a_shareds, a_arguments, a_minimum));
	return object;
}

void t_code::f_scan(t_scan a_scan)
{
	a_scan(v_module);
}

const t_at* t_code::f_at(void** a_address) const
{
	auto i = std::lower_bound(v_ats.begin(), v_ats.end(), t_address_at(a_address - &v_instructions[0], t_at(0, 0, 0)));
	return i == v_ats.end() ? nullptr : &*i;
}

const std::vector<bool>& t_code::f_stack_map(void** a_address) const
{
	return *std::lower_bound(v_stack_map.begin(), v_stack_map.end(), t_stack_map{static_cast<size_t>(a_address - &v_instructions[0]), nullptr})->v_pattern;
}

void t_code::f_stack_clear(void** a_address, t_stacked* a_base) const
{
	auto& map = f_stack_map(a_address);
	a_base[-1].f_destruct();
	for (size_t i = 0; i < v_arguments; ++i) a_base++->f_destruct();
	for (bool x : map) {
		if (x) a_base->f_destruct();
		++a_base;
	}
}

void t_code::f_stack_clear(void** a_address, t_stacked* a_base, t_stacked* a_stack) const
{
	auto& map = f_stack_map(a_address);
	auto p = a_stack;
	for (size_t i = p - a_base - v_arguments; i < map.size(); ++i) {
		if (map[i]) p->f_destruct();
		++p;
	}
	a_stack[0].f_construct();
}

t_type* t_type_of<t_code>::f_derive()
{
	return nullptr;
}

void t_type_of<t_code>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_code&>(a_this).f_scan(a_scan);
}

void t_type_of<t_code>::f_finalize(t_object* a_this)
{
	delete &f_as<t_code&>(a_this);
}

void t_type_of<t_code>::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(a_stack, a_n, L"uninstantiatable.");
}

}
