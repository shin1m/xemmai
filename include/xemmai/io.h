#ifndef XEMMAI__IO_H
#define XEMMAI__IO_H

#include "global.h"
#include "io/file.h"
#include "io/reader.h"
#include "io/writer.h"
#include "io/path.h"

namespace xemmai
{

class t_io : public t_extension
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
	t_io(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_io* t_io::f_extension<t_io>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_io::f_type_slot<io::t_file>()
{
	return v_type_file;
}

template<>
inline t_slot_of<t_type>& t_io::f_type_slot<io::t_reader>()
{
	return v_type_reader;
}

template<>
inline t_slot_of<t_type>& t_io::f_type_slot<io::t_writer>()
{
	return v_type_writer;
}

template<>
inline t_slot_of<t_type>& t_io::f_type_slot<portable::t_path>()
{
	return v_type_path;
}

}

#endif
