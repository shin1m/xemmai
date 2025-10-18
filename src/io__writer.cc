#include <xemmai/io.h>
#include <xemmai/convert.h>

namespace xemmai
{

namespace io
{

bool t_writer::f_write()
{
	auto& buffer = v_buffer->f_as<t_bytes>();
	auto p = reinterpret_cast<char*>(&buffer[0]);
	if (!v_remain) v_remain = p;
	auto n = t_pvalue(v_write)(t_pvalue(v_buffer), v_remain - p, v_p1 - v_remain);
	f_check<size_t>(n, L"result of write");
	v_remain += f_as<size_t>(n);
	if (v_remain < v_p1) return false;
	v_p1 = p;
	v_n1 = buffer.f_size();
	v_remain = nullptr;
	return true;
}

bool t_writer::f_write(char** a_p, size_t* a_n)
{
	while (iconv(v_cd, a_p, a_n, &v_p1, &v_n1) == size_t(-1)) {
		switch (errno) {
		case E2BIG:
			if (!f_write()) return false;
		case EINTR:
			break;
		default:
			portable::f_throw_system_error();
		}
	}
	return true;
}

bool t_writer::f__write(const t_pvalue& a_value)
{
	if (v_remain) f_throw(L"resume required."sv);
	auto x = a_value;
	if (!f_is<t_string>(x)) {
		x = x.f_string();
		f_check<t_string>(x, L"value");
	}
	auto& s = x->f_as<t_string>();
	v_p0 = const_cast<char*>(reinterpret_cast<const char*>(static_cast<const wchar_t*>(s)));
	v_n0 = s.f_size() * sizeof(wchar_t);
	if (f_write(&v_p0, &v_n0)) return true;
	v_value = x;
	return false;
}

bool t_writer::f__resume()
{
	if (v_value) {
		if (!f_write(&v_p0, &v_n0)) return false;
		v_value = nullptr;
	}
	switch (v_step) {
	case 4:
		v_p0 = const_cast<char*>(reinterpret_cast<const char*>(L"\n"));
		v_n0 = sizeof(wchar_t);
		--v_step;
	case 3:
		if (!f_write(&v_p0, &v_n0)) return false;
		--v_step;
	case 2:
		if (!f_write(NULL, NULL)) return false;
		--v_step;
	case 1:
		if (!f_write()) return false;
		--v_step;
	}
	return true;
}

t_object* t_writer::f_instantiate(const t_pvalue& a_write, std::wstring_view a_encoding)
{
	return f_new<t_writer>(&f_engine()->f_module_io()->f_as<t_module>().v_body->f_as<t_io>(), a_write, a_encoding);
}

t_writer::t_writer(const t_pvalue& a_write, std::wstring_view a_encoding) : t_iconv(portable::f_convert(a_encoding).c_str(), "wchar_t")
{
	v_write = a_write;
	v_buffer = t_bytes::f_instantiate(t_object::f_size_to_capacity<t_bytes, unsigned char>(sizeof(t_object) << 3));
	auto& buffer = v_buffer->f_as<t_bytes>();
	v_p1 = reinterpret_cast<char*>(&buffer[0]);
	v_n1 = buffer.f_size();
}

bool t_writer::f_write(const t_pvalue& a_value)
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		return f__write(a_value);
	});
}

bool t_writer::f_write_line(const t_pvalue& a_value)
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		auto b = f__write(a_value);
		v_step = 4;
		return b && f__resume();
	});
}

bool t_writer::f_flush()
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		if (v_remain) f_throw(L"resume required."sv);
		v_step = 2;
		return f__resume();
	});
}

bool t_writer::f_resume()
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		return f__resume();
	});
}

}

void t_type_of<io::t_writer>::f_define(t_io* a_library)
{
	t_define{a_library}
	(L"write"sv, t_member<bool(io::t_writer::*)(const t_pvalue&), &io::t_writer::f_write>())
	(L"write_line"sv, t_member<bool(io::t_writer::*)(const t_pvalue&), &io::t_writer::f_write_line>())
	(L"flush"sv, t_member<bool(io::t_writer::*)(), &io::t_writer::f_flush>())
	(L"resume"sv, t_member<bool(io::t_writer::*)(), &io::t_writer::f_resume>())
	.f_derive<io::t_writer, t_sharable>();
}

t_pvalue t_type_of<io::t_writer>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<const t_pvalue&, const t_string&>::t_bind<io::t_writer>::f_do(this, a_stack, a_n);
}

}
