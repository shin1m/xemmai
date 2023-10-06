#ifndef XEMMAI__IO__WRITER_H
#define XEMMAI__IO__WRITER_H

#include "../sharable.h"
#include "../string.h"
#include <iconv.h>

namespace xemmai
{

class t_io;

namespace io
{

class t_writer : public t_sharable
{
	friend struct t_type_of<t_writer>;

	iconv_t v_cd;
	t_slot v_stream;
	t_slot v_buffer;
	char* v_p0;
	size_t v_n0;
	char* v_p1;
	size_t v_n1;
	char* v_remain = nullptr;
	t_slot v_value;
	int v_step = 0;

	XEMMAI__LOCAL bool f_write(t_io* a_library);
	XEMMAI__LOCAL bool f_write(t_io* a_library, char** a_p, size_t* a_n);
	XEMMAI__LOCAL bool f__write(t_io* a_library, const t_pvalue& a_value);
	XEMMAI__LOCAL bool f__resume(t_io* a_library);

public:
	static t_object* f_instantiate(const t_pvalue& a_stream, std::wstring_view a_encoding);

	t_writer(const t_pvalue& a_stream, std::wstring_view a_encoding);
	~t_writer()
	{
		iconv_close(v_cd);
	}
	void f_close(t_io* a_library);
	bool f_write(t_io* a_library, const t_pvalue& a_value);
	bool f_write_line(t_io* a_library, const t_pvalue& a_value);
	bool f_flush(t_io* a_library);
	bool f_resume(t_io* a_library);
};

}

template<>
struct t_type_of<io::t_writer> : t_derivable<t_holds<io::t_writer>>
{
	using t_library = t_io;

	XEMMAI__LOCAL static void f_define(t_io* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		auto& p = a_this->f_as<io::t_writer>();
		a_scan(p.v_stream);
		a_scan(p.v_buffer);
		a_scan(p.v_value);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
