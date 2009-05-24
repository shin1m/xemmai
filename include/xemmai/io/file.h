#ifndef XEMMAI__IO__FILE_H
#define XEMMAI__IO__FILE_H

#include <cstdio>

#include "../bytes.h"

namespace xemmai
{

class t_io;

namespace io
{

class t_file
{
	friend struct t_type_of<t_file>;

	FILE* v_stream;

public:
	static t_transfer f_instantiate(const std::wstring& a_path, const std::wstring& a_mode);

	t_file(const std::wstring& a_path, const char* a_mode);
	t_file(const std::wstring& a_path, const std::wstring& a_mode);
	~t_file()
	{
		if (v_stream != NULL) std::fclose(v_stream);
	}
	operator FILE*() const
	{
		return v_stream;
	}
	void f_close();
	void f_seek(int a_offset, int a_whence);
	int f_tell();
	size_t f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	void f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	void f_flush();
};

}

template<>
struct t_type_of<io::t_file> : t_type
{
	typedef t_io t_extension;

	static void f_close(t_object* a_self);
	static void f_seek(t_object* a_self, int a_offset, int a_whence);
	static int f_tell(t_object* a_self);
	static size_t f_read(t_object* a_self, t_bytes& a_bytes, size_t a_offset, size_t a_size);
	static void f_write(t_object* a_self, t_bytes& a_bytes, size_t a_offset, size_t a_size);
	static void f_flush(t_object* a_self);
	static t_transfer f_define(t_io* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

#endif
