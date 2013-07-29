#ifndef XEMMAI__IO__READER_H
#define XEMMAI__IO__READER_H

#include <iconv.h>

#include "../object.h"

namespace xemmai
{

class t_io;

namespace io
{

class t_reader
{
	friend struct t_type_of<t_reader>;

	iconv_t v_cd;
	t_slot v_stream;
	t_slot v_buffer;
	char* v_p;
	size_t v_n;

	size_t f_read(t_io* a_extension);
	wint_t f_get(t_io* a_extension);

public:
	static t_scoped f_instantiate(t_scoped&& a_stream, const std::wstring& a_encoding);

	t_reader(t_scoped&& a_stream, const std::wstring& a_encoding);
	~t_reader()
	{
		iconv_close(v_cd);
	}
	void f_close(t_io* a_extension);
	std::wstring f_read(t_io* a_extension, size_t a_size);
	std::wstring f_read_line(t_io* a_extension);
};

}

template<>
struct t_type_of<io::t_reader> : t_type
{
	typedef t_io t_extension;

	static void f_define(t_io* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
