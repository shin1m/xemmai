#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

void t_io::f_scan(t_scan a_scan)
{
	XEMMAI__IO__TYPES(XEMMAI__TYPE__SCAN)
}

std::vector<std::pair<t_root, t_rvalue>> t_io::f_define()
{
	t_type_of<io::t_file>::f_define(this);
	v_type_file->v_builtin = true;
	t_type_of<io::t_reader>::f_define(this);
	v_type_reader->v_builtin = true;
	t_type_of<io::t_writer>::f_define(this);
	v_type_writer->v_builtin = true;
	return t_define(this)
	(L"File"sv, static_cast<t_object*>(v_type_file))
	(L"Reader"sv, static_cast<t_object*>(v_type_reader))
	(L"Writer"sv, static_cast<t_object*>(v_type_writer))
	;
}

}
