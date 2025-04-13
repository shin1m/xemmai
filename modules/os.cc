#include <xemmai/portable/path.h>
#include <xemmai/convert.h>
#include <xemmai/io.h>
#include <filesystem>
#ifdef __unix__
#include <spawn.h>
#include <wait.h>
#endif

namespace xemmai
{

struct t_os;

template<>
struct t_type_of<portable::t_path> : t_derivable<t_holds<portable::t_path>>
{
	using t_library = t_os;

	static t_object* f_transfer(const t_os* a_library, auto&& a_value)
	{
		return xemmai::f_new<portable::t_path>(a_library, std::forward<decltype(a_value)>(a_value));
	}
	static portable::t_path f__divide(const portable::t_path& a_self, std::wstring_view a_value)
	{
		return a_self / a_value;
	}
	static void f_define(t_os* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_divide(t_object* a_this, t_pvalue* a_stack);
};

template<>
struct t_type_of<std::filesystem::file_type> : t_enum_of<std::filesystem::file_type, t_os>
{
	static t_object* f_define(t_library* a_library)
	{
		return t_base::f_define(a_library, [](auto a_fields)
		{
			a_fields
			(L"NONE"sv, std::filesystem::file_type::none)
			(L"NOT_FOUND"sv, std::filesystem::file_type::not_found)
			(L"REGULAR"sv, std::filesystem::file_type::regular)
			(L"DIRECTORY"sv, std::filesystem::file_type::directory)
			(L"SYMLINK"sv, std::filesystem::file_type::symlink)
			(L"BLOCK"sv, std::filesystem::file_type::block)
			(L"CHARACTER"sv, std::filesystem::file_type::character)
			(L"FIFO"sv, std::filesystem::file_type::fifo)
			(L"SOCKET"sv, std::filesystem::file_type::socket)
			(L"UNKNOWN"sv, std::filesystem::file_type::unknown)
			;
		});
	}

	using t_base::t_base;
};

template<>
struct t_type_of<std::filesystem::perms> : t_enum_of<std::filesystem::perms, t_os>
{
	static t_object* f_define(t_library* a_library)
	{
		return t_base::f_define(a_library, [](auto a_fields)
		{
			a_fields
			(L"NONE"sv, std::filesystem::perms::none)
			(L"OWNER_READ"sv, std::filesystem::perms::owner_read)
			(L"OWNER_WRITE"sv, std::filesystem::perms::owner_write)
			(L"OWNER_EXEC"sv, std::filesystem::perms::owner_exec)
			(L"OWNER_ALL"sv, std::filesystem::perms::owner_all)
			(L"GROUP_READ"sv, std::filesystem::perms::group_read)
			(L"GROUP_WRITE"sv, std::filesystem::perms::group_write)
			(L"GROUP_EXEC"sv, std::filesystem::perms::group_exec)
			(L"GROUP_ALL"sv, std::filesystem::perms::group_all)
			(L"OTHERS_READ"sv, std::filesystem::perms::others_read)
			(L"OTHERS_WRITE"sv, std::filesystem::perms::others_write)
			(L"OTHERS_EXEC"sv, std::filesystem::perms::others_exec)
			(L"OTHERS_ALL"sv, std::filesystem::perms::others_all)
			(L"ALL"sv, std::filesystem::perms::all)
			(L"SET_UID"sv, std::filesystem::perms::set_uid)
			(L"SET_GID"sv, std::filesystem::perms::set_gid)
			(L"STICKY_BIT"sv, std::filesystem::perms::sticky_bit)
			(L"MASK"sv, std::filesystem::perms::mask)
			;
		});
	}

	using t_base::t_base;
};

struct t_directory : t_owned
{
	std::filesystem::directory_iterator v_i;

	t_directory(std::wstring_view a_path) : v_i(portable::f_convert(a_path))
	{
	}
	void f_close()
	{
		f_owned_or_throw();
		v_i = {};
	}
	t_pvalue f_read(t_os* a_library);
};

template<>
struct t_type_of<t_directory> : t_derivable<t_holds<t_directory>>
{
	using t_library = t_os;

	static void f_define(t_os* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

#ifdef __unix__
struct t_child : t_sharable
{
	pid_t v_pid;
	t_slot v_pipes;
	int v_status;

	void f_wait(int a_options)
	{
		if (!v_pid) return;
		while (true) {
			auto pid = waitpid(v_pid, &v_status, a_options);
			if (pid == v_pid) break;
			if (pid != -1) return;
			if (errno != EINTR) portable::f_throw_system_error();
		}
		v_pid = 0;
	}

	t_child(pid_t a_pid, t_object* a_pipes) : v_pid(a_pid), v_pipes(a_pipes)
	{
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_pipes);
	}
	t_object* f_pipe(size_t a_index) const
	{
		return v_pipes->f_as<t_tuple>().f_get_at(a_index);
	}
	void f_close()
	{
		auto& pipes = v_pipes->f_as<t_tuple>();
		for (size_t i = 0; i < pipes.f_size(); ++i) if (auto p = static_cast<t_object*>(pipes[i])) p->f_as<io::t_file>().f_close();
	}
	void f_wait()
	{
		f_owned_or_shared<std::lock_guard>([&]
		{
			f_wait(0);
		});
	}
	t_pvalue f_exited()
	{
		return f_owned_or_shared<std::lock_guard>([&]
		{
			f_wait(WNOHANG);
			return v_pid == 0 && WIFEXITED(v_status) ? t_pvalue(WEXITSTATUS(v_status)) : nullptr;
		});
	}
	t_pvalue f_signaled()
	{
		return f_owned_or_shared<std::lock_guard>([&]
		{
			f_wait(WNOHANG);
			return v_pid == 0 && WIFSIGNALED(v_status) ? t_pvalue(WTERMSIG(v_status)) : nullptr;
		});
	}
};

namespace
{

template<typename T>
struct t_defer
{
	T v_do;

	t_defer(T&& a_do) : v_do(std::move(a_do))
	{
	}
	~t_defer()
	{
		v_do();
	}
};

}

template<>
struct t_type_of<t_child> : t_derivable<t_holds<t_child>>
{
	using t_library = t_os;

	static t_pvalue f_construct(t_type* a_class, std::wstring_view a_file, const t_pvalue& a_arguments, const t_pvalue& a_environments, const t_pvalue& a_ios)
	{
		auto size = [](auto& a_xs)
		{
			static size_t index;
			auto n = a_xs.f_invoke(f_global()->f_symbol_size(), index);
			f_check<size_t>(n, L"size");
			return f_as<size_t>(n);
		};
		auto convert = [&](auto& a_xs0, auto& a_xs1)
		{
			auto n = size(a_xs0);
			for (size_t i = 0; i < n; ++i) {
				auto x = a_xs0.f_get_at(i).f_string();
				f_check<t_string>(x, L"value");
				a_xs1.emplace_back(portable::f_convert(x->template f_as<t_string>()));
			}
		};
		std::vector<std::string> arguments0{{portable::f_convert(a_file)}};
		convert(a_arguments, arguments0);
		std::vector<std::string> environments0;
		convert(a_environments, environments0);
		std::vector<const char*> arguments1;
		for (auto& x : arguments0) arguments1.push_back(x.c_str());
		arguments1.push_back(nullptr);
		std::vector<const char*> environments1;
		for (auto& x : environments0) environments1.push_back(x.c_str());
		environments1.push_back(nullptr);
		auto check = [](int a_error)
		{
			if (a_error) throw std::system_error(a_error, std::generic_category());
		};
		posix_spawn_file_actions_t actions;
		check(posix_spawn_file_actions_init(&actions));
		t_defer defer0([&]
		{
			check(posix_spawn_file_actions_destroy(&actions));
		});
		std::vector<int> closes[2];
		t_defer defer1([&]
		{
			for (auto xs : closes) for (auto x : xs) while (close(x) == -1) if (errno != EINTR) portable::f_throw_system_error();
		});
		t_object* pipes;
		{
			auto n = size(a_ios);
			pipes = t_tuple::f_instantiate(n, [&](auto& t)
			{
				std::uninitialized_default_construct_n(&t[0], n);
			});
			for (size_t i = 0; i < n; ++i) {
				auto x = a_ios.f_get_at(i);
				if (f_is<io::t_file>(x)) {
					check(posix_spawn_file_actions_adddup2(&actions, x->f_as<io::t_file>().f_fd(), i));
				} else if (f_is<int>(x)) {
					int fds[2];
					if (pipe(fds) != 0) portable::f_throw_system_error();
					auto [parent, child] = f_as<int>(x) > 0 ? std::make_tuple(fds[0], fds[1]) : std::make_tuple(fds[1], fds[0]);
					closes[0].push_back(parent);
					closes[1].push_back(child);
					check(posix_spawn_file_actions_adddup2(&actions, child, i));
					pipes->f_as<t_tuple>()[i] = io::t_file::f_instantiate(parent, true);
				} else if (i >= 3) {
					f_throw(L"must specify."sv);
				}
			}
			check(posix_spawn_file_actions_addclosefrom_np(&actions, std::max(static_cast<int>(n), 3)));
		}
		pid_t pid;
		check(posix_spawnp(&pid, arguments1[0], &actions, NULL, const_cast<char**>(arguments1.data()), const_cast<char**>(environments1.data())));
		closes[0].clear();
		return a_class->f_new<t_child>(pid, pipes);
	}
	static void f_define(t_os* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_child>().f_scan(a_scan);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};
#endif

struct t_os : t_library
{
	t_slot_of<t_type> v_type_path;
	t_slot_of<t_type> v_type_file_type;
	t_slot_of<t_type> v_type_permissions;
	t_slot_of<t_type> v_type_directory_entry;
	t_slot_of<t_type> v_type_directory;
#ifdef __unix__
	t_slot_of<t_type> v_type_child;
#endif

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_os, t_global, f_global())
XEMMAI__LIBRARY__TYPE_AS(t_os, portable::t_path, path)
XEMMAI__LIBRARY__TYPE_AS(t_os, std::filesystem::file_type, file_type)
XEMMAI__LIBRARY__TYPE_AS(t_os, std::filesystem::perms, permissions)
XEMMAI__LIBRARY__TYPE(t_os, directory)
#ifdef __unix__
XEMMAI__LIBRARY__TYPE(t_os, child)
#endif

void t_type_of<portable::t_path>::f_define(t_os* a_library)
{
	auto global = f_global();
	t_define{a_library}
		(global->f_symbol_string(), t_member<const std::wstring&(portable::t_path::*)() const, &portable::t_path::operator const std::wstring&>())
		(global->f_symbol_divide(), t_member<portable::t_path(*)(const portable::t_path&, std::wstring_view), f__divide>())
	.f_derive<portable::t_path, t_object>();
}

t_pvalue t_type_of<portable::t_path>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*, std::wstring_view), [](auto a_class, auto a_value)
	{
		return a_class->template f_new<portable::t_path>(a_value);
	}>::f_do(this, a_stack, a_n);
}

size_t t_type_of<portable::t_path>::f_do_divide(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_type()->v_module->f_as<t_os>().f_as(a_this->f_as<portable::t_path>() / f_as<std::wstring_view>(a_stack[2]));
	return -1;
}

t_pvalue t_directory::f_read(t_os* a_library)
{
	f_owned_or_throw();
	if (v_i == decltype(v_i){}) return nullptr;
	auto status = v_i->status();
	auto p = f_new_value(a_library->v_type_directory_entry, t_string::f_instantiate(v_i->path().filename().wstring()), a_library->f_as(status.type()), a_library->f_as(status.permissions()));
	++v_i;
	return p;
}

void t_type_of<t_directory>::f_define(t_os* a_library)
{
	t_define{a_library}
		(L"close"sv, t_member<void(t_directory::*)(), &t_directory::f_close>())
		(L"read"sv, t_member<t_pvalue(t_directory::*)(t_os*), &t_directory::f_read>())
	.f_derive<t_directory, t_object>();
}

t_pvalue t_type_of<t_directory>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<std::wstring_view>::t_bind<t_directory>::f_do(this, a_stack, a_n);
}

#ifdef __unix__
void t_type_of<t_child>::f_define(t_os* a_library)
{
	t_define{a_library}
		(L"pipe"sv, t_member<t_object*(t_child::*)(size_t) const, &t_child::f_pipe>())
		(L"close"sv, t_member<void(t_child::*)(), &t_child::f_close>())
		(L"wait"sv, t_member<void(t_child::*)(), &t_child::f_wait>())
		(L"exited"sv, t_member<t_pvalue(t_child::*)(), &t_child::f_exited>())
		(L"signaled"sv, t_member<t_pvalue(t_child::*)(), &t_child::f_signaled>())
	.f_derive<t_child, t_sharable>();
}

t_pvalue t_type_of<t_child>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, std::wstring_view, const t_pvalue&, const t_pvalue&, const t_pvalue&), f_construct>::t_bind<t_child>::f_do(this, a_stack, a_n);
}
#endif

namespace
{

int f_system(std::wstring_view a_command)
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
	while (nanosleep(&nano, &nano) == -1) if (errno != EINTR) portable::f_throw_system_error();
#endif
#ifdef _WIN32
	Sleep(a_miliseconds);
#endif
}

#ifdef __unix__
t_object* f_pipe()
{
	int fds[2];
	if (pipe(fds) != 0) portable::f_throw_system_error();
	return f_tuple(fds[0], fds[1]);
}
#endif

}

void t_os::f_scan(t_scan a_scan)
{
	a_scan(v_type_path);
	a_scan(v_type_file_type);
	a_scan(v_type_permissions);
	a_scan(v_type_directory_entry);
	a_scan(v_type_directory);
#ifdef __unix__
	a_scan(v_type_child);
#endif
}

std::vector<std::pair<t_root, t_rvalue>> t_os::f_define()
{
	t_type_of<portable::t_path>::f_define(this);
	v_type_directory_entry.f_construct(f_global()->f_type<t_object>()->f_derive({{
		t_symbol::f_instantiate(L"name"sv),
		t_symbol::f_instantiate(L"type"sv),
		t_symbol::f_instantiate(L"permissions"sv)
	}}));
	t_type_of<t_directory>::f_define(this);
#ifdef __unix__
	t_type_of<t_child>::f_define(this);
#endif
	return t_define(this)
		(L"Path"sv, static_cast<t_object*>(v_type_path))
		(L"FileType"sv, t_type_of<std::filesystem::file_type>::f_define(this))
		(L"Permissions"sv, t_type_of<std::filesystem::perms>::f_define(this))
		(L"DirectoryEntry"sv, static_cast<t_object*>(v_type_directory_entry))
		(L"Directory"sv, static_cast<t_object*>(v_type_directory))
#ifdef __unix__
		(L"Child"sv, static_cast<t_object*>(v_type_child))
#endif
		(L"system"sv, t_static<int(*)(std::wstring_view), f_system>())
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
