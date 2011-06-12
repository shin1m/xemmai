#ifndef XEMMAI__MODULE_H
#define XEMMAI__MODULE_H

#include "portable/library.h"
#include "native.h"

namespace xemmai
{

struct t_library;

struct t_module
{
	class t_scoped_lock
	{
		bool v_own;

	public:
		t_scoped_lock();
		~t_scoped_lock();
	};

	static t_transfer f_instantiate(const std::wstring& a_name, t_module* a_module);
	static t_transfer f_load_script(const std::wstring& a_path);
	static t_library* f_load_library(const std::wstring& a_path);
	static void f_execute_script(t_object* a_this, t_object* a_code);
	static t_transfer f_instantiate(const std::wstring& a_name);
	static int f_main(void (*a_main)(void*), void* a_p);
	static void f_main(void* a_p);

	std::wstring v_path;
	std::map<std::wstring, t_slot>::iterator v_iterator;

	t_module(const std::wstring& a_path);
	virtual ~t_module();
	virtual void f_scan(t_scan a_scan);
};

class XEMMAI__PORTABLE__EXPORT t_extension
{
	t_object* v_module;

public:
	t_extension(t_object* a_module) : v_module(a_module)
	{
	}
	virtual ~t_extension();
	t_object* f_module() const
	{
		return v_module;
	}
	virtual void f_scan(t_scan a_scan) = 0;
	t_transfer f_function(t_native::t_function a_function)
	{
		return t_native::f_instantiate(v_module, a_function);
	}
};

struct t_library : t_module
{
	struct t_handle
	{
		t_handle* v_next;
		portable::t_library v_library;
	};

	t_handle* v_handle;
	t_extension* v_extension;

	t_library(const std::wstring& a_path, t_handle* a_handle) : t_module(a_path), v_handle(a_handle), v_extension(0)
	{
	}
	virtual ~t_library();
	virtual void f_scan(t_scan a_scan);
	void f_initialize(t_object* a_this);
};

template<>
struct t_type_of<t_module> : t_type
{
	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_revive = v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<typename T>
inline T* f_extension(t_object* a_module)
{
	return static_cast<T*>(static_cast<t_library*>(&f_as<t_module&>(a_module))->v_extension);
}

}

#endif
