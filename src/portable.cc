#include <xemmai/portable/path.h>
#include <xemmai/portable/convert.h>
#include <vector>
#ifdef __unix__
#include <unistd.h>
#endif

namespace xemmai::portable
{

#ifdef __unix__
t_path::t_path(std::wstring_view a_path)
{
	if (a_path[0] == v_directory_separator) {
		v_path = L'/';
	} else {
		const char* mbs = getcwd(NULL, 0);
		std::mbstate_t state{};
		size_t n = std::mbsrtowcs(nullptr, &mbs, 0, &state) + 1;
		std::vector<wchar_t> wcs(n);
		std::mbsrtowcs(wcs.data(), &mbs, n, &state);
		std::free(const_cast<char*>(mbs));
		v_path = wcs.data();
	}
	*this /= a_path;
}
#endif

#ifdef _WIN32
t_path::t_path(std::wstring_view a_path)
{
	DWORD n = GetFullPathNameW(std::wstring(a_path).c_str(), 0, NULL, NULL);
	std::vector<wchar_t> cs(n);
	wchar_t* p;
	GetFullPathNameW(a_path.c_str(), n, &cs[0], &p);
	v_path = &cs[0];
}
#endif

t_path& t_path::operator/=(std::wstring_view a_path)
{
	auto i = a_path.begin();
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
			size_t n = v_path.find_last_of(v_directory_separator);
			if (n > 0) v_path.erase(n);
		} else {
			if (*--v_path.end() != v_directory_separator) v_path += v_directory_separator;
			v_path.append(cs.begin(), cs.end());
		}
	}
	return *this;
}

std::string f_convert(std::wstring_view a_string)
{
	std::wstring s(a_string);
	auto p = s.c_str();
	std::mbstate_t state{};
	size_t n = std::wcsrtombs(nullptr, &p, 0, &state) + 1;
	std::vector<char> cs(n);
	std::wcsrtombs(cs.data(), &p, n, &state);
	return cs.data();
}

std::wstring f_convert(std::string_view a_string)
{
	std::string s(a_string);
	auto p = s.c_str();
	std::mbstate_t state{};
	size_t n = std::mbsrtowcs(nullptr, &p, 0, &state) + 1;
	std::vector<wchar_t> cs(n);
	std::mbsrtowcs(cs.data(), &p, n, &state);
	return cs.data();
}

}
