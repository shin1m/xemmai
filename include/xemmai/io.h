#ifndef XEMMAI__IO_H
#define XEMMAI__IO_H

#include "io/file.h"
#include "io/reader.h"
#include "io/writer.h"
#include "io/path.h"
#include "global.h"

namespace xemmai
{

class t_io : public t_library
{
	t_slot v_symbol_close;
	t_slot v_symbol_read;
	t_slot v_symbol_write;
	t_slot v_symbol_flush;
	t_slot v_symbol_read_line;
	t_slot v_symbol_write_line;
	t_slot_of<t_type> v_type_file;
	t_slot_of<t_type> v_type_reader;
	t_slot_of<t_type> v_type_writer;
	t_slot_of<t_type> v_type_path;

public:
	t_io() : t_library(nullptr)
	{
	}
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	t_object* f_symbol_close() const
	{
		return v_symbol_close;
	}
	t_object* f_symbol_read() const
	{
		return v_symbol_read;
	}
	t_object* f_symbol_write() const
	{
		return v_symbol_write;
	}
	t_object* f_symbol_flush() const
	{
		return v_symbol_flush;
	}
	t_object* f_symbol_read_line() const
	{
		return v_symbol_read_line;
	}
	t_object* f_symbol_write_line() const
	{
		return v_symbol_write_line;
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_io*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		using t = t_type_of<typename t_fundamental<T>::t_type>;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_io* t_io::f_library<t_io>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE_AS(t_io, io::t_file, file)
XEMMAI__LIBRARY__TYPE_AS(t_io, io::t_reader, reader)
XEMMAI__LIBRARY__TYPE_AS(t_io, io::t_writer, writer)
XEMMAI__LIBRARY__TYPE_AS(t_io, portable::t_path, path)

template<typename... T_an>
inline t_object* io::t_file::f_instantiate(T_an&&... a_an)
{
	return f_new<t_file>(&f_engine()->f_module_io()->f_as<t_module>().v_body->f_as<t_io>(), std::forward<T_an>(a_an)...);
}

template<typename T>
inline t_object* t_type_of<portable::t_path>::f_transfer(const t_io* a_library, T&& a_value)
{
	return xemmai::f_new<portable::t_path>(a_library, std::forward<T>(a_value));
}

}

#endif
