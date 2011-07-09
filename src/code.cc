#include <xemmai/code.h>

#include <algorithm>
#include <xemmai/class.h>
#include <xemmai/lambda.h>
#include <xemmai/throwable.h>
#include <xemmai/array.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

void f_allocate(t_slot* a_stack, size_t a_n)
{
	t_stack* stack = f_stack();
	t_slot* used = a_stack + a_n;
	if (used > stack->v_used) {
		stack->f_allocate(used);
		stack->v_used = used;
	}
}

size_t f_expand(t_slot* a_stack, size_t a_n)
{
	assert(a_n > 0);
	t_native_context context;
	a_stack += a_n;
	t_transfer x = a_stack[0].f_transfer();
	size_t n;
	if (f_is<t_tuple>(x)) {
		const t_tuple& tuple = f_as<const t_tuple&>(x);
		n = tuple.f_size();
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) a_stack[i].f_construct(tuple[i]);
	} else if (f_is<t_array>(x)) {
		const t_array& array = f_as<const t_array&>(x);
		n = array.f_size();
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) {
			t_with_lock_for_read lock(x);
			a_stack[i].f_construct(array[i]);
		}
	} else {
		t_transfer size = x.f_get(f_global()->f_symbol_size())();
		f_check<size_t>(size, L"size");
		n = f_as<size_t>(size);
		f_allocate(a_stack, n);
		for (size_t i = 0; i < n; ++i) a_stack[i].f_construct(x.f_get_at(t_value(static_cast<int>(i))));
	}
	return a_n - 1 + n;
}

}

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
const void** t_code::v_labels = t_code::f_labels();

void t_code::f_loop(const void*** a_labels)
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
			&&label__OBJECT_GET_INDIRECT,
			&&label__OBJECT_PUT,
			&&label__OBJECT_PUT_INDIRECT,
			&&label__OBJECT_HAS,
			&&label__OBJECT_HAS_INDIRECT,
			&&label__OBJECT_REMOVE,
			&&label__OBJECT_REMOVE_INDIRECT,
			&&label__GLOBAL_GET,
			&&label__STACK_GET,
			&&label__STACK_PUT,
			&&label__SCOPE_GET0,
			&&label__SCOPE_GET1,
			&&label__SCOPE_GET2,
			&&label__SCOPE_GET,
			&&label__SCOPE_GET0_WITHOUT_LOCK,
			&&label__SCOPE_GET1_WITHOUT_LOCK,
			&&label__SCOPE_GET2_WITHOUT_LOCK,
			&&label__SCOPE_GET_WITHOUT_LOCK,
			&&label__SCOPE_PUT,
			&&label__LAMBDA,
			&&label__ADVANCED_LAMBDA,
			&&label__SELF,
			&&label__CLASS,
			&&label__SUPER,
			&&label__BOOLEAN,
			&&label__INTEGER,
			&&label__FLOAT,
			&&label__INSTANCE,
			&&label__RETURN,
			&&label__CALL,
			&&label__CALL_WITH_EXPANSION,
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
			&&label__##a_name##_LL,\
			&&label__##a_name##_VL,\
			&&label__##a_name##_TL,\
			&&label__##a_name##_LV,\
			&&label__##a_name##_VV,\
			&&label__##a_name##_TV,\
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
			&&label__FIBER_EXIT,
			&&label__END
		};
		*a_labels = labels;
		return;
	}
#else
void t_code::f_loop()
{
#endif
	t_slot* base;
	void** pc;
	while (true) {
		try {
			base = f_context()->v_base;
			pc = f_context()->v_pc;
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__CODE__CASE(a_name) XEMMAI__MACRO__CONCATENATE(label__, a_name):
#define XEMMAI__CODE__BREAK goto **pc;
			XEMMAI__CODE__BREAK
#else
#define XEMMAI__CODE__CASE(a_name) case XEMMAI__MACRO__CONCATENATE(e_instruction__, a_name):
#define XEMMAI__CODE__BREAK break;
			while (true) {
				switch (static_cast<t_instruction>(reinterpret_cast<int>(*pc))) {
#endif
				XEMMAI__CODE__CASE(JUMP)
					pc = static_cast<void**>(*++pc);
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(BRANCH)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						if (f_as<bool>(stack[0]))
							++pc;
						else
							pc = static_cast<void**>(*pc);
						stack[0] = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(TRY)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						void** catch0 = static_cast<void**>(*++pc);
						void** finally0 = static_cast<void**>(*++pc);
						t_fiber::t_context* p = f_context();
						p->v_pc = ++pc;
						t_fiber::t_try::f_push(stack, p, catch0, finally0);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CATCH)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& value = stack[0];
						t_slot& type = stack[1];
						if (value.f_is(type)) {
							size_t index = reinterpret_cast<size_t>(*++pc);
							++pc;
							f_as<t_fiber&>(t_fiber::f_current()).f_caught(value);
							if ((index & ~(~0 >> 1)) != 0) {
								t_slot& scope = f_context()->v_scope;
								t_with_lock_for_write lock(scope);
								f_as<t_scope&>(scope)[~index] = value.f_transfer();
							} else {
								base[index] = value.f_transfer();
							}
						} else {
							pc = static_cast<void**>(*pc);
						}
						type = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(FINALLY)
					{
						t_fiber::t_try::t_state state = static_cast<t_fiber::t_try::t_state>(reinterpret_cast<int>(*++pc));
						t_fiber& p = f_as<t_fiber&>(t_fiber::f_current());
						p.v_try->v_state = state;
						pc = p.v_try->v_finally;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(YRT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						void** break0 = static_cast<void**>(*++pc);
						void** continue0 = static_cast<void**>(*++pc);
						void** return0 = static_cast<void**>(*++pc);
						t_fiber& p = f_as<t_fiber&>(t_fiber::f_current());
						t_fiber::t_try* q = p.v_try;
						switch (q->v_state) {
						case t_fiber::t_try::e_state__BREAK:
							pc = break0;
							break;
						case t_fiber::t_try::e_state__CONTINUE:
							pc = continue0;
							break;
						case t_fiber::t_try::e_state__RETURN:
							pc = return0;
							break;
						case t_fiber::t_try::e_state__THROW:
							pc = p.v_caught;
							throw t_scoped(stack[0].f_transfer());
						default:
							++pc;
						}
						t_fiber::t_try::f_pop();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(THROW)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						throw t_scoped(stack[0].f_transfer());
					}
				XEMMAI__CODE__CASE(CLEAR)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						stack[0] = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_GET)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack[0];
						top = top.f_get(key);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_GET_INDIRECT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& key = stack[1];
						top = top.f_get(key);
						key = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_PUT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& value = stack[1];
						top.f_put(key, value);
						top = value.f_transfer();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_PUT_INDIRECT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& key = stack[1];
						t_slot& value = stack[2];
						top.f_put(key, value);
						key = 0;
						top = value.f_transfer();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_HAS)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack[0];
						top = t_value(top.f_has(key));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_HAS_INDIRECT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& key = stack[1];
						top = t_value(top.f_has(key));
						key = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_REMOVE)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack[0];
						top = top.f_remove(key);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_REMOVE_INDIRECT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& key = stack[1];
						top = top.f_remove(key);
						key = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(GLOBAL_GET)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						stack[0].f_construct(f_engine()->f_module_global()->f_get(key));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(STACK_GET)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						stack[0].f_construct(base[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(STACK_PUT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						base[index] = stack[0];
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET0)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope = f_context()->v_scope;
						t_with_lock_for_read lock(scope);
						stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET1)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope = f_context()->v_outer;
						t_with_lock_for_read lock(scope);
						stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET2)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope = f_as<const t_scope&>(f_context()->v_outer).v_outer;
						t_with_lock_for_read lock(scope);
						stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						assert(outer >= 3);
						t_object* scope = f_as<const t_scope&>(f_as<const t_scope&>(f_context()->v_outer).v_outer).v_outer;
						for (size_t i = 3; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
						t_with_lock_for_read lock(scope);
						stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET0_WITHOUT_LOCK)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						stack[0].f_construct(f_as<const t_scope&>(f_context()->v_scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET1_WITHOUT_LOCK)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						stack[0].f_construct(f_as<const t_scope&>(f_context()->v_outer)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET2_WITHOUT_LOCK)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						stack[0].f_construct(f_as<const t_scope&>(f_as<const t_scope&>(f_context()->v_outer).v_outer)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET_WITHOUT_LOCK)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						assert(outer >= 3);
						t_object* scope = f_as<const t_scope&>(f_as<const t_scope&>(f_context()->v_outer).v_outer).v_outer;
						for (size_t i = 3; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
						stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_PUT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope;
						if (outer > 0) {
							scope = f_context()->v_outer;
							for (size_t i = 1; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
						} else {
							scope = f_context()->v_scope;
						}
						t_with_lock_for_write lock(scope);
						f_as<t_scope&>(scope)[index] = stack[0];
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(LAMBDA)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* code = static_cast<t_object*>(*++pc);
						++pc;
						stack[0].f_construct(t_lambda::f_instantiate(f_context()->v_scope, code));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(ADVANCED_LAMBDA)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_object* code = static_cast<t_object*>(*++pc);
						++pc;
						stack[0].f_construct(t_advanced_lambda::f_instantiate(f_context()->v_scope, code, stack));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SELF)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						++pc;
						if (outer > 0) {
							t_object* scope = f_context()->v_outer;
							for (size_t i = 1; i < outer; ++i) scope = f_as<const t_scope&>(scope).v_outer;
							stack[0].f_construct(f_as<const t_scope&>(scope)[0]);
						} else {
							stack[0].f_construct(f_context()->v_self);
						}
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CLASS)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						top = top.f_type();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SUPER)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						if (top.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"not class.");
						top = f_as<t_type&>(top).v_super;
						if (!top) t_throwable::f_throw(L"no more super class.");
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(BOOLEAN)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						bool value = reinterpret_cast<int>(*++pc) != 0;
						++pc;
						stack[0].f_construct(value);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(INTEGER)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						int value = reinterpret_cast<int>(*++pc);
						++pc;
						stack[0].f_construct(value);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(FLOAT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						union
						{
							double v0;
							void* v1[sizeof(double) / sizeof(void*)];
						};
						for (size_t i = 0; i < sizeof(double) / sizeof(void*); ++i) v1[i] = *++pc;
						++pc;
						stack[0].f_construct(v0);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(INSTANCE)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						t_slot& value = *static_cast<t_slot*>(*++pc);
						++pc;
						stack[0].f_construct(value);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(RETURN)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						base[-1].f_construct(stack[0].f_transfer());
						t_fiber::t_context::f_pop();
						t_fiber::t_context* p = f_context();
						if (p->v_native > 0) return;
						base = p->v_base;
						pc = p->v_pc;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CALL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t n = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_transfer x = stack[0].f_transfer();
						t_fiber::t_context* p0 = f_context();
						p0->v_pc = pc;
						x.f_call(t_value(), stack, n);
						t_fiber::t_context* p1 = f_context();
						if (p1 != p0) {
							if (p1->v_native > 0) return;
							base = p1->v_base;
							pc = p1->v_pc;
						}
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CALL_WITH_EXPANSION)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t n = reinterpret_cast<size_t>(*++pc);
						++pc;
						n = f_expand(stack, n);
						t_transfer x = stack[0].f_transfer();
						t_fiber::t_context* p0 = f_context();
						p0->v_pc = pc;
						x.f_call(t_value(), stack, n);
						t_fiber::t_context* p1 = f_context();
						if (p1 != p0) {
							if (p1->v_native > 0) return;
							base = p1->v_base;
							pc = p1->v_pc;
						}
					}
					XEMMAI__CODE__BREAK
#define XEMMAI__CODE__FETCH()
#define XEMMAI__CODE__PRIMITIVE(a_x)\
								stack[0].f_construct(a_x);
#define XEMMAI__CODE__PREPARE()\
								t_transfer x = stack[0].f_transfer();
#define XEMMAI__CODE__FETCH_L()\
						t_slot& a0 = *static_cast<t_slot*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_L(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_L()\
								t_slot& x = a0;
#define XEMMAI__CODE__FETCH_V()\
						t_slot& a0 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_V(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_V() XEMMAI__CODE__PREPARE_L()
#define XEMMAI__CODE__FETCH_T()\
						t_slot& a0 = stack[0];
#define XEMMAI__CODE__PRIMITIVE_T(a_x)\
								stack[0] = t_value(a_x);
#define XEMMAI__CODE__PREPARE_T() XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__FETCH_LL()\
						t_slot& a0 = *static_cast<t_slot*>(*++pc);\
						t_slot& a1 = *static_cast<t_slot*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_LL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_LL()\
								t_slot& x = a0;\
								stack[1].f_construct(a1);
#define XEMMAI__CODE__FETCH_VL()\
						t_slot& a0 = base[reinterpret_cast<size_t>(*++pc)];\
						t_slot& a1 = *static_cast<t_slot*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_VL(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_VL() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TL()\
						t_slot& a0 = stack[0];\
						t_slot& a1 = *static_cast<t_slot*>(*++pc);
#define XEMMAI__CODE__PRIMITIVE_TL(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PREPARE_TL()\
								t_transfer x = stack[0].f_transfer();\
								stack[1].f_construct(a1);
#define XEMMAI__CODE__FETCH_LV()\
						t_slot& a0 = *static_cast<t_slot*>(*++pc);\
						t_slot& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_LV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_LV() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_VV()\
						t_slot& a0 = base[reinterpret_cast<size_t>(*++pc)];\
						t_slot& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_VV(a_x) XEMMAI__CODE__PRIMITIVE(a_x)
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_LL()
#define XEMMAI__CODE__FETCH_TV()\
						t_slot& a0 = stack[0];\
						t_slot& a1 = base[reinterpret_cast<size_t>(*++pc)];
#define XEMMAI__CODE__PRIMITIVE_TV(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PREPARE_TV() XEMMAI__CODE__PREPARE_TL()
#define XEMMAI__CODE__FETCH_LT()\
						t_slot& a0 = *static_cast<t_slot*>(*++pc);\
						t_slot& a1 = stack[0];
#define XEMMAI__CODE__PRIMITIVE_LT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PREPARE_LT()\
								t_slot& x = a0;\
								stack[1].f_construct(stack[0].f_transfer());
#define XEMMAI__CODE__FETCH_VT()\
						t_slot& a0 = base[reinterpret_cast<size_t>(*++pc)];\
						t_slot& a1 = stack[0];
#define XEMMAI__CODE__PRIMITIVE_VT(a_x) XEMMAI__CODE__PRIMITIVE_T(a_x)
#define XEMMAI__CODE__PREPARE_VT() XEMMAI__CODE__PREPARE_LT()
#define XEMMAI__CODE__FETCH_TT()\
						t_slot& a0 = stack[0];\
						t_slot& a1 = stack[1];
#define XEMMAI__CODE__PRIMITIVE_TT(a_x)\
								stack[0] = t_value(a_x);\
								stack[1] = 0;
#define XEMMAI__CODE__PREPARE_TT() XEMMAI__CODE__PREPARE()
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
				XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name, XEMMAI__CODE__OPERANDS))\
					{\
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);\
						XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
						++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
						XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PRIMITIVE, XEMMAI__CODE__OPERANDS)(a_x)
#define XEMMAI__CODE__OBJECT_CALL(a_method, a_n)\
						{\
							XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
							t_fiber::t_context* p0 = f_context();\
							f_as<t_type&>(x.v_p->f_type()).a_method(x.v_p, stack);\
							t_fiber::t_context* p1 = f_context();\
							if (p1 != p0) {\
								p0->v_pc = pc;\
								base = p1->v_base;\
								pc = p1->v_pc;\
							}\
						}
#define XEMMAI__CODE__CASE_END\
					}\
					XEMMAI__CODE__BREAK
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
#undef XEMMAI__CODE__CASE_BEGIN
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE(CALL_TAIL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t n = reinterpret_cast<size_t>(*++pc);
						t_transfer x = stack[0].f_transfer();
						t_fiber::t_context::f_pop(stack, n);
						x.f_call(t_value(), base - 1, n);
						t_fiber::t_context* p = f_context();
						if (p->v_native > 0) return;
						base = p->v_base;
						pc = p->v_pc;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CALL_WITH_EXPANSION_TAIL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t n = reinterpret_cast<size_t>(*++pc);
						n = f_expand(stack, n);
						t_transfer x = stack[0].f_transfer();
						t_fiber::t_context::f_pop(stack, n);
						x.f_call(t_value(), base - 1, n);
						t_fiber::t_context* p = f_context();
						if (p->v_native > 0) return;
						base = p->v_base;
						pc = p->v_pc;
					}
					XEMMAI__CODE__BREAK
#undef XEMMAI__CODE__PRIMITIVE
#define XEMMAI__CODE__PRIMITIVE(a_x)
#undef XEMMAI__CODE__PREPARE_V
#define XEMMAI__CODE__PREPARE_V()\
								t_transfer x = a0.f_transfer();
#undef XEMMAI__CODE__PRIMITIVE_T
#define XEMMAI__CODE__PRIMITIVE_T(a_x)\
								stack[0] = 0;
#undef XEMMAI__CODE__PREPARE_VL
#define XEMMAI__CODE__PREPARE_VL()\
								t_transfer x = a0.f_transfer();\
								stack[1].f_construct(a1);
#undef XEMMAI__CODE__PREPARE_VV
#define XEMMAI__CODE__PREPARE_VV() XEMMAI__CODE__PREPARE_VL()
#undef XEMMAI__CODE__PREPARE_VT
#define XEMMAI__CODE__PREPARE_VT()\
								t_transfer x = a0.f_transfer();\
								stack[1].f_construct(stack[0].f_transfer());
#undef XEMMAI__CODE__PRIMITIVE_TT
#define XEMMAI__CODE__PRIMITIVE_TT(a_x)\
								stack[0] = 0;\
								stack[1] = 0;
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
				XEMMAI__CODE__CASE(XEMMAI__MACRO__CONCATENATE(a_name##_TAIL, XEMMAI__CODE__OPERANDS))\
					{\
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);\
						XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__FETCH, XEMMAI__CODE__OPERANDS)()\
						++pc;
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
						base[-1].f_construct(a_x);\
						XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PRIMITIVE, XEMMAI__CODE__OPERANDS)(a_x)\
						t_fiber::t_context::f_pop();\
						if (f_context()->v_native > 0) return;
#define XEMMAI__CODE__OBJECT_CALL(a_method, a_n)\
						{\
							XEMMAI__MACRO__CONCATENATE(XEMMAI__CODE__PREPARE, XEMMAI__CODE__OPERANDS)()\
							t_fiber::t_context::f_pop(stack, a_n);\
							t_fiber::t_context* p = f_context();\
							f_as<t_type&>(x.v_p->f_type()).a_method(x.v_p, base - 1);\
							if (f_context() == p && p->v_native > 0) return;\
						}
#define XEMMAI__CODE__CASE_END\
						base = f_context()->v_base;\
						pc = f_context()->v_pc;\
					}\
					XEMMAI__CODE__BREAK
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
#undef XEMMAI__CODE__CASE_BEGIN
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE(FIBER_EXIT)
					{
						t_transfer x = base[0].f_transfer();
						f_context()->v_pc = pc;
						t_fiber& p = f_as<t_fiber&>(t_fiber::v_current);
						t_thread& thread = f_as<t_thread&>(t_thread::v_current);
						t_fiber& q = f_as<t_fiber&>(thread.v_fiber);
						t_fiber::t_try::t_state state = p.v_try->v_state;
						t_fiber::t_try::f_pop();
						if (state == t_fiber::t_try::e_state__THROW) {
							t_fiber::t_context::f_finalize(q.v_backtrace);
							t_fiber::t_context::v_instance->v_next = p.v_backtrace;
							p.v_backtrace = 0;
							q.v_backtrace = t_fiber::t_context::v_instance;
							p.v_context = 0;
							p.v_stack.f_clear(p.v_stack.f_head());
							p.v_stack.v_used = p.v_stack.f_head();
						} else {
							p.v_stack.v_used = p.v_stack.f_head();
							t_fiber::t_context::f_terminate();
						}
						q.v_active = true;
						thread.v_active = thread.v_fiber;
						t_fiber::v_current = thread.v_fiber;
						t_stack::v_instance = &q.v_stack;
						t_fiber::t_context::v_instance = q.v_context;
						p.v_active = false;
						if (state == t_fiber::t_try::e_state__THROW) {
							pc = f_context()->v_pc;
							throw t_scoped(x);
						}
						q.v_return->f_construct(x);
						if (f_context()->v_native > 0) return;
						base = f_context()->v_base;
						pc = f_context()->v_pc;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(END)
					f_context()->v_pc = pc;
					return;
#ifndef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
				}
			}
#endif
		} catch (const t_scoped& thrown) {
			f_context()->v_pc = pc;
			t_fiber::f_throw(thrown);
		} catch (...) {
			f_context()->v_pc = pc;
			t_fiber::f_throw(t_throwable::f_instantiate(L"<unknown>."));
		}
	}
}

t_transfer t_code::f_instantiate(const std::wstring& a_path, bool a_shared, bool a_variadic, size_t a_privates, size_t a_shareds, size_t a_arguments, size_t a_minimum)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_code>());
	object.f_pointer__(new t_code(a_path, a_shared, a_variadic, a_privates, a_shareds, a_arguments, a_minimum));
	return object;
}

void t_code::f_scan(t_scan a_scan)
{
	for (t_pointers<t_slot>::t_iterator i = v_objects.f_begin(); i != v_objects.f_end(); ++i) a_scan(**i);
}

const t_at* t_code::f_at(void** a_address) const
{
	std::vector<t_address_at>::const_iterator i = std::lower_bound(v_ats.begin(), v_ats.end(), t_address_at(a_address - &v_instructions[0], t_at(0, 0, 0)));
	return i == v_ats.end() ? 0 : &*i;
}

t_type* t_type_of<t_code>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_code>::f_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_code&>(a_this).f_scan(a_scan);
}

void t_type_of<t_code>::f_finalize(t_object* a_this)
{
	delete &f_as<t_code&>(a_this);
}

void t_type_of<t_code>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
