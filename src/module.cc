#include <xemmai/module.h>

#include <xemmai/portable/path.h>
#include <xemmai/parser.h>
#include <xemmai/convert.h>
#include <xemmai/io/file.h>

namespace xemmai
{

t_module::t_scoped_lock::t_scoped_lock()
{
	t_object*& thread = f_engine()->v_module__thread;
	t_object* current = t_thread::f_current();
	if (thread == current) return;
	std::mutex& mutex = f_engine()->v_module__mutex;
	std::condition_variable& condition = f_engine()->v_module__condition;
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

t_scoped t_module::f_instantiate(const std::wstring& a_name, t_module* a_module)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_module>());
	object.f_pointer__(a_module);
	t_scoped second = static_cast<t_object*>(object);
	{
		std::lock_guard<std::mutex> lock(f_engine()->v_module__mutex);
		a_module->v_iterator = f_engine()->v_module__instances.emplace(a_name, t_slot()).first;
		a_module->v_iterator->second = std::move(second);
	}
	return object;
}

t_scoped t_module::f_load_script(const std::wstring& a_path)
{
	io::t_file stream(a_path, "r");
	if (!stream) return t_value();
	t_parser parser(a_path, stream);
	ast::t_module module(a_path);
	parser.f_parse(module);
	t_generator generator;
	return generator.f_generate(module);
}

t_library* t_module::f_load_library(const std::wstring& a_path)
{
	portable::t_library library(a_path);
	if (!library) return nullptr;
	t_library::t_handle* handle = new t_library::t_handle();
	handle->v_library.f_swap(library);
	return new t_library(a_path, handle);
}

void t_module::f_execute_script(t_object* a_this, t_object* a_code)
{
	t_scoped lambda = t_lambda::f_instantiate(nullptr, a_code);
	t_scoped_stack stack(1);
	stack[0].f_construct(a_this);
	t_fiber::t_context::f_push(lambda, stack);
	t_code::f_loop();
	t_fiber::t_context::f_pop();
}

t_scoped t_module::f_instantiate(const std::wstring& a_name)
{
	t_scoped_lock lock;
	f_engine()->v_object__reviving__mutex.lock();
	f_engine()->v_module__mutex.lock();
	{
		std::map<std::wstring, t_slot>& instances = f_engine()->v_module__instances;
		auto i = instances.lower_bound(a_name);
		if (i != instances.end() && i->first == a_name) {
			f_engine()->v_object__reviving = true;
			f_as<t_thread&>(t_thread::f_current()).v_internal->f_revive();
			f_engine()->v_module__mutex.unlock();
			f_engine()->v_object__reviving__mutex.unlock();
			return i->second;
		}
	}
	f_engine()->v_module__mutex.unlock();
	f_engine()->v_object__reviving__mutex.unlock();
	t_scoped paths = f_engine()->f_module_system()->f_get(f_global()->f_symbol_path());
	t_scoped n = paths.f_get(f_global()->f_symbol_size())();
	f_check<size_t>(n, L"size");
	for (size_t i = 0; i < f_as<size_t>(n); ++i) {
		t_scoped x = paths.f_get_at(f_global()->f_as(i));
		f_check<std::wstring>(x, L"path");
		std::wstring path = portable::t_path(f_as<const std::wstring&>(x)) / a_name;
		std::wstring s = path + L".xm";
		t_scoped script = f_load_script(s);
		if (script) {
			t_scoped module = f_instantiate(a_name, new t_module(s));
			f_execute_script(module, script);
			return module;
		}
		t_library* library = f_load_library(path);
		if (library) {
			t_scoped module = f_instantiate(a_name, library);
			library->f_initialize(module);
			return module;
		}
	}
	t_throwable::f_throw(L"module \"" + a_name + L"\" not found.");
	return t_value();
}

void t_module::f_main()
{
	t_scoped x = f_engine()->f_module_system()->f_get(f_global()->f_symbol_script());
	f_check<std::wstring>(x, L"script");
	const std::wstring& path = f_as<const std::wstring&>(x);
	if (path.empty()) t_throwable::f_throw(L"script path is empty.");
	t_scoped script = f_load_script(path);
	if (!script) t_throwable::f_throw(L"file \"" + path + L"\" not found.");
	f_execute_script(f_instantiate(L"__main", new t_module(path)), script);
}

t_module::t_module(const std::wstring& a_path) : v_path(a_path), v_iterator(f_engine()->v_module__instances__null)
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

t_extension::~t_extension()
{
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

void t_library::f_initialize(t_object* a_this)
{
	t_extension* (*factory)(t_object*) = v_handle->v_library.f_symbol<t_extension* (*)(t_object*)>("f_factory");
	if (!factory) t_throwable::f_throw(L"f_factory not found.");
	v_extension = factory(a_this);
}

t_type* t_type_of<t_module>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_module>::f_scan(t_object* a_this, t_scan a_scan)
{
	if (a_this == f_engine()->f_module_global()) return;
	f_as<t_module&>(a_this).f_scan(a_scan);
}

void t_type_of<t_module>::f_finalize(t_object* a_this)
{
	delete &f_as<t_module&>(a_this);
}

void t_type_of<t_module>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_scoped a0 = std::move(a_stack[1]);
	f_check<std::wstring>(a0, L"argument0");
	a_stack[0].f_construct(t_module::f_instantiate(f_as<const std::wstring&>(a0)));
}

}
