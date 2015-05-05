#ifndef XEMMAI__PORTABLE__PATH_H
#define XEMMAI__PORTABLE__PATH_H

#include <string>

namespace xemmai
{

namespace portable
{

class t_path
{
#ifdef __unix__
	static const wchar_t v_directory_separator = L'/';
#endif
#ifdef _WIN32
	static const wchar_t v_directory_separator = L'\\';
#endif

	std::wstring v_path;

public:
	t_path(const std::wstring& a_path);
	t_path(const t_path& a_path) = default;
	t_path& operator=(const t_path& a_path) = default;
	t_path& operator/=(const std::wstring& a_path);
	t_path operator/(const std::wstring& a_path) const
	{
		return t_path(*this) /= a_path;
	}
	operator const std::wstring&() const
	{
		return v_path;
	}
};

}

}

#endif
