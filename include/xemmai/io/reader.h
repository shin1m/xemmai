#ifndef XEMMAI__IO__READER_H
#define XEMMAI__IO__READER_H

#include "../string.h"
#include <iconv.h>

namespace xemmai
{

class t_io;

namespace io
{

class t_reader
{
	friend struct t_type_of<t_reader>;

	std::mutex v_mutex;
	iconv_t v_cd;
	t_slot v_stream;
	t_slot v_buffer;
	char* v_p;
	size_t v_n;

	size_t f_read(t_io* a_library);
	wint_t f_get(t_io* a_library);

public:
	static t_object* f_instantiate(const t_pvalue& a_stream, std::wstring_view a_encoding, size_t a_buffer);

	t_reader(const t_pvalue& a_stream, std::wstring_view a_encoding, size_t a_buffer);
	t_reader(const t_pvalue& a_stream, std::wstring_view a_encoding) : t_reader(a_stream, a_encoding, 1024)
	{
	}
	~t_reader()
	{
		iconv_close(v_cd);
	}
	void f_close(t_io* a_library);
	t_object* f_read(t_io* a_library, size_t a_size);
	t_object* f_read_line(t_io* a_library);
};

}

template<>
struct t_type_of<io::t_reader> : t_derivable<t_holds<io::t_reader>>
{
	using t_library = t_io;

	static void f_define(t_io* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		auto& p = a_this->f_as<io::t_reader>();
		a_scan(p.v_stream);
		a_scan(p.v_buffer);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
