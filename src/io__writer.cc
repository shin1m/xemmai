#include <xemmai/io/writer.h>

#include <cerrno>
#include <xemmai/bytes.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>
#include <xemmai/io.h>

namespace xemmai
{

namespace io
{

void t_writer::f_write(t_io* a_extension)
{
	t_bytes& buffer = f_as<t_bytes&>(v_buffer);
	char* p = reinterpret_cast<char*>(&buffer[0]);
	v_stream.f_get(a_extension->f_symbol_write())(v_buffer, f_global()->f_as(0), f_global()->f_as(v_p - p));
	v_p = p;
	v_n = buffer.f_size();
}

void t_writer::f_write(t_io* a_extension, const wchar_t* a_p, size_t a_n)
{
	char* p = const_cast<char*>(reinterpret_cast<const char*>(a_p));
	size_t n = a_n * sizeof(wchar_t);
	while (iconv(v_cd, &p, &n, &v_p, &v_n) == size_t(-1)) {
		switch (errno) {
		case EILSEQ:
			t_throwable::f_throw(L"invalid character.");
		case E2BIG:
			f_write(a_extension);
			break;
		default:
			t_throwable::f_throw(L"failed to iconv.");
		}
	}
}

void t_writer::f_unshift(t_io* a_extension)
{
	while (iconv(v_cd, NULL, NULL, &v_p, &v_n) == size_t(-1)) {
		switch (errno) {
		case E2BIG:
			f_write(a_extension);
			break;
		default:
			t_throwable::f_throw(L"failed to iconv.");
		}
	}
	f_write(a_extension);
}

t_transfer t_writer::f_instantiate(const t_transfer& a_stream, const std::wstring& a_encoding)
{
	t_io* extension = f_extension<t_io>(f_engine()->f_module_io());
	t_transfer object = t_object::f_allocate(extension->f_type<t_writer>());
	object.f_pointer__(new t_writer(a_stream, a_encoding));
	return object;
}

t_writer::t_writer(const t_transfer& a_stream, const std::wstring& a_encoding) : v_cd(iconv_open(portable::f_convert(a_encoding).c_str(), "wchar_t"))
{
	if (v_cd == iconv_t(-1)) t_throwable::f_throw(L"failed to iconv_open.");
	v_stream = a_stream;
	v_buffer = t_bytes::f_instantiate(1024);
	static_cast<t_object*>(v_buffer)->f_share();
	t_bytes& buffer = f_as<t_bytes&>(v_buffer);
	v_p = reinterpret_cast<char*>(&buffer[0]);
	v_n = buffer.f_size();
}

void t_writer::f_close(t_io* a_extension)
{
	if (!v_stream) t_throwable::f_throw(L"already closed.");
	f_unshift(a_extension);
	v_stream.f_get(a_extension->f_symbol_close())();
	v_stream = nullptr;
}

void t_writer::f_write(t_io* a_extension, const t_value& a_value)
{
	if (!v_stream) t_throwable::f_throw(L"already closed.");
	if (f_is<const std::wstring&>(a_value)) {
		f_write(a_extension, f_as<const std::wstring&>(a_value));
	} else {
		t_transfer x = a_value.f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(x, L"value");
		f_write(a_extension, f_as<const std::wstring&>(x));
	}
}

void t_writer::f_write_line(t_io* a_extension)
{
	if (!v_stream) t_throwable::f_throw(L"already closed.");
	f_write(a_extension, L"\n", 1);
	f_unshift(a_extension);
	v_stream.f_get(a_extension->f_symbol_flush())();
}

void t_writer::f_write_line(t_io* a_extension, const t_value& a_value)
{
	if (!v_stream) t_throwable::f_throw(L"already closed.");
	if (f_is<const std::wstring&>(a_value)) {
		f_write(a_extension, f_as<const std::wstring&>(a_value));
	} else {
		t_transfer x = a_value.f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(x, L"value");
		f_write(a_extension, f_as<const std::wstring&>(x));
	}
	f_write(a_extension, L"\n", 1);
	f_unshift(a_extension);
	v_stream.f_get(a_extension->f_symbol_flush())();
}

void t_writer::f_flush(t_io* a_extension)
{
	if (!v_stream) t_throwable::f_throw(L"already closed.");
	f_unshift(a_extension);
	v_stream.f_get(a_extension->f_symbol_flush())();
}

}

void t_type_of<io::t_writer>::f_define(t_io* a_extension)
{
	t_define<io::t_writer, t_object>(a_extension, L"Writer")
		(t_construct<const t_transfer&, const std::wstring&>())
		(a_extension->f_symbol_close(), t_member<void (io::t_writer::*)(t_io*), &io::t_writer::f_close, t_with_lock_for_write>())
		(a_extension->f_symbol_write(), t_member<void (io::t_writer::*)(t_io*, const t_value&), &io::t_writer::f_write, t_with_lock_for_write>())
		(a_extension->f_symbol_write_line(),
			t_member<void (io::t_writer::*)(t_io*), &io::t_writer::f_write_line, t_with_lock_for_write>(),
			t_member<void (io::t_writer::*)(t_io*, const t_value&), &io::t_writer::f_write_line, t_with_lock_for_write>()
		)
		(a_extension->f_symbol_flush(), t_member<void (io::t_writer::*)(t_io*), &io::t_writer::f_flush, t_with_lock_for_write>())
	;
}

t_type* t_type_of<io::t_writer>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<io::t_writer>::f_scan(t_object* a_this, t_scan a_scan)
{
	io::t_writer* p = &f_as<io::t_writer&>(a_this);
	if (!p) return;
	a_scan(p->v_stream);
	a_scan(p->v_buffer);
}

void t_type_of<io::t_writer>::f_finalize(t_object* a_this)
{
	delete &f_as<io::t_writer&>(a_this);
}

t_transfer t_type_of<io::t_writer>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct<const t_transfer&, const std::wstring&>::t_bind<io::t_writer>::f_do(a_class, a_stack, a_n);
}

}
