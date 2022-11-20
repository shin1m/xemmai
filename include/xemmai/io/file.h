#ifndef XEMMAI__IO__FILE_H
#define XEMMAI__IO__FILE_H

#include "../engine.h"
#include "../bytes.h"

namespace xemmai
{

class t_io;

namespace io
{

class t_FILE
{
protected:
	std::FILE* v_stream;

public:
	t_FILE(std::FILE* a_stream) : v_stream(a_stream)
	{
	}
	~t_FILE()
	{
		if (v_stream != NULL) std::fclose(v_stream);
	}
	operator std::FILE*() const
	{
		return v_stream;
	}
};

class t_file : public t_FILE
{
	friend struct t_type_of<t_file>;

	std::shared_mutex v_mutex;
	bool v_own = false;

public:
	template<typename... T_an>
	static t_object* f_instantiate(T_an&&... a_an);

	using t_FILE::t_FILE;
	t_file(std::wstring_view a_path, std::wstring_view a_mode);
	t_file(int a_fd, std::wstring_view a_mode);
	~t_file()
	{
		if (!v_own) v_stream = NULL;
	}
	void f_reopen(std::wstring_view a_path, std::wstring_view a_mode);
	void f_close()
	{
		t_lock_with_safe_region lock(v_mutex);
		if (v_stream == NULL) f_throw(L"already closed."sv);
		if (!v_own) f_throw(L"can not close unown."sv);
		std::fclose(v_stream);
		v_stream = NULL;
	}
	void f_seek(intptr_t a_offset, int a_whence)
	{
		t_shared_lock_with_safe_region lock(v_mutex);
		if (v_stream == NULL) f_throw(L"already closed."sv);
		if (std::fseek(v_stream, a_offset, a_whence) == -1) f_throw(L"failed to seek."sv);
	}
	intptr_t f_tell()
	{
		t_shared_lock_with_safe_region lock(v_mutex);
		if (v_stream == NULL) f_throw(L"already closed."sv);
		intptr_t n = std::ftell(v_stream);
		if (n == -1) f_throw(L"failed to tell."sv);
		return n;
	}
	XEMMAI__PORTABLE__EXPORT size_t f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	XEMMAI__PORTABLE__EXPORT void f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	void f_flush()
	{
		t_shared_lock_with_safe_region lock(v_mutex);
		if (v_stream == NULL) f_throw(L"already closed."sv);
		std::fflush(v_stream);
	}
	bool f_tty();
#ifdef __unix__
	bool f_blocking();
	void f_blocking__(bool a_value);
#endif
};

}

template<>
struct t_type_of<io::t_file> : t_derivable<t_holds<io::t_file>>
{
	using t_library = t_io;

	static void f_define(t_io* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
