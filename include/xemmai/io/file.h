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

	static XEMMAI__PORTABLE__EXPORT t_scoped f_instantiate(t_file* a_file);

	std::FILE* v_stream;
	bool v_own = false;

public:
	static t_scoped f_instantiate(std::FILE* a_stream)
	{
		return f_instantiate(new t_file(a_stream));
	}
	static t_scoped f_instantiate(const std::wstring& a_path, const std::wstring& a_mode)
	{
		return f_instantiate(new t_file(a_path, a_mode));
	}
#ifdef __unix__
	static t_scoped f_instantiate(int a_fd, const std::wstring& a_mode)
	{
		return f_instantiate(new t_file(a_fd, a_mode));
	}
#endif

	t_file(std::FILE* a_stream) : v_stream(a_stream)
	{
	}
	XEMMAI__PORTABLE__EXPORT t_file(const std::wstring& a_path, const char* a_mode);
	XEMMAI__PORTABLE__EXPORT t_file(const std::wstring& a_path, const std::wstring& a_mode);
#ifdef __unix__
	t_file(int a_fd, const char* a_mode);
	t_file(int a_fd, const std::wstring& a_mode);
#endif
	~t_file()
	{
		if (v_stream != NULL && v_own) std::fclose(v_stream);
	}
	operator std::FILE*() const
	{
		return v_stream;
	}
	XEMMAI__PORTABLE__EXPORT void f_reopen(const std::wstring& a_path, const std::wstring& a_mode);
	XEMMAI__PORTABLE__EXPORT void f_close();
	XEMMAI__PORTABLE__EXPORT void f_seek(intptr_t a_offset, int a_whence);
	XEMMAI__PORTABLE__EXPORT intptr_t f_tell() const;
	XEMMAI__PORTABLE__EXPORT size_t f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	XEMMAI__PORTABLE__EXPORT void f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	XEMMAI__PORTABLE__EXPORT void f_flush();
	bool f_tty() const;
#ifdef __unix__
	bool f_blocking() const;
	void f_blocking__(bool a_value);
#endif
};

}

template<>
struct t_type_of<io::t_file> : t_derivable<t_holds<io::t_file>>
{
	typedef t_io t_extension;

	static void f_define(t_io* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
