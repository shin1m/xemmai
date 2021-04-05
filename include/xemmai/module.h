#ifndef XEMMAI__MODULE_H
#define XEMMAI__MODULE_H

#include "portable/library.h"
#include "symbol.h"
#include "code.h"

namespace xemmai
{

struct t_library;

struct t_module
{
	class t_scoped_lock
	{
		bool v_own = false;

	public:
		t_scoped_lock();
		~t_scoped_lock();
	};


	template<typename T, typename... T_an>
	static t_object* f_new(std::wstring_view a_name, T_an&&... a_an);
	static t_object* f_load_library(std::wstring_view a_name, std::wstring_view a_path);
	static void f_execute_script(t_object* a_this, t_object* a_code);
	static t_object* f_load_and_execute_script(std::wstring_view a_name, std::wstring_view a_path);
	XEMMAI__PORTABLE__EXPORT static t_object* f_instantiate(std::wstring_view a_name);
	static void f_main();

	std::map<std::wstring, t_slot, std::less<>>::iterator v_iterator;
	std::wstring v_path;

	t_module(std::map<std::wstring, t_slot, std::less<>>::iterator a_iterator, std::wstring_view a_path) : v_iterator(a_iterator), v_path(a_path)
	{
		v_iterator->second = t_object::f_of(this);
	}
	virtual ~t_module();
	virtual void f_scan(t_scan a_scan);
};

struct t_script : t_module
{
	std::vector<std::unique_ptr<t_svalue>> v_slots;
	std::mutex v_mutex;

	using t_module::t_module;
	virtual void f_scan(t_scan a_scan);
	t_svalue& f_slot(t_object* a_p)
	{
		auto p = new t_svalue(a_p);
		std::lock_guard lock(v_mutex);
		return *v_slots.emplace_back(p);
	}
};

struct t_debug_script : t_script
{
	t_slot v_code;
	std::map<std::pair<size_t, void**>, size_t> v_safe_points;

	using t_script::t_script;
	virtual void f_scan(t_scan a_scan);
	std::pair<size_t, size_t> f_replace_break_point(size_t a_line, size_t a_column, t_instruction a_old, t_instruction a_new);
	std::pair<size_t, size_t> f_set_break_point(size_t a_line, size_t a_column = 0)
	{
		return f_replace_break_point(a_line, a_column, e_instruction__SAFE_POINT, e_instruction__BREAK_POINT);
	}
	std::pair<size_t, size_t> f_reset_break_point(size_t a_line, size_t a_column = 0)
	{
		return f_replace_break_point(a_line, a_column, e_instruction__BREAK_POINT, e_instruction__SAFE_POINT);
	}
};

class XEMMAI__PORTABLE__EXPORT t_extension
{
	t_object* v_module;

public:
	using t_function = void(*)(t_extension*, t_pvalue*, size_t);

	t_extension(t_object* a_module) : v_module(a_module)
	{
	}
	virtual ~t_extension() = default;
	t_object* f_module() const
	{
		return v_module;
	}
	virtual void f_scan(t_scan a_scan) = 0;
};

struct t_library : t_module
{
	struct t_handle
	{
		t_handle* v_next;
		portable::t_library v_library;
	};

	t_handle* v_handle;
	t_extension* v_extension = nullptr;

	t_library(std::map<std::wstring, t_slot, std::less<>>::iterator a_iterator, std::wstring_view a_path, t_handle* a_handle) : t_module(a_iterator, a_path), v_handle(a_handle)
	{
	}
	virtual ~t_library();
	virtual void f_scan(t_scan a_scan);
};

template<>
struct t_type_of<t_module> : t_underivable<t_holds<t_module>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
};

template<typename T>
inline T* f_extension(t_object* a_module)
{
	return static_cast<T*>(a_module->f_as<t_library>().v_extension);
}

template<typename T_type, typename T_extension, typename... T_an>
inline t_object* f_new(T_extension* a_extension, bool a_shared, T_an&&... a_an)
{
	return a_extension->template f_type<T_type>()->template f_new<T_type>(a_shared, std::forward<T_an>(a_an)...);
}

}

#define XEMMAI__MODULE__FACTORY extern "C" XEMMAI__PORTABLE__DEFINE_EXPORT xemmai::t_extension* f_factory

#endif
