#include <xemmai/module.h>

#include <xemmai/portable/path.h>
#include <xemmai/parser.h>
#include <xemmai/convert.h>
#include <xemmai/io/file.h>

namespace xemmai
{

t_module::t_scoped_lock::t_scoped_lock()
{
	auto& thread = f_engine()->v_module__thread;
	auto current = t_thread::f_current();
	if (thread == current) return;
	auto& mutex = f_engine()->v_module__mutex;
	auto& condition = f_engine()->v_module__condition;
	std::unique_lock<std::mutex> lock(mutex);
	while (thread) condition.wait(lock);
	v_own = true;
	thread = current;
}

t_module::t_scoped_lock::~t_scoped_lock()
{
	if (!v_own) return;
	std::lock_guard<std::mutex> lock(f_engine()->v_module__mutex);
	f_engine()->v_module__thread = nullptr;
	f_engine()->v_module__condition.notify_all();
}

t_scoped t_module::f_load_library(std::wstring_view a_name, std::wstring_view a_path)
{
	portable::t_library library{std::wstring(a_path)};
	if (!library) return {};
	auto factory = library.f_symbol<t_extension* (*)(t_object*)>("f_factory");
	if (!factory) f_throw(L"f_factory not found."sv);
	auto handle = new t_library::t_handle();
	handle->v_library.f_swap(library);
	auto module = f_new<t_library>(a_name, a_path, handle);
	module->f_as<t_library>().v_extension = factory(module);
	return module;
}

void t_module::f_execute_script(t_object* a_this, t_object* a_code)
{
	auto scope = f_global()->f_type<t_scope>()->f_new<t_scope>(true);
	auto lambda = t_lambda::f_instantiate(scope->f_as<t_scope>().f_entries(), a_code);
	t_scoped_stack stack(2);
	stack[1].f_construct(*a_this);
	static_cast<t_object*>(lambda)->f_call_without_loop(stack, 0);
	stack.f_return();
}

t_scoped t_module::f_load_and_execute_script(std::wstring_view a_name, std::wstring_view a_path)
{
	io::t_file stream(a_path, "r");
	if (!stream) return {};
	ast::t_scope scope(nullptr);
	if (f_engine()->v_debugger) {
		auto module = f_new<t_debug_script>(a_name, a_path);
		auto& script = module->f_as<t_debug_script>();
		t_parser(script, stream)(scope);
		script.v_code = t_emit(module, &script.v_safe_points)(scope);
		f_engine()->f_debug_script_loaded();
		f_execute_script(module, script.v_code);
		return module;
	} else {
		auto module = f_new<t_script>(a_name, a_path);
		t_parser(module->f_as<t_script>(), stream)(scope);
		f_execute_script(module, t_emit(module, nullptr)(scope));
		return module;
	}
}

t_scoped t_module::f_instantiate(std::wstring_view a_name)
{
	t_scoped_lock lock;
	f_engine()->v_object__reviving__mutex.lock();
	f_engine()->v_module__mutex.lock();
	{
		auto& instances = f_engine()->v_module__instances;
		auto i = instances.lower_bound(a_name);
		if (i != instances.end() && i->first == a_name) {
			f_as<t_thread&>(t_thread::f_current()).v_internal->f_revive();
			f_engine()->v_module__mutex.unlock();
			f_engine()->v_object__reviving__mutex.unlock();
			return i->second;
		}
	}
	f_engine()->v_module__mutex.unlock();
	f_engine()->v_object__reviving__mutex.unlock();
	t_scoped paths = f_engine()->f_module_system()->f_get(f_global()->f_symbol_path());
	t_scoped n = paths.f_invoke(f_global()->f_symbol_size());
	f_check<size_t>(n, L"size");
	for (size_t i = 0; i < f_as<size_t>(n); ++i) {
		t_scoped x = paths.f_get_at(f_global()->f_as(i));
		f_check<t_string>(x, L"path");
		std::wstring path = portable::t_path(f_as<const t_string&>(x)) / a_name;
		t_scoped module = f_load_and_execute_script(a_name, path + L".xm");
		if (module) return module;
		module = f_load_library(a_name, path);
		if (module) return module;
	}
	f_throw(L"module \"" + std::wstring(a_name) + L"\" not found.");
	return {};
}

void t_module::f_main()
{
	t_scoped x = f_engine()->f_module_system()->f_get(f_global()->f_symbol_script());
	f_check<t_string>(x, L"script");
	auto path = f_as<std::wstring_view>(x);
	if (path.empty()) f_throw(L"script path is empty."sv);
	if (!f_load_and_execute_script(L"__main"sv, path)) f_throw(L"file \"" + std::wstring(path) + L"\" not found.");
}

t_module::t_module(std::wstring_view a_path) : v_path(a_path), v_iterator(f_engine()->v_module__instances__null)
{
}

t_module::~t_module()
{
	if (v_iterator == f_engine()->v_module__instances__null) return;
	std::lock_guard<std::mutex> lock(f_engine()->v_module__mutex);
	f_engine()->v_module__instances.erase(v_iterator);
}

void t_module::f_scan(t_scan a_scan)
{
	a_scan(v_iterator->second);
}

void t_script::f_scan(t_scan a_scan)
{
	t_module::f_scan(a_scan);
	for (auto& p : v_slots) a_scan(*p);
}

void t_debug_script::f_scan(t_scan a_scan)
{
	t_script::f_scan(a_scan);
	a_scan(v_code);
}

std::pair<size_t, size_t> t_debug_script::f_replace_break_point(size_t a_line, size_t a_column, t_instruction a_old, t_instruction a_new)
{
	auto i = v_safe_points.lower_bound({a_line, nullptr});
	if (i == v_safe_points.end()) return {0, 0};
	if (a_column > 0) {
		while (true) {
			if (i->first.first != a_line) return {0, 0};
			if (i->second == a_column) break;
			if (++i == v_safe_points.end()) return {0, 0};
		}
	}
	auto& code = f_as<t_code&>(v_code);
	if (*i->first.second == code.f_p(a_old)) *i->first.second = code.f_p(a_new);
	return {i->first.first, i->second};
}

t_library::~t_library()
{
	delete v_extension;
	if (v_handle) {
		v_handle->v_next = f_engine()->v_library__handle__finalizing;
		f_engine()->v_library__handle__finalizing = v_handle;
	}
}

void t_library::f_scan(t_scan a_scan)
{
	t_module::f_scan(a_scan);
	if (v_extension) v_extension->f_scan(a_scan);
}

void t_type_of<t_module>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	if (a_this == f_engine()->f_module_global()) return;
	a_this->f_as<t_module>().f_scan(a_scan);
}

void t_type_of<t_module>::f_do_instantiate(t_stacked* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(a_stack, a_n, L"must be called with an argument."sv);
	t_destruct<> a0(a_stack[2]);
	f_check<t_string>(a0.v_p, L"argument0");
	a_stack[0].f_construct(t_module::f_instantiate(f_as<const t_string&>(a0.v_p)));
}

}
