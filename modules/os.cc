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
	t_scoped f_as(const T& a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

namespace
{

int f_system(const std::wstring& a_command)
{
	t_safe_region region;
	return std::system(portable::f_convert(a_command).c_str());
}

void f_sleep(intptr_t a_miliseconds)
{
	t_safe_region region;
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
	f_define<void (*)(intptr_t), f_sleep>(this, L"sleep");
}

void t_os::f_scan(t_scan a_scan)
{
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmai::t_os(a_module);
}
