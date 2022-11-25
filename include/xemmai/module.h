#ifndef XEMMAI__MODULE_H
#define XEMMAI__MODULE_H

#include "portable/library.h"
#include "symbol.h"
#include "code.h"
#include <deque>

namespace xemmai
{

struct t_module
{
	class t_scoped_lock
	{
		bool v_own = false;

	public:
		t_scoped_lock();
		~t_scoped_lock();
	};
	struct t_body
	{
		virtual ~t_body() = default;
		virtual void f_scan(t_scan a_scan) = 0;
	};

	static t_object* f_load_script(std::wstring_view a_path);
	static std::vector<std::pair<t_root, t_rvalue>> f_execute_script(t_object* a_code);
	static t_object* f_load_library(std::wstring_view a_path);
	static t_object* f_new(std::wstring_view a_name, t_object* a_body, const std::vector<std::pair<t_root, t_rvalue>>& a_fields);
	XEMMAI__PORTABLE__EXPORT static t_object* f_instantiate(std::wstring_view a_name);
	static void f_main();

	std::map<std::wstring, t_slot, std::less<>>::iterator v_entry;
	t_slot v_body;

	t_module(std::map<std::wstring, t_slot, std::less<>>::iterator a_entry, t_object* a_body) : v_entry(a_entry), v_body(a_body)
	{
		v_entry->second = t_object::f_of(this);
	}
	~t_module();
	void f_scan(t_scan a_scan)
	{
		a_scan(v_entry->second);
		a_scan(v_body);
	}
};

struct t_script : t_module::t_body
{
	std::wstring v_path;
	std::deque<t_svalue> v_slots;
	std::mutex v_mutex;

	t_script(std::wstring_view a_path) : v_path(a_path)
	{
	}
	virtual void f_scan(t_scan a_scan);
	t_svalue& f_slot(t_object* a_p)
	{
		v_mutex.lock();
		auto& p = v_slots.emplace_back();
		v_mutex.unlock();
		p = a_p;
		return p;
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

struct t_library : t_module::t_body
{
	struct t_handle
	{
		t_handle* v_next;
		portable::t_library v_library;
	};
	using t_function = void(*)(t_library*, t_pvalue*, size_t);

	t_handle* v_handle;

	t_library(t_handle* a_handle) : v_handle(a_handle)
	{
	}
	XEMMAI__PORTABLE__EXPORT virtual ~t_library();
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define() = 0;
};

template<>
struct t_type_of<t_module::t_body> : t_uninstantiatable<t_finalizes<t_derives<t_module::t_body>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_module::t_body>().f_scan(a_scan);
	}
};

template<>
struct t_type_of<t_module> : t_holds<t_module>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
};

template<typename T_type, typename T_library, typename... T_an>
inline t_object* f_new(T_library* a_library, T_an&&... a_an)
{
	return a_library->template f_type<T_type>()->template f_new<T_type>(std::forward<T_an>(a_an)...);
}

}

#define XEMMAI__MODULE__FACTORY extern "C" XEMMAI__PORTABLE__DEFINE_EXPORT xemmai::t_object* f_factory

#define XEMMAI__LIBRARY__MEMBERS\
	virtual void f_scan(t_scan a_scan);\
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();\
	template<typename T>\
	const T* f_library() const\
	{\
		return this;\
	}\
	template<typename T>\
	t_slot_of<t_type>& f_type_slot();\
	template<typename T>\
	t_type* f_type() const\
	{\
		using t = t_type_of<typename t_fundamental<T>::t_type>;\
		return const_cast<typename t::t_library*>(f_library<typename t::t_library>())->template f_type_slot<T>();\
	}\
	template<typename T>\
	t_pvalue f_as(T&& a_value) const\
	{\
		using t = t_type_of<typename t_fundamental<T>::t_type>;\
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));\
	}

#define XEMMAI__LIBRARY__BASE(a_library, T_base, a_base)\
template<>\
inline const T_base* a_library::f_library<T_base>() const\
{\
	return a_base;\
}

#define XEMMAI__LIBRARY__TYPE(a_library, a_name)\
template<>\
inline t_slot_of<t_type>& a_library::f_type_slot<t_##a_name>()\
{\
	return v_type_##a_name;\
}

#define XEMMAI__LIBRARY__TYPE_AS(a_library, a_type, a_name)\
template<>\
inline t_slot_of<t_type>& a_library::f_type_slot<a_type>()\
{\
	return v_type_##a_name;\
}

#endif
