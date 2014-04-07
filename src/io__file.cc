#include <xemmai/io/file.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>
#include <xemmai/io.h>
#ifdef __unix__
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <io.h>
#endif

namespace xemmai
{

namespace io
{

t_scoped t_file::f_instantiate(std::FILE* a_stream)
{
	t_io* extension = f_extension<t_io>(f_engine()->f_module_io());
	t_scoped object = t_object::f_allocate(extension->f_type<t_file>());
	object.f_pointer__(new t_file(a_stream));
	return object;
}

t_scoped t_file::f_instantiate(const std::wstring& a_path, const std::wstring& a_mode)
{
	t_io* extension = f_extension<t_io>(f_engine()->f_module_io());
	t_scoped object = t_object::f_allocate(extension->f_type<t_file>());
	object.f_pointer__(new t_file(a_path, a_mode));
	return object;
}

t_file::t_file(const std::wstring& a_path, const char* a_mode) : v_stream(std::fopen(portable::f_convert(a_path).c_str(), a_mode)), v_own(true)
{
}

t_file::t_file(const std::wstring& a_path, const std::wstring& a_mode) : v_stream(std::fopen(portable::f_convert(a_path).c_str(), portable::f_convert(a_mode).c_str())), v_own(true)
{
	if (v_stream == NULL) t_throwable::f_throw(L"failed to open.");
}

void t_file::f_reopen(const std::wstring& a_path, const std::wstring& a_mode)
{
	v_stream = std::freopen(portable::f_convert(a_path).c_str(), portable::f_convert(a_mode).c_str(), v_stream);
	if (v_stream == NULL) t_throwable::f_throw(L"failed to open.");
}

void t_file::f_close()
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	if (!v_own) t_throwable::f_throw(L"can not close unown.");
	std::fclose(v_stream);
	v_stream = NULL;
}

void t_file::f_seek(intptr_t a_offset, int a_whence)
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	if (std::fseek(v_stream, a_offset, a_whence) == -1) t_throwable::f_throw(L"failed to seek.");
}

intptr_t t_file::f_tell()
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	intptr_t n = std::ftell(v_stream);
	if (n == -1) t_throwable::f_throw(L"failed to tell.");
	return n;
}

size_t t_file::f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	t_safe_region region;
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	if (a_offset + a_size > a_bytes.f_size()) t_throwable::f_throw(L"out of range.");
	if (isatty(fileno(v_stream))) {
		unsigned char* p = &a_bytes[0] + a_offset;
		unsigned char* q = p;
		while (a_size > 0) {
			int c = std::getc(v_stream);
			if (c == EOF) {
				if (std::ferror(v_stream)) t_throwable::f_throw(L"failed to read.");
				break;
			}
			*p++ = c;
			if (c == '\n') break;
			--a_size;
		}
		return p - q;
	} else {
		size_t n = std::fread(&a_bytes[0] + a_offset, 1, a_size, v_stream);
		if (std::ferror(v_stream)) t_throwable::f_throw(L"failed to read.");
		return n;
	}
}

void t_file::f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	t_safe_region region;
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	if (a_offset + a_size > a_bytes.f_size()) t_throwable::f_throw(L"out of range.");
	unsigned char* p = &a_bytes[0] + a_offset;
	while (true) {
		size_t n = std::fwrite(p, 1, a_size, v_stream);
		if (std::ferror(v_stream)) t_throwable::f_throw(L"failed to write.");
		a_size -= n;
		if (a_size <= 0) break;
		p += n;
	}
}

void t_file::f_flush()
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	std::fflush(v_stream);
}

}

void t_type_of<io::t_file>::f_define(t_io* a_extension)
{
	t_define<io::t_file, t_object>(a_extension, L"File")
		(t_construct<const std::wstring&, const std::wstring&>())
		(L"reopen", t_member<void (io::t_file::*)(const std::wstring&, const std::wstring&), &io::t_file::f_reopen, t_with_lock_for_write>())
		(a_extension->f_symbol_close(), t_member<void (io::t_file::*)(), &io::t_file::f_close, t_with_lock_for_write>())
		(L"seek", t_member<void (io::t_file::*)(intptr_t, int), &io::t_file::f_seek, t_with_lock_for_write>())
		(L"tell", t_member<intptr_t (io::t_file::*)(), &io::t_file::f_tell, t_with_lock_for_write>())
		(a_extension->f_symbol_read(), t_member<size_t (io::t_file::*)(t_bytes&, size_t, size_t), &io::t_file::f_read, t_with_lock_for_write>())
		(a_extension->f_symbol_write(), t_member<void (io::t_file::*)(t_bytes&, size_t, size_t), &io::t_file::f_write, t_with_lock_for_write>())
		(a_extension->f_symbol_flush(), t_member<void (io::t_file::*)(), &io::t_file::f_flush, t_with_lock_for_write>())
	;
}

t_type* t_type_of<io::t_file>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(t_scoped(v_module), a_this);
}

void t_type_of<io::t_file>::f_finalize(t_object* a_this)
{
	delete &f_as<io::t_file&>(a_this);
}

t_scoped t_type_of<io::t_file>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct<const std::wstring&, const std::wstring&>::t_bind<io::t_file>::f_do(a_class, a_stack, a_n);
}

}
