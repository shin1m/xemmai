#ifndef XEMMAI__PORTABLE__CONVERT_H
#define XEMMAI__PORTABLE__CONVERT_H

#include "define.h"
#include <string>
#include <iconv.h>

namespace xemmai::portable
{

XEMMAI__PUBLIC std::string f_convert(std::wstring_view a_string);
XEMMAI__PUBLIC std::wstring f_convert(std::string_view a_string);

struct t_iconv
{
	iconv_t v_cd;

	t_iconv(const char* a_to, const char* a_from) : v_cd(iconv_open(a_to, a_from))
	{
		if (v_cd == iconv_t(-1)) f_throw_system_error();
	}
	~t_iconv()
	{
		iconv_close(v_cd);
	}
};

}

#endif
