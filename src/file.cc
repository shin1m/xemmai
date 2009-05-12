#include <xemmai/file.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_transfer t_file::f_instantiate(const std::wstring& a_path, const std::wstring& a_mode)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_file>());
	object->v_pointer = new t_file(a_path, a_mode);
	return object;
}

t_file::t_file(const std::wstring& a_path, const char* a_mode) : v_stream(std::fopen(portable::f_convert(a_path).c_str(), a_mode))
{
}

t_file::t_file(const std::wstring& a_path, const std::wstring& a_mode) : v_stream(std::fopen(portable::f_convert(a_path).c_str(), portable::f_convert(a_mode).c_str()))
{
	if (v_stream == NULL) t_throwable::f_throw(L"failed to open.");
}

void t_file::f_close()
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	std::fclose(v_stream);
	v_stream = NULL;
}

void t_file::f_seek(int a_offset, int a_whence)
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	if (std::fseek(v_stream, a_offset, a_whence) == -1) t_throwable::f_throw(L"failed to seek.");
}

int t_file::f_tell()
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	int n = std::ftell(v_stream);
	if (n == -1) t_throwable::f_throw(L"failed to tell.");
	return n;
}

size_t t_file::f_read(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	if (v_stream == NULL) t_throwable::f_throw(L"already closed.");
	if (a_offset + a_size > a_bytes.f_size()) t_throwable::f_throw(L"out of range.");
	size_t n = std::fread(&a_bytes[0] + a_offset, 1, a_size, v_stream);
	if (std::ferror(v_stream)) t_throwable::f_throw(L"failed to read.");
	return n;
}

void t_file::f_write(t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
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

void t_type_of<t_file>::f_close(t_object* a_self)
{
	f_check<t_file>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_file&>(a_self).f_close();
}

void t_type_of<t_file>::f_seek(t_object* a_self, int a_offset, int a_whence)
{
	f_check<t_file>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_file&>(a_self).f_seek(a_offset, a_whence);
}

int t_type_of<t_file>::f_tell(t_object* a_self)
{
	f_check<t_file>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_file&>(a_self).f_tell();
}

size_t t_type_of<t_file>::f_read(t_object* a_self, t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	f_check<t_file>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_file&>(a_self).f_read(a_bytes, a_offset, a_size);
}

void t_type_of<t_file>::f_write(t_object* a_self, t_bytes& a_bytes, size_t a_offset, size_t a_size)
{
	f_check<t_file>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_file&>(a_self).f_write(a_bytes, a_offset, a_size);
}

void t_type_of<t_file>::f_flush(t_object* a_self)
{
	f_check<t_file>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_file&>(a_self).f_flush();
}

t_transfer t_type_of<t_file>::f_define()
{
	return t_define<t_file, t_object>(f_global(), L"File")
		(L"close", t_member<void (*)(t_object*), f_close>())
		(L"seek", t_member<void (*)(t_object*, int, int), f_seek>())
		(L"tell", t_member<int (*)(t_object*), f_tell>())
		(L"read", t_member<size_t (*)(t_object*, t_bytes&, size_t, size_t), f_read>())
		(L"write", t_member<void (*)(t_object*, t_bytes&, size_t, size_t), f_write>())
		(L"flush", t_member<void (*)(t_object*), f_flush>())
	;
}

t_type* t_type_of<t_file>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_file>::f_finalize(t_object* a_this)
{
	delete f_as<t_file*>(a_this);
}

void t_type_of<t_file>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct<t_file, const std::wstring&, const std::wstring&>::f_call(a_class, a_n, a_stack);
}

}
