#ifndef XEMMAI__IO__WRITER_H
#define XEMMAI__IO__WRITER_H

#include <iconv.h>

#include "../object.h"

namespace xemmai
{

class t_io;

namespace io
{

class t_writer
{
	friend struct t_type_of<t_writer>;

	iconv_t v_cd;
	t_slot v_stream;
	t_slot v_buffer;
	char* v_p;
	size_t v_n;

	void f_write(t_io* a_extension);
	void f_write(t_io* a_extension, const wchar_t* a_p, size_t a_n);
	void f_write(t_io* a_extension, const std::wstring& a_text)
	{
		f_write(a_extension, a_text.c_str(), a_text.size());
	}
	void f_unshift(t_io* a_extension);

public:
	static t_scoped f_instantiate(t_scoped&& a_stream, const std::wstring& a_encoding);

	t_writer(t_scoped&& a_stream, const std::wstring& a_encoding);
	~t_writer()
	{
		iconv_close(v_cd);
	}
	void f_close(t_io* a_extension);
	void f_write(t_io* a_extension, const t_value& a_value);
	void f_write_line(t_io* a_extension);
	void f_write_line(t_io* a_extension, const t_value& a_value);
	void f_flush(t_io* a_extension);
};

}

template<>
struct t_type_of<io::t_writer> : t_with_ids<io::t_writer>
{
	typedef t_io t_extension;

	static void f_define(t_io* a_extension);

	using t_with_ids<io::t_writer>::t_with_ids;
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
