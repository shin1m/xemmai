#include <cstdlib>
#include <xemmai/convert.h>

namespace xemmai
{

struct t_os : t_extension
{
	t_os(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

namespace
{

int f_system(const std::wstring& a_command)
{
	return std::system(portable::f_convert(a_command).c_str());
}

void f_sleep(int a_miliseconds)
{
#ifdef __unix__
	struct timespec nano;
	nano.tv_sec = a_miliseconds / 1000;
	nano.tv_nsec = a_miliseconds % 1000 * 1000000;
	nanosleep(&nano, NULL);
#endif
#ifdef _WIN32
	Sleep(a_miliseconds);
#endif
}

}

t_os::t_os(t_object* a_module) : t_extension(a_module)
{
	f_define<int (*)(const std::wstring&), f_system>(this, L"system");
	f_define<void (*)(int), f_sleep>(this, L"sleep");
}

void t_os::f_scan(t_scan a_scan)
{
}

}

#ifdef _MSC_VER
extern "C" __declspec(dllexport) xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#else
extern "C" xemmai::t_extension* f_factory(xemmai::t_object* a_module)
#endif
{
	return new xemmai::t_os(a_module);
}
