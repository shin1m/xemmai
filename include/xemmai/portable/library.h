#ifndef XEMMAI__PORTABLE__LIBRARY_H
#define XEMMAI__PORTABLE__LIBRARY_H

#ifdef __unix__
#include <dlfcn.h>
#include "convert.h"
#endif
#ifdef _WIN32
#include <windows.h>
#endif
#include <cstdio>
#include <string>

namespace xemmai::portable
{

#ifdef __unix__
class t_library
{
	void* v_handle = NULL;

public:
	t_library() = default;
	t_library(const std::wstring& a_path) : v_handle(dlopen(f_convert(a_path + L".so").c_str(), RTLD_LAZY | RTLD_GLOBAL))
	{
//		if (v_handle == NULL) std::fprintf(stderr, "dlopen: %s\n", dlerror());
	}
	~t_library()
	{
		if (v_handle != NULL) dlclose(v_handle);
	}
	operator bool() const
	{
		return v_handle != NULL;
	}
	void f_swap(t_library& a_library)
	{
		void* handle = v_handle;
		v_handle = a_library.v_handle;
		a_library.v_handle = handle;
	}
	template<typename T>
	T f_symbol(const char* a_name)
	{
		return reinterpret_cast<T>(dlsym(v_handle, a_name));
	}
};
#endif

#ifdef _WIN32
class t_library
{
	HMODULE v_handle = NULL;

public:
	t_library() = default;
	t_library(const std::wstring& a_path) : v_handle(LoadLibraryW((a_path + L".dll").c_str()))
	{
	}
	~t_library()
	{
		if (v_handle != NULL) FreeLibrary(v_handle);
	}
	operator bool() const
	{
		return v_handle != NULL;
	}
	void f_swap(t_library& a_library)
	{
		HMODULE handle = v_handle;
		v_handle = a_library.v_handle;
		a_library.v_handle = handle;
	}
	template<typename T>
	T f_symbol(const char* a_name)
	{
		return reinterpret_cast<T>(GetProcAddress(v_handle, a_name));
	}
};
#endif

}

#endif
