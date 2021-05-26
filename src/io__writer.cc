#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace io
{

void t_writer::f_write(t_io* a_library)
{
	auto& buffer = f_as<t_bytes&>(v_buffer);
	auto p = reinterpret_cast<char*>(&buffer[0]);
	static size_t index;
	t_pvalue(v_stream).f_invoke(a_library->f_symbol_write(), index, t_pvalue(v_buffer), f_global()->f_as(0), f_global()->f_as(v_p - p));
	v_p = p;
	v_n = buffer.f_size();
}

void t_writer::f_write(t_io* a_library, const wchar_t* a_p, size_t a_n)
{
	auto p = const_cast<char*>(reinterpret_cast<const char*>(a_p));
	size_t n = a_n * sizeof(wchar_t);
	while (iconv(v_cd, &p, &n, &v_p, &v_n) == size_t(-1)) {
		switch (errno) {
		case EILSEQ:
			f_throw(L"invalid character."sv);
		case E2BIG:
			f_write(a_library);
		case EINTR:
			break;
		default:
			throw std::system_error(errno, std::generic_category());
		}
	}
}

void t_writer::f_unshift(t_io* a_library)
{
	while (iconv(v_cd, NULL, NULL, &v_p, &v_n) == size_t(-1)) {
		switch (errno) {
		case E2BIG:
			f_write(a_library);
		case EINTR:
			break;
		default:
			throw std::system_error(errno, std::generic_category());
		}
	}
	f_write(a_library);
}

t_object* t_writer::f_instantiate(const t_pvalue& a_stream, std::wstring_view a_encoding)
{
	return f_new<t_writer>(&f_engine()->f_module_io()->f_as<t_module>().v_body->f_as<t_io>(), a_stream, a_encoding);
}

t_writer::t_writer(const t_pvalue& a_stream, std::wstring_view a_encoding) : v_cd(iconv_open(portable::f_convert(a_encoding).c_str(), "wchar_t"))
{
	if (v_cd == iconv_t(-1)) f_throw(L"failed to iconv_open."sv);
	v_stream = a_stream;
	v_buffer = t_bytes::f_instantiate(1024);
	auto& buffer = f_as<t_bytes&>(v_buffer);
	v_p = reinterpret_cast<char*>(&buffer[0]);
	v_n = buffer.f_size();
}

void t_writer::f_close(t_io* a_library)
{
	std::lock_guard lock(v_mutex);
	if (!v_stream) f_throw(L"already closed."sv);
	f_unshift(a_library);
	static size_t index;
	t_pvalue(v_stream).f_invoke(a_library->f_symbol_close(), index);
	v_stream = nullptr;
}

void t_writer::f_write(t_io* a_library, const t_pvalue& a_value)
{
	std::lock_guard lock(v_mutex);
	if (!v_stream) f_throw(L"already closed."sv);
	if (f_is<t_string>(a_value)) {
		f_write(a_library, f_as<const t_string&>(a_value));
	} else {
		auto x = a_value.f_string();
		f_check<t_string>(x, L"value");
		f_write(a_library, f_as<const t_string&>(x));
	}
}

void t_writer::f_write_line(t_io* a_library)
{
	std::lock_guard lock(v_mutex);
	if (!v_stream) f_throw(L"already closed."sv);
	f_write(a_library, L"\n", 1);
	f_unshift(a_library);
	static size_t index;
	t_pvalue(v_stream).f_invoke(a_library->f_symbol_flush(), index);
}

void t_writer::f_write_line(t_io* a_library, const t_pvalue& a_value)
{
	std::lock_guard lock(v_mutex);
	if (!v_stream) f_throw(L"already closed."sv);
	if (f_is<t_string>(a_value)) {
		f_write(a_library, f_as<const t_string&>(a_value));
	} else {
		auto x = a_value.f_string();
		f_check<t_string>(x, L"value");
		f_write(a_library, f_as<const t_string&>(x));
	}
	f_write(a_library, L"\n", 1);
	f_unshift(a_library);
	static size_t index;
	t_pvalue(v_stream).f_invoke(a_library->f_symbol_flush(), index);
}

void t_writer::f_flush(t_io* a_library)
{
	std::lock_guard lock(v_mutex);
	if (!v_stream) f_throw(L"already closed."sv);
	f_unshift(a_library);
	static size_t index;
	t_pvalue(v_stream).f_invoke(a_library->f_symbol_flush(), index);
}

}

void t_type_of<io::t_writer>::f_define(t_io* a_library)
{
	t_define{a_library}
		(a_library->f_symbol_close(), t_member<void(io::t_writer::*)(t_io*), &io::t_writer::f_close>())
		(a_library->f_symbol_write(), t_member<void(io::t_writer::*)(t_io*, const t_pvalue&), &io::t_writer::f_write>())
		(a_library->f_symbol_write_line(),
			t_member<void(io::t_writer::*)(t_io*), &io::t_writer::f_write_line>(),
			t_member<void(io::t_writer::*)(t_io*, const t_pvalue&), &io::t_writer::f_write_line>()
		)
		(a_library->f_symbol_flush(), t_member<void(io::t_writer::*)(t_io*), &io::t_writer::f_flush>())
	.f_derive<io::t_writer, t_object>();
}

t_pvalue t_type_of<io::t_writer>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<const t_pvalue&, const t_string&>::t_bind<io::t_writer>::f_do(this, a_stack, a_n);
}

}
