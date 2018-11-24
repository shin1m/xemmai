#include <xemmai/io/reader.h>

#include <xemmai/bytes.h>
#include <xemmai/convert.h>
#include <xemmai/io.h>

namespace xemmai
{

namespace io
{

size_t t_reader::f_read(t_io* a_extension)
{
	auto& buffer = f_as<t_bytes&>(v_buffer);
	auto p = reinterpret_cast<char*>(&buffer[0]);
	std::copy(v_p, v_p + v_n, p);
	v_p = p;
	t_scoped n = v_stream.f_invoke(a_extension->f_symbol_read(), v_buffer, f_global()->f_as(v_n), f_global()->f_as(buffer.f_size() - v_n));
	f_check<size_t>(n, L"result of read");
	v_n += f_as<size_t>(n);
	return f_as<size_t>(n);
}

wint_t t_reader::f_get(t_io* a_extension)
{
	if (v_n <= 0 && f_read(a_extension) <= 0) return WEOF;
	wchar_t c;
	auto p = reinterpret_cast<char*>(&c);
	size_t n = sizeof(c);
	while (true) {
		if (iconv(v_cd, &v_p, &v_n, &p, &n) == size_t(-1)) {
			switch (errno) {
			case EILSEQ:
				f_throw(L"invalid sequence."sv);
			case EINVAL:
				if (f_read(a_extension) <= 0) f_throw(L"incomplete sequence."sv);
				continue;
			case E2BIG:
				break;
			default:
				f_throw(L"failed to iconv."sv);
			}
			break;
		}
		if (n <= 0) break;
		if (f_read(a_extension) <= 0) return WEOF;
	}
	return c;
}

t_scoped t_reader::f_instantiate(t_scoped&& a_stream, std::wstring_view a_encoding, size_t a_buffer)
{
	return f_new<t_reader>(f_extension<t_io>(f_engine()->f_module_io()), false, std::move(a_stream), a_encoding, a_buffer);
}

t_reader::t_reader(t_scoped&& a_stream, std::wstring_view a_encoding, size_t a_buffer) : v_cd(iconv_open("wchar_t", portable::f_convert(a_encoding).c_str())), v_n(0)
{
	if (v_cd == iconv_t(-1)) f_throw(L"failed to iconv_open."sv);
	v_stream = std::move(a_stream);
	v_buffer = t_bytes::f_instantiate(std::max(a_buffer, size_t(1)));
	static_cast<t_object*>(v_buffer)->f_share();
}

void t_reader::f_close(t_io* a_extension)
{
	if (!v_stream) f_throw(L"already closed."sv);
	v_stream.f_invoke(a_extension->f_symbol_close());
	v_stream = nullptr;
}

t_scoped t_reader::f_read(t_io* a_extension, size_t a_size)
{
	if (!v_stream) f_throw(L"already closed."sv);
	std::vector<wchar_t> cs(a_size);
	for (size_t i = 0; i < a_size; ++i) {
		wint_t c = f_get(a_extension);
		if (c == WEOF) {
			cs.resize(i);
			break;
		}
		cs[i] = c;
	}
	return t_string::f_instantiate(cs.data(), cs.size());
}

t_scoped t_reader::f_read_line(t_io* a_extension)
{
	if (!v_stream) f_throw(L"already closed."sv);
	std::vector<wchar_t> cs;
	while (true) {
		wint_t c = f_get(a_extension);
		if (c == WEOF) break;
		cs.push_back(c);
		if (c == L'\n') break;
	}
	return t_string::f_instantiate(cs.data(), cs.size());
}

}

void t_type_of<io::t_reader>::f_define(t_io* a_extension)
{
	t_define<io::t_reader, t_object>(a_extension, L"Reader"sv)
		(
			t_construct<false, t_scoped&&, t_string&>(),
			t_construct<false, t_scoped&&, t_string&, size_t>()
		)
		(a_extension->f_symbol_close(), t_member<void(io::t_reader::*)(t_io*), &io::t_reader::f_close, t_with_lock_for_write>())
		(a_extension->f_symbol_read(), t_member<t_scoped(io::t_reader::*)(t_io*, size_t), &io::t_reader::f_read, t_with_lock_for_write>())
		(a_extension->f_symbol_read_line(), t_member<t_scoped(io::t_reader::*)(t_io*), &io::t_reader::f_read_line, t_with_lock_for_write>())
	;
}

t_scoped t_type_of<io::t_reader>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<false, t_scoped&&, const t_string&>,
		t_construct<false, t_scoped&&, const t_string&, size_t>
	>::t_bind<io::t_reader>::f_do(this, a_stack, a_n);
}

}
