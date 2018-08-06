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
struct t_type_of<io::t_writer> : t_derivable<t_holds<io::t_writer>>
{
	typedef t_io t_extension;

	static void f_define(t_io* a_extension);

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super, t_scoped&& a_module) : t_base(a_ids, a_super, std::move(a_module))
	{
		f_scan = f_do_scan;
		v_construct = static_cast<t_scoped (t_type::*)(t_stacked*, size_t)>(&t_type_of::f_do_construct);
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
