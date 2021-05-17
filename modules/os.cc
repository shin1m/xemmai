#include <xemmai/convert.h>
#ifdef __unix__
#include <unistd.h>
#endif

namespace xemmai
{

struct t_os : t_library
{
	using t_library::t_library;
	virtual void f_scan(t_scan a_scan)
	{
	}
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
};

namespace
{

int f_system(const t_string& a_command)
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
	while (nanosleep(&nano, &nano) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
#endif
#ifdef _WIN32
	Sleep(a_miliseconds);
#endif
}

#ifdef __unix__
t_object* f_pipe()
{
	int fds[2];
	if (pipe(fds) != 0) f_throw(L"pipe failed."sv);
	return f_tuple(fds[0], fds[1]);
}
#endif

}

std::vector<std::pair<t_root, t_rvalue>> t_os::f_define()
{
	return t_export(this)
		(L"system"sv, t_static<int(*)(const t_string&), f_system>())
		(L"sleep"sv, t_static<void(*)(intptr_t), f_sleep>())
#ifdef __unix__
		(L"pipe"sv, t_static<t_object*(*)(), f_pipe>())
#endif
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmai::t_os>(a_handle);
}
