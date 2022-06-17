#include <xemmai/portable/path.h>
#include <xemmai/portable/convert.h>
#include <system_error>
#include <climits>
#ifdef __unix__
#include <unistd.h>
#endif

namespace xemmai::portable
{

#ifdef __unix__
t_path::t_path(std::wstring_view a_path)
{
	if (!a_path.empty() && a_path[0] == v_directory_separator) {
		v_path = L'/';
	} else {
		const char* mbs = getcwd(NULL, 0);
		std::mbstate_t state{};
		size_t n = std::mbsrtowcs(nullptr, &mbs, 0, &state);
		v_path = std::wstring(n, L'\0');
		std::mbsrtowcs(v_path.data(), &mbs, n + 1, &state);
		std::free(const_cast<char*>(mbs));
	}
	*this /= a_path;
}
#endif

#ifdef _WIN32
t_path::t_path(std::wstring_view a_path)
{
	if (a_path.empty()) return;
	std::wstring path(a_path);
	DWORD n = GetFullPathNameW(path.c_str(), 0, NULL, NULL);
	v_path = std::wstring(n, L'\0');
	GetFullPathNameW(path.c_str(), n + 1, v_path.data(), NULL);
}
#endif

t_path& t_path::operator/=(std::wstring_view a_path)
{
	auto i = a_path.begin();
	while (i != a_path.end()) {
		std::wstring s;
		do {
			wchar_t c = *i++;
			if (c == v_directory_separator) break;
#ifdef __unix__
			s.push_back(c);
#endif
#ifdef _WIN32
			if (c != L'"') s.push_back(c);
#endif
		} while (i != a_path.end());
		if (s.empty() || s.size() == 1 && s[0] == L'.') continue;
		if (s.size() == 2 && s[0] == L'.' && s[1] == L'.') {
			size_t n = v_path.find_last_of(v_directory_separator);
			if (n > 0) v_path.erase(n);
		} else {
			if (v_path.back() != v_directory_separator) v_path.push_back(v_directory_separator);
			v_path += s;
		}
	}
	return *this;
}

std::string f_convert(std::wstring_view a_string)
{
	std::wstring s0(a_string);
	auto p = s0.c_str();
	std::mbstate_t state{};
	size_t n = std::wcsrtombs(nullptr, &p, 0, &state);
	std::string s1(n, '\0');
	std::wcsrtombs(s1.data(), &p, n + 1, &state);
	return s1;
}

std::wstring f_convert(std::string_view a_string)
{
	std::string s0(a_string);
	auto p = s0.c_str();
	std::mbstate_t state{};
	size_t n = std::mbsrtowcs(nullptr, &p, 0, &state);
	std::wstring s1(n, L'\0');
	std::mbsrtowcs(s1.data(), &p, n + 1, &state);
	return s1;
}

std::wstring f_executable_path()
{
#ifdef __unix__
	char cs[PATH_MAX];
	auto n = readlink("/proc/self/exe", cs, sizeof(cs));
	if (n == -1) throw std::system_error(errno, std::generic_category());
	return portable::f_convert({cs, static_cast<size_t>(n)});
#endif
#ifdef _WIN32
	wchar_t cs[MAX_PATH];
	auto n = GetModuleFileNameW(NULL, cs, MAX_PATH);
	if (n == 0) throw std::system_error(GetLastError(), std::system_category());
	return {cs, n};
#endif
}

}
