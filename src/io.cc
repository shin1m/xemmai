#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

void t_io::f_define(std::vector<std::pair<t_root, t_rvalue>>& a_fields)
{
	v_symbol_close = t_symbol::f_instantiate(L"close"sv);
	v_symbol_read = t_symbol::f_instantiate(L"read"sv);
	v_symbol_write = t_symbol::f_instantiate(L"write"sv);
	v_symbol_flush = t_symbol::f_instantiate(L"flush"sv);
	v_symbol_read_line = t_symbol::f_instantiate(L"read_line"sv);
	v_symbol_write_line = t_symbol::f_instantiate(L"write_line"sv);
	t_type_of<io::t_file>::f_define(this);
	v_type_file->v_builtin = true;
	t_type_of<io::t_reader>::f_define(this);
	v_type_reader->v_builtin = true;
	t_type_of<io::t_writer>::f_define(this);
	v_type_writer->v_builtin = true;
	t_type_of<portable::t_path>::f_define(this);
	v_type_path->v_builtin = true;
	t_export(this, a_fields)
		(L"File"sv, t_object::f_of(v_type_file))
		(L"Reader"sv, t_object::f_of(v_type_reader))
		(L"Writer"sv, t_object::f_of(v_type_writer))
		(L"Path"sv, t_object::f_of(v_type_path))
	;
}

void t_io::f_scan(t_scan a_scan)
{
	a_scan(v_symbol_close);
	a_scan(v_symbol_read);
	a_scan(v_symbol_write);
	a_scan(v_symbol_flush);
	a_scan(v_symbol_read_line);
	a_scan(v_symbol_write_line);
	a_scan(v_type_file);
	a_scan(v_type_reader);
	a_scan(v_type_writer);
	a_scan(v_type_path);
}

}
