#ifndef XEMMAI__IO_H
#define XEMMAI__IO_H

#include "io/file.h"
#include "io/reader.h"
#include "io/writer.h"
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

public:
	t_io() : t_library(nullptr)
	{
	}
	XEMMAI__LIBRARY__MEMBERS
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
};

XEMMAI__LIBRARY__BASE(t_io, t_global, f_global())
XEMMAI__LIBRARY__TYPE_AS(t_io, io::t_file, file)
XEMMAI__LIBRARY__TYPE_AS(t_io, io::t_reader, reader)
XEMMAI__LIBRARY__TYPE_AS(t_io, io::t_writer, writer)

inline t_object* io::t_file::f_instantiate(auto&&... a_xs)
{
	return f_new<t_file>(&f_engine()->f_module_io()->f_as<t_module>().v_body->f_as<t_io>(), std::forward<decltype(a_xs)>(a_xs)...);
}

}

#endif
