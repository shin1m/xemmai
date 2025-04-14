#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

void t_io::f_scan(t_scan a_scan)
{
	a_scan(v_symbol_close);
	a_scan(v_symbol_read);
	a_scan(v_symbol_write);
	a_scan(v_symbol_flush);
	a_scan(v_symbol_read_line);
	a_scan(v_symbol_write_line);
	a_scan(v_symbol_resume);
	a_scan(v_type_file);
	a_scan(v_type_reader);
	a_scan(v_type_writer);
}

std::vector<std::pair<t_root, t_rvalue>> t_io::f_define()
{
	v_symbol_close = t_symbol::f_instantiate(L"close"sv);
	v_symbol_read = t_symbol::f_instantiate(L"read"sv);
	v_symbol_write = t_symbol::f_instantiate(L"write"sv);
	v_symbol_flush = t_symbol::f_instantiate(L"flush"sv);
	v_symbol_read_line = t_symbol::f_instantiate(L"read_line"sv);
	v_symbol_write_line = t_symbol::f_instantiate(L"write_line"sv);
	v_symbol_resume = t_symbol::f_instantiate(L"resume"sv);
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
