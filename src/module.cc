#include <xemmai/module.h>

#include <xemmai/portable/path.h>
#include <xemmai/engine.h>
#include <xemmai/scope.h>
#include <xemmai/parser.h>
#include <xemmai/convert.h>
#include <xemmai/io/file.h>

namespace xemmai
{

t_module::t_scoped_lock::t_scoped_lock() : v_own(false)
{
	t_object*& thread = f_engine()->v_module__thread;
	t_object* current = t_thread::f_current();
	if (thread == current) return;
	portable::t_mutex& mutex = f_engine()->v_module__mutex;
	portable::t_condition& condition = f_engine()->v_module__condition;
	portable::t_scoped_lock lock(mutex);
	while (thread) condition.f_wait(mutex);
	v_own = true;
	thread = current;
}

t_module::t_scoped_lock::~t_scoped_lock()
{
	if (!v_own) return;
	portable::t_scoped_lock lock(f_engine()->v_module__mutex);
	f_engine()->v_module__thread = 0;
	f_engine()->v_module__condition.f_broadcast();
}

t_transfer t_module::f_instantiate(const std::wstring& a_name, t_module* a_module)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_module>());
	object->v_pointer = a_module;
	t_transfer second = static_cast<t_object*>(object);
	{
		portable::t_scoped_lock lock(f_engine()->v_module__mutex);
		a_module->v_iterator = f_engine()->v_module__instances.insert(std::make_pair(a_name, t_slot())).first;
		a_module->v_iterator->second = second;
	}
	return object;
}

t_transfer t_module::f_load_script(const std::wstring& a_path)
{
	io::t_file stream(a_path, "r");
	if (!stream) return 0;
	t_parser parser(a_path, stream);
	return parser.f_parse();
}

t_library* t_module::f_load_library(const std::wstring& a_path)
{
	portable::t_library library(a_path);
	if (!library) return 0;
	t_library::t_handle* handle = new t_library::t_handle();
	handle->v_library.f_swap(library);
	return new t_library(a_path, handle);
}

void t_module::f_execute_script(t_object* a_this, t_object* a_code)
{
	t_code* p = f_as<t_code*>(a_code);
	t_fiber::t_context::f_push(t_scope::f_instantiate(p->v_size, 0, a_this), a_code, &p->v_instructions[0]);
	t_code::f_loop();
	t_fiber::t_context::f_pop();
}

t_transfer t_module::f_instantiate(const std::wstring& a_name)
{
	t_scoped_lock lock;
	f_engine()->v_object__reviving__mutex.f_acquire();
	f_engine()->v_module__mutex.f_acquire();
	{
		std::map<std::wstring, t_slot>& instances = f_engine()->v_module__instances;
		std::map<std::wstring, t_slot>::iterator i = instances.lower_bound(a_name);
		if (i != instances.end() && i->first == a_name) {
			f_engine()->v_object__reviving = true;
			f_as<t_thread*>(t_thread::f_current())->v_internal->f_revive();
			f_engine()->v_module__mutex.f_release();
			f_engine()->v_object__reviving__mutex.f_release();
			return i->second;
		}
	}
	f_engine()->v_module__mutex.f_release();
	f_engine()->v_object__reviving__mutex.f_release();
	t_transfer paths = f_engine()->f_module_system()->f_get(f_global()->f_symbol_path());
	t_transfer n = paths->f_get(f_global()->f_symbol_size())->f_call();
	f_check<size_t>(n, L"size must be integer.");
	for (size_t i = 0; i < f_as<size_t>(n); ++i) {
		t_slot slots[] = {f_global()->f_as(i), 0};
		t_scoped_stack stack(slots, slots + 2);
		f_as<t_type*>(paths->f_type())->f_get_at(paths, stack);
		t_transfer x = stack.f_pop();
		f_check<std::wstring>(x, L"path must be string.");
		std::wstring path = portable::t_path(f_as<const std::wstring&>(x)) / a_name;
		std::wstring s = path + L".xm";
		t_transfer script = f_load_script(s);
		if (script) {
			t_transfer module = f_instantiate(a_name, new t_module(s));
			f_execute_script(module, script);
			return module;
		}
		t_library* library = f_load_library(path);
		if (library) {
			t_transfer module = f_instantiate(a_name, library);
			library->f_initialize(module);
			return module;
		}
	}
	t_throwable::f_throw(L"module \"" + a_name + L"\" not found.");
	return 0;
}

int t_module::f_main(void (*a_main)(void*), void* a_p)
{
	t_fiber::t_context::f_initiate(0, 0);
	try {
		t_native_context context;
		try {
			a_main(a_p);
			return 0;
		} catch (const t_scoped& thrown) {
			f_as<t_fiber*>(t_fiber::f_current())->f_caught(thrown);
			std::wstring s = L"<unprintable>";
			try {
				t_scoped p = thrown->f_get(f_global()->f_symbol_string())->f_call();
				if (f_is<std::wstring>(p)) s = f_as<const std::wstring&>(p);
			} catch (...) {
			}
			std::fprintf(stderr, "caught: %ls\n", s.c_str());
			if (f_is<t_throwable>(thrown)) thrown->f_get(t_symbol::f_instantiate(L"dump"))->f_call();
		}
	} catch (...) {
		std::fprintf(stderr, "caught <unexpected>.\n");
	}
	t_fiber::t_context::f_terminate();
	return -1;
}

void t_module::f_main(void* a_p)
{
	t_transfer x = f_engine()->f_module_system()->f_get(f_global()->f_symbol_script());
	f_check<std::wstring>(x, L"script must be string");
	const std::wstring& path = f_as<const std::wstring&>(x);
	if (path.empty()) t_throwable::f_throw(L"script path is empty.");
	t_transfer script = f_load_script(path);
	if (!script) t_throwable::f_throw(L"file \"" + path + L"\" not found.");
	f_execute_script(f_instantiate(L"__main", new t_module(path)), script);
}

t_module::t_module(const std::wstring& a_path) : v_path(a_path), v_iterator(f_engine()->v_module__instances__null)
{
}

t_module::~t_module()
{
	if (v_iterator == f_engine()->v_module__instances__null) return;
	portable::t_scoped_lock lock(f_engine()->v_module__mutex);
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
	f_as<t_module*>(a_this)->f_scan(a_scan);
}

void t_type_of<t_module>::f_finalize(t_object* a_this)
{
	delete f_as<t_module*>(a_this);
}

void t_type_of<t_module>::f_instantiate(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	if (a_n != 1) t_throwable::f_throw(L"must be called with an argument.");
	t_transfer a0 = a_stack.f_pop();
	f_check<std::wstring>(a0, L"argument0");
	a_stack.f_return(t_module::f_instantiate(f_as<const std::wstring&>(a0)));
}

}
