#ifndef XEMMAI__PORTABLE__CONVERT_H
#define XEMMAI__PORTABLE__CONVERT_H

#include "define.h"
#include <string>

namespace xemmai::portable
{

XEMMAI__PUBLIC std::string f_convert(std::wstring_view a_string);
XEMMAI__PUBLIC std::wstring f_convert(std::string_view a_string);

}

#endif
