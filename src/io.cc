#include <xemmai/io.h>

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
	a_scan(v_type_file);
	a_scan(v_type_reader);
	a_scan(v_type_writer);
}

}
