#include <xemmai/convert.h>
#include <xemmai/tuple.h>
#ifdef __unix__
#include <unistd.h>
#endif

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
	t_scoped f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
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

#ifdef __unix__
t_scoped f_pipe()
{
	int fds[2];
	if (pipe(fds) != 0) f_throw(L"pipe failed.");
	return f_tuple(fds[0], fds[1]);
}
#endif

}

t_os::t_os(t_object* a_module) : t_extension(a_module)
{
	f_define<int(*)(const std::wstring&), f_system>(this, L"system");
	f_define<void(*)(intptr_t), f_sleep>(this, L"sleep");
#ifdef __unix__
	f_define<t_scoped(*)(), f_pipe>(this, L"pipe");
#endif
}

void t_os::f_scan(t_scan a_scan)
{
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmai::t_os(a_module);
}
