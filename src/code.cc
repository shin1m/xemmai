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
			&&label__STACK_GET,
			&&label__STACK_PUT,
			&&label__SCOPE_GET,
			&&label__SCOPE_GET_WITHOUT_LOCK,
			&&label__SCOPE_PUT,
			&&label__LAMBDA,
			&&label__SELF,
			&&label__CLASS,
			&&label__SUPER,
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
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						if (f_as<bool>(stack[0]))
							++pc;
						else
							pc = reinterpret_cast<void**>(*pc);
						stack[0] = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(TRY)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						void** catch0 = reinterpret_cast<void**>(*++pc);
						void** finally0 = reinterpret_cast<void**>(*++pc);
						f_context()->v_pc = ++pc;
						t_fiber::t_try::f_push(stack, f_context(), catch0, finally0);
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
								t_with_lock_for_write lock(f_context()->v_scope);
								f_as<t_scope&>(f_context()->v_scope)[~index] = value.f_transfer();
							} else {
								base[index] = value.f_transfer();
							}
						} else {
							pc = reinterpret_cast<void**>(*pc);
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
				XEMMAI__CODE__CASE(POP)
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
						top = f_global()->f_as(top.f_has(key));
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(OBJECT_HAS_INDIRECT)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& key = stack[1];
						top = f_global()->f_as(top.f_has(key));
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
				XEMMAI__CODE__CASE(SCOPE_GET)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope;
						if (outer > 0) {
							scope = f_context()->v_outer;
							for (size_t i = 1; i < outer; ++i) scope = f_as<t_scope&>(scope).v_outer;
						} else {
							scope = f_context()->v_scope;
						}
						portable::t_scoped_lock_for_read lock(scope->v_lock);
						stack[0].f_construct(f_as<const t_scope&>(scope)[index]);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(SCOPE_GET_WITHOUT_LOCK)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						size_t index = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_object* scope;
						if (outer > 0) {
							scope = f_context()->v_outer;
							for (size_t i = 1; i < outer; ++i) scope = f_as<t_scope&>(scope).v_outer;
						} else {
							scope = f_context()->v_scope;
						}
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
							for (size_t i = 1; i < outer; ++i) scope = f_as<t_scope&>(scope).v_outer;
						} else {
							scope = f_context()->v_scope;
						}
						portable::t_scoped_lock_for_write lock(scope->v_lock);
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
				XEMMAI__CODE__CASE(SELF)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t outer = reinterpret_cast<size_t>(*++pc);
						++pc;
						if (outer > 0) {
							t_object* scope = f_context()->v_outer;
							for (size_t i = 1; i < outer; ++i) scope = f_as<t_scope&>(scope).v_outer;
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
						bool value = static_cast<bool>(reinterpret_cast<int>(*++pc));
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
						t_object* value = static_cast<t_object*>(*++pc);
						++pc;
						stack[0].f_construct(value);
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(IDENTICAL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& x = stack[1];
						top = f_global()->f_as(top == x);
						x = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(NOT_IDENTICAL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						++pc;
						t_slot& top = stack[0];
						t_slot& x = stack[1];
						top = f_global()->f_as(top != x);
						x = 0;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(RETURN)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						base[-1].f_construct(stack[0].f_transfer());
						t_fiber::t_context::f_pop();
						if (f_context()->v_native > 0) return;
						base = f_context()->v_base;
						pc = f_context()->v_pc;
					}
					XEMMAI__CODE__BREAK
				XEMMAI__CODE__CASE(CALL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t n = reinterpret_cast<size_t>(*++pc);
						++pc;
						t_transfer x = stack[0].f_transfer();
						t_fiber::t_context* p = f_context();
						p->v_pc = pc;
						x.f_call(t_value(), stack, n);
						if (f_context() != p) {
							if (f_context()->v_native > 0) return;
							base = f_context()->v_base;
							pc = f_context()->v_pc;
						}
					}
					XEMMAI__CODE__BREAK
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
				XEMMAI__CODE__CASE(a_name)\
					{\
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);\
						++pc;\
						t_transfer x = stack[0].f_transfer();
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
						stack[0].f_construct(a_x);
#define XEMMAI__CODE__OBJECT_CALL(a_method, a_n)\
						{\
							t_fiber::t_context* p = f_context();\
							f_as<t_type&>(x.v_p->f_type()).a_method(x.v_p, stack);\
							if (f_context() != p) {\
								p->v_pc = pc;\
								base = f_context()->v_base;\
								pc = f_context()->v_pc;\
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
				XEMMAI__CODE__CASE(CALL_TAIL)
					{
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);
						size_t n = reinterpret_cast<size_t>(*++pc);
						t_transfer x = stack[0].f_transfer();
						t_fiber::t_context::f_pop(stack, n);
						x.f_call(t_value(), base - 1, n);
						if (f_context()->v_native > 0) return;
						base = f_context()->v_base;
						pc = f_context()->v_pc;
					}
					XEMMAI__CODE__BREAK
#define XEMMAI__CODE__CASE_BEGIN(a_name)\
				XEMMAI__CODE__CASE(a_name##_TAIL)\
					{\
						t_slot* stack = base + reinterpret_cast<size_t>(*++pc);\
						t_transfer x = stack[0].f_transfer();
#define XEMMAI__CODE__PRIMITIVE_CALL(a_x)\
						base[-1].f_construct(a_x);\
						t_fiber::t_context::f_pop();\
						if (f_context()->v_native > 0) return;
#define XEMMAI__CODE__OBJECT_CALL(a_method, a_n)\
						{\
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
#include "code_call.h"
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
						if (state == t_fiber::t_try::e_state__THROW) throw t_scoped(x);
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

t_transfer t_code::f_instantiate(const std::wstring& a_path, bool a_shared, size_t a_privates, size_t a_shareds, size_t a_arguments)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_code>());
	object.f_pointer__(new t_code(a_path, a_shared, a_privates, a_shareds, a_arguments));
	return object;
}

void t_code::f_scan(t_scan a_scan)
{
	for (std::vector<void*>::iterator i = v_objects.begin(); i != v_objects.end(); ++i) a_scan(*reinterpret_cast<t_slot*>(&*i));
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
