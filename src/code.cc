#include <xemmai/list.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

void f_method_bind(t_pvalue* a_stack)
{
	a_stack[2] = a_stack[1];
	auto p = static_cast<t_object*>(a_stack[0]);
	size_t n = p->f_type()->f_get_at(p, a_stack);
	if (n != size_t(-1)) f_loop(a_stack, n);
	a_stack[1] = nullptr;
}

inline void f_allocate(t_pvalue* a_stack, size_t a_n)
{
	auto used = a_stack + a_n;
	if (used > f_stack()) f_stack__(used);
}

size_t f_expand(void**& a_pc, t_pvalue* a_stack, size_t a_n)
{
	assert(a_n > 0);
	a_stack += a_n + 1;
	auto x = a_stack[0];
	size_t n;
	if (f_is<t_tuple>(x)) {
		auto& tuple = f_as<const t_tuple&>(x);
		n = tuple.f_size();
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) a_stack[i] = tuple[i];
	} else if (f_is<t_list>(x)) {
		auto& list = f_as<t_list&>(x);
		list.f_owned_or_shared<t_scoped_lock_for_read>([&]
		{
			n = list.f_size();
			f_allocate(a_stack, n);
			for (size_t i = 0; i < n; ++i) a_stack[i] = list[i];
		});
	} else {
		static size_t index;
		auto size = x.f_invoke(f_global()->f_symbol_size(), index);
		f_check<size_t>(size, L"size");
		n = f_as<size_t>(size);
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) a_stack[i] = x.f_get_at(i);
	}
	return a_n - 1 + n;
}

}

void t_code::f_object_get(t_pvalue* a_base, void**& a_pc, void* a_instance, void* a_class, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 7;
	auto stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	auto& top = stack[0];
	auto& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto type = top.f_type();
		*static_cast<t_slot*>(pc0[4]) = t_object::f_of(type);
		pc0[4] = type;
		auto index = type->f_index(key);
		*reinterpret_cast<size_t*>(pc0 + 5) = index;
		std::atomic_thread_fence(std::memory_order_release);
		if (index < type->v_instance_fields) {
			pc0[0] = a_instance;
			top = top->f_fields()[index];
		} else if (index < type->v_fields) {
			pc0[0] = a_class;
			auto& field = type->f_fields()[index].second;
			if (f_is_bindable(field))
				top = f_new<t_method>(f_global(), field, top);
			else
				top = field;
		} else {
			pc0[0] = a_megamorphic;
			top = (type->*type->v_get)(top, key, reinterpret_cast<size_t&>(pc0[6]));
		}
	} else {
		top = top.f_get(key, reinterpret_cast<size_t&>(pc0[6]));
	}
}

void t_code::f_object_put(t_pvalue* a_base, void**& a_pc, void* a_set, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 7;
	auto stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	auto& top = stack[0];
	auto& value = stack[1];
	auto& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto type = top.f_type();
		auto index = type->f_index(key);
		if (index < type->v_instance_fields) {
			*static_cast<t_slot*>(pc0[4]) = t_object::f_of(type);
			pc0[4] = type;
			*reinterpret_cast<size_t*>(pc0 + 5) = index;
			std::atomic_thread_fence(std::memory_order_release);
			pc0[0] = a_set;
			top->f_fields()[index] = value;
		} else {
			pc0[0] = a_megamorphic;
			type->v_put(top, key, value);
		}
	} else {
		top.f_put(key, reinterpret_cast<size_t&>(pc0[6]), value);
	}
	top = value;
}

void t_code::f_method_get(t_pvalue* a_base, void**& a_pc, void* a_instance, void* a_class, void* a_megamorphic)
{
	void** pc0 = a_pc;
	a_pc += 7;
	auto stack = a_base + reinterpret_cast<size_t>(pc0[1]);
	auto key = static_cast<t_object*>(pc0[2]);
	auto top = stack[0];
	auto& count = *reinterpret_cast<size_t*>(pc0 + 3);
	if (f_atomic_increment(count) == 2) {
		auto type = top.f_type();
		*static_cast<t_slot*>(pc0[4]) = t_object::f_of(type);
		pc0[4] = type;
		auto index = type->f_index(key);
		*reinterpret_cast<size_t*>(pc0 + 5) = index;
		std::atomic_thread_fence(std::memory_order_release);
		if (index < type->v_instance_fields) {
			pc0[0] = a_instance;
			stack[0] = top->f_fields()[index];
			stack[1] = nullptr;
		} else if (index < type->v_fields) {
			pc0[0] = a_class;
			type->f_bind_class(top, index, stack);
		} else {
			pc0[0] = a_megamorphic;
			stack[0] = (type->*type->v_get)(top, key, reinterpret_cast<size_t&>(pc0[6]));
			stack[1] = nullptr;
		}
	} else {
		top.f_bind(key, reinterpret_cast<size_t&>(pc0[6]), stack);
	}
}

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
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
			&&label__OBJECT_GET,
			&&label__OBJECT_GET_MONOMORPHIC_INSTANCE,
			&&label__OBJECT_GET_MONOMORPHIC_CLASS,
			&&label__OBJECT_GET_MEGAMORPHIC,
			&&label__OBJECT_GET_INDIRECT,
			&&label__OBJECT_PUT,
			&&label__OBJECT_PUT_MONOMORPHIC,
			&&label__OBJECT_PUT_MEGAMORPHIC,
			&&label__OBJECT_PUT_INDIRECT,
			&&label__OBJECT_HAS,
			&&label__OBJECT_HAS_INDIRECT,
			&&label__METHOD_GET,
			&&label__METHOD_GET_MONOMORPHIC_INSTANCE,
			&&label__METHOD_GET_MONOMORPHIC_CLASS,
			&&label__METHOD_GET_MEGAMORPHIC,
			&&label__METHOD_BIND,
			&&label__GLOBAL_GET,
			&&label__STACK_GET,
			&&label__STACK_PUT,
			&&label__SCOPE_GET0,
			&&label__SCOPE_GET1,
			&&label__SCOPE_GET2,
			&&label__SCOPE_GET,
			&&label__SCOPE_PUT0,
			&&label__SCOPE_PUT,
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
			&&label__STACK_CALL,
			&&label__SCOPE_CALL0,
			&&label__SCOPE_CALL1,
			&&label__SCOPE_CALL2,
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
			&&label__CALL_TAIL,
			&&label__CALL_WITH_EXPANSION_TAIL,
			&&label__STACK_CALL_TAIL,
			&&label__SCOPE_CALL0_TAIL,
			&&label__SCOPE_CALL1_TAIL,
			&&label__SCOPE_CALL2_TAIL,
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
	auto& pc = a_context->v_pc;
	auto base = a_context->v_base;
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
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				if (f_as<bool>(stack[0]))
					++pc;
				else
					pc = static_cast<void**>(*pc);
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
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				throw t_rvalue(stack[0]);
			}
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__CODE__INSTRUCTION(a_name) &&XEMMAI__MACRO__CONCATENATE(label__, a_name)
#else
#define XEMMAI__CODE__INSTRUCTION(a_name) reinterpret_cast<void*>(XEMMAI__MACRO__CONCATENATE(e_instruction__, a_name))
#endif
		XEMMAI__CODE__CASE(OBJECT_GET)
			f_object_get(base, pc, XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MONOMORPHIC_INSTANCE), XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MONOMORPHIC_CLASS), XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_MONOMORPHIC_INSTANCE)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto& top = stack[0];
				auto type = top.f_type();
				if (type == pc0[4]) {
					top = top->f_fields()[reinterpret_cast<size_t>(pc0[5])];
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC);
					top = type->f_get(top, key, reinterpret_cast<size_t&>(pc0[6]));
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_MONOMORPHIC_CLASS)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto& top = stack[0];
				auto type = top.f_type();
				if (type == pc0[4]) {
					auto& field = type->f_fields()[reinterpret_cast<size_t>(pc0[5])].second;
					if (f_is_bindable(field))
						top = f_new<t_method>(f_global(), field, top);
					else
						top = field;
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_GET_MEGAMORPHIC);
					top = type->f_get(top, key, reinterpret_cast<size_t&>(pc0[6]));
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				auto& top = stack[0];
				top = top.f_get(key, reinterpret_cast<size_t&>(pc0[6]));
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_INDIRECT)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto& index = reinterpret_cast<size_t&>(*++pc);
				++pc;
				auto& top = stack[0];
				auto key = static_cast<t_object*>(stack[1]);
				top = top.f_get(key, index);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT)
			f_object_put(base, pc, XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MONOMORPHIC), XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_MONOMORPHIC)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto& top = stack[0];
				auto& value = stack[1];
				auto type = top.f_type();
				if (type == pc0[4]) {
					top->f_fields()[reinterpret_cast<size_t>(pc0[5])] = value;
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(OBJECT_PUT_MEGAMORPHIC);
					type->f_put(top, key, reinterpret_cast<size_t&>(pc0[6]), value);
				}
				top = value;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				auto& top = stack[0];
				auto& value = stack[1];
				top.f_put(key, reinterpret_cast<size_t&>(pc0[6]), value);
				top = value;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_INDIRECT)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto& index = reinterpret_cast<size_t&>(*++pc);
				++pc;
				auto& top = stack[0];
				auto key = static_cast<t_object*>(stack[1]);
				auto& value = stack[2];
				top.f_put(key, index, value);
				top = value;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_HAS)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto key = static_cast<t_object*>(*++pc);
				auto& index = reinterpret_cast<size_t&>(*++pc);
				++pc;
				auto& top = stack[0];
				top = top.f_has(key, index);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_HAS_INDIRECT)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto& index = reinterpret_cast<size_t&>(*++pc);
				++pc;
				auto& top = stack[0];
				auto key = static_cast<t_object*>(stack[1]);
				top = top.f_has(key, index);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET)
			f_method_get(base, pc, XEMMAI__CODE__INSTRUCTION(METHOD_GET_MONOMORPHIC_INSTANCE), XEMMAI__CODE__INSTRUCTION(METHOD_GET_MONOMORPHIC_CLASS), XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC));
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET_MONOMORPHIC_INSTANCE)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto top = stack[0];
				auto type = top.f_type();
				if (type == pc0[4]) {
					stack[0] = top->f_fields()[reinterpret_cast<size_t>(pc0[5])];
					stack[1] = nullptr;
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC);
					type->f_bind(top, key, reinterpret_cast<size_t&>(pc0[6]), stack);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET_MONOMORPHIC_CLASS)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto top = stack[0];
				auto type = top.f_type();
				if (type == pc0[4]) {
					type->f_bind_class(top, reinterpret_cast<size_t>(pc0[5]), stack);
				} else {
					auto key = static_cast<t_object*>(pc0[2]);
					pc0[0] = XEMMAI__CODE__INSTRUCTION(METHOD_GET_MEGAMORPHIC);
					type->f_bind(top, key, reinterpret_cast<size_t&>(pc0[6]), stack);
				}
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET_MEGAMORPHIC)
			{
				void** pc0 = pc;
				pc += 7;
				auto stack = base + reinterpret_cast<size_t>(pc0[1]);
				auto key = static_cast<t_object*>(pc0[2]);
				auto top = stack[0];
				top.f_bind(key, reinterpret_cast<size_t&>(pc0[6]), stack);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_BIND)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				auto p = static_cast<t_object*>(stack[0]);
				if (reinterpret_cast<uintptr_t>(p) < e_tag__OBJECT) goto label__THROW_NOT_SUPPORTED;
				if (p->f_type() == f_global()->f_type<t_method>())
					stack[0] = f_as<t_method&>(p).f_function();
				else if (!f_is_bindable(p))
					f_method_bind(stack);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(GLOBAL_GET)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto key = static_cast<t_object*>(*++pc);
				auto& index = reinterpret_cast<size_t&>(*++pc);
				++pc;
				stack[0] = f_engine()->f_module_global()->f_get(key, index);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_GET)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0] = base[index];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_PUT)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				base[index] = stack[0];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET0)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0] = a_context->v_scope[index];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET1)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0] = f_as<t_lambda&>(a_context->v_lambda).v_scope_entries[index];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET2)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0] = t_scope::f_outer(f_as<t_lambda&>(a_context->v_lambda).v_scope_entries)[index];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto outer = reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				assert(outer >= 3);
				auto scope = t_scope::f_outer(t_scope::f_outer(f_as<t_lambda&>(a_context->v_lambda).v_scope_entries));
				for (size_t i = 3; i < outer; ++i) scope = t_scope::f_outer(scope);
				stack[0] = scope[index];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_PUT0)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				a_context->v_scope[index] = stack[0];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_PUT)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto outer = reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				++pc;
				assert(outer >= 1);
				auto scope = f_as<t_lambda&>(a_context->v_lambda).v_scope_entries;
				for (size_t i = 1; i < outer; ++i) scope = t_scope::f_outer(scope);
				scope[index] = stack[0];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(LAMBDA)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto code = static_cast<t_object*>(*++pc);
				++pc;
				stack[0] = t_lambda::f_instantiate(a_context->v_scope, code);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(ADVANCED_LAMBDA)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto code = static_cast<t_object*>(*++pc);
				++pc;
				stack[0] = t_lambda::f_instantiate(a_context->v_scope, code, stack);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SELF)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0] = base[-1];
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CLASS)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				auto& top = stack[0];
				top = t_object::f_of(top.f_type());
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SUPER)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				auto& top = stack[0];
				if (top.f_type() != f_global()->f_type<t_type>()) f_throw(L"not class."sv);
				top = static_cast<t_object*>(static_cast<t_slot&>(f_as<t_type&>(top).v_super));
				if (!top) f_throw(L"no more super class."sv);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(NUL)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0] = nullptr;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BOOLEAN)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto value = reinterpret_cast<intptr_t>(*++pc) != 0;
				++pc;
				stack[0] = value;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(INTEGER)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto value = reinterpret_cast<intptr_t>(*++pc);
				++pc;
				stack[0] = value;
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
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				XEMMAI__CODE__FLOAT(v0, v1)
				++pc;
				stack[0] = v0;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(INSTANCE)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto& value = *static_cast<const t_pvalue*>(*++pc);
				++pc;
				stack[0] = value;
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(RETURN_N)
			a_context->f_return(nullptr);
			return -1;
		XEMMAI__CODE__CASE(RETURN_B)
			a_context->f_return(reinterpret_cast<intptr_t>(*++pc) != 0);
			return -1;
		XEMMAI__CODE__CASE(RETURN_I)
			a_context->f_return(reinterpret_cast<intptr_t>(*++pc));
			return -1;
		XEMMAI__CODE__CASE(RETURN_F)
			{
				XEMMAI__CODE__FLOAT(v0, v1)
				a_context->f_return(v0);
			}
			return -1;
		XEMMAI__CODE__CASE(RETURN_L)
			a_context->f_return(*static_cast<const t_pvalue*>(*++pc));
			return -1;
		XEMMAI__CODE__CASE(RETURN_V)
			a_context->f_return(reinterpret_cast<size_t>(*++pc));
			return -1;
		XEMMAI__CODE__CASE(RETURN_T)
			a_context->f_return();
			return -1;
		XEMMAI__CODE__CASE(CALL)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[0].f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CALL_WITH_EXPANSION)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				++pc;
				n = f_expand(pc, stack, n);
				stack[0].f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_CALL)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[1] = nullptr;
				base[index].f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_CALL0)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[1] = nullptr;
				a_context->v_scope[index].f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_CALL1)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[1] = nullptr;
				f_as<t_lambda&>(a_context->v_lambda).v_scope_entries[index].f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_CALL2)
			{
				auto stack = base + reinterpret_cast<size_t>(*++pc);
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				++pc;
				stack[1] = nullptr;
				t_scope::f_outer(f_as<t_lambda&>(a_context->v_lambda).v_scope_entries)[index].f_call(stack, n);
			}
			XEMMAI__CODE__BREAK
#define XEMMAI__CODE__FETCH()
#define XEMMAI__CODE__PREPARE()\
						auto x = stack[1];
#define XEMMAI__CODE__FETCH_L()\
				auto& a0 = *static_cast<const t_pvalue*>(*++pc);
#define XEMMAI__CODE__PREPARE_L()\
						auto& x = a0;
#define XEMMAI__CODE__FETCH_V()\
				auto& a0 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_V() XEMMAI__CODE__PREPARE_L()
#define XEMMAI__CODE__FETCH_T()\
				auto& a0 = stack[1];
#define XEMMAI__CODE__PREPARE_T() XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__PREPARE_LL()\
						auto& x = a0;\
						stack[2] = a1;
#define XEMMAI__CODE__PREPARE_TL()\
						auto x = stack[1];\
						stack[2] = a1;
#define XEMMAI__CODE__FETCH_LI()\
				auto& a0 = *static_cast<const t_pvalue*>(*++pc);\
				auto a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PREPARE_LI() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VI()\
				auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				auto a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PREPARE_VI() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TI()\
				auto& a0 = stack[1];\
				auto a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PREPARE_TI() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_LF()\
				auto& a0 = *static_cast<const t_pvalue*>(*++pc);\
				XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PREPARE_LF() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VF()\
				auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PREPARE_VF() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TF()\
				auto& a0 = stack[1];\
				XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PREPARE_TF() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_IL()\
				auto a0 = reinterpret_cast<intptr_t>(*++pc);\
				auto& a1 = *static_cast<const t_pvalue*>(*++pc);
#define XEMMAI__CODE__FETCH_FL()\
				XEMMAI__CODE__FLOAT(a0, v0)\
				auto& a1 = *static_cast<const t_pvalue*>(*++pc);
#define XEMMAI__CODE__FETCH_LL()\
				auto& a0 = *static_cast<const t_pvalue*>(*++pc);\
				auto& a1 = *static_cast<const t_pvalue*>(*++pc);
#define XEMMAI__CODE__FETCH_VL()\
				auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				auto& a1 = *static_cast<const t_pvalue*>(*++pc);
#define XEMMAI__CODE__PREPARE_VL() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TL()\
				auto& a0 = stack[1];\
				auto& a1 = *static_cast<const t_pvalue*>(*++pc);
#define XEMMAI__CODE__FETCH_IV()\
				auto a0 = reinterpret_cast<intptr_t>(*++pc);\
				auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__FETCH_FV()\
				XEMMAI__CODE__FLOAT(a0, v0)\
				auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__FETCH_LV()\
				auto& a0 = *static_cast<const t_pvalue*>(*++pc);\
				auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_LV() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VV()\
				auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TV()\
				auto& a0 = stack[1];\
				auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_TV() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_IT()\
				auto a0 = reinterpret_cast<intptr_t>(*++pc);\
				auto& a1 = stack[1];
#define XEMMAI__CODE__FETCH_FT()\
				XEMMAI__CODE__FLOAT(a0, v0)\
				auto& a1 = stack[1];
#define XEMMAI__CODE__FETCH_LT()\
				auto& a0 = *static_cast<const t_pvalue*>(*++pc);\
				auto& a1 = stack[1];
#define XEMMAI__CODE__PREPARE_LT()\
						auto& x = a0;\
						stack[2] = stack[1];
#define XEMMAI__CODE__FETCH_VT()\
				auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
				auto& a1 = stack[1];
#define XEMMAI__CODE__PREPARE_VT() XEMMAI__CODE__PREPARE_LT()
#define XEMMAI__CODE__FETCH_TT()\
				auto& a0 = stack[1];\
				auto& a1 = stack[2];
#define XEMMAI__CODE__PREPARE_TT() XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name, XEMMAI__CODE__OPERANDS))\
			{\
				auto stack = base + reinterpret_cast<size_t>(*++pc);\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
				++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_p, a_x)\
						stack[0] = a_x;
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
				{\
					XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
					auto p = static_cast<t_object*>(x);\
					auto n = p->f_type()->a_method(p, stack);\
					if (n != size_t(-1)) xemmai::f_loop(stack, n);\
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
				auto n = reinterpret_cast<size_t>(*++pc);
				a_context->f_tail(n);
				return n;
			}
		XEMMAI__CODE__CASE(CALL_WITH_EXPANSION_TAIL)
			{
				auto n = reinterpret_cast<size_t>(*++pc);
				n = f_expand(pc, base + f_as<t_lambda&>(a_context->v_lambda).v_privates, n);
				a_context->f_tail(n);
				return n;
			}
		XEMMAI__CODE__CASE(STACK_CALL_TAIL)
			{
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				auto stack = base + f_as<t_lambda&>(a_context->v_lambda).v_privates;
				stack[0] = base[index];
				stack[1] = nullptr;
				a_context->f_tail(n);
				return n;
			}
		XEMMAI__CODE__CASE(SCOPE_CALL0_TAIL)
			{
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				auto stack = base + f_as<t_lambda&>(a_context->v_lambda).v_privates;
				stack[0] = a_context->v_scope[index];
				stack[1] = nullptr;
				a_context->f_tail(n);
				return n;
			}
		XEMMAI__CODE__CASE(SCOPE_CALL1_TAIL)
			{
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				auto& lambda = f_as<t_lambda&>(a_context->v_lambda);
				auto stack = base + lambda.v_privates;
				stack[0] = lambda.v_scope_entries[index];
				stack[1] = nullptr;
				a_context->f_tail(n);
				return n;
			}
		XEMMAI__CODE__CASE(SCOPE_CALL2_TAIL)
			{
				auto index = reinterpret_cast<size_t>(*++pc);
				auto n = reinterpret_cast<size_t>(*++pc);
				auto& lambda = f_as<t_lambda&>(a_context->v_lambda);
				auto stack = base + lambda.v_privates;
				stack[0] = t_scope::f_outer(lambda.v_scope_entries)[index];
				stack[1] = nullptr;
				a_context->f_tail(n);
				return n;
			}
#undef XEMMAI__CODE__PREPARE_V
#define XEMMAI__CODE__PREPARE_V()\
						auto x = a0;
#undef XEMMAI__CODE__PREPARE_VL
#define XEMMAI__CODE__PREPARE_VL()\
						stack[2] = a1;\
						auto x = a0;
#undef XEMMAI__CODE__PREPARE_VI
#define XEMMAI__CODE__PREPARE_VI() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_VF
#define XEMMAI__CODE__PREPARE_VF() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_LV
#define XEMMAI__CODE__PREPARE_LV()\
						auto& x = a0;\
						stack[2] = a1;
#undef XEMMAI__CODE__PREPARE_VV
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_TV
#define XEMMAI__CODE__PREPARE_TV()\
						auto x = stack[1];\
						stack[2] = a1;
#undef XEMMAI__CODE__PREPARE_VT
#define XEMMAI__CODE__PREPARE_VT()\
						auto x = a0;\
						stack[2] = stack[1];
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name##_TAIL, XEMMAI__CODE__OPERANDS))\
			{\
				auto stack = base + f_as<t_lambda&>(a_context->v_lambda).v_privates;\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
				++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_p, a_x)\
				a_context->f_return(a_x);\
				return -1;
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
				{\
					XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
					return a_context->f_tail<&t_type::a_method>(x);\
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
			a_context->f_return(nullptr);
			return -1;
		XEMMAI__CODE__CASE(SAFE_POINT)
			++pc;
			f_engine()->f_debug_safe_point();
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BREAK_POINT)
			++pc;
			f_engine()->f_debug_break_point();
			XEMMAI__CODE__BREAK
#ifndef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
		}
	}
#endif
label__THROW_NOT_SUPPORTED:
	f_throw(L"not supported."sv);
}

void t_code::f_rethrow()
{
	try {
		std::rethrow_exception(std::current_exception());
	} catch (const t_rvalue&) {
		throw;
	} catch (std::exception& e) {
		throw t_rvalue(t_throwable::f_instantiate(portable::f_convert(e.what())));
	} catch (...) {
		throw t_rvalue(t_throwable::f_instantiate(L"<unknown>."sv));
	}
}

void t_code::f_try(t_context* a_context)
{
	auto& pc = a_context->v_pc;
	auto base = a_context->v_base;
	auto stack = base + reinterpret_cast<size_t>(*++pc);
	auto catch0 = static_cast<void**>(*++pc);
	auto finally0 = static_cast<void**>(*++pc);
	++pc;
	t_try try0;
	try {
		try {
			try0 = static_cast<t_try>(f_loop(a_context));
		} catch (...) {
			f_rethrow();
		}
	} catch (const t_rvalue& thrown) {
		stack[0] = nullptr;
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
					auto type = stack[0];
					if (thrown != f_engine()->v_fiber_exit && thrown.f_is(&f_as<t_type&>(type))) {
						auto index = reinterpret_cast<size_t>(*++pc);
						++pc;
						p.f_caught(thrown, a_context->v_lambda, caught);
						if ((index & ~(~0 >> 1)) != 0)
							a_context->v_scope[~index] = thrown;
						else
							base[index] = thrown;
					} else {
						pc = static_cast<void**>(*pc);
					}
				} catch (...) {
					f_rethrow();
				}
			} catch (const t_rvalue& thrown) {
				stack[0] = nullptr;
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

t_object* t_code::f_instantiate(t_object* a_module, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum)
{
	return f_new<t_code>(f_global(), a_module, a_shared, a_variadic, a_privates, a_shareds, a_arguments, a_minimum);
}

const t_at* t_code::f_at(void** a_address) const
{
	auto i = std::lower_bound(v_ats.begin(), v_ats.end(), static_cast<size_t>(a_address - v_instructions.data()));
	return i == v_ats.end() ? nullptr : &*i;
}

}
