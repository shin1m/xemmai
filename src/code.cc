#include <xemmai/code.h>

#include <algorithm>
#include <xemmai/engine.h>
#include <xemmai/class.h>
#include <xemmai/lambda.h>
#include <xemmai/throwable.h>
#include <xemmai/array.h>
#include <xemmai/convert.h>

namespace xemmai
{

t_transfer t_code::f_instantiate(const std::wstring& a_path, size_t a_arguments)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_code>());
	object.f_pointer__(new t_code(a_path, a_arguments));
	return object;
}

void t_code::f_generate(void** a_p)
{
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
	const void** labels;
	f_loop(&labels);
#endif
	while (true) {
		int i = reinterpret_cast<int>(*a_p);
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__CODE__REPLACE(a_name) *a_p = const_cast<void*>(labels[e_instruction__##a_name]);
		*a_p = const_cast<void*>(labels[i]);
#else
#define XEMMAI__CODE__REPLACE(a_name) *a_p = reinterpret_cast<void*>(e_instruction__##a_name);
#endif
		switch (static_cast<t_instruction>(i)) {
		case e_instruction__JUMP:
		case e_instruction__BRANCH:
			a_p += 2;
			break;
		case e_instruction__TRY:
			a_p += 3;
			break;
		case e_instruction__CATCH:
			a_p += 2;
			*a_p = reinterpret_cast<void*>(*static_cast<int*>(*a_p) & e_access__INDEX);
			++a_p;
			break;
		case e_instruction__FINALLY:
			a_p += 2;
			break;
		case e_instruction__YRT:
			a_p += 4;
			break;
		case e_instruction__THROW:
		case e_instruction__POP:
			++a_p;
			break;
		case e_instruction__OBJECT_GET:
			a_p += 2;
			break;
		case e_instruction__OBJECT_GET_INDIRECT:
			++a_p;
			break;
		case e_instruction__OBJECT_PUT:
			a_p += 2;
			break;
		case e_instruction__OBJECT_PUT_INDIRECT:
			++a_p;
			break;
		case e_instruction__OBJECT_HAS:
			a_p += 2;
			break;
		case e_instruction__OBJECT_HAS_INDIRECT:
			++a_p;
			break;
		case e_instruction__OBJECT_REMOVE:
			a_p += 2;
			break;
		case e_instruction__OBJECT_REMOVE_INDIRECT:
			++a_p;
			break;
		case e_instruction__GLOBAL_GET:
			a_p += 2;
			break;
		case e_instruction__SCOPE_GET:
			{
				int index = *static_cast<int*>(a_p[2]);
				if ((index & e_access__VARIES) == 0) XEMMAI__CODE__REPLACE(SCOPE_GET_WITHOUT_LOCK)
				a_p += 2;
				*a_p = reinterpret_cast<void*>(index & e_access__INDEX);
				++a_p;
			}
			break;
		case e_instruction__SCOPE_GET0:
			{
				int index = *static_cast<int*>(a_p[1]);
				if ((index & (e_access__SHARED | e_access__VARIES)) != (e_access__SHARED | e_access__VARIES)) XEMMAI__CODE__REPLACE(SCOPE_GET0_WITHOUT_LOCK)
				*++a_p = reinterpret_cast<void*>(index & e_access__INDEX);
				++a_p;
			}
			break;
		case e_instruction__SCOPE_PUT:
			a_p += 2;
			*a_p = reinterpret_cast<void*>(*static_cast<int*>(*a_p) & e_access__INDEX);
			++a_p;
			break;
		case e_instruction__SCOPE_PUT0:
			{
				int index = *static_cast<int*>(a_p[1]);
				if ((index & e_access__SHARED) == 0) XEMMAI__CODE__REPLACE(SCOPE_PUT0_WITHOUT_LOCK)
				*++a_p = reinterpret_cast<void*>(index & e_access__INDEX);
				++a_p;
			}
			break;
		case e_instruction__LAMBDA:
			f_as<t_code&>(static_cast<t_object*>(*++a_p)).f_generate();
			++a_p;
			break;
		case e_instruction__SELF:
			a_p += 2;
			break;
		case e_instruction__CLASS:
		case e_instruction__SUPER:
		case e_instruction__NULL:
			++a_p;
			break;
		case e_instruction__BOOLEAN:
		case e_instruction__INTEGER:
			a_p += 2;
			break;
		case e_instruction__FLOAT:
			a_p += 1 + sizeof(double) / sizeof(void*);
			break;
		case e_instruction__INSTANCE:
			a_p += 2;
			break;
		case e_instruction__IDENTICAL:
		case e_instruction__NOT_IDENTICAL:
			++a_p;
			break;
		case e_instruction__RETURN:
			return;
		case e_instruction__CALL:
			a_p += 2;
			break;
		case e_instruction__GET_AT:
		case e_instruction__SET_AT:
		case e_instruction__PLUS:
		case e_instruction__MINUS:
		case e_instruction__NOT:
		case e_instruction__COMPLEMENT:
		case e_instruction__MULTIPLY:
		case e_instruction__DIVIDE:
		case e_instruction__MODULUS:
		case e_instruction__ADD:
		case e_instruction__SUBTRACT:
		case e_instruction__LEFT_SHIFT:
		case e_instruction__RIGHT_SHIFT:
		case e_instruction__LESS:
		case e_instruction__LESS_EQUAL:
		case e_instruction__GREATER:
		case e_instruction__GREATER_EQUAL:
		case e_instruction__EQUALS:
		case e_instruction__NOT_EQUALS:
		case e_instruction__AND:
		case e_instruction__XOR:
		case e_instruction__OR:
		case e_instruction__SEND:
			++a_p;
			break;
		case e_instruction__CALL_TAIL:
			a_p += 2;
			break;
		case e_instruction__GET_AT_TAIL:
		case e_instruction__SET_AT_TAIL:
		case e_instruction__PLUS_TAIL:
		case e_instruction__MINUS_TAIL:
		case e_instruction__NOT_TAIL:
		case e_instruction__COMPLEMENT_TAIL:
		case e_instruction__MULTIPLY_TAIL:
		case e_instruction__DIVIDE_TAIL:
		case e_instruction__MODULUS_TAIL:
		case e_instruction__ADD_TAIL:
		case e_instruction__SUBTRACT_TAIL:
		case e_instruction__LEFT_SHIFT_TAIL:
		case e_instruction__RIGHT_SHIFT_TAIL:
		case e_instruction__LESS_TAIL:
		case e_instruction__LESS_EQUAL_TAIL:
		case e_instruction__GREATER_TAIL:
		case e_instruction__GREATER_EQUAL_TAIL:
		case e_instruction__EQUALS_TAIL:
		case e_instruction__NOT_EQUALS_TAIL:
		case e_instruction__AND_TAIL:
		case e_instruction__XOR_TAIL:
		case e_instruction__OR_TAIL:
		case e_instruction__SEND_TAIL:
			++a_p;
			break;
		case e_instruction__FIBER_EXIT:
		case e_instruction__END:
			return;
		}
	}
}

namespace
{

struct t_finalize_fixed_scope
{
	t_scope* v_p;

	t_finalize_fixed_scope(t_scope* a_p) : v_p(a_p)
	{
	}
	~t_finalize_fixed_scope()
	{
		t_fixed_scope::f_finalize(v_p);
	}
};

}

#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
t_transfer t_code::f_loop(const void*** a_labels)
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
			&&label__POP,
			&&label__OBJECT_GET,
			&&label__OBJECT_GET_INDIRECT,
			&&label__OBJECT_PUT,
			&&label__OBJECT_PUT_INDIRECT,
			&&label__OBJECT_HAS,
			&&label__OBJECT_HAS_INDIRECT,
			&&label__OBJECT_REMOVE,
			&&label__OBJECT_REMOVE_INDIRECT,
			&&label__GLOBAL_GET,
			&&label__SCOPE_GET,
			&&label__SCOPE_GET_WITHOUT_LOCK,
			&&label__SCOPE_GET0,
			&&label__SCOPE_GET0_WITHOUT_LOCK,
			&&label__SCOPE_PUT,
			&&label__SCOPE_PUT0,
			&&label__SCOPE_PUT0_WITHOUT_LOCK,
			&&label__LAMBDA,
			&&label__SELF,
			&&label__CLASS,
			&&label__SUPER,
			&&label__NULL,
			&&label__BOOLEAN,
			&&label__INTEGER,
			&&label__FLOAT,
			&&label__INSTANCE,
			&&label__IDENTICAL,
			&&label__NOT_IDENTICAL,
			&&label__RETURN,
			&&label__CALL,
			&&label__GET_AT,
			&&label__SET_AT,
			&&label__PLUS,
			&&label__MINUS,
			&&label__NOT,
			&&label__COMPLEMENT,
			&&label__MULTIPLY,
			&&label__DIVIDE,
			&&label__MODULUS,
			&&label__ADD,
			&&label__SUBTRACT,
			&&label__LEFT_SHIFT,
			&&label__RIGHT_SHIFT,
			&&label__LESS,
			&&label__LESS_EQUAL,
			&&label__GREATER,
			&&label__GREATER_EQUAL,
			&&label__EQUALS,
			&&label__NOT_EQUALS,
			&&label__AND,
			&&label__XOR,
			&&label__OR,
			&&label__SEND,
			&&label__CALL_TAIL,
			&&label__GET_AT_TAIL,
			&&label__SET_AT_TAIL,
			&&label__PLUS_TAIL,
			&&label__MINUS_TAIL,
			&&label__NOT_TAIL,
			&&label__COMPLEMENT_TAIL,
			&&label__MULTIPLY_TAIL,
			&&label__DIVIDE_TAIL,
			&&label__MODULUS_TAIL,
			&&label__ADD_TAIL,
			&&label__SUBTRACT_TAIL,
			&&label__LEFT_SHIFT_TAIL,
			&&label__RIGHT_SHIFT_TAIL,
			&&label__LESS_TAIL,
			&&label__LESS_EQUAL_TAIL,
			&&label__GREATER_TAIL,
			&&label__GREATER_EQUAL_TAIL,
			&&label__EQUALS_TAIL,
			&&label__NOT_EQUALS_TAIL,
			&&label__AND_TAIL,
			&&label__XOR_TAIL,
			&&label__OR_TAIL,
			&&label__SEND_TAIL,
			&&label__FIBER_EXIT,
			&&label__END,
		};
		*a_labels = labels;
		return 0;
	}
#else
t_transfer t_code::f_loop()
{
#endif
	void** pc;
	t_scope* stack;
	while (true) {
		try {
			pc = f_context()->v_pc;
			stack = f_context()->v_stack;
#ifdef XEMMAI__PORTABLE__SUPPORTS_COMPUTED_GOTO
#define XEMMAI__CODE__CASE(a_name) label__##a_name:
#define XEMMAI__CODE__BREAK goto **pc;
			XEMMAI__CODE__BREAK
#else
#define XEMMAI__CODE__CASE(a_name) case e_instruction__##a_name:
#define XEMMAI__CODE__BREAK break;
			while (true) {
				switch (static_cast<t_instruction>(reinterpret_cast<int>(*pc))) {
#endif
				XEMMAI__CODE__CASE(JUMP)
					pc = reinterpret_cast<void**>(*++pc);
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(BRANCH)
					++pc;
					if (f_as<bool>(stack->f_pop()))
						++pc;
					else
						pc = reinterpret_cast<void**>(*pc);
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(TRY)
					{
						void** catch0 = reinterpret_cast<void**>(*++pc);
						void** finally0 = reinterpret_cast<void**>(*++pc);
						f_context()->v_pc = ++pc;
						t_fiber::t_try::f_push(stack->v_top, f_context(), catch0, finally0);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CATCH)
					{
						++pc;
						t_transfer type = stack->f_pop();
						if (stack->f_top().f_is(type.f_object())) {
							size_t index = reinterpret_cast<size_t>(*++pc);
							++pc;
							t_transfer value = stack->f_pop();
							f_as<t_fiber&>(t_fiber::f_current()).f_caught(value);
							if (f_context()->v_simple) {
								(*stack)[index] = value;
							} else {
								t_with_lock_for_write lock(f_context()->v_scope);
								(*stack)[index] = value;
							}
						} else {
							pc = reinterpret_cast<void**>(*pc);
						}
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
						void** break0 = reinterpret_cast<void**>(*++pc);
						void** continue0 = reinterpret_cast<void**>(*++pc);
						void** return0 = reinterpret_cast<void**>(*++pc);
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
							throw t_scoped(stack->f_pop());
						default:
							++pc;
						}
						t_fiber::t_try::f_pop();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(THROW)
					++pc;
					throw t_scoped(stack->f_pop());
				XEMMAI__CODE__CASE(POP)
					++pc;
					stack->f_pop();
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_GET)
					{
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack->f_top();
						top = top.f_get(key);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_GET_INDIRECT)
					{
						++pc;
						t_transfer key = stack->f_pop();
						t_slot& top = stack->f_top();
						top = top.f_get(key.f_object());
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_PUT)
					{
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_transfer value = stack->f_pop();
						t_slot& top = stack->f_top();
						top.f_put(key, t_value(value));
						top = value;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_PUT_INDIRECT)
					{
						++pc;
						t_transfer value = stack->f_pop();
						t_transfer key = stack->f_pop();
						t_slot& top = stack->f_top();
						top.f_put(key.f_object(), t_value(value));
						top = value;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_HAS)
					{
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack->f_top();
						top = f_global()->f_as(top.f_has(key));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_HAS_INDIRECT)
					{
						++pc;
						t_transfer key = stack->f_pop();
						t_slot& top = stack->f_top();
						top = f_global()->f_as(top.f_has(key.f_object()));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_REMOVE)
					{
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						t_slot& top = stack->f_top();
						top = top.f_remove(key);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_REMOVE_INDIRECT)
					{
						++pc;
						t_transfer key = stack->f_pop();
						t_slot& top = stack->f_top();
						top = top.f_remove(key.f_object());
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(GLOBAL_GET)
					{
						t_object* key = static_cast<t_object*>(*++pc);
						++pc;
						stack->f_push(f_engine()->f_module_global()->f_get(key));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET)
					{
						size_t outer = reinterpret_cast<size_t>(*++pc);
						assert(outer > 0);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope = stack->v_outer.f_object();
						for (size_t i = 1; i < outer; ++i) scope = f_as<t_scope&>(scope).v_outer.f_object();
						portable::t_scoped_lock_for_read lock(scope->v_lock);
						stack->f_push(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET_WITHOUT_LOCK)
					{
						size_t outer = reinterpret_cast<size_t>(*++pc);
						assert(outer > 0);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_scope* scope = &f_as<t_scope&>(stack->v_outer);
						for (size_t i = 1; i < outer; ++i) scope = &f_as<t_scope&>(scope->v_outer);
						stack->f_push((*scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET0)
					{
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_with_lock_for_read lock(f_context()->v_scope);
						stack->f_push((*stack)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET0_WITHOUT_LOCK)
					{
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						stack->f_push((*stack)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_PUT)
					{
						size_t outer = reinterpret_cast<size_t>(*++pc);
						assert(outer > 0);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope = stack->v_outer.f_object();
						for (size_t i = 1; i < outer; ++i) scope = f_as<t_scope&>(scope).v_outer.f_object();
						portable::t_scoped_lock_for_write lock(scope->v_lock);
						f_as<t_scope&>(scope)[index] = stack->f_top();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_PUT0)
					{
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_with_lock_for_write lock(f_context()->v_scope);
						(*stack)[index] = stack->f_top();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_PUT0_WITHOUT_LOCK)
					{
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						(*stack)[index] = stack->f_top();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(LAMBDA)
					{
						t_object* code = static_cast<t_object*>(*++pc);
						++pc;
						stack->f_push(t_lambda::f_instantiate(f_context()->v_scope, code));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SELF)
					{
						size_t outer = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_scope* scope = stack;
						for (size_t i = 0; i < outer; ++i) scope = &f_as<t_scope&>(scope->v_outer);
						stack->f_push(scope->v_self);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CLASS)
					{
						++pc;
						t_slot& top = stack->f_top();
						top = top.f_type();
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SUPER)
					{
						++pc;
						t_slot& top = stack->f_top();
						if (top.f_type() != f_global()->f_type<t_class>()) t_throwable::f_throw(L"not class.");
						top = f_as<t_type&>(top).v_super;
						if (!top.f_object()) t_throwable::f_throw(L"no more super class.");
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(NULL)
					++pc;
					stack->f_push(t_value());
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(BOOLEAN)
					{
						bool value = static_cast<bool>(reinterpret_cast<int>(*++pc));
						++pc;
						stack->f_push(f_global()->f_as(value));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(INTEGER)
					{
						int value = reinterpret_cast<int>(*++pc);
						++pc;
						stack->f_push(f_global()->f_as(value));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(FLOAT)
					{
						union
						{
							double v0;
							void* v1[sizeof(double) / sizeof(void*)];
						};
						for (size_t i = 0; i < sizeof(double) / sizeof(void*); ++i) v1[i] = *++pc;
						++pc;
						stack->f_push(f_global()->f_as(v0));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(INSTANCE)
					{
						t_object* value = static_cast<t_object*>(*++pc);
						++pc;
						stack->f_push(t_value(value));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(IDENTICAL)
					{
						++pc;
						t_transfer x = stack->f_pop();
						t_slot& top = stack->f_top();
						top = f_global()->f_as(x == top);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(NOT_IDENTICAL)
					{
						++pc;
						t_transfer x = stack->f_pop();
						t_slot& top = stack->f_top();
						top = f_global()->f_as(x != top);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(RETURN)
					{
						t_transfer x = stack->f_top().f_transfer();
						t_fiber::t_context::f_pop();
						if (f_context()->v_native > 0) return x;
						pc = f_context()->v_pc;
						stack = f_context()->v_stack;
						stack->f_return(x);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CALL)
					{
						size_t n = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_transfer x = stack->f_at(n).f_transfer();
						t_fiber::t_context* p = f_context();
						p->v_pc = pc;
						x.f_call(t_value(), n, *stack);
						if (f_context() != p) {
							if (f_context()->v_native > 0) return stack->f_top().f_transfer();
							pc = f_context()->v_pc;
							stack = f_context()->v_stack;
						}
					}
					XEMMAI__CODE__BREAK
#define XEMMAI__CODE__CASE_BEGIN(a_name, a_n)\
				XEMMAI__CODE__CASE(a_name)\
					{\
						++pc;\
						t_transfer x = stack->f_at(a_n).f_transfer();
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
						stack->f_return(t_value(a_x));
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
						{\
							t_fiber::t_context* p = f_context();\
							f_as<t_type&>(x.v_p->f_type()).a_method(x.v_p, *stack);\
							if (f_context() != p) {\
								p->v_pc = pc;\
								pc = f_context()->v_pc;\
								stack = f_context()->v_stack;\
							}\
						}
#define XEMMAI__CODE__CASE_END\
					}\
					XEMMAI__CODE__BREAK
#include "code_call.h"
#undef XEMMAI__CODE__CASE_BEGIN
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
#define XEMMAI__CODE__CALL_TAIL\
							t_fiber::t_context::f_pop();\
							t_fiber::t_context* p = f_context();\
							x.f_call(t_value(), n, *stack);\
							if (f_context() == p) {\
								x = stack->f_top().f_transfer();\
								if (p->v_native > 0) return x;\
								stack = p->v_stack;\
								stack->f_return(x);\
							} else {\
								if (f_context()->v_native > 0) return stack->f_top().f_transfer();\
								stack = f_context()->v_stack;\
							}
				XEMMAI__CODE__CASE(CALL_TAIL)
					{
						size_t n = reinterpret_cast<size_t>(*++pc);
						t_transfer x = stack->f_at(n).f_transfer();
						if (f_context()->v_simple) {
							t_finalize_fixed_scope finalize(stack);
							f_context()->v_simple = false;
							XEMMAI__CODE__CALL_TAIL
						} else {
							t_transfer scope = f_context()->v_scope.f_transfer();
							XEMMAI__CODE__CALL_TAIL
						}
						pc = f_context()->v_pc;
					}
					XEMMAI__CODE__BREAK
#undef XEMMAI__CODE__CALL_TAIL
#define XEMMAI__CODE__CASE_BEGIN(a_name, a_n)\
				XEMMAI__CODE__CASE(a_name##_TAIL)\
					{\
						t_transfer x = stack->f_at(a_n).f_transfer();
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
						{\
							t_value y(a_x);\
							t_fiber::t_context::f_pop();\
							if (f_context()->v_native > 0) return y;\
							stack = f_context()->v_stack;\
							stack->f_return(y);\
						}
#define XEMMAI__CODE__CALL_TAIL(a_method)\
							t_fiber::t_context::f_pop();\
							t_fiber::t_context* p = f_context();\
							f_as<t_type&>(x.v_p->f_type()).a_method(x.v_p, *stack);\
							if (f_context() == p) {\
								t_transfer x = stack->f_top().f_transfer();\
								if (p->v_native > 0) return x;\
								stack = p->v_stack;\
								stack->f_return(x);\
							} else {\
								stack = f_context()->v_stack;\
							}
#define XEMMAI__CODE__OBJECT_CALL(a_method)\
						if (f_context()->v_simple) {\
							t_finalize_fixed_scope finalize(stack);\
							f_context()->v_simple = false;\
							XEMMAI__CODE__CALL_TAIL(a_method)\
						} else {\
							t_transfer scope = f_context()->v_scope.f_transfer();\
							XEMMAI__CODE__CALL_TAIL(a_method)\
						}
#define XEMMAI__CODE__CASE_END\
						pc = f_context()->v_pc;\
					}\
					XEMMAI__CODE__BREAK
#include "code_call.h"
#undef XEMMAI__CODE__CASE_BEGIN
#undef XEMMAI__CODE__PRIMITIVE_CALL
#undef XEMMAI__CODE__CALL_TAIL
#undef XEMMAI__CODE__OBJECT_CALL
#undef XEMMAI__CODE__CASE_END
				XEMMAI__CODE__CASE(FIBER_EXIT)
					{
						t_transfer x = stack->f_top().f_transfer();
						f_context()->v_pc = pc;
						t_fiber& p = f_as<t_fiber&>(t_fiber::v_current);
						t_thread& thread = f_as<t_thread&>(t_thread::v_current);
						t_fiber& q = f_as<t_fiber&>(thread.v_fiber);
						t_fiber::t_try::t_state state = p.v_try->v_state;
						t_fiber::t_try::f_pop();
						if (state == t_fiber::t_try::e_state__THROW) {
							t_fiber::t_context::f_finalize(q.v_backtrace);
							p.v_context->v_next = p.v_backtrace;
							p.v_backtrace = 0;
							q.v_backtrace = p.v_context;
							p.v_context = 0;
						} else {
							t_fiber::t_context::f_terminate();
						}
						q.v_active = true;
						thread.v_active = thread.v_fiber;
						t_fiber::v_current = thread.v_fiber.f_object();
						t_fiber::t_context::v_instance = q.v_context;
						p.v_active = false;
						pc = f_context()->v_pc;
						if (state == t_fiber::t_try::e_state__THROW) throw t_scoped(x);
						if (f_context()->v_native > 0) return x;
						stack = f_context()->v_stack;
						stack->f_return(x);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(END)
					f_context()->v_pc = pc;
					return 0;
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
	return 0;
}

void t_code::f_scan(t_scan a_scan)
{
	for (std::vector<void*>::iterator i = v_objects.begin(); i != v_objects.end(); ++i) a_scan(*reinterpret_cast<t_slot*>(&*i));
}

const t_code::t_at* t_code::f_at(void** a_address) const
{
	std::vector<t_at>::const_iterator i = std::lower_bound(v_ats.begin(), v_ats.end(), t_at(a_address - &v_instructions[0], 0, 0, 0));
	return i == v_ats.end() ? 0 : &*i;
}

void t_code::f_estimate(size_t a_n, void** a_p)
{
	while (true) {
		int i = reinterpret_cast<int>(*a_p);
		if ((i & e_instruction__DEAD) == 0) break;
		i &= ~e_instruction__DEAD;
		*a_p = reinterpret_cast<void*>(i);
		switch (static_cast<t_instruction>(i)) {
		case e_instruction__JUMP:
			f_resolve(a_n, ++a_p);
			++a_p;
			break;
		case e_instruction__BRANCH:
			--a_n;
			f_resolve(a_n, ++a_p);
			++a_p;
			break;
		case e_instruction__TRY:
			if (++a_n > v_size) v_size = a_n;
			f_resolve(a_n, ++a_p);
			f_resolve(a_n, ++a_p);
			++a_p;
			break;
		case e_instruction__CATCH:
			--a_n;
			f_resolve(a_n, ++a_p);
			--a_n;
			a_p += 2;
			break;
		case e_instruction__FINALLY:
			a_p += 2;
			break;
		case e_instruction__YRT:
			f_resolve(a_n, ++a_p);
			f_resolve(a_n, ++a_p);
			f_resolve(a_n, ++a_p);
			++a_p;
			break;
		case e_instruction__THROW:
		case e_instruction__POP:
			--a_n;
			++a_p;
			break;
		case e_instruction__OBJECT_GET:
			a_p += 2;
			break;
		case e_instruction__OBJECT_GET_INDIRECT:
			--a_n;
			++a_p;
			break;
		case e_instruction__OBJECT_PUT:
			--a_n;
			a_p += 2;
			break;
		case e_instruction__OBJECT_PUT_INDIRECT:
			a_n -= 2;
			++a_p;
			break;
		case e_instruction__OBJECT_HAS:
			a_p += 2;
			break;
		case e_instruction__OBJECT_HAS_INDIRECT:
			--a_n;
			++a_p;
			break;
		case e_instruction__OBJECT_REMOVE:
			a_p += 2;
			break;
		case e_instruction__OBJECT_REMOVE_INDIRECT:
			--a_n;
			++a_p;
			break;
		case e_instruction__GLOBAL_GET:
			if (++a_n > v_size) v_size = a_n;
			a_p += 2;
			break;
		case e_instruction__SCOPE_GET:
			if (++a_n > v_size) v_size = a_n;
			a_p += 3;
			break;
		case e_instruction__SCOPE_GET0:
			if (++a_n > v_size) v_size = a_n;
			a_p += 2;
			break;
		case e_instruction__SCOPE_PUT:
			a_p += 3;
			break;
		case e_instruction__SCOPE_PUT0:
			a_p += 2;
			break;
		case e_instruction__LAMBDA:
		case e_instruction__SELF:
			if (++a_n > v_size) v_size = a_n;
			a_p += 2;
			break;
		case e_instruction__CLASS:
		case e_instruction__SUPER:
			++a_p;
			break;
		case e_instruction__NULL:
			if (++a_n > v_size) v_size = a_n;
			++a_p;
			break;
		case e_instruction__BOOLEAN:
		case e_instruction__INTEGER:
			if (++a_n > v_size) v_size = a_n;
			a_p += 2;
			break;
		case e_instruction__FLOAT:
			if (++a_n > v_size) v_size = a_n;
			a_p += 1 + sizeof(double) / sizeof(void*);
			break;
		case e_instruction__INSTANCE:
			if (++a_n > v_size) v_size = a_n;
			a_p += 2;
			break;
		case e_instruction__IDENTICAL:
		case e_instruction__NOT_IDENTICAL:
			--a_n;
			++a_p;
			break;
		case e_instruction__RETURN:
			return;
		case e_instruction__CALL:
			a_n -= reinterpret_cast<size_t>(*++a_p);
			++a_p;
			break;
		case e_instruction__GET_AT:
			--a_n;
			++a_p;
			break;
		case e_instruction__SET_AT:
			a_n -= 2;
			++a_p;
			break;
		case e_instruction__PLUS:
		case e_instruction__MINUS:
		case e_instruction__NOT:
		case e_instruction__COMPLEMENT:
			++a_p;
			break;
		case e_instruction__MULTIPLY:
		case e_instruction__DIVIDE:
		case e_instruction__MODULUS:
		case e_instruction__ADD:
		case e_instruction__SUBTRACT:
		case e_instruction__LEFT_SHIFT:
		case e_instruction__RIGHT_SHIFT:
		case e_instruction__LESS:
		case e_instruction__LESS_EQUAL:
		case e_instruction__GREATER:
		case e_instruction__GREATER_EQUAL:
		case e_instruction__EQUALS:
		case e_instruction__NOT_EQUALS:
		case e_instruction__AND:
		case e_instruction__XOR:
		case e_instruction__OR:
		case e_instruction__SEND:
			--a_n;
			++a_p;
			break;
		case e_instruction__FIBER_EXIT:
		case e_instruction__END:
			return;
		}
	}
}

bool t_code::f_tail(void** a_p)
{
	while (true) {
		t_instruction instruction = static_cast<t_instruction>(reinterpret_cast<int>(*a_p));
		if (instruction == e_instruction__RETURN) return true;
		if (instruction != e_instruction__JUMP) break;
		a_p = reinterpret_cast<void**>(*++a_p);
	}
	return false;
}

void t_code::f_tail()
{
	void** p = &v_instructions[0];
	while (true) {
		switch (static_cast<t_instruction>(reinterpret_cast<int>(*p))) {
		case e_instruction__JUMP:
		case e_instruction__BRANCH:
			p += 2;
			break;
		case e_instruction__TRY:
		case e_instruction__CATCH:
			p += 3;
			break;
		case e_instruction__FINALLY:
			p += 2;
			break;
		case e_instruction__YRT:
			p += 4;
			break;
		case e_instruction__THROW:
		case e_instruction__POP:
			++p;
			break;
		case e_instruction__OBJECT_GET:
			p += 2;
			break;
		case e_instruction__OBJECT_GET_INDIRECT:
			++p;
			break;
		case e_instruction__OBJECT_PUT:
			p += 2;
			break;
		case e_instruction__OBJECT_PUT_INDIRECT:
			++p;
			break;
		case e_instruction__OBJECT_HAS:
			p += 2;
			break;
		case e_instruction__OBJECT_HAS_INDIRECT:
			++p;
			break;
		case e_instruction__OBJECT_REMOVE:
			p += 2;
			break;
		case e_instruction__OBJECT_REMOVE_INDIRECT:
			++p;
			break;
		case e_instruction__GLOBAL_GET:
			p += 2;
			break;
		case e_instruction__SCOPE_GET:
			p += 3;
			break;
		case e_instruction__SCOPE_GET0:
			p += 2;
			break;
		case e_instruction__SCOPE_PUT:
			p += 3;
			break;
		case e_instruction__SCOPE_PUT0:
		case e_instruction__LAMBDA:
		case e_instruction__SELF:
			p += 2;
			break;
		case e_instruction__CLASS:
		case e_instruction__SUPER:
		case e_instruction__NULL:
			++p;
			break;
		case e_instruction__BOOLEAN:
		case e_instruction__INTEGER:
			p += 2;
			break;
		case e_instruction__FLOAT:
			p += 1 + sizeof(double) / sizeof(void*);
			break;
		case e_instruction__INSTANCE:
			p += 2;
			break;
		case e_instruction__IDENTICAL:
		case e_instruction__NOT_IDENTICAL:
			++p;
			break;
		case e_instruction__RETURN:
			return;
		case e_instruction__CALL:
			if (f_tail(p + 2)) *p = reinterpret_cast<void*>(e_instruction__CALL_TAIL);
			p += 2;
			break;
		case e_instruction__GET_AT:
		case e_instruction__SET_AT:
		case e_instruction__PLUS:
		case e_instruction__MINUS:
		case e_instruction__NOT:
		case e_instruction__COMPLEMENT:
		case e_instruction__MULTIPLY:
		case e_instruction__DIVIDE:
		case e_instruction__MODULUS:
		case e_instruction__ADD:
		case e_instruction__SUBTRACT:
		case e_instruction__LEFT_SHIFT:
		case e_instruction__RIGHT_SHIFT:
		case e_instruction__LESS:
		case e_instruction__LESS_EQUAL:
		case e_instruction__GREATER:
		case e_instruction__GREATER_EQUAL:
		case e_instruction__EQUALS:
		case e_instruction__NOT_EQUALS:
		case e_instruction__AND:
		case e_instruction__XOR:
		case e_instruction__OR:
		case e_instruction__SEND:
			if (f_tail(p + 1)) *reinterpret_cast<int*>(p) += e_instruction__CALL_TAIL - e_instruction__CALL;
			++p;
			break;
		case e_instruction__FIBER_EXIT:
		case e_instruction__END:
			return;
		}
	}
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

void t_type_of<t_code>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
