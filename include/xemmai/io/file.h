#ifndef XEMMAI__IO__FILE_H
#define XEMMAI__IO__FILE_H

#include "../engine.h"
#include "../bytes.h"
#include "../sharable.h"
#ifdef _WIN32
#include <io.h>
#endif

namespace xemmai
{

class t_io;

namespace io
{

class t_FILE
{
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

class t_file : public t_sharable
{
	friend struct t_type_of<t_file>;

	int v_fd;
	bool v_own;

public:
	static t_object* f_instantiate(auto&&... a_xs);

	t_file(std::wstring_view a_path, std::wstring_view a_mode);
	t_file(int a_fd, bool a_own) : v_fd(a_fd), v_own(a_own)
	{
	}
	~t_file() noexcept(false)
	{
		if (v_fd >= 0 && v_own) while (close(v_fd) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
	}
	int f_fd() const
	{
		return v_fd;
	}
	void f_close()
	{
		f_owned_or_shared<t_lock_with_safe_region>([&]
		{
			if (v_fd < 0) f_throw(L"already closed."sv);
			if (!v_own) f_throw(L"can not close unown."sv);
			while (close(v_fd) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
			v_fd = -1;
		});
	}
	intptr_t f_seek(intptr_t a_offset, int a_whence)
	{
		return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
		{
			if (v_fd < 0) f_throw(L"already closed."sv);
			auto n = lseek(v_fd, a_offset, a_whence);
			if (n == -1) throw std::system_error(errno, std::generic_category());
			return n;
		});
	}
	XEMMAI__PUBLIC intptr_t f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	XEMMAI__PUBLIC size_t f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size);
	bool f_tty();
#ifdef __unix__
	bool f_blocking();
	void f_blocking__(bool a_value);
#endif
};

}

template<>
struct t_type_of<io::t_file> : t_derivable<t_holds<io::t_file, t_type_of<t_sharable>>>
{
	using t_library = t_io;

	XEMMAI__LOCAL static void f_define(t_io* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
