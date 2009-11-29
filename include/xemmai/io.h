#ifndef XEMMAI__IO_H
#define XEMMAI__IO_H

#include "symbol.h"
#include "global.h"
#include "io/file.h"
#include "io/reader.h"
#include "io/writer.h"

namespace xemmai
{

class t_io : public t_extension
{
	template<typename T, typename T_super> friend class t_define;

	t_slot v_symbol_close;
	t_slot v_symbol_read;
	t_slot v_symbol_write;
	t_slot v_symbol_flush;
	t_slot v_symbol_read_line;
	t_slot v_symbol_write_line;
	t_slot v_type_file;
	t_slot v_type_reader;
	t_slot v_type_writer;

	template<typename T>
	void f_type__(const t_transfer& a_type);

public:
	t_io(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
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
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

template<>
inline void t_io::f_type__<io::t_file>(const t_transfer& a_type)
{
	v_type_file = a_type;
}

template<>
inline void t_io::f_type__<io::t_reader>(const t_transfer& a_type)
{
	v_type_reader = a_type;
}

template<>
inline void t_io::f_type__<io::t_writer>(const t_transfer& a_type)
{
	v_type_writer = a_type;
}

template<>
inline t_object* t_io::f_type<io::t_file>() const
{
	return v_type_file;
}

template<>
inline t_object* t_io::f_type<io::t_reader>() const
{
	return v_type_reader;
}

template<>
inline t_object* t_io::f_type<io::t_writer>() const
{
	return v_type_writer;
}

}

#endif
