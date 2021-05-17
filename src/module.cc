#include <xemmai/portable/path.h>
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
	std::unique_lock lock(mutex);
	while (thread) condition.wait(lock);
	v_own = true;
	thread = current;
}

t_module::t_scoped_lock::~t_scoped_lock()
{
	if (!v_own) return;
	std::lock_guard lock(f_engine()->v_module__mutex);
	f_engine()->v_module__thread = nullptr;
	f_engine()->v_module__condition.notify_all();
}

t_object* t_module::f_load_script(std::wstring_view a_path)
{
	io::t_file stream(a_path, "r");
	if (!stream) return nullptr;
	ast::t_scope scope(nullptr);
	if (f_engine()->v_debugger) {
		auto body = f_global()->f_type<t_module::t_body>()->f_new<t_debug_script>(a_path);
		auto& script = body->f_as<t_debug_script>();
		t_parser(script, stream)(scope);
		script.v_code = t_emit(body, &script.v_safe_points)(scope);
		f_engine()->f_debug_script_loaded(script);
		return script.v_code;
	} else {
		auto body = f_global()->f_type<t_module::t_body>()->f_new<t_script>(a_path);
		t_parser(body->f_as<t_script>(), stream)(scope);
		return t_emit(body, nullptr)(scope);
	}
}

std::vector<std::pair<t_root, t_rvalue>> t_module::f_execute_script(t_object* a_code)
{
	auto scope = xemmai::f_new<t_scope>(f_global());
	auto lambda = t_lambda::f_instantiate(scope->f_as<t_scope>().f_entries(), a_code);
	t_fields fields;
	t_builder::f_do(fields, [&](auto builder)
	{
		t_scoped_stack stack(2);
		stack[1] = builder;
		lambda->f_call_without_loop(stack, 0);
	});
	return fields.v_class;
}

t_object* t_module::f_load_library(std::wstring_view a_path)
{
	portable::t_library library{std::wstring(a_path)};
	if (!library) return nullptr;
	auto factory = library.f_symbol<t_object*(*)(t_library::t_handle*)>("f_factory");
	if (!factory) f_throw(L"f_factory not found."sv);
	auto handle = new t_library::t_handle();
	handle->v_library.f_swap(library);
	return factory(handle);
}

t_object* t_module::f_new(std::wstring_view a_name, t_object* a_body, const std::vector<std::pair<t_root, t_rvalue>>& a_fields)
{
	t_fields fields;
	for (auto& x : a_fields) fields.v_instance.push_back(x.first);
	auto type = f_global()->f_type<t_module>()->f_derive<t_type_of<t_module>>(t_object::f_of(f_global()), fields);
	type->f_as<t_type>().v_builtin = true;
	decltype(f_engine()->v_module__instances)::iterator i;
	{
		std::lock_guard lock(f_engine()->v_module__mutex);
		i = f_engine()->v_module__instances.emplace(a_name, nullptr).first;
	}
	auto n = type->f_as<t_type>().v_instance_fields;
	auto p = f_engine()->f_allocate(t_object::f_align_for_fields(sizeof(t_module)) + sizeof(t_svalue) * n);
	auto q = p->f_fields(sizeof(t_module));
	for (size_t i = 0; i < n; ++i) new(q + i) t_svalue(a_fields[i].second);
	new(p->f_data()) t_module(i, a_body);
	p->f_be(&type->f_as<t_type>());
	return p;
}

t_object* t_module::f_instantiate(std::wstring_view a_name)
{
	t_scoped_lock lock;
	f_engine()->v_object__reviving__mutex.lock();
	f_engine()->v_module__mutex.lock();
	{
		auto& instances = f_engine()->v_module__instances;
		auto i = instances.lower_bound(a_name);
		if (i != instances.end() && i->first == a_name) {
			f_engine()->v_object__reviving = true;
			t_thread::v_current->f_revive();
			f_engine()->v_module__mutex.unlock();
			f_engine()->v_object__reviving__mutex.unlock();
			return i->second;
		}
	}
	f_engine()->v_module__mutex.unlock();
	f_engine()->v_object__reviving__mutex.unlock();
	auto paths = f_engine()->f_module_system()->f_get(f_global()->f_symbol_path());
	auto n = paths.f_invoke(f_global()->f_symbol_size());
	f_check<size_t>(n, L"size");
	for (size_t i = 0; i < f_as<size_t>(n); ++i) {
		auto x = paths.f_get_at(f_global()->f_as(i));
		f_check<t_string>(x, L"path");
		std::wstring path = portable::t_path(f_as<const t_string&>(x)) / a_name;
		if (auto code = f_load_script(path + L".xm")) return f_new(a_name, code->f_as<t_code>().v_module, f_execute_script(code));
		if (auto body = f_load_library(path)) return f_new(a_name, body, body->f_as<t_library>().f_define());
	}
	f_throw(L"module \"" + std::wstring(a_name) + L"\" not found.");
	return nullptr;
}

void t_module::f_main()
{
	auto path = f_as<std::wstring_view>(f_engine()->f_module_system()->f_get(f_global()->f_symbol_script()));
	if (path.empty()) f_throw(L"script path is empty."sv);
	auto code = f_load_script(path);
	if (!code) f_throw(L"file \"" + std::wstring(path) + L"\" not found.");
	f_execute_script(code);
}

t_module::~t_module()
{
	std::lock_guard lock(f_engine()->v_module__mutex);
	f_engine()->v_module__instances.erase(v_iterator);
}

void t_script::f_scan(t_scan a_scan)
{
	std::lock_guard lock(v_mutex);
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
	if (!v_handle) return;
	v_handle->v_next = f_engine()->v_library__handle__finalizing;
	f_engine()->v_library__handle__finalizing = v_handle;
}

void t_type_of<t_module>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	if (a_this != f_engine()->f_module_global()) a_this->f_as<t_module>().f_scan(a_scan);
}

void t_type_of<t_module>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	if (a_n != 1) f_throw(L"must be called with an argument."sv);
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = t_module::f_instantiate(f_as<const t_string&>(a_stack[2]));
}

}
