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
	bool v_own;

public:
	static XEMMAI__PORTABLE__EXPORT t_transfer f_instantiate(FILE* a_stream);
	static XEMMAI__PORTABLE__EXPORT t_transfer f_instantiate(const std::wstring& a_path, const std::wstring& a_mode);

	t_file(FILE* a_stream) : v_stream(a_stream), v_own(false)
	{
	}
	XEMMAI__PORTABLE__EXPORT t_file(const std::wstring& a_path, const char* a_mode);
	XEMMAI__PORTABLE__EXPORT t_file(const std::wstring& a_path, const std::wstring& a_mode);
	~t_file()
	{
		if (v_stream != NULL && v_own) std::fclose(v_stream);
	}
	operator FILE*() const
	{
		return v_stream;
	}
	XEMMAI__PORTABLE__EXPORT void f_reopen(const std::wstring& a_path, const std::wstring& a_mode);
	XEMMAI__PORTABLE__EXPORT void f_close();
	XEMMAI__PORTABLE__EXPORT void f_seek(ptrdiff_t a_offset, int a_whence);
	XEMMAI__PORTABLE__EXPORT ptrdiff_t f_tell();
	XEMMAI__PORTABLE__EXPORT size_t f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	XEMMAI__PORTABLE__EXPORT void f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	XEMMAI__PORTABLE__EXPORT void f_flush();
};

}

template<>
struct t_type_of<io::t_file> : t_type
{
	typedef t_io t_extension;

	static void f_define(t_io* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
