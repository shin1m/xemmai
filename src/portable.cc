#include <xemmai/portable/path.h>
#include <xemmai/portable/convert.h>

#include <cstdlib>
#include <vector>
#ifdef __unix__
#include <unistd.h>
#endif

namespace xemmai
{

namespace portable
{

#ifdef __unix__
t_path::t_path(const std::wstring& a_path)
{
	if (a_path[0] == v_directory_separator) {
		v_path = L'/';
	} else {
		char* mbs = getcwd(NULL, 0);
		size_t n = std::mbstowcs(NULL, mbs, 0) + 1;
		std::vector<wchar_t> wcs(n);
		std::mbstowcs(&wcs[0], mbs, n);
		std::free(mbs);
		v_path = &wcs[0];
	}
	*this /= a_path;
}
#endif

#ifdef _WIN32
t_path::t_path(const std::wstring& a_path)
{
	DWORD n = GetFullPathNameW(a_path.c_str(), 0, NULL, NULL);
	std::vector<wchar_t> cs(n);
	wchar_t* p;
	GetFullPathNameW(a_path.c_str(), n, &cs[0], &p);
	v_path = &cs[0];
}
#endif

t_path& t_path::operator/=(const std::wstring& a_path)
{
	std::wstring::const_iterator i = a_path.begin();
	while (i != a_path.end()) {
		std::vector<wchar_t> cs;
		do {
			wchar_t c = *i;
			++i;
			if (c == v_directory_separator) break;
#ifdef __unix__
			cs.push_back(c);
#endif
#ifdef _WIN32
			if (c != L'"') cs.push_back(c);
#endif
		} while (i != a_path.end());
		if (cs.empty() || cs.size() == 1 && cs[0] == L'.') continue;
		if (cs.size() == 2 && cs[0] == L'.' && cs[1] == L'.') {
			std::wstring::size_type n = v_path.find_last_of(v_directory_separator);
			if (n > 0) v_path.erase(n);
		} else {
			if (*--v_path.end() != v_directory_separator) v_path += v_directory_separator;
			v_path.append(cs.begin(), cs.end());
		}
	}
	return *this;
}

std::string f_convert(const std::wstring& a_string)
{
	size_t n = std::wcstombs(NULL, a_string.c_str(), 0) + 1;
	std::vector<char> cs(n);
	std::wcstombs(&cs[0], a_string.c_str(), n);
	return &cs[0];
}

std::wstring f_convert(const std::string& a_string)
{
	size_t n = std::mbstowcs(NULL, a_string.c_str(), 0) + 1;
	std::vector<wchar_t> cs(n);
	std::mbstowcs(&cs[0], a_string.c_str(), n);
	return &cs[0];
}

}

}
