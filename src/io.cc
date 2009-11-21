#include <xemmai/io.h>

namespace xemmai
{

t_io::t_io(t_object* a_module) : t_extension(a_module)
{
	v_symbol_close = t_symbol::f_instantiate(L"close");
	v_symbol_read = t_symbol::f_instantiate(L"read");
	v_symbol_write = t_symbol::f_instantiate(L"write");
	v_symbol_flush = t_symbol::f_instantiate(L"flush");
	v_symbol_read_line = t_symbol::f_instantiate(L"read_line");
	v_symbol_write_line = t_symbol::f_instantiate(L"write_line");
	t_type_of<io::t_file>::f_define(this);
	xemmai::f_as<t_type*>(v_type_file)->v_builtin = true;
	t_type_of<io::t_reader>::f_define(this);
	xemmai::f_as<t_type*>(v_type_reader)->v_builtin = true;
	t_type_of<io::t_writer>::f_define(this);
	xemmai::f_as<t_type*>(v_type_writer)->v_builtin = true;
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
}

}
