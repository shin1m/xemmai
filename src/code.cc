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

size_t f_expand(void**& a_pc, t_pvalue* a_stack, size_t a_n)
{
	a_stack += 2;
	size_t n = 0;
	auto expand = [&](size_t m, auto get)
	{
		if (m > 0) {
			auto used = a_stack + m + a_n;
			if (used > f_stack()) f_stack__(used);
			std::move_backward(a_stack + 1, a_stack + 1 + a_n, a_stack + m + a_n);
			for (size_t i = 0; i < m; ++i) *a_stack++ = get(i);
			n += m;
		} else {
			std::move(a_stack + 1, a_stack + 1 + a_n, a_stack);
		}
	};
	auto list = [&](auto& xs)
	{
		expand(xs.f_size(), [&](size_t i)
		{
			return t_pvalue(xs[i]);
		});
	};
	while (true) {
		auto d = reinterpret_cast<size_t>(*++a_pc);
		n += d;
		a_n -= d;
		if (a_n <= 0) break;
		a_stack += d;
		auto x = *a_stack;
		--a_n;
		if (f_is<t_tuple>(x)) {
			list(x->f_as<t_tuple>());
		} else if (f_is<t_list>(x)) {
			list(x->f_as<t_list>());
		} else {
			static size_t index;
			auto size = x.f_invoke(f_global()->f_symbol_size(), index);
			f_check<size_t>(size, L"size");
			expand(f_as<size_t>(size), [&](size_t i)
			{
				return x.f_get_at(i);
			});
		}
	}
	return n;
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
			&&label__OBJECT_GET_INDIRECT,
			&&label__OBJECT_PUT,
			&&label__OBJECT_PUT_INDIRECT,
			&&label__OBJECT_HAS,
			&&label__OBJECT_HAS_INDIRECT,
			&&label__METHOD_GET,
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
			&&label__INTEGER,
			&&label__FLOAT,
			&&label__INSTANCE,
			&&label__RETURN_NUL,
			&&label__RETURN_INTEGER,
			&&label__RETURN_FLOAT,
			&&label__RETURN_INSTANCE,
			&&label__RETURN_V,
			&&label__RETURN_T,
#define XEMMAI__CODE__LABEL_UNARY(a_name)\
			&&label__##a_name##_L,\
			&&label__##a_name##_V,\
			&&label__##a_name##_T,
#define XEMMAI__CODE__LABEL_BINARY(a_name)\
			nullptr,\
			nullptr,\
			&&label__##a_name##_LI,\
			&&label__##a_name##_VI,\
			&&label__##a_name##_TI,\
			nullptr,\
			nullptr,\
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
#define XEMMAI__CODE__LABELS(a_tail)\
			&&label__CALL##a_tail,\
			&&label__CALL_WITH_EXPANSION##a_tail,\
			&&label__STACK_CALL##a_tail,\
			&&label__SCOPE_CALL0##a_tail,\
			&&label__SCOPE_CALL1##a_tail,\
			&&label__SCOPE_CALL2##a_tail,\
			&&label__GET_AT##a_tail,\
			&&label__SET_AT##a_tail,\
			XEMMAI__CODE__LABEL_UNARY(PLUS##a_tail)\
			XEMMAI__CODE__LABEL_UNARY(MINUS##a_tail)\
			XEMMAI__CODE__LABEL_UNARY(NOT##a_tail)\
			XEMMAI__CODE__LABEL_UNARY(COMPLEMENT##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(MULTIPLY##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(DIVIDE##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(MODULUS##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(ADD##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(SUBTRACT##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(LEFT_SHIFT##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(RIGHT_SHIFT##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(LESS##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(LESS_EQUAL##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(GREATER##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(GREATER_EQUAL##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(EQUALS##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(NOT_EQUALS##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(IDENTICAL##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(NOT_IDENTICAL##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(AND##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(XOR##a_tail)\
			XEMMAI__CODE__LABEL_BINARY(OR##a_tail)
			XEMMAI__CODE__LABELS()
			XEMMAI__CODE__LABELS(_TAIL)
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
	auto pc = a_context->v_pc;
	auto base = a_context->v_base;
	try {
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
	goto **pc;
#define XEMMAI__CODE__CASE(a_name) XEMMAI__MACRO__CONCATENATE(label__, a_name):\
		{
#define XEMMAI__CODE__BREAK goto **pc;\
		}
#else
	while (true) {
		switch (static_cast<t_instruction>(reinterpret_cast<intptr_t>(*pc))) {
#define XEMMAI__CODE__CASE(a_name) case XEMMAI__MACRO__CONCATENATE(c_instruction__, a_name):\
		{
#define XEMMAI__CODE__BREAK break;\
		}
#endif
#define XEMMAI__CODE__RETURN(a_n) return a_n;\
		}
#define XEMMAI__CODE__CLOSE }
		XEMMAI__CODE__CASE(JUMP)
			pc = static_cast<void**>(*++pc);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BRANCH)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			if (stack[0])
				++pc;
			else
				pc = static_cast<void**>(*pc);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(TRY)
			a_context->v_pc = pc;
			f_try(a_context);
			pc = a_context->v_pc;
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CATCH)
			a_context->v_pc = pc;
			XEMMAI__CODE__RETURN(c_try__CATCH)
		XEMMAI__CODE__CASE(FINALLY)
			a_context->v_pc = ++pc;
			XEMMAI__CODE__RETURN(static_cast<t_try>(reinterpret_cast<intptr_t>(*pc)))
		XEMMAI__CODE__CASE(YRT)
			a_context->v_pc = pc;
			XEMMAI__CODE__RETURN(0)
		XEMMAI__CODE__CASE(THROW)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			throw t_rvalue(stack[0]);
			XEMMAI__CODE__CLOSE
		XEMMAI__CODE__CASE(OBJECT_GET)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto key = static_cast<t_object*>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto& top = stack[0];
			top = top.f_get(key, index);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_GET_INDIRECT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto& top = stack[0];
			auto key = static_cast<t_object*>(stack[1]);
			top = top.f_get(key, index);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto key = static_cast<t_object*>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto& top = stack[0];
			auto& value = stack[1];
			top.f_put(key, index, value);
			top = value;
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_PUT_INDIRECT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto& top = stack[0];
			auto key = static_cast<t_object*>(stack[1]);
			auto& value = stack[2];
			top.f_put(key, index, value);
			top = value;
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_HAS)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto key = static_cast<t_object*>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto& top = stack[0];
			top = top.f_has(key, index);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(OBJECT_HAS_INDIRECT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto& top = stack[0];
			auto key = static_cast<t_object*>(stack[1]);
			top = top.f_has(key, index);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_GET)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto key = static_cast<t_object*>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			auto top = stack[0];
			top.f_bind(key, index, stack);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(METHOD_BIND)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			auto p = static_cast<t_object*>(stack[0]);
			if (reinterpret_cast<uintptr_t>(p) < c_tag__OBJECT) goto label__THROW_NOT_SUPPORTED;
			if (!p->f_type()->v_bindable) f_method_bind(stack);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(GLOBAL_GET)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto key = static_cast<t_object*>(*++pc);
			auto& index = reinterpret_cast<size_t&>(*++pc);
			++pc;
			stack[0] = f_engine()->f_module_global()->f_get(key, index);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_GET)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			stack[0] = base[index];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_PUT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			base[index] = stack[0];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET0)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			stack[0] = a_context->v_scope->f_as<t_scope>().f_entries()[index];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET1)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			stack[0] = a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_entries()[index];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET2)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			stack[0] = a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_outer()->f_as<t_scope>().f_entries()[index];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_GET)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto outer = reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			assert(outer >= 3);
			auto scope = a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_outer()->f_as<t_scope>().f_outer();
			for (size_t i = 3; i < outer; ++i) scope = scope->f_as<t_scope>().f_outer();
			stack[0] = scope->f_as<t_scope>().f_entries()[index];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_PUT0)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			a_context->v_scope->f_as<t_scope>().f_entries()[index] = stack[0];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_PUT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto outer = reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			++pc;
			assert(outer >= 1);
			t_object* scope = a_context->v_lambda->f_as<t_lambda>().v_scope;
			for (size_t i = 1; i < outer; ++i) scope = scope->f_as<t_scope>().f_outer();
			scope->f_as<t_scope>().f_entries()[index] = stack[0];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(LAMBDA)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto code = static_cast<t_object*>(*++pc);
			++pc;
			stack[0] = t_lambda::f_instantiate(a_context->v_scope, code);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(ADVANCED_LAMBDA)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto code = static_cast<t_object*>(*++pc);
			++pc;
			stack[0] = t_lambda::f_instantiate(a_context->v_scope, code, stack);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SELF)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			stack[0] = base[-1];
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CLASS)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			auto& top = stack[0];
			top = t_object::f_of(top.f_type());
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SUPER)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			auto& top = stack[0];
			if (top.f_type() != f_global()->f_type<t_type>()) f_throw(L"not class."sv);
			top = top->f_as<t_type>().v_super;
			if (!top) f_throw(L"no more super class."sv);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(NUL)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			++pc;
			stack[0] = nullptr;
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(INTEGER)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto value = reinterpret_cast<intptr_t>(*++pc);
			++pc;
			stack[0] = value;
			XEMMAI__CODE__BREAK
#define XEMMAI__CODE__FLOAT(a_x, a_y)\
			union\
			{\
				double a_x;\
				void* a_y[sizeof(double) / sizeof(void*)];\
			};\
			for (size_t i = 0; i < sizeof(double) / sizeof(void*); ++i) a_y[i] = *++pc;
		XEMMAI__CODE__CASE(FLOAT)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			XEMMAI__CODE__FLOAT(v0, v1)
			++pc;
			stack[0] = v0;
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(INSTANCE)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto value = static_cast<t_object*>(*++pc);
			++pc;
			stack[0] = value;
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(RETURN_NUL)
			a_context->f_return(nullptr);
			XEMMAI__CODE__RETURN(-1)
		XEMMAI__CODE__CASE(RETURN_INTEGER)
			a_context->f_return(reinterpret_cast<intptr_t>(*++pc));
			XEMMAI__CODE__RETURN(-1)
		XEMMAI__CODE__CASE(RETURN_FLOAT)
			XEMMAI__CODE__FLOAT(v0, v1)
			a_context->f_return(v0);
			XEMMAI__CODE__RETURN(-1)
		XEMMAI__CODE__CASE(RETURN_INSTANCE)
			a_context->f_return(static_cast<t_object*>(*++pc));
			XEMMAI__CODE__RETURN(-1)
		XEMMAI__CODE__CASE(RETURN_V)
			a_context->f_return(base[reinterpret_cast<size_t>(*++pc)]);
			XEMMAI__CODE__RETURN(-1)
		XEMMAI__CODE__CASE(RETURN_T)
			a_context->f_return(base[a_context->v_lambda->f_as<t_lambda>().v_privates]);
			XEMMAI__CODE__RETURN(-1)
		XEMMAI__CODE__CASE(CALL)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			a_context->v_pc = ++pc;
			stack[0].f_call(stack, n);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(CALL_WITH_EXPANSION)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			n = f_expand(pc, stack, n);
			a_context->v_pc = ++pc;
			stack[0].f_call(stack, n);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(STACK_CALL)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			a_context->v_pc = ++pc;
			stack[1] = nullptr;
			base[index].f_call(stack, n);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_CALL0)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			a_context->v_pc = ++pc;
			stack[1] = nullptr;
			a_context->v_scope->f_as<t_scope>().f_entries()[index].f_call(stack, n);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_CALL1)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			a_context->v_pc = ++pc;
			stack[1] = nullptr;
			a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_entries()[index].f_call(stack, n);
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(SCOPE_CALL2)
			auto stack = base + reinterpret_cast<size_t>(*++pc);
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			a_context->v_pc = ++pc;
			stack[1] = nullptr;
			a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_outer()->f_as<t_scope>().f_entries()[index].f_call(stack, n);
			XEMMAI__CODE__BREAK
#define XEMMAI__CODE__CASE_NAME(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name, XEMMAI__CODE__OPERANDS))
#define XEMMAI__CODE__STACK()\
			auto stack = base + reinterpret_cast<size_t>(*++pc);
#define XEMMAI__CODE__FETCH()\
			auto& a0 = stack[1];
#define XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__FETCH_L()\
			t_pvalue a0 = static_cast<t_object*>(*++pc);
#define XEMMAI__CODE__PREPARE_L()
#define XEMMAI__CODE__FETCH_V()\
			auto& a0 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_V()
#define XEMMAI__CODE__FETCH_T() XEMMAI__CODE__FETCH()
#define XEMMAI__CODE__PREPARE_T()
#define XEMMAI__CODE__PREPARE_A1()\
					stack[2] = a1;
#define XEMMAI__CODE__FETCH_LI()\
			t_pvalue a0 = static_cast<t_object*>(*++pc);\
			auto a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PREPARE_LI() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_VI()\
			auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
			auto a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PREPARE_VI() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_TI()\
			auto& a0 = stack[1];\
			auto a1 = reinterpret_cast<intptr_t>(*++pc);
#define XEMMAI__CODE__PREPARE_TI() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_LF()\
			t_pvalue a0 = static_cast<t_object*>(*++pc);\
			XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PREPARE_LF() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_VF()\
			auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
			XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PREPARE_VF() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_TF()\
			auto& a0 = stack[1];\
			XEMMAI__CODE__FLOAT(a1, v1)
#define XEMMAI__CODE__PREPARE_TF() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_IL()\
			auto a0 = reinterpret_cast<intptr_t>(*++pc);\
			t_pvalue a1 = static_cast<t_object*>(*++pc);
#define XEMMAI__CODE__FETCH_FL()\
			XEMMAI__CODE__FLOAT(a0, v0)\
			t_pvalue a1 = static_cast<t_object*>(*++pc);
#define XEMMAI__CODE__FETCH_LL()\
			t_pvalue a0 = static_cast<t_object*>(*++pc);\
			t_pvalue a1 = static_cast<t_object*>(*++pc);
#define XEMMAI__CODE__PREPARE_LL() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_VL()\
			auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
			t_pvalue a1 = static_cast<t_object*>(*++pc);
#define XEMMAI__CODE__PREPARE_VL() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_TL()\
			auto& a0 = stack[1];\
			t_pvalue a1 = static_cast<t_object*>(*++pc);
#define XEMMAI__CODE__PREPARE_TL() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_IV()\
			auto a0 = reinterpret_cast<intptr_t>(*++pc);\
			auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__FETCH_FV()\
			XEMMAI__CODE__FLOAT(a0, v0)\
			auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__FETCH_LV()\
			t_pvalue a0 = static_cast<t_object*>(*++pc);\
			auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_LV() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_VV()\
			auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
			auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_TV()\
			auto& a0 = stack[1];\
			auto& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PREPARE_TV() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_IT()\
			auto a0 = reinterpret_cast<intptr_t>(*++pc);\
			auto& a1 = stack[1];
#define XEMMAI__CODE__FETCH_FT()\
			XEMMAI__CODE__FLOAT(a0, v0)\
			auto& a1 = stack[1];
#define XEMMAI__CODE__FETCH_LT()\
			t_pvalue a0 = static_cast<t_object*>(*++pc);\
			auto& a1 = stack[1];
#define XEMMAI__CODE__PREPARE_LT() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_VT()\
			auto& a0 = base[reinterpret_cast<size_t>(*++pc)];\
			auto& a1 = stack[1];
#define XEMMAI__CODE__PREPARE_VT() XEMMAI__CODE__PREPARE_A1()
#define XEMMAI__CODE__FETCH_TT()\
			auto& a0 = stack[1];\
			auto& a1 = stack[2];
#define XEMMAI__CODE__PREPARE_TT()
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
		XEMMAI__CODE__CASE_NAME(a_name)\
			XEMMAI__CODE__STACK()\
			XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
			++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
					stack[0] = a_x;
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
			{\
				XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
				a_context->v_pc = pc;\
				auto n = a0->f_type()->a_method(a0, stack);\
				if (n != size_t(-1)) xemmai::f_loop(stack, n);\
			}
#define XEMMAI__CODE__CASE_END XEMMAI__CODE__BREAK
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
#define XEMMAI__CODE__BINARY_LX
#define XEMMAI__CODE__OPERANDS _LL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_LX
#define XEMMAI__CODE__BINARY_XL
#define XEMMAI__CODE__OPERANDS _VL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XL
#define XEMMAI__CODE__BINARY_XX
#define XEMMAI__CODE__OPERANDS _VV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XX
#define XEMMAI__CODE__BINARY_LI
#define XEMMAI__CODE__OPERANDS _LI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_LI
#define XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__OPERANDS _VI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__BINARY_IL
#define XEMMAI__CODE__OPERANDS _IL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_IL
#define XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__OPERANDS _IV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__BINARY_LF
#define XEMMAI__CODE__OPERANDS _LF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_LF
#define XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__OPERANDS _VF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__BINARY_FL
#define XEMMAI__CODE__OPERANDS _FL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_FL
#define XEMMAI__CODE__BINARY_FX
#define XEMMAI__CODE__OPERANDS _FV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_FX
#undef XEMMAI__CODE__CASE_NAME
#undef XEMMAI__CODE__STACK
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE(CALL_TAIL)
			auto n = reinterpret_cast<size_t>(*++pc);
			a_context->f_tail(base + a_context->v_lambda->f_as<t_lambda>().v_privates, n);
			XEMMAI__CODE__RETURN(n)
		XEMMAI__CODE__CASE(CALL_WITH_EXPANSION_TAIL)
			auto n = reinterpret_cast<size_t>(*++pc);
			auto stack = base + a_context->v_lambda->f_as<t_lambda>().v_privates;
			n = f_expand(pc, stack, n);
			a_context->f_tail(stack, n);
			XEMMAI__CODE__RETURN(n)
		XEMMAI__CODE__CASE(STACK_CALL_TAIL)
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			auto stack = base + a_context->v_lambda->f_as<t_lambda>().v_privates;
			stack[0] = base[index];
			stack[1] = nullptr;
			a_context->f_tail(stack, n);
			XEMMAI__CODE__RETURN(n)
		XEMMAI__CODE__CASE(SCOPE_CALL0_TAIL)
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			auto stack = base + a_context->v_lambda->f_as<t_lambda>().v_privates;
			stack[0] = a_context->v_scope->f_as<t_scope>().f_entries()[index];
			stack[1] = nullptr;
			a_context->f_tail(stack, n);
			XEMMAI__CODE__RETURN(n)
		XEMMAI__CODE__CASE(SCOPE_CALL1_TAIL)
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			auto stack = base + a_context->v_lambda->f_as<t_lambda>().v_privates;
			stack[0] = a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_entries()[index];
			stack[1] = nullptr;
			a_context->f_tail(stack, n);
			XEMMAI__CODE__RETURN(n)
		XEMMAI__CODE__CASE(SCOPE_CALL2_TAIL)
			auto index = reinterpret_cast<size_t>(*++pc);
			auto n = reinterpret_cast<size_t>(*++pc);
			auto stack = base + a_context->v_lambda->f_as<t_lambda>().v_privates;
			stack[0] = a_context->v_lambda->f_as<t_lambda>().v_scope->f_as<t_scope>().f_outer()->f_as<t_scope>().f_entries()[index];
			stack[1] = nullptr;
			a_context->f_tail(stack, n);
			XEMMAI__CODE__RETURN(n)
#define XEMMAI__CODE__CASE_NAME(a_name)\
		XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name##_TAIL, XEMMAI__CODE__OPERANDS))
#define XEMMAI__CODE__STACK()\
			auto stack = base + a_context->v_lambda->f_as<t_lambda>().v_privates;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
			a_context->f_return(a_x);\
			return -1;
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
			XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
			a_context->v_pc = pc;\
			return a_context->f_tail<&t_type::a_method>(a0);
#define XEMMAI__CODE__CASE_END XEMMAI__CODE__CLOSE
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
#define XEMMAI__CODE__BINARY_LX
#define XEMMAI__CODE__OPERANDS _LL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _LT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_LX
#define XEMMAI__CODE__BINARY_XL
#define XEMMAI__CODE__OPERANDS _VL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XL
#define XEMMAI__CODE__BINARY_XX
#define XEMMAI__CODE__OPERANDS _VV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _VT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XX
#define XEMMAI__CODE__BINARY_LI
#define XEMMAI__CODE__OPERANDS _LI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_LI
#define XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__OPERANDS _VI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TI
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XI
#define XEMMAI__CODE__BINARY_IL
#define XEMMAI__CODE__OPERANDS _IL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_IL
#define XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__OPERANDS _IV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _IT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_IX
#define XEMMAI__CODE__BINARY_LF
#define XEMMAI__CODE__OPERANDS _LF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_LF
#define XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__OPERANDS _VF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _TF
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_XF
#define XEMMAI__CODE__BINARY_FL
#define XEMMAI__CODE__OPERANDS _FL
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_FL
#define XEMMAI__CODE__BINARY_FX
#define XEMMAI__CODE__OPERANDS _FV
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#define XEMMAI__CODE__OPERANDS _FT
#include "code_operator.h"
#undef XEMMAI__CODE__OPERANDS
#undef XEMMAI__CODE__BINARY_FX
#undef XEMMAI__CODE__CASE_NAME
#undef XEMMAI__CODE__STACK
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
		XEMMAI__CODE__CASE(SAFE_POINT)
			a_context->v_pc = ++pc;
			f_engine()->f_debug_safe_point();
			XEMMAI__CODE__BREAK
		XEMMAI__CODE__CASE(BREAK_POINT)
			a_context->v_pc = ++pc;
			f_engine()->f_debug_break_point();
			XEMMAI__CODE__BREAK
#ifndef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
		}
	}
#endif
label__THROW_NOT_SUPPORTED:
	f_throw(L"not supported."sv);
	} catch (const std::pair<t_rvalue, void**>&) {
		throw;
	} catch (...) {
		f_rethrow(pc);
	}
}

void t_code::f_rethrow(void** a_pc)
{
	try {
		std::rethrow_exception(std::current_exception());
	} catch (const t_rvalue& thrown) {
		throw std::make_pair(thrown, a_pc);
	} catch (std::exception& e) {
		throw std::make_pair(t_rvalue(t_throwable::f_instantiate(portable::f_convert(e.what()))), a_pc);
	} catch (...) {
		throw std::make_pair(t_rvalue(t_throwable::f_instantiate(L"<unknown>."sv)), a_pc);
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
		try0 = static_cast<t_try>(f_loop(a_context));
	} catch (const std::pair<t_rvalue, void**>& pair) {
		auto& thrown = pair.first;
		pc = catch0;
		while (true) {
			try {
				try0 = static_cast<t_try>(f_loop(a_context));
				if (try0 != c_try__CATCH) break;
				try {
					++pc;
					auto type = stack[0];
					f_check<t_type>(type, L"type");
					if (thrown != f_engine()->v_fiber_exit && thrown.f_is(&type->f_as<t_type>())) {
						auto index = reinterpret_cast<size_t>(*++pc);
						++pc;
						a_context->f_backtrace(thrown, pair.second);
						if (index & ~(~size_t(0) >> 1))
							a_context->v_scope->f_as<t_scope>().f_entries()[~index] = thrown;
						else
							base[index] = thrown;
					} else {
						pc = static_cast<void**>(*pc);
					}
				} catch (...) {
					f_rethrow(pc);
				}
			} catch (const std::pair<t_rvalue, void**>&) {
				pc = finally0;
				f_loop(a_context);
				throw;
			}
		}
		if (try0 == c_try__THROW) {
			pc = finally0;
			f_loop(a_context);
			throw;
		}
	}
	pc = finally0;
	f_loop(a_context);
	auto break0 = static_cast<void**>(*++pc);
	auto continue0 = static_cast<void**>(*++pc);
	auto return0 = static_cast<void**>(*++pc);
	switch (try0) {
	case c_try__BREAK:
		pc = break0;
		break;
	case c_try__CONTINUE:
		pc = continue0;
		break;
	case c_try__RETURN:
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
