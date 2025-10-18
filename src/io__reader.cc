#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace io
{

intptr_t t_reader::f_read()
{
	auto& buffer = v_buffer->f_as<t_bytes>();
	auto p = reinterpret_cast<char*>(&buffer[0]);
	std::copy(v_p0, v_p0 + v_n0, p);
	v_p0 = p;
	auto n = t_pvalue(v_read)(t_pvalue(v_buffer), v_n0, buffer.f_size() - v_n0);
	f_check<intptr_t>(n, L"result of read");
	v_result = f_as<intptr_t>(n);
	if (v_result > 0) v_n0 += v_result;
	return v_result;
}

wint_t t_reader::f_get()
{
	if (v_n0 <= 0 && f_read() <= 0) return WEOF;
	while (true) {
		if (iconv(v_cd, &v_p0, &v_n0, &v_p1, &v_n1) == size_t(-1)) {
			switch (errno) {
			case EINVAL:
				if (f_read() < 0) return WEOF;
				if (v_result == 0) f_throw(L"incomplete sequence."sv);
			case EINTR:
				continue;
			case E2BIG:
				break;
			default:
				portable::f_throw_system_error();
			}
			break;
		}
		if (v_n1 <= 0) break;
		if (f_read() <= 0) return WEOF;
	}
	v_p1 = reinterpret_cast<char*>(&v_c);
	v_n1 = sizeof(v_c);
	return v_c;
}

t_object* t_reader::f_instantiate(const t_pvalue& a_read, std::wstring_view a_encoding)
{
	return f_new<t_reader>(&f_engine()->f_module_io()->f_as<t_module>().v_body->f_as<t_io>(), a_read, a_encoding);
}

t_reader::t_reader(const t_pvalue& a_read, std::wstring_view a_encoding) : t_iconv("wchar_t", portable::f_convert(a_encoding).c_str())
{
	v_read = a_read;
	v_buffer = t_bytes::f_instantiate(t_object::f_size_to_capacity<t_bytes, unsigned char>(sizeof(t_object) << 3));
}

t_object* t_reader::f_read(size_t a_size)
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		if (a_size <= 0) return f_global()->f_string_empty();
		wint_t c = f_get();
		if (c == WEOF) return v_result < 0 ? nullptr : f_global()->f_string_empty();
		return t_string::f_instantiate(a_size, [&](auto p)
		{
			for (auto q = p + a_size;;) {
				*p = c;
				if (++p >= q) break;
				c = f_get();
				if (c == WEOF) break;
			}
			return p;
		});
	});
}

t_object* t_reader::f_read_line()
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		wint_t c = f_get();
		if (c == WEOF) return v_result < 0 ? nullptr : f_global()->f_string_empty();
		t_stringer s;
		do {
			s << c;
			if (c == L'\n') break;
			c = f_get();
		} while (c != WEOF);
		return static_cast<t_object*>(s);
	});
}

}

void t_type_of<io::t_reader>::f_define(t_io* a_library)
{
	t_define{a_library}
	(L"read"sv, t_member<t_object*(io::t_reader::*)(size_t), &io::t_reader::f_read>())
	(L"read_line"sv, t_member<t_object*(io::t_reader::*)(), &io::t_reader::f_read_line>())
	.f_derive<io::t_reader, t_sharable>();
}

t_pvalue t_type_of<io::t_reader>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<const t_pvalue&, const t_string&>::t_bind<io::t_reader>::f_do(this, a_stack, a_n);
}

}
