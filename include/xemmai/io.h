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
#define XEMMAI__IO__SYMBOLS(_)\
	_(close)\
	_(read)\
	_(write)\
	_(flush)\
	_(read_line)\
	_(write_line)\
	_(resume)
	XEMMAI__IO__SYMBOLS(XEMMAI__SYMBOL__DECLARE)
#define XEMMAI__IO__TYPES(_)\
	_##_AS(io::t_file, file)\
	_##_AS(io::t_reader, reader)\
	_##_AS(io::t_writer, writer)
	XEMMAI__IO__TYPES(XEMMAI__TYPE__DECLARE)

public:
	t_io() : t_library(nullptr)
	{
	}
	XEMMAI__LIBRARY__MEMBERS
	XEMMAI__IO__SYMBOLS(XEMMAI__SYMBOL__DEFINE)
};

XEMMAI__LIBRARY__BASE(t_io, t_global, f_global())
#define XEMMAI__TYPE__LIBRARY t_io
XEMMAI__IO__TYPES(XEMMAI__TYPE__DEFINE)
#undef XEMMAI__TYPE__LIBRARY

inline t_object* io::t_file::f_instantiate(auto&&... a_xs)
{
	return f_new<t_file>(&f_engine()->f_module_io()->f_as<t_module>().v_body->f_as<t_io>(), std::forward<decltype(a_xs)>(a_xs)...);
}

}

#endif
