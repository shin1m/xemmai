// TODO
// * implement inline field caches.
// * use llvm::Value*s directly.
// * construct/destruct stack t_scopeds on demand.
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Mangler.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>
#include <xemmai/convert.h>

//#define XEMMAI__JIT__DEBUG_PRINT

namespace xemmai
{

namespace
{

template<typename T>
bool f_derives(t_object* a_type)
{
	return dynamic_cast<t_type_of<T>*>(&f_as<t_type&>(a_type));
}

void f_throw_message(const wchar_t* a_message)
{
	t_throwable::f_throw(a_message);
}

void f_throw_value(const t_scoped& a_value)
{
	throw a_value;
}

void f_lambda(t_scoped* a_stack, t_object* a_scope, t_object* a_code)
{
	a_stack[0].f_construct(t_lambda::f_instantiate(t_scoped(a_scope), t_scoped(a_code)));
}

void f_advanced_lambda(t_scoped* a_stack, t_object* a_scope, t_object* a_code)
{
	a_stack[0].f_construct(t_advanced_lambda::f_instantiate(t_scoped(a_scope), t_scoped(a_code), a_stack));
}

void f_throw_move(t_scoped* a_stack)
{
	throw t_scoped(std::move(a_stack[0]));
}

void f_object_get(t_scoped* a_stack, t_object* a_key)
{
	t_scoped& top = a_stack[0];
	top = top.f_get(a_key);
}

void f_method_get(t_scoped* a_stack, t_object* a_key)
{
	t_scoped top = std::move(a_stack[0]);
	top.f_get(a_key, a_stack);
}

void f_object_put(t_scoped* a_stack, t_object* a_key)
{
	t_scoped& top = a_stack[0];
	t_scoped& value = a_stack[1];
	top.f_put(a_key, t_scoped(value));
	top = std::move(value);
}

void f_object_put_clear(t_scoped* a_stack, t_object* a_key)
{
	t_scoped& top = a_stack[0];
	t_scoped& value = a_stack[1];
	top.f_put(a_key, std::move(value));
	top.f_destruct();
}

void f_object_put_indirect(t_scoped* a_stack)
{
	t_scoped& top = a_stack[0];
	t_scoped& key = a_stack[1];
	t_scoped& value = a_stack[2];
	top.f_put(key, t_scoped(value));
	key.f_destruct();
	top = std::move(value);
}

void f_object_has(t_scoped* a_stack, t_object* a_key)
{
	t_scoped& top = a_stack[0];
	top = top.f_has(a_key);
}

void f_object_remove(t_scoped* a_stack, t_object* a_key)
{
	t_scoped& top = a_stack[0];
	top = top.f_remove(a_key);
}

void f_method_bind(t_scoped* a_stack)
{
	if (a_stack[0].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_object* p = a_stack[0];
	if (p->f_type() == f_global()->f_type<t_method>())
		a_stack[0] = f_as<t_method&>(p).f_function();
	else if (!f_is<t_lambda>(p) && p->f_type() != f_global()->f_type<t_native>())
		t_code::f_method_bind(a_stack);
}

void f_global_get(t_scoped* a_stack, t_object* a_key)
{
	a_stack[0] = f_engine()->f_module_global()->f_get(a_key);
}

template<size_t (t_type::*A_function)(t_object*, t_scoped*)>
void f_operator(const t_value& a_this, t_scoped* a_stack)
{
	t_code::f_operator<A_function>(a_this, a_stack);
}

template<size_t (t_type::*A_function)(t_object*, t_scoped*)>
void f_operator_move(t_scoped&& a_this, t_scoped* a_stack)
{
	t_scoped x = std::move(a_this);
	t_code::f_operator<A_function>(x, a_stack);
}

template<size_t (t_type::*A_function)(t_object*, t_scoped*)>
size_t f_operator_tail(t_context* a_context, t_scoped* a_base, const t_value& a_this, t_scoped* a_stack)
{
	return t_code::f_operator<A_function>(*a_context, a_base, a_this, a_stack);
}

template<size_t (t_type::*A_function)(t_object*, t_scoped*)>
size_t f_operator_tail_move(t_context* a_context, t_scoped* a_base, t_scoped&& a_this, t_scoped* a_stack)
{
	t_scoped x = std::move(a_this);
	return t_code::f_operator<A_function>(*a_context, a_base, x, a_stack);
}

void f_call(t_scoped* a_stack, size_t a_n)
{
	t_scoped x = std::move(a_stack[0]);
	x.f_call(a_stack, a_n);
}

void f_get_at(t_scoped* a_stack)
{
	if (a_stack[1].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_scoped x = std::move(a_stack[1]);
	t_code::f_operator<&t_type::f_get_at>(x, a_stack);
}

size_t f_get_at_tail(t_context* a_context, t_scoped* a_base, t_scoped* a_stack)
{
	if (a_stack[1].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_scoped x = std::move(a_stack[1]);
	return t_code::f_operator<&t_type::f_get_at>(*a_context, a_base, x, a_stack);
}

void f_set_at(t_scoped* a_stack)
{
	if (a_stack[1].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_scoped x = std::move(a_stack[1]);
	t_code::f_operator<&t_type::f_set_at>(x, a_stack);
}

size_t f_set_at_tail(t_context* a_context, t_scoped* a_base, t_scoped* a_stack)
{
	if (a_stack[1].f_tag() < t_value::e_tag__OBJECT) t_throwable::f_throw(L"not supported");
	t_scoped x = std::move(a_stack[1]);
	return t_code::f_operator<&t_type::f_set_at>(*a_context, a_base, x, a_stack);
}

void f_safe_point(t_context* a_context)
{
	f_as<t_fiber&>(t_fiber::f_current()).v_context = a_context;
	auto& lambda = f_as<t_lambda&>(a_context->v_lambda);
	auto& code = f_as<t_code&>(lambda.f_code());
	if (a_context->f_pc()[-1] == code.f_p(e_instruction__BREAK_POINT))
		f_engine()->f_debug_break_point();
	else
		f_engine()->f_debug_safe_point();
}

}

struct t_engine_jit
{
	static void* f_symbol(const std::string& a_name)
	{
		if (a_name == "std::this_thread::yield") return reinterpret_cast<void*>(std::this_thread::yield);
		if (a_name == "xemmai::t_value::t_increments::f_next") return reinterpret_cast<void*>(&t_value::t_increments::f_next);
		if (a_name == "xemmai::t_value::f_increments") return reinterpret_cast<void*>(t_value::f_increments);
		if (a_name == "xemmai::t_value::t_decrements::f_next") return reinterpret_cast<void*>(&t_value::t_decrements::f_next);
		if (a_name == "xemmai::t_value::f_decrements") return reinterpret_cast<void*>(t_value::f_decrements);
		if (a_name == "xemmai::f_stack") return reinterpret_cast<void*>(f_stack);
		if (a_name == "f_derives_integer") return reinterpret_cast<void*>(f_derives<intptr_t>);
		if (a_name == "f_derives_float") return reinterpret_cast<void*>(f_derives<double>);
		if (a_name == "f_throw_message") return reinterpret_cast<void*>(f_throw_message);
		if (a_name == "f_throw_value") return reinterpret_cast<void*>(f_throw_value);
		if (a_name == "f_lambda") return reinterpret_cast<void*>(f_lambda);
		if (a_name == "f_advanced_lambda") return reinterpret_cast<void*>(f_advanced_lambda);
		if (a_name == "f_throw_move") return reinterpret_cast<void*>(f_throw_move);
		if (a_name == "xemmai::t_code::f_jit_try") return reinterpret_cast<void*>(xemmai::t_code::f_jit_try);
		if (a_name == "xemmai::t_code::f_jit_catch_stack") return reinterpret_cast<void*>(xemmai::t_code::f_jit_catch_stack);
		if (a_name == "xemmai::t_code::f_jit_catch_scope") return reinterpret_cast<void*>(xemmai::t_code::f_jit_catch_scope);
		if (a_name == "f_object_get") return reinterpret_cast<void*>(f_object_get);
		if (a_name == "f_method_get") return reinterpret_cast<void*>(f_method_get);
		if (a_name == "f_object_put") return reinterpret_cast<void*>(f_object_put);
		if (a_name == "f_object_put_clear") return reinterpret_cast<void*>(f_object_put_clear);
		if (a_name == "f_object_put_indirect") return reinterpret_cast<void*>(f_object_put_indirect);
		if (a_name == "f_object_has") return reinterpret_cast<void*>(f_object_has);
		if (a_name == "f_object_remove") return reinterpret_cast<void*>(f_object_remove);
		if (a_name == "f_method_bind") return reinterpret_cast<void*>(f_method_bind);
		if (a_name == "f_global_get") return reinterpret_cast<void*>(f_global_get);
#define XEMMAI__JIT__OPERATOR_SYMBOL(a_operator)\
		if (a_name == "f_"#a_operator) return reinterpret_cast<void*>(f_operator<&t_type::f_##a_operator>);\
		if (a_name == "f_"#a_operator"_move") return reinterpret_cast<void*>(f_operator_move<&t_type::f_##a_operator>);\
		if (a_name == "f_"#a_operator"_tail") return reinterpret_cast<void*>(f_operator_tail<&t_type::f_##a_operator>);\
		if (a_name == "f_"#a_operator"_tail_move") return reinterpret_cast<void*>(f_operator_tail_move<&t_type::f_##a_operator>);
		XEMMAI__JIT__OPERATOR_SYMBOL(plus)
		XEMMAI__JIT__OPERATOR_SYMBOL(minus)
		XEMMAI__JIT__OPERATOR_SYMBOL(not)
		XEMMAI__JIT__OPERATOR_SYMBOL(complement)
		XEMMAI__JIT__OPERATOR_SYMBOL(multiply)
		XEMMAI__JIT__OPERATOR_SYMBOL(divide)
		XEMMAI__JIT__OPERATOR_SYMBOL(modulus)
		XEMMAI__JIT__OPERATOR_SYMBOL(add)
		XEMMAI__JIT__OPERATOR_SYMBOL(subtract)
		XEMMAI__JIT__OPERATOR_SYMBOL(left_shift)
		XEMMAI__JIT__OPERATOR_SYMBOL(right_shift)
		XEMMAI__JIT__OPERATOR_SYMBOL(less)
		XEMMAI__JIT__OPERATOR_SYMBOL(less_equal)
		XEMMAI__JIT__OPERATOR_SYMBOL(greater)
		XEMMAI__JIT__OPERATOR_SYMBOL(greater_equal)
		XEMMAI__JIT__OPERATOR_SYMBOL(equals)
		XEMMAI__JIT__OPERATOR_SYMBOL(not_equals)
		XEMMAI__JIT__OPERATOR_SYMBOL(and)
		XEMMAI__JIT__OPERATOR_SYMBOL(xor)
		XEMMAI__JIT__OPERATOR_SYMBOL(or)
		XEMMAI__JIT__OPERATOR_SYMBOL(send)
		if (a_name == "xemmai::t_value::f_call") return reinterpret_cast<void*>(static_cast<void (t_value::*)(t_scoped*, size_t) const>(&t_value::f_call));
		if (a_name == "f_call") return reinterpret_cast<void*>(f_call);
		if (a_name == "xemmai::t_context::f_tail") return reinterpret_cast<void*>(&t_context::f_tail);
		if (a_name == "xemmai::t_code::f_expand") return reinterpret_cast<void*>(t_code::f_expand);
		if (a_name == "f_get_at") return reinterpret_cast<void*>(f_get_at);
		if (a_name == "f_get_at_tail") return reinterpret_cast<void*>(f_get_at_tail);
		if (a_name == "f_set_at") return reinterpret_cast<void*>(f_set_at);
		if (a_name == "f_set_at_tail") return reinterpret_cast<void*>(f_set_at_tail);
		if (a_name == "f_safe_point") return reinterpret_cast<void*>(f_safe_point);
		return dlsym(RTLD_DEFAULT, a_name.c_str());
	}

	std::mutex v_mutex;
	std::unique_ptr<llvm::TargetMachine> v_target;
	llvm::LLVMContext v_context;
	llvm::FunctionType* v_ft_personality;
	llvm::StructType* v_type_landing_pad;
	llvm::FunctionType* v_ft_void;
	llvm::StructType* v_type_object;
	llvm::PointerType* v_type_object_pointer;
	llvm::Type* v_type_increments;
	llvm::FunctionType* v_ft_void__increments_pointer__object_pointer;
	llvm::FunctionType* v_ft_increments_pointer;
	llvm::Type* v_type_decrements;
	llvm::FunctionType* v_ft_void__decrements_pointer__object_pointer;
	llvm::FunctionType* v_ft_decrements_pointer;
	llvm::StructType* v_type_value;
	llvm::StructType* v_type_stack;
	llvm::FunctionType* v_ft_stack;
	llvm::StructType* v_type_type;
	llvm::StructType* v_type_scope;
	llvm::StructType* v_type_lambda;
	llvm::StructType* v_type_context;
	llvm::PointerType* v_type_pc;
	llvm::FunctionType* v_ft_size_t__context_pointer;
	llvm::FunctionType* v_ft_bool__object_pointer;
	llvm::FunctionType* v_ft_void__wstring;
	llvm::FunctionType* v_ft_void__value_pointer;
	llvm::FunctionType* v_ft_size_t__context_pointer__value_pointer;
	llvm::FunctionType* v_ft_bool__context_pointer__value_pointer__value_pointer__pc__size_t;
	llvm::FunctionType* v_ft_void__context_pointer;
	llvm::FunctionType* v_ft_size_t__context_pointer__value_pointer__fp__fp__fp;
	llvm::FunctionType* v_ft_void__value_pointer__object_pointer__object_pointer;
	llvm::FunctionType* v_ft_void__value_pointer__object_pointer;
	llvm::FunctionType* v_ft_void__value_pointer__value_pointer;
	llvm::FunctionType* v_ft_size_t__context_pointer__value_pointer__value_pointer__value_pointer;
	llvm::FunctionType* v_ft_void__value_pointer__value_pointer__size_t;
	llvm::FunctionType* v_ft_void__value_pointer__size_t;
	llvm::FunctionType* v_ft_void__context_pointer__value_pointer__size_t;
	llvm::FunctionType* v_ft_size_t__value_pointer__size_t;
	llvm::FunctionType* v_ft_size_t__context_pointer__value_pointer__value_pointer;

	t_engine_jit() : v_target(llvm::EngineBuilder().selectTarget())
	{
		v_ft_personality = llvm::FunctionType::get(llvm::Type::getInt32Ty(v_context), true);
		v_type_landing_pad = llvm::StructType::get(
			llvm::Type::getInt8PtrTy(v_context),
			llvm::Type::getInt32Ty(v_context),
			nullptr
		);
		auto type_void = llvm::Type::getVoidTy(v_context);
		v_ft_void = llvm::FunctionType::get(type_void, false);
		auto type_size_t = llvm::Type::getIntNTy(v_context, sizeof(size_t) * 8);
		v_type_object = llvm::StructType::create(v_context, "xemmai::t_object");
		v_type_object_pointer = llvm::PointerType::getUnqual(v_type_object);
		auto type_pointer = llvm::PointerType::getUnqual(v_type_object_pointer);
		v_type_increments = llvm::StructType::create("xemmai::t_value::t_increments",
			llvm::ArrayType::get(v_type_object_pointer, t_value::t_increments::V_SIZE),
			type_pointer,
			type_pointer,
			nullptr
		);
		auto type_increments_pointer = llvm::PointerType::getUnqual(v_type_increments);
		v_ft_void__increments_pointer__object_pointer = llvm::FunctionType::get(type_void, {type_increments_pointer, v_type_object_pointer}, false);
		v_ft_increments_pointer = llvm::FunctionType::get(type_increments_pointer, false);
		v_type_decrements = llvm::StructType::create("xemmai::t_value::t_decrements",
			llvm::ArrayType::get(v_type_object_pointer, t_value::t_decrements::V_SIZE),
			type_pointer,
			type_pointer,
			nullptr
		);
		auto type_decrements_pointer = llvm::PointerType::getUnqual(v_type_decrements);
		v_ft_void__decrements_pointer__object_pointer = llvm::FunctionType::get(type_void, {type_decrements_pointer, v_type_object_pointer}, false);
		v_ft_decrements_pointer = llvm::FunctionType::get(type_decrements_pointer, false);
		v_type_value = llvm::StructType::create("xemmai::t_value",
			v_type_object_pointer,
			llvm::Type::getIntNTy(v_context, std::max(sizeof(void*), sizeof(double)) * 8),
			nullptr
		);
		v_type_object->setBody(
			v_type_object_pointer,
			v_type_object_pointer,
			type_size_t,
			type_size_t,
			type_size_t,
			v_type_value,
			v_type_object_pointer,
			v_type_object_pointer,
			type_size_t,
			nullptr
		);
		auto type_value_pointer = llvm::PointerType::getUnqual(v_type_value);
		v_type_stack = llvm::StructType::create("xemmai::t_stack",
			type_size_t,
			type_value_pointer,
			type_value_pointer,
			type_value_pointer,
			nullptr
		);
		v_ft_stack = llvm::FunctionType::get(llvm::PointerType::getUnqual(v_type_stack), false);
		v_type_type = llvm::StructType::create("xemmai::t_type",
			type_size_t,
			v_type_value,
			v_type_value,
			nullptr
		);
		v_type_scope = llvm::StructType::create("xemmai::t_scope",
			type_size_t,
			v_type_value,
			v_type_value,
			nullptr
		);
		auto type_bool = llvm::Type::getInt1Ty(v_context);
		v_type_lambda = llvm::StructType::create("xemmai::t_lambda",
			v_type_value,
			v_type_value,
			llvm::PointerType::getUnqual(v_type_scope),
			type_size_t,
			type_bool,
			type_size_t,
			type_size_t,
			type_size_t,
			type_size_t,
			type_pointer,
			nullptr
		);
		v_type_context = llvm::StructType::create(v_context, "xemmai::t_context");
		auto type_context_pointer = llvm::PointerType::getUnqual(v_type_context);
		v_type_context->setBody(
			type_context_pointer,
			type_value_pointer,
			type_size_t,
			v_type_value,
			v_type_value,
			nullptr
		);
		v_type_pc = type_pointer;
		v_ft_size_t__context_pointer = llvm::FunctionType::get(type_size_t, {type_context_pointer}, false);
		v_ft_bool__object_pointer = llvm::FunctionType::get(type_bool, {v_type_object_pointer}, false);
		auto type_wstring = llvm::PointerType::getUnqual(llvm::Type::getIntNTy(v_context, sizeof(wchar_t) * 8));
		v_ft_void__wstring = llvm::FunctionType::get(type_void, {type_wstring}, false);
		v_ft_void__value_pointer = llvm::FunctionType::get(type_void, {type_value_pointer}, false);
		v_ft_size_t__context_pointer__value_pointer = llvm::FunctionType::get(type_size_t, {type_context_pointer, type_value_pointer}, false);
		v_ft_bool__context_pointer__value_pointer__value_pointer__pc__size_t = llvm::FunctionType::get(type_bool, {type_context_pointer, type_value_pointer, type_value_pointer, v_type_pc, type_size_t}, false);
		v_ft_void__context_pointer = llvm::FunctionType::get(type_void, {type_context_pointer}, false);
		v_ft_size_t__context_pointer__value_pointer__fp__fp__fp = llvm::FunctionType::get(type_size_t, {type_context_pointer, type_value_pointer, llvm::PointerType::getUnqual(v_ft_size_t__context_pointer), llvm::PointerType::getUnqual(v_ft_size_t__context_pointer__value_pointer), llvm::PointerType::getUnqual(v_ft_void__context_pointer)}, false);
		v_ft_void__value_pointer__object_pointer__object_pointer = llvm::FunctionType::get(type_void, {type_value_pointer, v_type_object_pointer, v_type_object_pointer}, false);
		v_ft_void__value_pointer__object_pointer = llvm::FunctionType::get(type_void, {type_value_pointer, v_type_object_pointer}, false);
		v_ft_void__value_pointer__value_pointer = llvm::FunctionType::get(type_void, {type_value_pointer, type_value_pointer}, false);
		v_ft_size_t__context_pointer__value_pointer__value_pointer__value_pointer = llvm::FunctionType::get(type_size_t, {type_context_pointer, type_value_pointer, type_value_pointer, type_value_pointer}, false);
		v_ft_void__value_pointer__value_pointer__size_t = llvm::FunctionType::get(type_void, {type_value_pointer, type_value_pointer, type_size_t}, false);
		v_ft_void__value_pointer__size_t = llvm::FunctionType::get(type_void, {type_value_pointer, type_size_t}, false);
		v_ft_void__context_pointer__value_pointer__size_t = llvm::FunctionType::get(type_void, {type_context_pointer, type_value_pointer, type_size_t}, false);
		v_ft_size_t__value_pointer__size_t = llvm::FunctionType::get(type_size_t, {type_value_pointer, type_size_t}, false);
		v_ft_size_t__context_pointer__value_pointer__value_pointer = llvm::FunctionType::get(type_size_t, {type_context_pointer, type_value_pointer, type_value_pointer}, false);
	}
};

void t_engine::f_jit_construct()
{
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	v_jit = new t_engine_jit();
}

void t_engine::f_jit_destruct()
{
	delete v_jit;
}

inline t_engine_jit* f_jit()
{
	return f_engine()->v_jit;
}

namespace
{

struct t_jit
{
	typedef llvm::orc::IRCompileLayer<llvm::orc::ObjectLinkingLayer<>>::ModuleSetHandleT t_handle;

	const llvm::DataLayout& v_layout;
	llvm::orc::ObjectLinkingLayer<> v_linking;
	llvm::orc::IRCompileLayer<llvm::orc::ObjectLinkingLayer<>> v_compile;

	t_jit(llvm::TargetMachine& a_target) : v_layout(*a_target.getDataLayout()), v_compile(v_linking, llvm::orc::SimpleCompiler(a_target))
	{
	}
	t_handle f_add(std::unique_ptr<llvm::Module>&& a_module)
	{
		return v_compile.addModuleSet(std::array<llvm::Module*, 1>{a_module.get()}, std::make_unique<llvm::SectionMemoryManager>(), llvm::orc::createLambdaResolver([this](const std::string& a_name)
		{
			if (auto p = t_engine_jit::f_symbol(a_name)) return llvm::RuntimeDyld::SymbolInfo(reinterpret_cast<uint64_t>(p), llvm::JITSymbolFlags::Weak);
			return llvm::RuntimeDyld::SymbolInfo(nullptr);
		}, [](const std::string& a_name)
		{
			return nullptr;
		}));
	}
	void f_remove(t_handle a_handle)
	{
		v_compile.removeModuleSet(a_handle);
	}
	llvm::orc::JITSymbol f_find_symbol(const std::string& a_name)
	{
		return v_compile.findSymbol(a_name, true);
	}
	std::string f_mangle(const std::string& a_name) const
	{
		std::string name;
		{
			llvm::raw_string_ostream stream(name);
			llvm::Mangler::getNameWithPrefix(stream, a_name, v_layout);
		}
		return name;
	}
	llvm::orc::JITSymbol f_find_unmangled_symbol(const std::string& a_name)
	{
		return f_find_symbol(f_mangle(a_name));
	}
};

std::string f_name(void* a_p)
{
	char cs[sizeof(void*) * 2 + 7];
	std::sprintf(cs, "xemmai%0*zu", sizeof(void*) * 2, a_p);
	return cs;
}

}

size_t t_code::f_jit_loop_nojit(t_context* a_context)
{
	a_context->f_pc() = f_as<t_lambda&>(a_context->v_lambda).v_instructions;
	return f_loop(a_context);
}

t_code::t_try t_code::f_jit_try(t_context* a_context, t_scoped* a_stack, t_try (*a_try)(t_context*), t_try (*a_catch)(t_context*, const t_scoped&), void (*a_finally)(t_context*))
{
	t_try try0;
	try {
		try {
			try0 = a_try(a_context);
		} catch (const t_scoped&) {
			throw;
		} catch (...) {
			throw t_throwable::f_instantiate(L"<unknown>.");
		}
	} catch (const t_scoped& thrown) {
		auto used = a_context->v_base + f_as<t_lambda&>(a_context->v_lambda).v_size;
		auto& p = f_as<t_fiber&>(t_fiber::f_current());
		p.v_stack.f_clear(a_stack);
		p.v_stack.v_used = used;
		try {
			try {
				try0 = a_catch(a_context, thrown);
			} catch (const t_scoped&) {
				throw;
			} catch (...) {
				throw t_throwable::f_instantiate(L"<unknown>.");
			}
		} catch (const t_scoped& thrown) {
			auto& pc = a_context->f_pc();
			void** caught = pc;
			p.v_stack.f_clear(a_stack);
			p.v_stack.v_used = used;
			a_finally(a_context);
			pc = caught;
			throw thrown;
		}
	}
	a_finally(a_context);
	return try0;
}

bool t_code::f_jit_catch_stack(t_context* a_context, t_scoped* a_stack, const t_scoped& a_thrown, void** a_caught, size_t a_index)
{
	t_scoped type = std::move(a_stack[0]);
	if (a_thrown == f_engine()->v_fiber_exit || !a_thrown.f_is(type)) return false;
	f_as<t_fiber&>(t_fiber::f_current()).f_caught(a_thrown, a_caught);
	a_context->v_base[a_index] = a_thrown;
	return true;
}

bool t_code::f_jit_catch_scope(t_context* a_context, t_scoped* a_stack, const t_scoped& a_thrown, void** a_caught, size_t a_index)
{
	t_scoped type = std::move(a_stack[0]);
	if (a_thrown == f_engine()->v_fiber_exit || !a_thrown.f_is(type)) return false;
	f_as<t_fiber&>(t_fiber::f_current()).f_caught(a_thrown, a_caught);
	t_scoped& scope = a_context->v_scope;
	t_with_lock_for_write lock(scope);
	f_as<t_scope&>(scope)[a_index] = a_thrown;
	return true;
}

t_script::t_script(const std::wstring& a_path) : t_module(a_path), v_jit(new t_jit(*f_jit()->v_target))
{
}

t_script::~t_script()
{
	delete static_cast<t_jit*>(v_jit);
}

void t_script::f_jit_add(void* a_module)
{
	static_cast<t_jit*>(v_jit)->f_add(std::unique_ptr<llvm::Module>(static_cast<llvm::Module*>(a_module)));
}

uint64_t t_script::f_jit_find(const std::string& a_name)
{
	return static_cast<t_jit*>(v_jit)->f_find_unmangled_symbol(a_name).getAddress();
}

struct t_jit_generator : llvm::IRBuilder<>
{
	struct t_operator
	{
		llvm::Function* v_call;
		llvm::Function* v_call_move;
		llvm::Function* v_tail;
		llvm::Function* v_tail_move;
	};
	struct t_globals
	{
		t_generator& v_generator;
		std::unique_ptr<llvm::Module> v_module;
		std::unique_ptr<llvm::legacy::FunctionPassManager> v_fpm;
		std::vector<std::pair<t_code&, std::string>> v_codes;
		llvm::Function* v_personality;
		llvm::Function* v_yield;
		llvm::Function* v_increments__f_next;
		llvm::Function* v_increments;
		llvm::Function* v_decrements__f_next;
		llvm::Function* v_decrements;
		llvm::Function* v_stack;
		llvm::Function* v_derives_integer;
		llvm::Function* v_derives_float;
		llvm::Function* v_throw_message;
		llvm::Function* v_throw_value;
		llvm::Function* v_lambda;
		llvm::Function* v_advanced_lambda;
		llvm::Function* v_throw_move;
		llvm::Function* v_try;
		llvm::Function* v_catch_stack;
		llvm::Function* v_catch_scope;
		llvm::Function* v_object_get;
		llvm::Function* v_method_get;
		llvm::Function* v_object_put;
		llvm::Function* v_object_put_clear;
		llvm::Function* v_object_put_indirect;
		llvm::Function* v_object_has;
		llvm::Function* v_object_remove;
		llvm::Function* v_method_bind;
		llvm::Function* v_global_get;
		t_operator v_plus;
		t_operator v_minus;
		t_operator v_not;
		t_operator v_complement;
		t_operator v_multiply;
		t_operator v_divide;
		t_operator v_modulus;
		t_operator v_add;
		t_operator v_subtract;
		t_operator v_left_shift;
		t_operator v_right_shift;
		t_operator v_less;
		t_operator v_less_equal;
		t_operator v_greater;
		t_operator v_greater_equal;
		t_operator v_equals;
		t_operator v_not_equals;
		t_operator v_and;
		t_operator v_xor;
		t_operator v_or;
		t_operator v_send;
		llvm::Function* v_value__call;
		llvm::Function* v_call;
		llvm::Function* v_tail;
		llvm::Function* v_expand;
		llvm::Function* v_get_at;
		llvm::Function* v_get_at_tail;
		llvm::Function* v_set_at;
		llvm::Function* v_set_at_tail;
		llvm::Function* v_safe_point;

		t_globals(t_generator& a_generator) : v_generator(a_generator), v_module(new llvm::Module(f_name(v_generator.v_module), f_jit()->v_context))
		{
			v_module->setDataLayout(*f_jit()->v_target->getDataLayout());
			auto module = v_module.get();
			v_fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module);
			v_fpm->add(llvm::createBasicAliasAnalysisPass());
			v_fpm->add(llvm::createPromoteMemoryToRegisterPass());
			v_fpm->add(llvm::createInstructionCombiningPass());
			v_fpm->add(llvm::createReassociatePass());
			v_fpm->add(llvm::createGVNPass());
			v_fpm->add(llvm::createCFGSimplificationPass());
			v_fpm->doInitialization();
			v_personality = llvm::Function::Create(f_jit()->v_ft_personality, llvm::Function::ExternalLinkage, "__gxx_personality_v0", module);
			v_yield = llvm::Function::Create(f_jit()->v_ft_void, llvm::Function::ExternalLinkage, "std::this_thread::yield", module);
			v_increments__f_next = llvm::Function::Create(f_jit()->v_ft_void__increments_pointer__object_pointer, llvm::Function::ExternalLinkage, "xemmai::t_value::t_increments::f_next", module);
			v_increments = llvm::Function::Create(f_jit()->v_ft_increments_pointer, llvm::Function::ExternalLinkage, "xemmai::t_value::f_increments", module);
			v_decrements__f_next = llvm::Function::Create(f_jit()->v_ft_void__decrements_pointer__object_pointer, llvm::Function::ExternalLinkage, "xemmai::t_value::t_decrements::f_next", module);
			v_decrements = llvm::Function::Create(f_jit()->v_ft_decrements_pointer, llvm::Function::ExternalLinkage, "xemmai::t_value::f_decrements", module);
			v_stack = llvm::Function::Create(f_jit()->v_ft_stack, llvm::Function::ExternalLinkage, "xemmai::f_stack", module);
			v_derives_integer = llvm::Function::Create(f_jit()->v_ft_bool__object_pointer, llvm::Function::ExternalLinkage, "f_derives_integer", module);
			v_derives_float = llvm::Function::Create(f_jit()->v_ft_bool__object_pointer, llvm::Function::ExternalLinkage, "f_derives_float", module);
			v_throw_message = llvm::Function::Create(f_jit()->v_ft_void__wstring, llvm::Function::ExternalLinkage, "f_throw_message", module);
			v_throw_value = llvm::Function::Create(f_jit()->v_ft_void__value_pointer, llvm::Function::ExternalLinkage, "f_throw_value", module);
			v_lambda = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_lambda", module);
			v_advanced_lambda = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_advanced_lambda", module);
			v_throw_move = llvm::Function::Create(f_jit()->v_ft_void__value_pointer, llvm::Function::ExternalLinkage, "f_throw_move", module);
			v_try = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer__value_pointer__fp__fp__fp, llvm::Function::ExternalLinkage, "xemmai::t_code::f_jit_try", module);
			v_catch_stack = llvm::Function::Create(f_jit()->v_ft_bool__context_pointer__value_pointer__value_pointer__pc__size_t, llvm::Function::ExternalLinkage, "xemmai::t_code::f_jit_catch_stack", module);
			v_catch_scope = llvm::Function::Create(f_jit()->v_ft_bool__context_pointer__value_pointer__value_pointer__pc__size_t, llvm::Function::ExternalLinkage, "xemmai::t_code::f_jit_catch_scope", module);
			v_object_get = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_object_get", module);
			v_method_get = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_method_get", module);
			v_object_put = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_object_put", module);
			v_object_put_clear = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_object_put_clear", module);
			v_object_put_indirect = llvm::Function::Create(f_jit()->v_ft_void__value_pointer, llvm::Function::ExternalLinkage, "f_object_put_indirect", module);
			v_object_has = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_object_has", module);
			v_object_remove = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_object_remove", module);
			v_method_bind = llvm::Function::Create(f_jit()->v_ft_void__value_pointer, llvm::Function::ExternalLinkage, "f_method_bind", module);
			v_global_get = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__object_pointer, llvm::Function::ExternalLinkage, "f_global_get", module);
#define XEMMAI__JIT__OPERATOR_CREATE(a_operator)\
			v_##a_operator.v_call = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__value_pointer, llvm::Function::ExternalLinkage, "f_"#a_operator, module);\
			v_##a_operator.v_call_move = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__value_pointer, llvm::Function::ExternalLinkage, "f_"#a_operator"_move", module);\
			v_##a_operator.v_tail = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer__value_pointer__value_pointer__value_pointer, llvm::Function::ExternalLinkage, "f_"#a_operator"_tail", module);\
			v_##a_operator.v_tail_move = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer__value_pointer__value_pointer__value_pointer, llvm::Function::ExternalLinkage, "f_"#a_operator"_tail_move", module);
			XEMMAI__JIT__OPERATOR_CREATE(plus)
			XEMMAI__JIT__OPERATOR_CREATE(minus)
			XEMMAI__JIT__OPERATOR_CREATE(not)
			XEMMAI__JIT__OPERATOR_CREATE(complement)
			XEMMAI__JIT__OPERATOR_CREATE(multiply)
			XEMMAI__JIT__OPERATOR_CREATE(divide)
			XEMMAI__JIT__OPERATOR_CREATE(modulus)
			XEMMAI__JIT__OPERATOR_CREATE(add)
			XEMMAI__JIT__OPERATOR_CREATE(subtract)
			XEMMAI__JIT__OPERATOR_CREATE(left_shift)
			XEMMAI__JIT__OPERATOR_CREATE(right_shift)
			XEMMAI__JIT__OPERATOR_CREATE(less)
			XEMMAI__JIT__OPERATOR_CREATE(less_equal)
			XEMMAI__JIT__OPERATOR_CREATE(greater)
			XEMMAI__JIT__OPERATOR_CREATE(greater_equal)
			XEMMAI__JIT__OPERATOR_CREATE(equals)
			XEMMAI__JIT__OPERATOR_CREATE(not_equals)
			XEMMAI__JIT__OPERATOR_CREATE(and)
			XEMMAI__JIT__OPERATOR_CREATE(xor)
			XEMMAI__JIT__OPERATOR_CREATE(or)
			XEMMAI__JIT__OPERATOR_CREATE(send)
			v_value__call = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__value_pointer__size_t, llvm::Function::ExternalLinkage, "xemmai::t_value::f_call", module);
			v_call = llvm::Function::Create(f_jit()->v_ft_void__value_pointer__size_t, llvm::Function::ExternalLinkage, "f_call", module);
			v_tail = llvm::Function::Create(f_jit()->v_ft_void__context_pointer__value_pointer__size_t, llvm::Function::ExternalLinkage, "xemmai::t_context::f_tail", module);
			v_expand = llvm::Function::Create(f_jit()->v_ft_size_t__value_pointer__size_t, llvm::Function::ExternalLinkage, "xemmai::t_code::f_expand", module);
			v_get_at = llvm::Function::Create(f_jit()->v_ft_void__value_pointer, llvm::Function::ExternalLinkage, "f_get_at", module);
			v_get_at_tail = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer__value_pointer__value_pointer, llvm::Function::ExternalLinkage, "f_get_at_tail", module);
			v_set_at = llvm::Function::Create(f_jit()->v_ft_void__value_pointer, llvm::Function::ExternalLinkage, "f_set_at", module);
			v_set_at_tail = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer__value_pointer__value_pointer, llvm::Function::ExternalLinkage, "f_set_at_tail", module);
			v_safe_point = llvm::Function::Create(f_jit()->v_ft_void__context_pointer, llvm::Function::ExternalLinkage, "f_safe_point", module);
		}
		void f_install()
		{
//			v_module->dump();
			auto& script = static_cast<t_script&>(xemmai::f_as<t_module&>(v_generator.v_module));
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "ADDING A MODULE...\n");
#endif
			script.f_jit_add(v_module.release());
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "DONE.\n");
#endif
			for (auto& x : v_codes) x.first.v_jit_loop = reinterpret_cast<size_t (*)(t_context*)>(script.f_jit_find(x.second));
		}
	};
	struct t_queue
	{
		llvm::Function* v_f_next;
		llvm::Value* v_this;
		llvm::Value* v_head;
		llvm::Value* v_next;
	};
	struct t_targets
	{
		llvm::BasicBlock* v_break;
		bool v_break_is_tail;
		bool v_break_must_clear;
		llvm::BasicBlock* v_continue;
		llvm::BasicBlock* v_return;
		bool v_return_is_tail;
	};

	template<typename T>
	t_value::t_tag f_tag();
	static llvm::StructType* f_type_landing_pad()
	{
		return f_jit()->v_type_landing_pad;
	}
	static llvm::StructType* f_type_object()
	{
		return f_jit()->v_type_object;
	}
	static llvm::PointerType* f_type_object_pointer()
	{
		return f_jit()->v_type_object_pointer;
	}
	static llvm::StructType* f_type_value()
	{
		return f_jit()->v_type_value;
	}
	static llvm::StructType* f_type_stack()
	{
		return f_jit()->v_type_stack;
	}
	static llvm::StructType* f_type_type()
	{
		return f_jit()->v_type_type;
	}
	static llvm::StructType* f_type_scope()
	{
		return f_jit()->v_type_scope;
	}
	static llvm::StructType* f_type_lambda()
	{
		return f_jit()->v_type_lambda;
	}
	static llvm::StructType* f_type_context()
	{
		return f_jit()->v_type_context;
	}
	static llvm::PointerType* f_type_pc()
	{
		return f_jit()->v_type_pc;
	}

	t_code& v_code;
	t_globals& v_globals;
	t_queue v_increments;
	t_queue v_decrements;
	llvm::Value* v_stack;
	llvm::Value* v_context;
	llvm::Value* v_base;
	llvm::Value* v_lambda;
	llvm::Value* v_instructions;
	llvm::Value* v_scope;
	llvm::Value* v_pc;
	t_targets* v_targets;

	t_jit_generator(llvm::Function* a_function, t_code& a_code, t_globals& a_globals) : llvm::IRBuilder<>(llvm::BasicBlock::Create(f_jit()->v_context, "entry", a_function)), v_code(a_code), v_globals(a_globals)
	{
		v_increments.v_f_next = v_globals.v_increments__f_next;
		v_increments.v_this = CreateCall(v_globals.v_increments);
		v_increments.v_head = CreateStructGEP(f_jit()->v_type_increments, v_increments.v_this, 1);
		v_increments.v_next = CreateStructGEP(f_jit()->v_type_increments, v_increments.v_this, 2);
		v_decrements.v_f_next = v_globals.v_decrements__f_next;
		v_decrements.v_this = CreateCall(v_globals.v_decrements);
		v_decrements.v_head = CreateStructGEP(f_jit()->v_type_decrements, v_decrements.v_this, 1);
		v_decrements.v_next = CreateStructGEP(f_jit()->v_type_decrements, v_decrements.v_this, 2);
		v_stack = CreateCall(v_globals.v_stack);
		v_context = a_function->arg_begin();
		v_base = CreateLoad(CreateStructGEP(f_type_context(), v_context, 1));
		v_lambda = f_as(f_type_lambda(), f_value__p(CreateStructGEP(f_type_context(), v_context, 3)));
		v_instructions = CreatePointerCast(CreateLoad(CreateStructGEP(f_type_lambda(), v_lambda, 9)), f_type_pc());
		auto scope = CreateStructGEP(f_type_context(), v_context, 4);
		v_scope = f_value__p(scope);
		v_pc = CreatePointerCast(CreateStructGEP(f_type_value(), scope, 1), llvm::PointerType::getUnqual(f_type_pc()));
	}
	llvm::BasicBlock* f_block(const char* a_name)
	{
		return llvm::BasicBlock::Create(getContext(), a_name, GetInsertBlock()->getParent());
	}
	llvm::Type* f_type_integer()
	{
		return getIntNTy(sizeof(intptr_t) * 8);
	}
	template<typename T>
	llvm::Type* f_type();
	llvm::ConstantInt* f_integer(uint64_t a_value)
	{
		return getIntN(sizeof(intptr_t) * 8, a_value);
	}
	llvm::Constant* f_float(double a_value)
	{
		return llvm::ConstantFP::get(getDoubleTy(), a_value);
	}
	void f_lock__acquire_for_read(llvm::Value* a_lock)
	{
		auto loopbb = f_block("loop");
		auto mergebb = f_block("merge");
		auto rmw = CreateAtomicRMW(llvm::AtomicRMWInst::Add, a_lock, f_integer(1), llvm::Acquire);
		CreateCondBr(CreateICmpSLT(rmw, f_integer(0)), loopbb, mergebb);
		SetInsertPoint(loopbb);
		CreateCall(v_globals.v_yield);
		auto lock = CreateLoad(a_lock);
		lock->setAtomic(llvm::Acquire);
		lock->setAlignment(sizeof(intptr_t));
		CreateCondBr(CreateICmpSGT(lock, f_integer(0)), mergebb, loopbb);
		SetInsertPoint(mergebb);
	}
	void f_lock__release_for_read(llvm::Value* a_lock)
	{
		CreateAtomicRMW(llvm::AtomicRMWInst::Sub, a_lock, f_integer(1), llvm::Release);
	}
	void f_lock__acquire_for_write(llvm::Value* a_lock)
	{
		auto loopbb = f_block("loop");
		auto elsebb = f_block("else");
		auto mergebb = f_block("merge");
		CreateBr(loopbb);
		SetInsertPoint(loopbb);
		auto cx = CreateAtomicCmpXchg(a_lock, f_integer(0), f_integer(~(~size_t(0) >> 1)), llvm::Acquire, llvm::Monotonic);
		CreateCondBr(CreateExtractValue(cx, {1}), mergebb, elsebb);
		SetInsertPoint(elsebb);
		CreateCall(v_globals.v_yield);
		auto lock = CreateLoad(a_lock);
		lock->setAtomic(llvm::Monotonic);
		lock->setAlignment(sizeof(intptr_t));
		CreateCondBr(CreateICmpEQ(lock, f_integer(0)), loopbb, elsebb);
		SetInsertPoint(mergebb);
	}
	void f_lock__release_for_write(llvm::Value* a_lock)
	{
		CreateAtomicRMW(llvm::AtomicRMWInst::Add, a_lock, f_integer(~(~size_t(0) >> 1)), llvm::Release);
	}
	llvm::ConstantPointerNull* f_null()
	{
		return llvm::ConstantPointerNull::get(f_type_object_pointer());
	}
	llvm::Value* f_pointer(t_object* a_value)
	{
		return CreateIntToPtr(f_integer(reinterpret_cast<uint64_t>(a_value)), f_type_object_pointer());
	}
	llvm::Value* f_tag(llvm::Value* a_p)
	{
		return CreatePtrToInt(a_p, f_type_integer());
	}
	llvm::Value* f_as(llvm::Type* a_type, llvm::Value* a_value)
	{
		auto slot = CreateStructGEP(f_type_object(), a_value, 5);
		auto pointer = CreateLoad(CreateStructGEP(f_type_value(), slot, 1));
		return CreateIntToPtr(pointer, llvm::PointerType::getUnqual(a_type));
	}
	template<typename T>
	llvm::Value* f_derives(llvm::Value* a_p);
	void f_value__push(t_queue& a_queue, llvm::Value* a_p)
	{
		auto head = CreateLoad(a_queue.v_head, true);
		auto next = CreateLoad(a_queue.v_next, true);
		auto thenbb = f_block("then");
		auto elsebb = f_block("else");
		auto mergebb = f_block("merge");
		CreateCondBr(CreateICmpEQ(head, next), thenbb, elsebb);
		SetInsertPoint(thenbb);
		CreateCall(a_queue.v_f_next, {a_queue.v_this, a_p});
		CreateBr(mergebb);
		SetInsertPoint(elsebb);
		CreateStore(a_p, head, true);
		CreateStore(CreateConstGEP1_32(head, 1), a_queue.v_head, true);
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
	}
	void f_value__push_if_object(t_queue& a_queue, llvm::Value* a_p)
	{
		auto elsebb = f_block("else");
		auto mergebb = f_block("merge");
		auto tag = f_tag(a_p);
		CreateCondBr(CreateICmpULT(tag, f_integer(t_value::e_tag__OBJECT)), mergebb, elsebb);
		SetInsertPoint(elsebb);
		f_value__push(a_queue, a_p);
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
	}
	llvm::Value* f_value__p(llvm::Value* a_this)
	{
		return CreateLoad(CreateStructGEP(f_type_value(), a_this, 0));
	}
	void f_value__p__(llvm::Value* a_this, llvm::Value* a_p)
	{
		CreateStore(a_p, CreateStructGEP(f_type_value(), a_this, 0));
	}
	llvm::Value* f_value__union(llvm::Value* a_this, llvm::Type* a_type)
	{
		auto p = CreateStructGEP(f_type_value(), a_this, 1);
		return CreateLoad(CreatePointerCast(p, llvm::PointerType::getUnqual(a_type)));
	}
	void f_value__union__(llvm::Value* a_this, llvm::Value* a_value)
	{
		auto p = CreateStructGEP(f_type_value(), a_this, 1);
		p = CreatePointerCast(p, llvm::PointerType::getUnqual(a_value->getType()));
		CreateStore(a_value, p);
	}
	void f_value__copy_union(llvm::Value* a_this, llvm::Value* a_value, llvm::Type* a_type)
	{
		f_value__union__(a_this, f_value__union(a_value, a_type));
	}
	void f_value__copy_union_value(llvm::Value* a_this, llvm::Value* a_value, llvm::Type* a_type)
	{
		auto p = CreateExtractValue(a_value, {1});
		p = CreateBitOrPointerCast(p, a_type);
		f_value__union__(a_this, p);
	}
	void f_value__copy(llvm::Value* a_this, llvm::Value* a_p, llvm::Value* a_value)
	{
		auto integerbb = f_block("integer");
		auto floatbb = f_block("float");
		auto objectbb = f_block("object");
		auto mergebb = f_block("merge");
		auto s = CreateSwitch(f_tag(a_p), objectbb, 4);
		s->addCase(f_integer(t_value::e_tag__NULL), mergebb);
		s->addCase(f_integer(t_value::e_tag__BOOLEAN), integerbb);
		s->addCase(f_integer(t_value::e_tag__INTEGER), integerbb);
		SetInsertPoint(integerbb);
		f_value__copy_union(a_this, a_value, f_type_integer());
		CreateBr(mergebb);
		s->addCase(f_integer(t_value::e_tag__FLOAT), floatbb);
		SetInsertPoint(floatbb);
		f_value__copy_union(a_this, a_value, getDoubleTy());
		CreateBr(mergebb);
		SetInsertPoint(objectbb);
		f_value__push(v_increments, a_p);
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
	}
	void f_value__assign_pointer(llvm::Value* a_this, llvm::Value* a_value)
	{
		f_value__push_if_object(v_increments, a_value);
		auto p1 = CreateStructGEP(f_type_value(), a_this, 0);
		auto p = CreateLoad(p1);
		CreateStore(a_value, p1);
		f_value__push_if_object(v_decrements, p);
	}
	void f_value__assign(llvm::Value* a_this, llvm::Value* a_value)
	{
		auto p0 = f_value__p(a_value);
		f_value__copy(a_this, p0, a_value);
		auto p1 = CreateStructGEP(f_type_value(), a_this, 0);
		auto p = CreateLoad(p1);
		CreateStore(p0, p1);
		f_value__push_if_object(v_decrements, p);
	}
	void f_value__move_union(llvm::Value* a_this, llvm::Value* a_p, llvm::Value* a_value)
	{
		auto integerbb = f_block("integer");
		auto floatbb = f_block("float");
		auto mergebb = f_block("merge");
		auto s = CreateSwitch(f_tag(a_p), mergebb, 3);
		s->addCase(f_integer(t_value::e_tag__BOOLEAN), integerbb);
		s->addCase(f_integer(t_value::e_tag__INTEGER), integerbb);
		SetInsertPoint(integerbb);
		f_value__copy_union(a_this, a_value, f_type_integer());
		CreateBr(mergebb);
		s->addCase(f_integer(t_value::e_tag__FLOAT), floatbb);
		SetInsertPoint(floatbb);
		f_value__copy_union(a_this, a_value, getDoubleTy());
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
		f_value__p__(a_value, f_null());
	}
	void f_value__construct(llvm::Value* a_this, llvm::Value* a_value)
	{
		auto p = f_value__p(a_value);
		f_value__copy(a_this, p, a_value);
		f_value__p__(a_this, p);
	}
	void f_value__construct(llvm::Value* a_this, t_value::t_tag a_tag, llvm::Value* a_value)
	{
		auto p = CreateIntToPtr(f_integer(a_tag), f_type_object_pointer());
		f_value__p__(a_this, p);
		f_value__union__(a_this, a_value);
	}
	void f_value__construct(llvm::Value* a_this, bool a_value)
	{
		f_value__construct(a_this, t_value::e_tag__BOOLEAN, getInt1(a_value));
	}
	void f_value__construct(llvm::Value* a_this, intptr_t a_value)
	{
		f_value__construct(a_this, t_value::e_tag__INTEGER, f_integer(a_value));
	}
	void f_value__construct(llvm::Value* a_this, double a_value)
	{
		f_value__construct(a_this, t_value::e_tag__FLOAT, llvm::ConstantFP::get(getContext(), llvm::APFloat(a_value)));
	}
	void f_value__construct(llvm::Value* a_this, const t_value& a_value)
	{
		switch (a_value.f_tag()) {
		case t_value::e_tag__NULL:
			f_value__p__(a_this, f_null());
			break;
		case t_value::e_tag__BOOLEAN:
			f_value__construct(a_this, a_value.f_boolean());
			break;
		case t_value::e_tag__INTEGER:
			f_value__construct(a_this, a_value.f_integer());
			break;
		case t_value::e_tag__FLOAT:
			f_value__construct(a_this, a_value.f_float());
			break;
		default:
			{
				auto p = f_pointer(a_value);
				f_value__push(v_increments, p);
				f_value__p__(a_this, p);
			}
		}
	}
	void f_value__destruct(llvm::Value* a_this)
	{
		auto p0 = CreateStructGEP(f_type_value(), a_this, 0);
		auto p1 = CreateLoad(p0);
		auto elsebb = f_block("else");
		auto mergebb = f_block("merge");
		auto tag = f_tag(p1);
		CreateCondBr(CreateICmpULT(tag, f_integer(t_value::e_tag__OBJECT)), mergebb, elsebb);
		SetInsertPoint(elsebb);
		f_value__push(v_decrements, p1);
		CreateStore(f_null(), p0);
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
	}
	void f_value__construct_move(llvm::Value* a_this, llvm::Value* a_value)
	{
		auto p = f_value__p(a_value);
		f_value__p__(a_this, p);
		f_value__move_union(a_this, p, a_value);
	}
	void f_scoped__move(llvm::Value* a_this, llvm::Value* a_value)
	{
		auto p0 = f_value__p(a_value);
		auto p1 = CreateStructGEP(f_type_value(), a_this, 0);
		auto p = CreateLoad(p1);
		CreateStore(p0, p1);
		f_value__move_union(a_this, p0, a_value);
		f_value__push_if_object(v_decrements, p);
	}
	llvm::Value* f_value__type(llvm::Value* a_value)
	{
		auto nullbb = f_block("null");
		auto booleanbb = f_block("boolean");
		auto integerbb = f_block("integer");
		auto floatbb = f_block("float");
		auto objectbb = f_block("object");
		auto mergebb = f_block("merge");
		auto p = f_value__p(a_value);
		auto s = CreateSwitch(f_tag(p), objectbb, 4);
		s->addCase(f_integer(t_value::e_tag__NULL), nullbb);
		s->addCase(f_integer(t_value::e_tag__BOOLEAN), booleanbb);
		s->addCase(f_integer(t_value::e_tag__INTEGER), integerbb);
		s->addCase(f_integer(t_value::e_tag__FLOAT), floatbb);
		SetInsertPoint(nullbb);
		auto type0 = f_pointer(f_global()->f_type<nullptr_t>());
		CreateBr(mergebb);
		SetInsertPoint(booleanbb);
		auto type1 = f_pointer(f_global()->f_type<bool>());
		CreateBr(mergebb);
		SetInsertPoint(integerbb);
		auto type2 = f_pointer(f_global()->f_type<intptr_t>());
		CreateBr(mergebb);
		SetInsertPoint(floatbb);
		auto type3 = f_pointer(f_global()->f_type<double>());
		CreateBr(mergebb);
		SetInsertPoint(objectbb);
		auto type4 = f_value__p(CreateStructGEP(f_type_object(), p, 5));
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
		auto phi = CreatePHI(f_type_object_pointer(), 5);
		phi->addIncoming(type0, nullbb);
		phi->addIncoming(type1, booleanbb);
		phi->addIncoming(type2, integerbb);
		phi->addIncoming(type3, floatbb);
		phi->addIncoming(type4, objectbb);
		return phi;
	}
	void f_at(const t_at& a_at)
	{
		v_code.f_operand(nullptr);
		v_code.f_at(a_at);
		CreateStore(CreateConstGEP1_64(v_instructions, v_code.f_last()), v_pc);
	}
	void f_throw(const wchar_t* a_message, const t_at& a_at)
	{
		f_at(a_at);
		auto message = CreateIntToPtr(f_integer(reinterpret_cast<uint64_t>(a_message)), llvm::PointerType::getUnqual(getIntNTy(sizeof(wchar_t) * 8)));
		CreateCall(v_globals.v_throw_message, {message});
		CreateUnreachable();
	}
	llvm::BasicBlock* f_throw_not_supported(const t_at& a_at)
	{
		auto b0 = GetInsertBlock();
		auto b1 = f_block("throw not supported");
		SetInsertPoint(b1);
		f_throw(L"not supported", a_at);
		SetInsertPoint(b0);
		return b1;
	}
	void f_throw(llvm::Value* a_value)
	{
		CreateCall(v_globals.v_throw_value, {a_value});
		CreateUnreachable();
	}
	void f_return(llvm::Value* a_value)
	{
		CreateRet(a_value);
		SetInsertPoint(f_block("unreachable"));
	}
	template<typename T_cleanup>
	llvm::Value* f_invoke(llvm::Value* a_callee, llvm::ArrayRef<llvm::Value*> a_arguments, const t_at& a_at, T_cleanup a_cleanup)
	{
		GetInsertBlock()->getParent()->setPersonalityFn(v_globals.v_personality);
		auto normalbb = f_block("normal");
		auto unwindbb = f_block("unwind");
		auto value = CreateInvoke(a_callee, normalbb, unwindbb, a_arguments);
		SetInsertPoint(unwindbb);
		auto pad = CreateLandingPad(f_type_landing_pad(), 0);
		pad->setCleanup(true);
		f_at(a_at);
		a_cleanup();
		CreateResume(pad);
		SetInsertPoint(normalbb);
		a_cleanup();
		return value;
	}
	llvm::Value* f_base(size_t a_stack)
	{
		return CreateConstGEP1_64(v_base, a_stack);
	}
	void f_context__pop()
	{
		f_value__destruct(f_base(-1));
		for (size_t i = 0; i < v_code.v_privates; ++i) f_value__destruct(f_base(i));
		auto used = CreateStructGEP(f_type_stack(), v_stack, 3);
		auto lambda = CreateStructGEP(f_type_context(), v_context, 3);
		auto previous = CreateStructGEP(f_type_value(), lambda, 1);
		previous = CreateLoad(CreatePointerCast(previous, llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(f_type_value()))));
		CreateStore(previous, used);
	}
	llvm::Value* f_outer(size_t a_outer)
	{
		auto scope = f_value__p(CreateStructGEP(f_type_lambda(), v_lambda, 0));
		for (size_t i = 1; i < a_outer; ++i) scope = f_value__p(CreateStructGEP(f_type_scope(), f_as(f_type_scope(), scope), 1));
		return scope;
	}
	llvm::Value* f_outer_as_scope()
	{
		return CreateLoad(CreateStructGEP(f_type_lambda(), v_lambda, 2));
	}
	llvm::Value* f_entry(llvm::Value* a_scope, size_t a_index)
	{
		auto entries = CreateStructGEP(f_type_scope(), a_scope, 2);
		return a_index > 0 ? CreateConstGEP1_64(entries, a_index) : entries;
	}
	void f_reserve(size_t a_size)
	{
		if (a_size > v_code.v_size) v_code.v_size = a_size;
	}
	void f_emit_LAMBDA(size_t a_stack, t_object* a_code, bool a_advanced)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "LAMBDA %zd %d\n", a_stack, a_advanced);
#endif
		auto stack = f_base(a_stack);
		auto code = f_pointer(a_code);
		CreateCall(a_advanced ? v_globals.v_advanced_lambda : v_globals.v_lambda, {stack, v_scope, code});
	}
	void f_emit_BRANCH(size_t a_stack, llvm::BasicBlock* a_then, llvm::BasicBlock* a_else)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "BRANCH %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto p = f_value__union(stack, getInt1Ty());
		CreateCondBr(CreateICmpNE(p, getFalse()), a_then, a_else);
	}
	void f_emit_TRY(size_t a_stack, llvm::Function* a_try, llvm::Function* a_catch, llvm::Function* a_finally)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "TRY %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto stepbb = f_block("step");
		auto try0 = CreateCall(v_globals.v_try, {v_context, stack, a_try, a_catch, a_finally});
		size_t n = 1;
		if (v_targets->v_break) ++n;
		if (v_targets->v_continue) ++n;
		auto s = CreateSwitch(try0, stepbb, n);
		if (v_targets->v_break) s->addCase(f_integer(t_code::e_try__BREAK), v_targets->v_break);
		if (v_targets->v_continue) s->addCase(f_integer(t_code::e_try__CONTINUE), v_targets->v_continue);
		s->addCase(f_integer(t_code::e_try__RETURN), v_targets->v_return);
		SetInsertPoint(stepbb);
	}
	void f_emit_CATCH(size_t a_stack, llvm::Value* a_thrown, llvm::Value* a_caught, const t_code::t_variable& a_variable, llvm::BasicBlock* a_next)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "CATCH %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto stepbb = f_block("step");
		auto caught = CreateCall(a_variable.v_shared ? v_globals.v_catch_scope : v_globals.v_catch_stack, {v_context, stack, a_thrown, a_caught, f_integer(a_variable.v_index)});
		CreateCondBr(caught, stepbb, a_next);
		SetInsertPoint(stepbb);
	}
	void f_emit_THROW(size_t a_stack, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "THROW %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		f_invoke(v_globals.v_throw_move, {stack}, a_at, [] {});
	}
	void f_emit_CLEAR(size_t a_stack)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "CLEAR %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		f_value__destruct(stack);
	}
	void f_emit_OBJECT_GET(size_t a_stack, t_object* a_key, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT GET %zd %p\n", a_stack, a_key);
#endif
		auto stack = f_base(a_stack);
		auto key = f_pointer(a_key);
		f_invoke(v_globals.v_object_get, {stack, key}, a_at, [] {});
	}
	void f_emit_METHOD_GET(size_t a_stack, t_object* a_key, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "METHOD GET %zd %p\n", a_stack, a_key);
#endif
		auto stack = f_base(a_stack);
		auto key = f_pointer(a_key);
		f_invoke(v_globals.v_method_get, {stack, key}, a_at, [] {});
	}
	void f_emit_OBJECT_GET_INDIRECT(size_t a_stack, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT GET INDIRECT %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto key = f_base(a_stack + 1);
		f_invoke(v_globals.v_object_get, {stack, f_value__p(key)}, a_at, [this, key]
		{
			f_value__destruct(key);
		});
	}
	void f_emit_OBJECT_PUT(size_t a_stack, t_object* a_key, bool a_clear, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT PUT %zd %p %d\n", a_stack, a_key, a_clear);
#endif
		auto stack = f_base(a_stack);
		auto key = f_pointer(a_key);
		f_invoke(a_clear ? v_globals.v_object_put_clear : v_globals.v_object_put, {stack, key}, a_at, [] {});
	}
	void f_emit_OBJECT_PUT_INDIRECT(size_t a_stack, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT PUT %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		f_invoke(v_globals.v_object_put_indirect, {stack}, a_at, [] {});
	}
	void f_emit_OBJECT_HAS(size_t a_stack, t_object* a_key)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT HAS %zd %p\n", a_stack, a_key);
#endif
		auto stack = f_base(a_stack);
		auto key = f_pointer(a_key);
		CreateCall(v_globals.v_object_has, {stack, key});
	}
	void f_emit_OBJECT_HAS_INDIRECT(size_t a_stack)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT HAS INDIRECT %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto key = f_base(a_stack + 1);
		CreateCall(v_globals.v_object_has, {stack, f_value__p(key)});
		f_value__destruct(key);
	}
	void f_emit_OBJECT_REMOVE(size_t a_stack, t_object* a_key, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT REMOVE %zd %p\n", a_stack, a_key);
#endif
		auto stack = f_base(a_stack);
		auto key = f_pointer(a_key);
		f_invoke(v_globals.v_object_remove, {stack, key}, a_at, [] {});
	}
	void f_emit_OBJECT_REMOVE_INDIRECT(size_t a_stack, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "OBJECT REMOVE INDIRECT %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto key = f_base(a_stack + 1);
		f_invoke(v_globals.v_object_remove, {stack, f_value__p(key)}, a_at, [this, key]
		{
			f_value__destruct(key);
		});
	}
	void f_emit_METHOD_BIND(size_t a_stack, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "METHOD BIND %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		f_invoke(v_globals.v_method_bind, {stack}, a_at, [] {});
	}
	void f_emit_GLOBAL_GET(size_t a_stack, t_object* a_key, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "GLOBAL GET %zd %p\n", a_stack, a_key);
#endif
		auto stack = f_base(a_stack);
		auto key = f_pointer(a_key);
		f_invoke(v_globals.v_global_get, {stack, key}, a_at, [] {});
	}
	void f_emit_STACK_GET(size_t a_stack, size_t a_index)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "STACK GET %zd %zd\n", a_stack, a_index);
#endif
		auto stack = f_base(a_stack);
		f_value__construct(stack, f_base(a_index));
	}
	void f_emit_STACK_PUT(size_t a_stack, size_t a_index, bool a_clear)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "STACK PUT %zd %zd %d\n", a_stack, a_index, a_clear);
#endif
		auto stack = f_base(a_stack);
		if (a_clear)
			f_scoped__move(f_base(a_index), stack);
		else
			f_value__assign(f_base(a_index), stack);
	}
	void f_emit_SCOPE_GET(size_t a_stack, size_t a_outer, size_t a_index, bool a_lock)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SCOPE GET %zd %zd %zd %d\n", a_stack, a_outer, a_index, a_lock);
#endif
		auto stack = f_base(a_stack);
		if (a_outer == 1 && !a_lock) {
			auto entry = f_entry(f_outer_as_scope(), a_index);
			f_value__construct(stack, entry);
			return;
		}
		auto scope = a_outer > 0 ? f_outer(a_outer) : v_scope;
		auto entry = f_entry(f_as(f_type_scope(), scope), a_index);
		if (a_lock) {
			auto lock = CreateStructGEP(f_type_object(), scope, 8);
			f_lock__acquire_for_read(lock);
			f_value__construct(stack, entry);
			f_lock__release_for_read(lock);
		} else {
			f_value__construct(stack, entry);
		}
	}
	void f_emit_SCOPE_PUT(size_t a_stack, size_t a_outer, size_t a_index, bool a_clear)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SCOPE PUT %zd %zd %zd %d\n", a_stack, a_outer, a_index, a_clear);
#endif
		auto stack = f_base(a_stack);
		auto scope = a_outer > 0 ? f_outer(a_outer) : v_scope;
		auto entry = f_entry(f_as(f_type_scope(), scope), a_index);
		auto lock = CreateStructGEP(f_type_object(), scope, 8);
		f_lock__acquire_for_write(lock);
		f_value__assign(entry, stack);
		if (a_clear) f_value__destruct(stack);
		f_lock__release_for_write(lock);
	}
	void f_emit_SELF(size_t a_stack, size_t a_outer)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SELF %zd %zd\n", a_stack, a_outer);
#endif
		auto stack = f_base(a_stack);
		if (a_outer > 0) {
			auto scope = a_outer == 1 ? f_outer_as_scope() : f_as(f_type_scope(), f_outer(a_outer));
			f_value__construct(stack, f_entry(scope, 0));
		} else {
			f_value__construct(stack, f_base(-1));
		}
	}
	void f_emit_CLASS(size_t a_stack)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "CLASS %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		auto type = f_value__type(stack);
		f_value__assign_pointer(stack, type);
	}
	void f_emit_SUPER(size_t a_stack, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SUPER %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		{
			auto elsebb = f_block("else");
			auto mergebb = f_block("merge");
			auto type = f_value__type(stack);
			auto clazz = f_pointer(f_global()->f_type<xemmai::t_class>());
			CreateCondBr(CreateICmpEQ(type, clazz), mergebb, elsebb);
			SetInsertPoint(elsebb);
			f_throw(L"not class.", a_at);
			SetInsertPoint(mergebb);
		}
		auto super = f_value__p(CreateStructGEP(f_type_type(), f_as(f_type_type(), f_value__p(stack)), 2));
		{
			auto thenbb = f_block("then");
			auto mergebb = f_block("merge");
			CreateCondBr(CreateIsNull(super), thenbb, mergebb);
			SetInsertPoint(thenbb);
			f_throw(L"no more super class.", a_at);
			SetInsertPoint(mergebb);
		}
		f_value__assign_pointer(stack, super);
	}
	void f_emit_NUL(size_t a_stack)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "NUL %zd\n", a_stack);
#endif
		f_value__p__(f_base(a_stack), f_null());
	}
	void f_emit_BOOLEAN(size_t a_stack, bool a_value)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "BOOLEAN %zd %d\n", a_stack, a_value);
#endif
		f_value__construct(f_base(a_stack), a_value);
	}
	void f_emit_INTEGER(size_t a_stack, intptr_t a_value)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "INTEGER %zd %ld\n", a_stack, a_value);
#endif
		f_value__construct(f_base(a_stack), a_value);
	}
	void f_emit_FLOAT(size_t a_stack, double a_value)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "FLOAT %zd %g\n", a_stack, a_value);
#endif
		f_value__construct(f_base(a_stack), a_value);
	}
	void f_emit_INSTANCE(size_t a_stack, const t_value& a_value)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "INSTANCE %zd %p\n", a_stack, static_cast<t_object*>(a_value));
#endif
		f_value__construct(f_base(a_stack), a_value);
	}
	llvm::Value* f_value(const ast::t_operand& a_value)
	{
		switch (a_value.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			return f_integer(a_value.v_integer);
		case ast::t_operand::e_tag__FLOAT:
			return f_float(a_value.v_float);
		case ast::t_operand::e_tag__LITERAL:
			return CreateIntToPtr(f_integer(reinterpret_cast<uint64_t>(&a_value.v_value)), llvm::PointerType::getUnqual(f_type_value()));
		default:
			return f_base(a_value.v_index);
		}
	}
	void f_return_or_jump(llvm::Value* a_value, llvm::BasicBlock* a_block, bool a_tail)
	{
		if (a_tail)
			CreateRet(a_value);
		else
			CreateBr(a_block);
	}
	void f_primitive_call(llvm::Value* a_stack, t_value::t_tag a_tag, llvm::Value* a_value, bool a_tail)
	{
		if (a_tail) {
			f_value__construct(f_base(-2), a_tag, a_value);
			f_context__pop();
		} else {
			f_value__construct(a_stack, a_tag, a_value);
		}
	}
	llvm::Value* f_object_call(const t_operator& a_operator, llvm::Value* a_stack, const ast::t_operand& a_operand, llvm::Value* a_value, bool a_tail, const t_at& a_at)
	{
		if (a_tail)
			return f_invoke(a_operand.v_tag == ast::t_operand::e_tag__LITERAL ? a_operator.v_tail : a_operator.v_tail_move, {v_context, v_base, a_value, a_stack}, a_at, [] {});
		else
			return f_invoke(a_operand.v_tag == ast::t_operand::e_tag__TEMPORARY ? a_operator.v_call_move : a_operator.v_call, {a_value, a_stack}, a_at, [] {});
	}
	llvm::Value* f_object_call(const t_operator& a_operator, llvm::Value* a_stack, const ast::t_operand& a_x_operand, llvm::Value* a_x, const ast::t_operand& a_y_operand, llvm::Value* a_y, bool a_tail, const t_at& a_at)
	{
		auto y = CreateConstGEP1_64(a_stack, 2);
		switch (a_y_operand.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			f_value__construct(y, t_value::e_tag__INTEGER, a_y);
			break;
		case ast::t_operand::e_tag__FLOAT:
			f_value__construct(y, t_value::e_tag__FLOAT, a_y);
			break;
		case ast::t_operand::e_tag__LITERAL:
			f_value__construct(y, a_y);
			break;
		case ast::t_operand::e_tag__VARIABLE:
			if (a_tail && (a_x_operand.v_tag != ast::t_operand::e_tag__VARIABLE || a_x_operand.v_index != a_y_operand.v_index))
				f_value__construct_move(y, a_y);
			else
				f_value__construct(y, a_y);
			break;
		default:
			if (a_x_operand.v_tag != ast::t_operand::e_tag__TEMPORARY) f_value__construct_move(y, a_y);
		}
		return f_object_call(a_operator, a_stack, a_x_operand, a_x, a_tail, a_at);
	}
	template<typename T, llvm::Value* (t_jit_generator::*A_operator)(llvm::Value*)>
	llvm::BasicBlock* f_unary(llvm::Value* a_stack, llvm::Value* a_value, llvm::BasicBlock* a_merge, bool a_tail)
	{
		auto block = f_block("block");
		SetInsertPoint(block);
		auto x = (this->*A_operator)(f_value__union(a_value, f_type<T>()));
		f_primitive_call(a_stack, f_tag<T>(), x, a_tail);
		f_return_or_jump(f_integer(-1), a_merge, a_tail);
		return block;
	}
	template<llvm::BasicBlock* (t_jit_generator::*A_boolean)(llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_integer)(llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_float)(llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool)>
	void f_emit_UNARY(const t_operator& a_operator, size_t a_stack, const ast::t_operand& a_value, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "UNARY %zd %zd %d\n", a_stack, a_value.v_index, a_tail);
#endif
		auto throwbb = f_throw_not_supported(a_at);
		auto mergebb = f_block("merge");
		auto block = GetInsertBlock();
		auto stack = f_base(a_stack);
		switch (a_value.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			{
				auto to = A_integer ? (this->*A_integer)(stack, f_integer(a_value.v_integer), mergebb, a_tail) : throwbb;
				SetInsertPoint(block);
				CreateBr(to);
				SetInsertPoint(mergebb);
				return;
			}
		case ast::t_operand::e_tag__FLOAT:
			{
				auto to = A_float ? (this->*A_float)(stack, f_float(a_value.v_float), mergebb, a_tail) : throwbb;
				SetInsertPoint(block);
				CreateBr(to);
				SetInsertPoint(mergebb);
				return;
			}
		}
		auto value = f_value(a_value);
		auto objectbb = f_block("object");
		auto s = CreateSwitch(f_tag(f_value__p(value)), objectbb, 4);
		s->addCase(f_integer(t_value::e_tag__NULL), throwbb);
		s->addCase(f_integer(t_value::e_tag__BOOLEAN), A_boolean ? (this->*A_boolean)(stack, value, mergebb, a_tail) : throwbb);
		s->addCase(f_integer(t_value::e_tag__INTEGER), A_integer ? (this->*A_integer)(stack, value, mergebb, a_tail) : throwbb);
		s->addCase(f_integer(t_value::e_tag__FLOAT), A_float ? (this->*A_float)(stack, value, mergebb, a_tail) : throwbb);
		SetInsertPoint(objectbb);
		auto z = f_object_call(a_operator, stack, a_value, value, a_tail, a_at);
		f_return_or_jump(z, mergebb, a_tail);
		SetInsertPoint(mergebb);
	}
	llvm::Value* f_plus(llvm::Value* a_value)
	{
		return a_value;
	}
	llvm::Value* f_minus_integer(llvm::Value* a_value)
	{
		return CreateMul(a_value, f_integer(-1));
	}
	llvm::Value* f_minus_float(llvm::Value* a_value)
	{
		return CreateFMul(a_value, f_float(-1.0));
	}
	llvm::Value* f_not(llvm::Value* a_value)
	{
		return CreateXor(a_value, getTrue());
	}
	llvm::Value* f_complement(llvm::Value* a_value)
	{
		return CreateXor(a_value, f_integer(~0));
	}
	template<typename T, typename T_x, typename T_y, llvm::Value* (t_jit_generator::*A_operator)(llvm::Value*, llvm::Value*)>
	llvm::BasicBlock* f_binary_xy(llvm::Value* a_stack, llvm::Value* a_x, llvm::Value* a_y, llvm::BasicBlock* a_merge, bool a_tail)
	{
		auto block = f_block("block");
		SetInsertPoint(block);
		auto x = llvm::ConstantInt::classof(a_x) || llvm::ConstantFP::classof(a_x) ? a_x : f_value__union(a_x, f_type<T_x>());
		auto y = llvm::ConstantInt::classof(a_y) || llvm::ConstantFP::classof(a_y) ? a_y : f_value__union(a_y, f_type<T_y>());
		f_primitive_call(a_stack, f_tag<T>(), (this->*A_operator)(x, y), a_tail);
		f_return_or_jump(f_integer(-1), a_merge, a_tail);
		return block;
	}
	template<typename T, typename T_x, llvm::Value* (t_jit_generator::*A_operator)(llvm::Value*, llvm::Value*), llvm::Value* (t_jit_generator::*A_other)(llvm::Value*, llvm::Value*)>
	llvm::BasicBlock* f_binary_xo(llvm::Value* a_stack, llvm::Value* a_x, ast::t_operand::t_tag a_y_tag, llvm::Value* a_y, llvm::BasicBlock* a_merge, llvm::BasicBlock* a_throw, bool a_tail)
	{
		auto thenbb = f_block("then");
		auto elsebb = A_other ? f_block("else") : a_throw;
		auto block = f_block("block");
		SetInsertPoint(block);
		auto type = CreateStructGEP(f_type_object(), f_value__p(a_y), 5);
		CreateCondBr(f_derives<T_x>(f_value__p(type)), thenbb, elsebb);
		SetInsertPoint(thenbb);
		auto x = llvm::ConstantInt::classof(a_x) || llvm::ConstantFP::classof(a_x) ? a_x : f_value__union(a_x, f_type<T_x>());
		auto y = f_value__union(type, f_type<T_x>());
		auto z = (this->*A_operator)(x, y);
		if (A_other) {
			auto mergebb = f_block("merge");
			CreateBr(mergebb);
			thenbb = GetInsertBlock();
			SetInsertPoint(elsebb);
			auto w = (this->*A_other)(x, y);
			CreateBr(mergebb);
			elsebb = GetInsertBlock();
			SetInsertPoint(mergebb);
			auto phi = CreatePHI(f_type<T>(), 2);
			phi->addIncoming(z, thenbb);
			phi->addIncoming(w, elsebb);
			z = phi;
		}
		f_primitive_call(a_stack, f_tag<T>(), z, a_tail);
		if (a_y_tag == ast::t_operand::e_tag__TEMPORARY) f_value__destruct(a_y);
		f_return_or_jump(f_integer(-1), a_merge, a_tail);
		return block;
	}
	template<llvm::BasicBlock* (t_jit_generator::*A_null)(llvm::Value*, llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_boolean)(llvm::Value*, llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_integer)(llvm::Value*, llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_float)(llvm::Value*, llvm::Value*, llvm::Value*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_object)(llvm::Value*, llvm::Value*, ast::t_operand::t_tag, llvm::Value*, llvm::BasicBlock*, llvm::BasicBlock*, bool)>
	llvm::BasicBlock* f_binary_x(llvm::Value* a_stack, llvm::Value* a_x, const ast::t_operand& a_y, llvm::BasicBlock* a_merge, llvm::BasicBlock* a_throw, bool a_tail)
	{
		switch (a_y.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			return A_integer ? (this->*A_integer)(a_stack, a_x, f_integer(a_y.v_integer), a_merge, a_tail) : a_throw;
		case ast::t_operand::e_tag__FLOAT:
			return A_float ? (this->*A_float)(a_stack, a_x, f_float(a_y.v_float), a_merge, a_tail) : a_throw;
		}
		auto block = f_block("block");
		SetInsertPoint(block);
		auto y = f_value(a_y);
		auto objectbb = A_object ? (this->*A_object)(a_stack, a_x, a_y.v_tag, y, a_merge, a_throw, a_tail) : a_throw;
		SetInsertPoint(block);
		auto s = CreateSwitch(f_tag(f_value__p(y)), objectbb, 4);
		s->addCase(f_integer(t_value::e_tag__NULL), A_null ? (this->*A_null)(a_stack, a_x, y, a_merge, a_tail) : a_throw);
		s->addCase(f_integer(t_value::e_tag__BOOLEAN), A_boolean ? (this->*A_boolean)(a_stack, a_x, y, a_merge, a_tail) : a_throw);
		s->addCase(f_integer(t_value::e_tag__INTEGER), A_integer ? (this->*A_integer)(a_stack, a_x, y, a_merge, a_tail) : a_throw);
		s->addCase(f_integer(t_value::e_tag__FLOAT), A_float ? (this->*A_float)(a_stack, a_x, y, a_merge, a_tail) : a_throw);
		return block;
	}
	template<llvm::BasicBlock* (t_jit_generator::*A_null)(llvm::Value*, llvm::Value*, const ast::t_operand&, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_boolean)(llvm::Value*, llvm::Value*, const ast::t_operand&, llvm::BasicBlock*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_integer)(llvm::Value*, llvm::Value*, const ast::t_operand&, llvm::BasicBlock*, llvm::BasicBlock*, bool), llvm::BasicBlock* (t_jit_generator::*A_float)(llvm::Value*, llvm::Value*, const ast::t_operand&, llvm::BasicBlock*, llvm::BasicBlock*, bool)>
	void f_emit_BINARY(const t_operator& a_operator, size_t a_stack, const ast::t_operand& a_x, const ast::t_operand& a_y, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "BINARY %zd %zd %zd %d\n", a_stack, a_x.v_index, a_y.v_index, a_tail);
#endif
		auto throwbb = f_throw_not_supported(a_at);
		auto mergebb = f_block("merge");
		auto block = GetInsertBlock();
		auto stack = f_base(a_stack);
		switch (a_x.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			{
				auto to = A_integer ? (this->*A_integer)(stack, f_integer(a_x.v_integer), a_y, mergebb, throwbb, a_tail) : throwbb;
				SetInsertPoint(block);
				CreateBr(to);
				SetInsertPoint(mergebb);
				return;
			}
		case ast::t_operand::e_tag__FLOAT:
			{
				auto to = A_float ? (this->*A_float)(stack, f_float(a_x.v_float), a_y, mergebb, throwbb, a_tail) : throwbb;
				SetInsertPoint(block);
				CreateBr(to);
				SetInsertPoint(mergebb);
				return;
			}
		}
		auto x = f_value(a_x);
		auto objectbb = f_block("object");
		auto s = CreateSwitch(f_tag(f_value__p(x)), objectbb, 4);
		s->addCase(f_integer(t_value::e_tag__NULL), A_null ? (this->*A_null)(stack, x, a_y, mergebb, a_tail) : throwbb);
		s->addCase(f_integer(t_value::e_tag__BOOLEAN), A_boolean ? (this->*A_boolean)(stack, x, a_y, mergebb, throwbb, a_tail) : throwbb);
		s->addCase(f_integer(t_value::e_tag__INTEGER), A_integer ? (this->*A_integer)(stack, x, a_y, mergebb, throwbb, a_tail) : throwbb);
		s->addCase(f_integer(t_value::e_tag__FLOAT), A_float ? (this->*A_float)(stack, x, a_y, mergebb, throwbb, a_tail) : throwbb);
		SetInsertPoint(objectbb);
		auto y = f_value(a_y);
		auto z = f_object_call(a_operator, stack, a_x, x, a_y, y, a_tail, a_at);
		f_return_or_jump(z, mergebb, a_tail);
		SetInsertPoint(mergebb);
	}
	llvm::Value* f_multiply_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateMul(a_x, a_y);
	}
	llvm::Value* f_multiply_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFMul(a_x, a_y);
	}
	llvm::Value* f_multiply_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFMul(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_multiply_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFMul(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_divide_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateSDiv(a_x, a_y);
	}
	llvm::Value* f_divide_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFDiv(a_x, a_y);
	}
	llvm::Value* f_divide_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFDiv(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_divide_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFDiv(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_modulus(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateSRem(a_x, a_y);
	}
	llvm::Value* f_add_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateAdd(a_x, a_y);
	}
	llvm::Value* f_add_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFAdd(a_x, a_y);
	}
	llvm::Value* f_add_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFAdd(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_add_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFAdd(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_subtract_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateSub(a_x, a_y);
	}
	llvm::Value* f_subtract_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFSub(a_x, a_y);
	}
	llvm::Value* f_subtract_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFSub(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_subtract_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFSub(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_left_shift(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateShl(a_x, a_y);
	}
	llvm::Value* f_right_shift(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateLShr(a_x, a_y);
	}
	llvm::Value* f_less_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateICmpSLT(a_x, a_y);
	}
	llvm::Value* f_less_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOLT(a_x, a_y);
	}
	llvm::Value* f_less_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOLT(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_less_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOLT(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_less_equal_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateICmpSLE(a_x, a_y);
	}
	llvm::Value* f_less_equal_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOLE(a_x, a_y);
	}
	llvm::Value* f_less_equal_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOLE(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_less_equal_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOLE(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_greater_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateICmpSGT(a_x, a_y);
	}
	llvm::Value* f_greater_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOGT(a_x, a_y);
	}
	llvm::Value* f_greater_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOGT(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_greater_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOGT(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_greater_equal_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateICmpSGE(a_x, a_y);
	}
	llvm::Value* f_greater_equal_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOGE(a_x, a_y);
	}
	llvm::Value* f_greater_equal_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOGE(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_greater_equal_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOGE(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	template<bool A_boolean>
	llvm::BasicBlock* f_equals_return(llvm::Value* a_stack, llvm::Value* a_x, llvm::Value* a_y, llvm::BasicBlock* a_merge, bool a_tail)
	{
		auto block = f_block("block");
		SetInsertPoint(block);
		f_primitive_call(a_stack, f_tag<bool>(), A_boolean ? getTrue() : getFalse(), a_tail);
		f_return_or_jump(f_integer(-1), a_merge, a_tail);
		return block;
	}
	template<bool A_not>
	llvm::BasicBlock* f_equals_null(llvm::Value* a_stack, llvm::Value* a_x, const ast::t_operand& a_y, llvm::BasicBlock* a_merge, bool a_tail)
	{
		switch (a_y.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
		case ast::t_operand::e_tag__FLOAT:
			return f_equals_return<A_not>(a_stack, nullptr, nullptr, a_merge, a_tail);
		}
		auto block = f_block("block");
		SetInsertPoint(block);
		auto y = f_value(a_y);
		auto tag = f_tag(f_value__p(y));
		auto b = A_not ? CreateICmpNE(tag, f_integer(t_value::e_tag__NULL)) : CreateICmpEQ(tag, f_integer(t_value::e_tag__NULL));
		f_primitive_call(a_stack, f_tag<bool>(), b, a_tail);
		if (a_y.v_tag == ast::t_operand::e_tag__TEMPORARY) f_value__destruct(y);
		f_return_or_jump(f_integer(-1), a_merge, a_tail);
		return block;
	}
	template<bool A_not>
	llvm::BasicBlock* f_equals_boolean(llvm::Value* a_stack, llvm::Value* a_x, const ast::t_operand& a_y, llvm::BasicBlock* a_merge, llvm::BasicBlock* a_throw, bool a_tail)
	{
		switch (a_y.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
		case ast::t_operand::e_tag__FLOAT:
			return f_equals_return<A_not>(a_stack, nullptr, nullptr, a_merge, a_tail);
		}
		auto block = f_block("block");
		SetInsertPoint(block);
		auto mergebb = f_block("merge");
		auto thenbb = f_block("then");
		auto elsebb = f_block("else");
		auto y = f_value(a_y);
		CreateCondBr(CreateICmpEQ(f_tag(f_value__p(y)), f_integer(t_value::e_tag__BOOLEAN)), thenbb, elsebb);
		SetInsertPoint(thenbb);
		auto xb = f_value__union(a_x, f_type<bool>());
		auto yb = f_value__union(y, f_type<bool>());
		auto b0 = A_not ? CreateICmpNE(xb, yb) : CreateICmpEQ(xb, yb);
		CreateBr(mergebb);
		SetInsertPoint(elsebb);
		auto b1 = A_not ? getTrue() : getFalse();
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
		auto phi = CreatePHI(f_type<bool>(), 2);
		phi->addIncoming(b0, thenbb);
		phi->addIncoming(b1, elsebb);
		f_primitive_call(a_stack, f_tag<bool>(), phi, a_tail);
		if (a_y.v_tag == ast::t_operand::e_tag__TEMPORARY) f_value__destruct(y);
		f_return_or_jump(f_integer(-1), a_merge, a_tail);
		return block;
	}
	template<bool A_boolean>
	llvm::Value* f_equals_other(llvm::Value* a_x, llvm::Value* a_y)
	{
		return A_boolean ? getTrue() : getFalse();
	}
	llvm::Value* f_equals_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateICmpEQ(a_x, a_y);
	}
	llvm::Value* f_equals_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOEQ(a_x, a_y);
	}
	llvm::Value* f_equals_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOEQ(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_equals_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpOEQ(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_not_equals_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateICmpNE(a_x, a_y);
	}
	llvm::Value* f_not_equals_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpONE(a_x, a_y);
	}
	llvm::Value* f_not_equals_integer_float(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpONE(CreateSIToFP(a_x, getDoubleTy()), a_y);
	}
	llvm::Value* f_not_equals_float_integer(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateFCmpONE(a_x, CreateSIToFP(a_y, getDoubleTy()));
	}
	llvm::Value* f_and(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateAnd(a_x, a_y);
	}
	llvm::Value* f_xor(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateXor(a_x, a_y);
	}
	llvm::Value* f_or(llvm::Value* a_x, llvm::Value* a_y)
	{
		return CreateOr(a_x, a_y);
	}
	template<bool A_not>
	void f_emit_IDENTICAL(size_t a_stack, const ast::t_operand& a_x, const ast::t_operand& a_y, bool a_tail)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "%sIDENTICAL %zd %zd %zd %d\n", A_not ? "NOT " : "", a_stack, a_x.v_index, a_y.v_index, a_tail);
#endif
		auto x = f_value(a_x);
		llvm::Value* xtag;
		switch (a_x.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			xtag = f_integer(f_tag<intptr_t>());
			break;
		case ast::t_operand::e_tag__FLOAT:
			xtag = f_integer(f_tag<double>());
			break;
		default:
			xtag = f_tag(f_value__p(x));
		}
		auto y = f_value(a_y);
		llvm::Value* ytag;
		switch (a_y.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			ytag = f_integer(f_tag<intptr_t>());
			break;
		case ast::t_operand::e_tag__FLOAT:
			ytag = f_integer(f_tag<double>());
			break;
		default:
			ytag = f_tag(f_value__p(y));
		}
		auto mergebb = f_block("merge");
		auto thenbb = f_block("then");
		auto elsebb = f_block("else");
		CreateCondBr(CreateICmpEQ(xtag, ytag), thenbb, elsebb);
		SetInsertPoint(thenbb);
		llvm::Value* b0;
		switch (a_x.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
			{
				auto yi = llvm::ConstantInt::classof(y) ? y : f_value__union(y, f_type<intptr_t>());
				b0 = A_not ? CreateICmpNE(x, yi) : CreateICmpEQ(x, yi);
			}
			break;
		case ast::t_operand::e_tag__FLOAT:
			{
				auto yf = llvm::ConstantFP::classof(y) ? y : f_value__union(y, f_type<double>());
				b0 = A_not ? CreateFCmpONE(x, yf) : CreateFCmpOEQ(x, yf);
			}
			break;
		default:
			switch (a_y.v_tag) {
			case ast::t_operand::e_tag__INTEGER:
				{
					auto xi = f_value__union(x, f_type<intptr_t>());
					b0 = A_not ? CreateICmpNE(xi, y) : CreateICmpEQ(xi, y);
				}
				break;
			case ast::t_operand::e_tag__FLOAT:
				{
					auto xf = f_value__union(x, f_type<double>());
					b0 = A_not ? CreateFCmpONE(xf, y) : CreateFCmpOEQ(xf, y);
				}
				break;
			default:
				{
					auto truebb = f_block("true");
					auto booleanbb = f_block("boolean");
					auto integerbb = f_block("integer");
					auto floatbb = f_block("float");
					auto mergebb = f_block("merge");
					auto s = CreateSwitch(xtag, truebb, 4);
					s->addCase(f_integer(t_value::e_tag__NULL), truebb);
					s->addCase(f_integer(t_value::e_tag__BOOLEAN), booleanbb);
					s->addCase(f_integer(t_value::e_tag__INTEGER), integerbb);
					s->addCase(f_integer(t_value::e_tag__FLOAT), floatbb);
					SetInsertPoint(truebb);
					auto b1 = A_not ? getFalse() : getTrue();
					CreateBr(mergebb);
					SetInsertPoint(booleanbb);
					auto xb = f_value__union(x, f_type<bool>());
					auto yb = f_value__union(y, f_type<bool>());
					auto b2 = A_not ? CreateICmpNE(xb, yb) : CreateICmpEQ(xb, yb);
					CreateBr(mergebb);
					SetInsertPoint(integerbb);
					auto xi = f_value__union(x, f_type<intptr_t>());
					auto yi = f_value__union(y, f_type<intptr_t>());
					auto b3 = A_not ? CreateICmpNE(xi, yi) : CreateICmpEQ(xi, yi);
					CreateBr(mergebb);
					SetInsertPoint(floatbb);
					auto xf = f_value__union(x, f_type<double>());
					auto yf = f_value__union(y, f_type<double>());
					auto b4 = A_not ? CreateFCmpONE(xf, yf) : CreateFCmpOEQ(xf, yf);
					CreateBr(mergebb);
					SetInsertPoint(mergebb);
					auto phi = CreatePHI(f_type<bool>(), 4);
					phi->addIncoming(b1, truebb);
					phi->addIncoming(b2, booleanbb);
					phi->addIncoming(b3, integerbb);
					phi->addIncoming(b4, floatbb);
					b0 = phi;
					thenbb = mergebb;
				}
			}
		}
		CreateBr(mergebb);
		SetInsertPoint(elsebb);
		auto b1 = A_not ? getTrue() : getFalse();
		CreateBr(mergebb);
		SetInsertPoint(mergebb);
		auto phi = CreatePHI(f_type<bool>(), 2);
		phi->addIncoming(b0, thenbb);
		phi->addIncoming(b1, elsebb);
		auto stack = f_base(a_stack);
		f_primitive_call(stack, f_tag<bool>(), phi, a_tail);
		if (a_x.v_tag == ast::t_operand::e_tag__TEMPORARY) f_value__destruct(x);
		if (a_y.v_tag == ast::t_operand::e_tag__TEMPORARY) f_value__destruct(y);
		if (a_tail) f_return(f_integer(-1));
	}
	void f_emit_SEND(size_t a_stack, const ast::t_operand& a_x, const ast::t_operand& a_y, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SEND %zd %zd %zd %d\n", a_stack, a_x.v_index, a_y.v_index, a_tail);
#endif
		auto throwbb = f_throw_not_supported(a_at);
		auto mergebb = f_block("merge");
		switch (a_x.v_tag) {
		case ast::t_operand::e_tag__INTEGER:
		case ast::t_operand::e_tag__FLOAT:
			CreateBr(throwbb);
			SetInsertPoint(mergebb);
			return;
		}
		auto elsebb = f_block("else");
		auto x = f_value(a_x);
		auto tag = f_tag(f_value__p(x));
		CreateCondBr(CreateICmpULT(tag, f_integer(t_value::e_tag__OBJECT)), throwbb, elsebb);
		SetInsertPoint(elsebb);
		auto stack = f_base(a_stack);
		auto y = f_value(a_y);
		auto z = f_object_call(v_globals.v_send, stack, a_x, x, a_y, y, a_tail, a_at);
		f_return_or_jump(z, mergebb, a_tail);
		SetInsertPoint(mergebb);
	}
	void f_emit_RETURN(size_t a_stack)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "RETURN %zd\n", a_stack);
#endif
		auto stack = f_base(a_stack);
		f_value__construct_move(f_base(-2), stack);
		f_context__pop();
		CreateRet(f_integer(-1));
	}
	void f_emit_CALL(size_t a_stack, size_t a_n, bool a_expand, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "CALL %zd %zd %d %d\n", a_stack, a_n, a_expand, a_tail);
#endif
		auto stack = f_base(a_stack);
		llvm::Value* n = f_integer(a_n);
		if (a_expand) n = f_invoke(v_globals.v_expand, {stack, n}, a_at, [] {});
		if (a_tail) {
			f_invoke(v_globals.v_tail, {v_context, stack, n}, a_at, [] {});
			f_return(n);
		} else {
			f_invoke(v_globals.v_call, {stack, n}, a_at, [] {});
		}
	}
	void f_emit_CALL_OUTER(size_t a_stack, size_t a_index, size_t a_n, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "CALL OUTER %zd %zd %zd %d\n", a_stack, a_index, a_n, a_tail);
#endif
		auto stack = f_base(a_stack);
		auto x = f_entry(f_outer_as_scope(), a_index);
		auto n = f_integer(a_n);
		if (a_tail) {
			f_value__construct(stack, x);
			f_emit_NUL(a_stack + 1);
			f_invoke(v_globals.v_tail, {v_context, stack, n}, a_at, [] {});
			f_return(n);
		} else {
			f_emit_NUL(a_stack + 1);
			f_invoke(v_globals.v_value__call, {x, stack, n}, a_at, [] {});
		}
	}
	void f_emit_GET_AT(size_t a_stack, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "GET AT %zd %d\n", a_stack, a_tail);
#endif
		auto stack = f_base(a_stack);
		if (a_tail)
			f_return(f_invoke(v_globals.v_get_at_tail, {v_context, v_base, stack}, a_at, [] {}));
		else
			f_invoke(v_globals.v_get_at, {stack}, a_at, [] {});
	}
	void f_emit_SET_AT(size_t a_stack, bool a_tail, const t_at& a_at)
	{
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SET AT %zd %d\n", a_stack, a_tail);
#endif
		auto stack = f_base(a_stack);
		if (a_tail)
			f_return(f_invoke(v_globals.v_set_at_tail, {v_context, v_base, stack}, a_at, [] {}));
		else
			f_invoke(v_globals.v_set_at, {stack}, a_at, [] {});
	}
	void f_emit_SAFE_POINT(const t_at& a_at)
	{
		if (!v_globals.v_generator.v_safe_points) return;
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "SAFE POINT\n");
#endif
		size_t n = v_code.f_last();
		v_globals.v_generator.v_safe_positions->push_back(std::make_tuple(a_at.f_line(), n, a_at.f_column()));
		f_at(a_at);
		v_code.v_instructions[n] = v_code.f_p(e_instruction__SAFE_POINT);
		CreateCall(v_globals.v_safe_point, {v_context});
	}
};

template<>
inline t_value::t_tag t_jit_generator::f_tag<bool>()
{
	return t_value::e_tag__BOOLEAN;
}

template<>
inline t_value::t_tag t_jit_generator::f_tag<intptr_t>()
{
	return t_value::e_tag__INTEGER;
}

template<>
inline t_value::t_tag t_jit_generator::f_tag<double>()
{
	return t_value::e_tag__FLOAT;
}

template<>
inline llvm::Type* t_jit_generator::f_type<bool>()
{
	return getInt1Ty();
}

template<>
inline llvm::Type* t_jit_generator::f_type<intptr_t>()
{
	return getIntNTy(sizeof(intptr_t) * 8);
}

template<>
inline llvm::Type* t_jit_generator::f_type<double>()
{
	return getDoubleTy();
}

template<>
inline llvm::Value* t_jit_generator::f_derives<intptr_t>(llvm::Value* a_p)
{
	return CreateCall(v_globals.v_derives_integer, {a_p});
}

template<>
inline llvm::Value* t_jit_generator::f_derives<double>(llvm::Value* a_p)
{
	return CreateCall(v_globals.v_derives_float, {a_p});
}

t_generator::t_generator(t_object* a_module, std::map<std::pair<size_t, void**>, size_t>* a_safe_points) : v_module(a_module), v_safe_points(a_safe_points)
{
	v_jit = new t_jit_generator::t_globals(*this);
}

t_generator::~t_generator()
{
	delete static_cast<t_jit_generator::t_globals*>(v_jit);
}

void t_generator::f_jit_install()
{
	static_cast<t_jit_generator::t_globals*>(v_jit)->f_install();
}

namespace ast
{

namespace
{

void f_generate_block(t_jit_generator& a_generator, size_t a_stack, const std::vector<std::unique_ptr<t_node>>& a_nodes, bool a_tail, bool a_clear)
{
	auto i = a_nodes.begin();
	auto j = a_nodes.end();
	if (i == j) {
		if (a_clear) return;
		a_generator.f_reserve(a_stack + 1);
		a_generator.f_emit_NUL(a_stack);
	} else {
		for (--j; i != j; ++i) (*i)->f_generate(a_generator, a_stack, false, false, true);
		(*i)->f_generate(a_generator, a_stack, a_tail, false, a_clear);
	}
}

void f_generate_block_without_value(t_jit_generator& a_generator, size_t a_stack, const std::vector<std::unique_ptr<t_node>>& a_nodes)
{
	for (auto& p : a_nodes) p->f_generate(a_generator, a_stack, false, false, true);
}

}

void t_lambda::f_jit_generate(t_generator& a_generator, t_code& a_code)
{
	auto safe_positions0 = a_generator.v_safe_positions;
	std::vector<std::tuple<size_t, size_t, size_t>> safe_positions1;
	a_generator.v_safe_positions = &safe_positions1;
	auto name = f_name(this);
	auto& globals = *static_cast<t_jit_generator::t_globals*>(a_generator.v_jit);
	auto function = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer, llvm::Function::ExternalLinkage, name, globals.v_module.get());
	t_jit_generator generator(function, a_code, globals);
	size_t stack = v_privates.size();
	auto returnbb = generator.f_block("return");
	t_jit_generator::t_targets targets{nullptr, false, false, nullptr, returnbb, true};
	generator.v_targets = &targets;
	if (v_self_shared) {
		generator.f_reserve(stack + 1);
		generator.f_emit_SELF(stack, 0);
		generator.f_emit_SCOPE_PUT(stack, 0, 0, true);
	}
	for (size_t i = 0; i < v_arguments; ++i) {
		if (!v_privates[i]->v_shared) continue;
		generator.f_reserve(stack + 1);
		generator.f_emit_STACK_GET(stack, i);
		generator.f_emit_SCOPE_PUT(stack, 0, v_privates[i]->v_index, true);
	}
	f_generate_block(generator, stack, v_block, true, false);
	generator.CreateBr(returnbb);
	generator.SetInsertPoint(returnbb);
	generator.f_emit_RETURN(stack);
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "VERIFYING A FUNCTION...\n");
#endif
	llvm::verifyFunction(*function);
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "RUNNING FPM...\n");
#endif
	globals.v_fpm->run(*function);
#ifdef XEMMAI__JIT__DEBUG_PRINT
std::fprintf(stderr, "DONE.\n");
#endif
	a_generator.v_safe_positions = safe_positions0;
	if (a_generator.v_safe_points) f_safe_points(a_code, *a_generator.v_safe_points, safe_positions1);
	globals.v_codes.emplace_back(a_code, name);
}

void t_lambda::f_jit_generate_with_lock(t_generator& a_generator, t_code& a_code)
{
	std::lock_guard<std::mutex> lock(f_jit()->v_mutex);
	f_jit_generate(a_generator, a_code);
}

t_operand t_lambda::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	auto code = f_code(a_generator.v_code.v_module);
	f_jit_generate(a_generator.v_globals.v_generator, f_as<t_code&>(code));
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit_SAFE_POINT(v_at);
	if (v_variadic || v_defaults.size() > 0) {
		for (size_t i = 0; i < v_defaults.size(); ++i) v_defaults[i]->f_generate(a_generator, a_stack + i, false, false);
		a_generator.f_emit_LAMBDA(a_stack, code, true);
	} else {
		a_generator.f_emit_LAMBDA(a_stack, code, false);
	}
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_if::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_expression->f_generate(a_generator, a_stack, false, false);
	auto thenbb = a_generator.f_block("then");
	auto elsebb = a_generator.f_block("else");
	auto mergebb = a_generator.f_block("merge");
	a_generator.f_emit_BRANCH(a_stack, thenbb, elsebb);
	a_generator.SetInsertPoint(thenbb);
	f_generate_block(a_generator, a_stack, v_true, a_tail, a_clear);
	a_generator.CreateBr(mergebb);
	a_generator.SetInsertPoint(elsebb);
	f_generate_block(a_generator, a_stack, v_false, a_tail, a_clear);
	a_generator.CreateBr(mergebb);
	a_generator.SetInsertPoint(mergebb);
	return a_stack;
}

t_operand t_while::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	auto loopbb = a_generator.f_block("loop");
	auto thenbb = a_generator.f_block("then");
	auto elsebb = a_generator.f_block("else");
	auto breakbb = a_generator.f_block("break");
	a_generator.CreateBr(loopbb);
	a_generator.SetInsertPoint(loopbb);
	v_expression->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_BRANCH(a_stack, thenbb, elsebb);
	a_generator.SetInsertPoint(thenbb);
	auto targets0 = a_generator.v_targets;
	t_jit_generator::t_targets targets1{breakbb, a_tail, a_clear, loopbb, targets0->v_return, targets0->v_return_is_tail};
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, a_stack, v_block);
	a_generator.v_targets = targets0;
	a_generator.CreateBr(loopbb);
	a_generator.SetInsertPoint(elsebb);
	if (!a_clear) {
		a_generator.f_reserve(a_stack + 1);
		a_generator.f_emit_NUL(a_stack);
	}
	a_generator.CreateBr(breakbb);
	a_generator.SetInsertPoint(breakbb);
	return a_stack;
}

t_operand t_for::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	f_generate_block_without_value(a_generator, a_stack, v_initialization);
	auto loopbb = a_generator.f_block("loop");
	auto continuebb = a_generator.f_block("continue");
	auto thenbb = a_generator.f_block("then");
	auto elsebb = a_generator.f_block("else");
	auto breakbb = a_generator.f_block("break");
	a_generator.CreateBr(loopbb);
	a_generator.SetInsertPoint(loopbb);
	if (v_condition) {
		v_condition->f_generate(a_generator, a_stack, false, false);
		a_generator.f_emit_BRANCH(a_stack, thenbb, elsebb);
	} else {
		a_generator.CreateBr(thenbb);
	}
	a_generator.SetInsertPoint(thenbb);
	auto targets0 = a_generator.v_targets;
	t_jit_generator::t_targets targets1{breakbb, a_tail, a_clear, continuebb, targets0->v_return, targets0->v_return_is_tail};
	a_generator.v_targets = &targets1;
	f_generate_block_without_value(a_generator, a_stack, v_block);
	a_generator.v_targets = targets0;
	a_generator.CreateBr(continuebb);
	a_generator.SetInsertPoint(continuebb);
	f_generate_block_without_value(a_generator, a_stack, v_next);
	a_generator.CreateBr(loopbb);
	a_generator.SetInsertPoint(elsebb);
	if (!a_clear) {
		a_generator.f_reserve(a_stack + 1);
		a_generator.f_emit_NUL(a_stack);
	}
	a_generator.CreateBr(breakbb);
	a_generator.SetInsertPoint(breakbb);
	return a_stack;
}

t_operand t_break::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_expression) {
		v_expression->f_generate(a_generator, a_stack, a_generator.v_targets->v_break_is_tail, false, a_generator.v_targets->v_break_must_clear);
	} else if (!a_generator.v_targets->v_break_must_clear) {
		a_generator.f_reserve(a_stack + 1);
		a_generator.f_emit_NUL(a_stack);
	}
	a_generator.CreateBr(a_generator.v_targets->v_break);
	a_generator.SetInsertPoint(a_generator.f_block("unreachable"));
	return a_stack;
}

t_operand t_continue::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	a_generator.CreateBr(a_generator.v_targets->v_continue);
	a_generator.SetInsertPoint(a_generator.f_block("unreachable"));
	return a_stack;
}

t_operand t_return::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (v_expression) {
		v_expression->f_generate(a_generator, a_stack, a_generator.v_targets->v_return_is_tail, false);
	} else {
		a_generator.f_reserve(a_stack + 1);
		a_generator.f_emit_NUL(a_stack);
	}
	a_generator.CreateBr(a_generator.v_targets->v_return);
	a_generator.SetInsertPoint(a_generator.f_block("unreachable"));
	return a_stack;
}

void* t_try::f_jit_try(t_jit_generator& a_generator, size_t a_stack, bool a_clear)
{
	auto function = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer, llvm::Function::PrivateLinkage, "try", a_generator.GetInsertBlock()->getModule());
	t_jit_generator generator(function, a_generator.v_code, a_generator.v_globals);
	auto stepbb = generator.f_block("step");
	auto breakbb = a_generator.v_targets->v_break ? generator.f_block("break") : nullptr;
	auto continuebb = a_generator.v_targets->v_continue ? generator.f_block("continue") : nullptr;
	auto returnbb = a_generator.v_targets->v_return ? generator.f_block("return") : nullptr;
	t_jit_generator::t_targets targets{breakbb, false, a_generator.v_targets->v_break_must_clear, continuebb, returnbb, false};
	generator.v_targets = &targets;
	f_generate_block(generator, a_stack, v_block, false, a_clear);
	generator.CreateBr(stepbb);
	generator.SetInsertPoint(stepbb);
	generator.CreateRet(generator.f_integer(t_code::e_try__STEP));
	if (breakbb) {
		generator.SetInsertPoint(breakbb);
		generator.CreateRet(generator.f_integer(t_code::e_try__BREAK));
	}
	if (continuebb) {
		generator.SetInsertPoint(continuebb);
		generator.CreateRet(generator.f_integer(t_code::e_try__CONTINUE));
	}
	if (returnbb) {
		generator.SetInsertPoint(returnbb);
		generator.CreateRet(generator.f_integer(t_code::e_try__RETURN));
	}
	llvm::verifyFunction(*function);
	generator.v_globals.v_fpm->run(*function);
	return function;
}

void* t_try::f_jit_catch(t_jit_generator& a_generator, size_t a_stack, bool a_clear)
{
	auto function = llvm::Function::Create(f_jit()->v_ft_size_t__context_pointer__value_pointer, llvm::Function::PrivateLinkage, "catch", a_generator.GetInsertBlock()->getModule());
	t_jit_generator generator(function, a_generator.v_code, a_generator.v_globals);
	auto thrown = ++function->arg_begin();
	auto caught = generator.CreateLoad(generator.v_pc);
	auto stepbb = generator.f_block("step");
	auto breakbb = a_generator.v_targets->v_break ? generator.f_block("break") : nullptr;
	auto continuebb = a_generator.v_targets->v_continue ? generator.f_block("continue") : nullptr;
	auto returnbb = a_generator.v_targets->v_return ? generator.f_block("return") : nullptr;
	t_jit_generator::t_targets targets{breakbb, false, a_generator.v_targets->v_break_must_clear, continuebb, returnbb, false};
	generator.v_targets = &targets;
	for (auto& p : v_catches) {
		p->v_expression->f_generate(generator, a_stack, false, false);
		auto nextbb = generator.f_block("next");
		generator.f_emit_CATCH(a_stack, thrown, caught, p->v_variable, nextbb);
		f_generate_block(generator, a_stack, p->v_block, false, a_clear);
		generator.CreateBr(stepbb);
		generator.SetInsertPoint(nextbb);
	}
	generator.CreateStore(caught, generator.v_pc);
	generator.f_throw(thrown);
	generator.SetInsertPoint(stepbb);
	generator.CreateRet(generator.f_integer(t_code::e_try__STEP));
	if (breakbb) {
		generator.SetInsertPoint(breakbb);
		generator.CreateRet(generator.f_integer(t_code::e_try__BREAK));
	}
	if (continuebb) {
		generator.SetInsertPoint(continuebb);
		generator.CreateRet(generator.f_integer(t_code::e_try__CONTINUE));
	}
	if (returnbb) {
		generator.SetInsertPoint(returnbb);
		generator.CreateRet(generator.f_integer(t_code::e_try__RETURN));
	}
	llvm::verifyFunction(*function);
	generator.v_globals.v_fpm->run(*function);
	return function;
}

void* t_try::f_jit_finally(t_jit_generator& a_generator, size_t a_stack)
{
	auto function = llvm::Function::Create(f_jit()->v_ft_void__context_pointer, llvm::Function::PrivateLinkage, "finally", a_generator.GetInsertBlock()->getModule());
	t_jit_generator generator(function, a_generator.v_code, a_generator.v_globals);
	t_jit_generator::t_targets targets{nullptr, false, false, nullptr, nullptr, false};
	generator.v_targets = &targets;
	f_generate_block_without_value(generator, a_stack + 1, v_finally);
	generator.CreateRetVoid();
	llvm::verifyFunction(*function);
	generator.v_globals.v_fpm->run(*function);
	return function;
}

t_operand t_try::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	auto try0 = static_cast<llvm::Function*>(f_jit_try(a_generator, a_stack, a_clear));
	auto catch0 = static_cast<llvm::Function*>(f_jit_catch(a_generator, a_stack, a_clear));
	auto finally0 = static_cast<llvm::Function*>(f_jit_finally(a_generator, a_stack));
	a_generator.f_reserve(a_stack + 1);
	a_generator.f_emit_TRY(a_stack, try0, catch0, finally0);
	return a_stack;
}

t_operand t_throw::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_expression->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_THROW(a_stack, v_at);
	return a_stack;
}

t_operand t_object_get::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_GET(a_stack, v_key, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

void t_object_get::f_method_get(t_jit_generator& a_generator, size_t a_stack)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_METHOD_GET(a_stack, v_key, v_at);
}

t_operand t_object_get_indirect::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_GET_INDIRECT(a_stack, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_object_put::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_value->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_PUT(a_stack, v_key, a_clear, v_at);
	return a_stack;
}

t_operand t_object_put_indirect::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	v_value->f_generate(a_generator, a_stack + 2, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_PUT_INDIRECT(a_stack, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_object_has::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_HAS(a_stack, v_key);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_object_has_indirect::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_HAS_INDIRECT(a_stack);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_object_remove::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_REMOVE(a_stack, v_key, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_object_remove_indirect::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_key->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_OBJECT_REMOVE_INDIRECT(a_stack, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_symbol_get::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	f_resolve();
	if (!v_variable) {
		a_generator.f_reserve(a_stack + 1);
		if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
		a_generator.f_emit_GLOBAL_GET(a_stack, v_symbol, v_at);
	} else if (v_variable->v_shared) {
		a_generator.f_reserve(a_stack + 1);
		if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
		a_generator.f_emit_SCOPE_GET(a_stack, v_resolved, v_variable->v_index, v_variable->v_varies);
	} else {
		if (a_operand) return t_operand(t_operand::e_tag__VARIABLE, v_variable->v_index);
		if (a_tail) {
			a_generator.f_emit_SAFE_POINT(v_at);
			a_generator.f_emit_RETURN(v_variable->v_index);
			a_generator.SetInsertPoint(a_generator.f_block("unreachable"));
		} else {
			a_generator.f_reserve(a_stack + 1);
			a_generator.f_emit_STACK_GET(a_stack, v_variable->v_index);
		}
	}
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_scope_put::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_value->f_generate(a_generator, a_stack, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	if (v_variable.v_shared)
		a_generator.f_emit_SCOPE_PUT(a_stack, v_outer, v_variable.v_index, a_clear);
	else
		a_generator.f_emit_STACK_PUT(a_stack, v_variable.v_index, a_clear);
	return a_stack;
}

t_operand t_self::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_clear) return a_stack;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_SELF(a_stack, v_outer);
	return a_stack;
}

t_operand t_class::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_CLASS(a_stack);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_super::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_SUPER(a_stack, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_null::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return t_value::v_null;
	if (a_clear) return a_stack;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_NUL(a_stack);
	return a_stack;
}

t_operand t_boolean::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value ? t_value::v_true : t_value::v_false;
	if (a_clear) return a_stack;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_BOOLEAN(a_stack, v_value);
	return a_stack;
}

t_operand t_integer::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return a_stack;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_INTEGER(a_stack, v_value);
	return a_stack;
}

t_operand t_float::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return a_stack;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_FLOAT(a_stack, v_value);
	return a_stack;
}

t_operand t_instance::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	if (a_operand) return v_value;
	if (a_clear) return a_stack;
	a_generator.f_reserve(a_stack + 1);
	if (a_tail) a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_INSTANCE(a_stack, v_value);
	return a_stack;
}

t_operand t_unary::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	auto operand = v_expression->f_generate(a_generator, a_stack + 1, false, true);
	if (operand.v_tag == t_operand::e_tag__INTEGER) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_integer(v_at, operand.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
		case e_instruction__MINUS_T:
			return t_integer(v_at, -operand.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
		case e_instruction__COMPLEMENT_T:
			return t_integer(v_at, ~operand.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
		default:
			t_throwable::f_throw(L"not supported");
		}
	} else if (operand.v_tag == t_operand::e_tag__FLOAT) {
		switch (v_instruction) {
		case e_instruction__PLUS_T:
			return t_float(v_at, operand.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
		case e_instruction__MINUS_T:
			return t_float(v_at, -operand.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
		default:
			t_throwable::f_throw(L"not supported");
		}
	}
	a_generator.f_reserve(a_stack + 2);
	a_generator.f_emit_SAFE_POINT(v_at);
	switch (v_instruction) {
	case e_instruction__PLUS_T:
		a_generator.f_emit_UNARY<nullptr, &t_jit_generator::f_unary<intptr_t, &t_jit_generator::f_plus>, &t_jit_generator::f_unary<double, &t_jit_generator::f_plus>>(a_generator.v_globals.v_plus, a_stack, operand, a_tail, v_at);
		break;
	case e_instruction__MINUS_T:
		a_generator.f_emit_UNARY<nullptr, &t_jit_generator::f_unary<intptr_t, &t_jit_generator::f_minus_integer>, &t_jit_generator::f_unary<double, &t_jit_generator::f_minus_float>>(a_generator.v_globals.v_minus, a_stack, operand, a_tail, v_at);
		break;
	case e_instruction__NOT_T:
		a_generator.f_emit_UNARY<&t_jit_generator::f_unary<bool, &t_jit_generator::f_not>, nullptr, nullptr>(a_generator.v_globals.v_not, a_stack, operand, a_tail, v_at);
		break;
	case e_instruction__COMPLEMENT_T:
		a_generator.f_emit_UNARY<nullptr, &t_jit_generator::f_unary<intptr_t, &t_jit_generator::f_complement>, nullptr>(a_generator.v_globals.v_complement, a_stack, operand, a_tail, v_at);
		break;
	}
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_binary::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	bool operand = v_instruction != e_instruction__SEND;
	auto left = v_left->f_generate(a_generator, a_stack + 1, false, operand);
	auto right = v_right->f_generate(a_generator, a_stack + (left.v_tag == t_operand::e_tag__TEMPORARY ? 2 : 1), false, operand);
	if (left.v_tag == t_operand::e_tag__INTEGER) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_integer(v_at, left.v_integer * right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_integer(v_at, left.v_integer / right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__MODULUS_TT:
				return t_integer(v_at, left.v_integer % right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_integer(v_at, left.v_integer + right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_integer(v_at, left.v_integer - right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LEFT_SHIFT_TT:
				return t_integer(v_at, left.v_integer << right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__RIGHT_SHIFT_TT:
				return t_integer(v_at, static_cast<size_t>(left.v_integer) >> right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_integer < right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_integer <= right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_integer > right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_integer >= right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, left.v_integer == right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, left.v_integer != right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__AND_TT:
				return t_integer(v_at, left.v_integer & right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__XOR_TT:
				return t_integer(v_at, left.v_integer ^ right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__OR_TT:
				return t_integer(v_at, left.v_integer | right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_integer * right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_integer / right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_integer + right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_integer - right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_integer < right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_integer <= right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_integer > right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_integer >= right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
				return t_boolean(v_at, left.v_integer == right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
				return t_boolean(v_at, left.v_integer != right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, false).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, true).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		}
	} else if (left.v_tag == t_operand::e_tag__FLOAT) {
		if (right.v_tag == t_operand::e_tag__INTEGER) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_float * right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_float / right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_float + right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_float - right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_float < right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_float <= right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_float > right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_float >= right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
				return t_boolean(v_at, left.v_float == right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
				return t_boolean(v_at, left.v_float != right.v_integer).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, false).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, true).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		} else if (right.v_tag == t_operand::e_tag__FLOAT) {
			switch (v_instruction) {
			case e_instruction__MULTIPLY_TT:
				return t_float(v_at, left.v_float * right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__DIVIDE_TT:
				return t_float(v_at, left.v_float / right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__ADD_TT:
				return t_float(v_at, left.v_float + right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__SUBTRACT_TT:
				return t_float(v_at, left.v_float - right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_TT:
				return t_boolean(v_at, left.v_float < right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__LESS_EQUAL_TT:
				return t_boolean(v_at, left.v_float <= right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_TT:
				return t_boolean(v_at, left.v_float > right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__GREATER_EQUAL_TT:
				return t_boolean(v_at, left.v_float >= right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__EQUALS_TT:
			case e_instruction__IDENTICAL_TT:
				return t_boolean(v_at, left.v_float == right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			case e_instruction__NOT_EQUALS_TT:
			case e_instruction__NOT_IDENTICAL_TT:
				return t_boolean(v_at, left.v_float != right.v_float).f_generate(a_generator, a_stack, a_tail, a_operand, a_clear);
			default:
				t_throwable::f_throw(L"not supported");
			}
		}
	}
	a_generator.f_reserve(a_stack + 3);
	a_generator.f_emit_SAFE_POINT(v_at);
	switch (v_instruction) {
#define XEMMAI__JIT__BINARY_ARITHMETIC(a_label, a_operator)\
	case e_instruction__##a_label##_TT:\
		a_generator.f_emit_BINARY<nullptr, nullptr,\
			&t_jit_generator::f_binary_x<nullptr, nullptr,\
				&t_jit_generator::f_binary_xy<intptr_t, intptr_t, intptr_t, &t_jit_generator::f_##a_operator##_integer>,\
				&t_jit_generator::f_binary_xy<double, intptr_t, double, &t_jit_generator::f_##a_operator##_integer_float>,\
				&t_jit_generator::f_binary_xo<intptr_t, intptr_t, &t_jit_generator::f_##a_operator##_integer, nullptr>\
			>,\
			&t_jit_generator::f_binary_x<nullptr, nullptr,\
				&t_jit_generator::f_binary_xy<double, double, intptr_t, &t_jit_generator::f_##a_operator##_float_integer>,\
				&t_jit_generator::f_binary_xy<double, double, double, &t_jit_generator::f_##a_operator##_float>,\
				&t_jit_generator::f_binary_xo<double, double, &t_jit_generator::f_##a_operator##_float, nullptr>\
			>\
		>(a_generator.v_globals.v_##a_operator, a_stack, left, right, a_tail, v_at);\
		break;
#define XEMMAI__JIT__BINARY_INTEGRAL(a_label, a_operator)\
	case e_instruction__##a_label##_TT:\
		a_generator.f_emit_BINARY<nullptr, nullptr,\
			&t_jit_generator::f_binary_x<nullptr, nullptr,\
				&t_jit_generator::f_binary_xy<intptr_t, intptr_t, intptr_t, &t_jit_generator::f_##a_operator>, nullptr,\
				&t_jit_generator::f_binary_xo<intptr_t, intptr_t, &t_jit_generator::f_##a_operator, nullptr>\
			>, nullptr\
		>(a_generator.v_globals.v_##a_operator, a_stack, left, right, a_tail, v_at);\
		break;
#define XEMMAI__JIT__BINARY_RELATIONAL(a_label, a_operator)\
	case e_instruction__##a_label##_TT:\
		a_generator.f_emit_BINARY<nullptr, nullptr,\
			&t_jit_generator::f_binary_x<nullptr, nullptr,\
				&t_jit_generator::f_binary_xy<bool, intptr_t, intptr_t, &t_jit_generator::f_##a_operator##_integer>,\
				&t_jit_generator::f_binary_xy<bool, intptr_t, double, &t_jit_generator::f_##a_operator##_integer_float>,\
				&t_jit_generator::f_binary_xo<bool, intptr_t, &t_jit_generator::f_##a_operator##_integer, nullptr>\
			>,\
			&t_jit_generator::f_binary_x<nullptr, nullptr,\
				&t_jit_generator::f_binary_xy<bool, double, intptr_t, &t_jit_generator::f_##a_operator##_float_integer>,\
				&t_jit_generator::f_binary_xy<bool, double, double, &t_jit_generator::f_##a_operator##_float>,\
				&t_jit_generator::f_binary_xo<bool, double, &t_jit_generator::f_##a_operator##_float, nullptr>\
			>\
		>(a_generator.v_globals.v_##a_operator, a_stack, left, right, a_tail, v_at);\
		break;
#define XEMMAI__JIT__BINARY_EQUALITY(a_label, a_operator, a_not)\
	case e_instruction__##a_label##_TT:\
		a_generator.f_emit_BINARY<\
			&t_jit_generator::f_equals_null<a_not>,\
			&t_jit_generator::f_equals_boolean<a_not>,\
			&t_jit_generator::f_binary_x<\
				&t_jit_generator::f_equals_return<a_not>,\
				&t_jit_generator::f_equals_return<a_not>,\
				&t_jit_generator::f_binary_xy<bool, intptr_t, intptr_t, &t_jit_generator::f_##a_operator##_integer>,\
				&t_jit_generator::f_binary_xy<bool, intptr_t, double, &t_jit_generator::f_##a_operator##_integer_float>,\
				&t_jit_generator::f_binary_xo<bool, intptr_t, &t_jit_generator::f_##a_operator##_integer, &t_jit_generator::f_equals_other<a_not>>\
			>,\
			&t_jit_generator::f_binary_x<\
				&t_jit_generator::f_equals_return<a_not>,\
				&t_jit_generator::f_equals_return<a_not>,\
				&t_jit_generator::f_binary_xy<bool, double, intptr_t, &t_jit_generator::f_##a_operator##_float_integer>,\
				&t_jit_generator::f_binary_xy<bool, double, double, &t_jit_generator::f_##a_operator##_float>,\
				&t_jit_generator::f_binary_xo<bool, double, &t_jit_generator::f_##a_operator##_float, &t_jit_generator::f_equals_other<a_not>>\
			>\
		>(a_generator.v_globals.v_##a_operator, a_stack, left, right, a_tail, v_at);\
		break;
#define XEMMAI__JIT__BINARY_LOGICAL(a_label, a_operator)\
	case e_instruction__##a_label##_TT:\
		a_generator.f_emit_BINARY<nullptr,\
			&t_jit_generator::f_binary_x<nullptr,\
				&t_jit_generator::f_binary_xy<bool, bool, bool, &t_jit_generator::f_##a_operator>, nullptr, nullptr, nullptr\
			>,\
			&t_jit_generator::f_binary_x<nullptr, nullptr,\
				&t_jit_generator::f_binary_xy<intptr_t, intptr_t, intptr_t, &t_jit_generator::f_##a_operator>, nullptr,\
				&t_jit_generator::f_binary_xo<intptr_t, intptr_t, &t_jit_generator::f_##a_operator, nullptr>\
			>, nullptr\
		>(a_generator.v_globals.v_##a_operator, a_stack, left, right, a_tail, v_at);\
		break;
	XEMMAI__JIT__BINARY_ARITHMETIC(MULTIPLY, multiply)
	XEMMAI__JIT__BINARY_ARITHMETIC(DIVIDE, divide)
	XEMMAI__JIT__BINARY_INTEGRAL(MODULUS, modulus)
	XEMMAI__JIT__BINARY_ARITHMETIC(ADD, add)
	XEMMAI__JIT__BINARY_ARITHMETIC(SUBTRACT, subtract)
	XEMMAI__JIT__BINARY_INTEGRAL(LEFT_SHIFT, left_shift)
	XEMMAI__JIT__BINARY_INTEGRAL(RIGHT_SHIFT, right_shift)
	XEMMAI__JIT__BINARY_RELATIONAL(LESS, less)
	XEMMAI__JIT__BINARY_RELATIONAL(LESS_EQUAL, less_equal)
	XEMMAI__JIT__BINARY_RELATIONAL(GREATER, greater)
	XEMMAI__JIT__BINARY_RELATIONAL(GREATER_EQUAL, greater_equal)
	XEMMAI__JIT__BINARY_EQUALITY(EQUALS, equals, false)
	XEMMAI__JIT__BINARY_EQUALITY(NOT_EQUALS, not_equals, true)
	case e_instruction__IDENTICAL_TT:
		a_generator.f_emit_IDENTICAL<false>(a_stack, left, right, a_tail);
		break;
	case e_instruction__NOT_IDENTICAL_TT:
		a_generator.f_emit_IDENTICAL<true>(a_stack, left, right, a_tail);
		break;
	XEMMAI__JIT__BINARY_LOGICAL(AND, and)
	XEMMAI__JIT__BINARY_LOGICAL(XOR, xor)
	XEMMAI__JIT__BINARY_LOGICAL(OR, or)
	case e_instruction__SEND:
		a_generator.f_emit_SEND(a_stack, left, right, a_tail, v_at);
		break;
	}
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_call::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	const t_code::t_variable* variable = nullptr;
	if (auto get = v_expand ? nullptr : dynamic_cast<t_symbol_get*>(v_target.get())) {
		get->f_resolve();
		if (get->v_resolved == 1 && get->v_variable && !get->v_variable->v_varies) variable = get->v_variable;
	}
	if (!variable) {
		if (auto p = dynamic_cast<t_object_get*>(v_target.get())) {
			p->f_method_get(a_generator, a_stack);
		} else if (auto p = dynamic_cast<t_get_at*>(v_target.get())) {
			p->f_bind(a_generator, a_stack);
		} else {
			v_target->f_generate(a_generator, a_stack, false, false);
			a_generator.f_reserve(a_stack + 2);
			a_generator.f_emit_NUL(a_stack + 1);
		}
	}
	for (size_t i = 0; i < v_arguments.size(); ++i) v_arguments[i]->f_generate(a_generator, a_stack + 2 + i, false, false);
	a_generator.f_reserve(a_stack + 2);
	a_generator.f_emit_SAFE_POINT(v_at);
	if (variable)
		a_generator.f_emit_CALL_OUTER(a_stack, variable->v_index, v_arguments.size(), a_tail, v_at);
	else
		a_generator.f_emit_CALL(a_stack, v_arguments.size(), v_expand, a_tail, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

t_operand t_get_at::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack + 1, false, false);
	v_index->f_generate(a_generator, a_stack + 2, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_GET_AT(a_stack, a_tail, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

void t_get_at::f_bind(t_jit_generator& a_generator, size_t a_stack)
{
	v_target->f_generate(a_generator, a_stack, false, false);
	v_index->f_generate(a_generator, a_stack + 1, false, false);
	a_generator.f_reserve(a_stack + 3);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_METHOD_BIND(a_stack, v_at);
}

t_operand t_set_at::f_generate(t_jit_generator& a_generator, size_t a_stack, bool a_tail, bool a_operand, bool a_clear)
{
	v_target->f_generate(a_generator, a_stack + 1, false, false);
	v_index->f_generate(a_generator, a_stack + 2, false, false);
	v_value->f_generate(a_generator, a_stack + 3, false, false);
	a_generator.f_emit_SAFE_POINT(v_at);
	a_generator.f_emit_SET_AT(a_stack, a_tail, v_at);
	if (a_clear) a_generator.f_emit_CLEAR(a_stack);
	return a_stack;
}

}

}
