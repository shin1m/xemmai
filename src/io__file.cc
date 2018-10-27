#include <xemmai/io/file.h>

#include <xemmai/convert.h>
#include <xemmai/io.h>
#ifdef __unix__
#include <unistd.h>
#include <fcntl.h>
#endif
#ifdef _MSC_VER
#include <io.h>
#endif

namespace xemmai
{

namespace io
{

t_file::t_file(std::wstring_view a_path, const char* a_mode) : v_stream(std::fopen(portable::f_convert(a_path).c_str(), a_mode)), v_own(true)
{
}

t_file::t_file(std::wstring_view a_path, std::wstring_view a_mode) : v_stream(std::fopen(portable::f_convert(a_path).c_str(), portable::f_convert(a_mode).c_str())), v_own(true)
{
	if (v_stream == NULL) f_throw(L"failed to open."sv);
	std::setbuf(v_stream, NULL);
}

#ifdef __unix__
t_file::t_file(int a_fd, const char* a_mode) : v_stream(fdopen(a_fd, a_mode)), v_own(true)
{
	if (v_stream == NULL) f_throw(L"failed to open."sv);
	std::setbuf(v_stream, NULL);
}

t_file::t_file(int a_fd, std::wstring_view a_mode) : t_file(a_fd, portable::f_convert(a_mode).c_str())
{
}
#endif

void t_file::f_reopen(std::wstring_view a_path, std::wstring_view a_mode)
{
	v_stream = std::freopen(portable::f_convert(a_path).c_str(), portable::f_convert(a_mode).c_str(), v_stream);
	if (v_stream == NULL) f_throw(L"failed to open."sv);
	std::setbuf(v_stream, NULL);
}

size_t t_file::f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	t_safe_region region;
	if (v_stream == NULL) f_throw(L"already closed."sv);
	if (a_offset + a_size > a_bytes.f_size()) f_throw(L"out of range."sv);
	size_t n = std::fread(&a_bytes[0] + a_offset, 1, a_size, v_stream);
	if (n <= 0 && std::ferror(v_stream)) f_throw(L"failed to read."sv);
	return n;
}

void t_file::f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	t_safe_region region;
	if (v_stream == NULL) f_throw(L"already closed."sv);
	if (a_offset + a_size > a_bytes.f_size()) f_throw(L"out of range."sv);
	unsigned char* p = &a_bytes[0] + a_offset;
	while (true) {
		size_t n = std::fwrite(p, 1, a_size, v_stream);
		if (n <= 0 && std::ferror(v_stream)) f_throw(L"failed to write."sv);
		a_size -= n;
		if (a_size <= 0) break;
		p += n;
	}
}

bool t_file::f_tty() const
{
	if (v_stream == NULL) f_throw(L"already closed."sv);
	return isatty(fileno(v_stream)) == 1;
}

#ifdef __unix__
bool t_file::f_blocking() const
{
	if (v_stream == NULL) f_throw(L"already closed."sv);
	return (fcntl(fileno(v_stream), F_GETFL) & O_NONBLOCK) == 0;
}

void t_file::f_blocking__(bool a_value)
{
	if (v_stream == NULL) f_throw(L"already closed."sv);
	int flags = fcntl(fileno(v_stream), F_GETFL);
	if (a_value)
		flags &= ~O_NONBLOCK;
	else
		flags |= O_NONBLOCK;
	if (fcntl(fileno(v_stream), F_SETFL, flags) == -1) f_throw(L"failed to F_SETFL."sv);
}
#endif

}

void t_type_of<io::t_file>::f_define(t_io* a_extension)
{
	t_define<io::t_file, t_object>(a_extension, L"File"sv)
#ifdef __unix__
		(
			t_construct<false, std::wstring_view, std::wstring_view>(),
			t_construct<false, int, std::wstring_view>()
		)
#else
		(t_construct<false, std::wstring_view, std::wstring_view>())
#endif
		(L"reopen"sv, t_member<void(io::t_file::*)(std::wstring_view, std::wstring_view), &io::t_file::f_reopen, t_with_lock_for_write>())
		(a_extension->f_symbol_close(), t_member<void(io::t_file::*)(), &io::t_file::f_close, t_with_lock_for_write>())
		(L"seek"sv, t_member<void(io::t_file::*)(intptr_t, int), &io::t_file::f_seek, t_with_lock_for_write>())
		(L"tell"sv, t_member<intptr_t(io::t_file::*)() const, &io::t_file::f_tell, t_with_lock_for_read>())
		(a_extension->f_symbol_read(), t_member<size_t(io::t_file::*)(t_bytes&, size_t, size_t), &io::t_file::f_read, t_with_lock_for_write>())
		(a_extension->f_symbol_write(), t_member<void(io::t_file::*)(t_bytes&, size_t, size_t), &io::t_file::f_write, t_with_lock_for_write>())
		(a_extension->f_symbol_flush(), t_member<void(io::t_file::*)(), &io::t_file::f_flush, t_with_lock_for_write>())
		(L"tty"sv, t_member<bool(io::t_file::*)() const, &io::t_file::f_tty, t_with_lock_for_read>())
#ifdef __unix__
		(L"blocking"sv, t_member<bool(io::t_file::*)() const, &io::t_file::f_blocking, t_with_lock_for_read>())
		(L"blocking__"sv, t_member<void(io::t_file::*)(bool), &io::t_file::f_blocking__, t_with_lock_for_write>())
#endif
	;
}

t_scoped t_type_of<io::t_file>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
#ifdef __unix__
	return t_overload<
		t_construct<false, std::wstring_view, std::wstring_view>,
		t_construct<false, int, std::wstring_view>
	>::t_bind<io::t_file>::f_do(this, a_stack, a_n);
#else
	return t_construct<false, std::wstring_view, std::wstring_view>::t_bind<io::t_file>::f_do(this, a_stack, a_n);
#endif
}

}
