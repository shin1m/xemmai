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
	char* v_p;
	size_t v_n;

	void f_write(t_io* a_library);
	void f_write(t_io* a_library, const wchar_t* a_p, size_t a_n);
	void f_write(t_io* a_library, const t_string& a_text)
	{
		f_write(a_library, a_text, a_text.f_size());
	}
	void f_unshift(t_io* a_library);

public:
	static t_object* f_instantiate(const t_pvalue& a_stream, std::wstring_view a_encoding);

	t_writer(const t_pvalue& a_stream, std::wstring_view a_encoding);
	~t_writer()
	{
		iconv_close(v_cd);
	}
	void f_close(t_io* a_library);
	void f_write(t_io* a_library, const t_pvalue& a_value);
	void f_write_line(t_io* a_library);
	void f_write_line(t_io* a_library, const t_pvalue& a_value);
	void f_flush(t_io* a_library);
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
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
