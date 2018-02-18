#ifndef XEMMAI__PORTABLE__CONVERT_H
#define XEMMAI__PORTABLE__CONVERT_H

#include <string>

#include "define.h"

namespace xemmai::portable
{

XEMMAI__PORTABLE__EXPORT std::string f_convert(const std::wstring& a_string);
XEMMAI__PORTABLE__EXPORT std::wstring f_convert(const std::string& a_string);

}

#endif
