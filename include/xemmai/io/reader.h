#ifndef XEMMAI__IO__READER_H
#define XEMMAI__IO__READER_H

#include "../sharable.h"
#include "../string.h"

namespace xemmai
{

class t_io;

namespace io
{

class t_reader : public t_sharable, portable::t_iconv
{
	friend struct t_type_of<t_reader>;

	t_slot v_read;
	t_slot v_buffer;
	char* v_p0;
	size_t v_n0 = 0;
	intptr_t v_result;
	wchar_t v_c;
	char* v_p1 = reinterpret_cast<char*>(&v_c);
	size_t v_n1 = sizeof(v_c);

	XEMMAI__LOCAL intptr_t f_read();
	XEMMAI__LOCAL wint_t f_get();

public:
	static t_object* f_instantiate(const t_pvalue& a_read, std::wstring_view a_encoding);

	t_reader(const t_pvalue& a_read, std::wstring_view a_encoding);
	t_object* f_read(size_t a_size);
	t_object* f_read_line();
};

}

template<>
struct t_type_of<io::t_reader> : t_derivable<t_holds<io::t_reader>>
{
	using t_library = t_io;

	XEMMAI__LOCAL static void f_define(t_io* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		auto& p = a_this->f_as<io::t_reader>();
		a_scan(p.v_read);
		a_scan(p.v_buffer);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
