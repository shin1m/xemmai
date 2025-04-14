#include <xemmai/io.h>
#include <xemmai/convert.h>
#include <fcntl.h>

namespace xemmai
{

namespace io
{

t_file::t_file(std::wstring_view a_path, std::wstring_view a_mode) : v_own(true)
{
	auto flags = [&]
	{
		switch (a_mode.size()) {
		case 1:
			switch (a_mode[0]) {
			case L'r':
				return O_RDONLY;
			case L'w':
				return O_WRONLY | O_CREAT | O_TRUNC;
			case L'a':
				return O_WRONLY | O_CREAT | O_APPEND;
			}
		case 2:
			if (a_mode[1] == L'+')
				switch (a_mode[0]) {
				case L'r':
					return O_RDWR;
				case L'w':
					return O_RDWR | O_CREAT | O_TRUNC;
				case L'a':
					return O_RDWR | O_CREAT | O_APPEND;
				}
		}
		f_throw(L"invalid mode."sv);
	}();
	while (true) {
#ifdef _WIN32
		v_fd = open(portable::f_convert(a_path).c_str(), flags, S_IREAD | S_IWRITE);
#else
		v_fd = open(portable::f_convert(a_path).c_str(), flags, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
		if (v_fd != -1) break;
		if (errno != EINTR) portable::f_throw_system_error();
	}
}

intptr_t t_file::f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
	{
		if (v_fd < 0) f_throw(L"already closed."sv);
		if (a_offset + a_size > a_bytes.f_size()) f_throw(L"out of range."sv);
		for (auto p = &a_bytes[0] + a_offset;;) {
			auto n = read(v_fd, p, a_size);
			if (n == -1) {
				if (errno == EINTR) continue;
				if (errno != EAGAIN && errno != EWOULDBLOCK) portable::f_throw_system_error();
			}
			return n;
		}
	});
}

size_t t_file::f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
	{
		if (v_fd < 0) f_throw(L"already closed."sv);
		if (a_offset + a_size > a_bytes.f_size()) f_throw(L"out of range."sv);
		auto size = a_size;
		for (auto p = &a_bytes[0] + a_offset;;) {
			auto n = write(v_fd, p, size);
			if (n == -1) {
				if (errno == EINTR) continue;
				if (errno == EAGAIN || errno == EWOULDBLOCK) break;
				portable::f_throw_system_error();
			}
			size -= n;
			if (size <= 0) break;
			p += n;
		}
		return a_size - size;
	});
}

bool t_file::f_tty()
{
	return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
	{
		if (v_fd < 0) f_throw(L"already closed."sv);
		return isatty(v_fd) == 1;
	});
}

#ifdef __unix__
bool t_file::f_blocking()
{
	return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
	{
		if (v_fd < 0) f_throw(L"already closed."sv);
		int flags = fcntl(v_fd, F_GETFL);
		if (flags == -1) portable::f_throw_system_error();
		return !(flags & O_NONBLOCK);
	});
}

void t_file::f_blocking__(bool a_value)
{
	f_owned_or_shared<t_shared_lock_with_safe_region>([&]
	{
		if (v_fd < 0) f_throw(L"already closed."sv);
		int flags = fcntl(v_fd, F_GETFL);
		if (flags == -1) portable::f_throw_system_error();
		if (a_value)
			flags &= ~O_NONBLOCK;
		else
			flags |= O_NONBLOCK;
		if (fcntl(v_fd, F_SETFL, flags) == -1) portable::f_throw_system_error();
	});
}
#endif

}

void t_type_of<io::t_file>::f_define(t_io* a_library)
{
	t_define{a_library}
	(a_library->f_symbol_close(), t_member<void(io::t_file::*)(), &io::t_file::f_close>())
	(L"seek"sv, t_member<intptr_t(io::t_file::*)(intptr_t, int), &io::t_file::f_seek>())
	(a_library->f_symbol_read(), t_member<intptr_t(io::t_file::*)(t_bytes&, size_t, size_t), &io::t_file::f_read>())
	(a_library->f_symbol_write(), t_member<size_t(io::t_file::*)(t_bytes&, size_t, size_t), &io::t_file::f_write>())
	(L"fd"sv, t_member<int(io::t_file::*)() const, &io::t_file::f_fd>())
	(L"tty"sv, t_member<bool(io::t_file::*)(), &io::t_file::f_tty>())
#ifdef __unix__
	(L"blocking"sv, t_member<bool(io::t_file::*)(), &io::t_file::f_blocking>())
	(L"blocking__"sv, t_member<void(io::t_file::*)(bool), &io::t_file::f_blocking__>())
#endif
	.f_derive<io::t_file, t_sharable>();
}

t_pvalue t_type_of<io::t_file>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<std::wstring_view, std::wstring_view>,
		t_construct<int, bool>
	>::t_bind<io::t_file>::f_do(this, a_stack, a_n);
}

}
