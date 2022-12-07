#include <xemmai/portable/path.h>
#include <xemmai/convert.h>
#include <filesystem>
#ifdef __unix__
#include <unistd.h>
#endif

namespace xemmai
{

struct t_os;

template<>
struct t_type_of<portable::t_path> : t_derivable<t_holds<portable::t_path>>
{
	using t_library = t_os;

	template<typename T>
	static t_object* f_transfer(const t_os* a_library, T&& a_value)
	{
		return xemmai::f_new<portable::t_path>(a_library, std::forward<T>(a_value));
	}
	static t_pvalue f_construct(t_type* a_class, const t_string& a_value)
	{
		return a_class->f_new<portable::t_path>(a_value);
	}
	static portable::t_path f__divide(const portable::t_path& a_self, const t_string& a_value)
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

struct t_os : t_library
{
	t_slot_of<t_type> v_type_path;
	t_slot_of<t_type> v_type_file_type;
	t_slot_of<t_type> v_type_permissions;
	t_slot_of<t_type> v_type_directory_entry;
	t_slot_of<t_type> v_type_directory;

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_os, t_global, f_global())
XEMMAI__LIBRARY__TYPE_AS(t_os, portable::t_path, path)
XEMMAI__LIBRARY__TYPE_AS(t_os, std::filesystem::file_type, file_type)
XEMMAI__LIBRARY__TYPE_AS(t_os, std::filesystem::perms, permissions)
XEMMAI__LIBRARY__TYPE(t_os, directory)

void t_type_of<portable::t_path>::f_define(t_os* a_library)
{
	t_define{a_library}
		(f_global()->f_symbol_string(), t_member<const std::wstring&(portable::t_path::*)() const, &portable::t_path::operator const std::wstring&>())
		(f_global()->f_symbol_divide(), t_member<portable::t_path(*)(const portable::t_path&, const t_string&), f__divide>())
	.f_derive<portable::t_path, t_object>();
}

t_pvalue t_type_of<portable::t_path>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, const t_string&), f_construct>::t_bind<portable::t_path>::f_do(this, a_stack, a_n);
}

size_t t_type_of<portable::t_path>::f_do_divide(t_object* a_this, t_pvalue* a_stack)
{
	f_check<t_string>(a_stack[2], L"argument0");
	a_stack[0] = a_this->f_type()->v_module->f_as<t_os>().f_as(a_this->f_as<portable::t_path>() / f_as<std::wstring>(a_stack[2]));
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

void t_os::f_scan(t_scan a_scan)
{
	a_scan(v_type_path);
	a_scan(v_type_file_type);
	a_scan(v_type_permissions);
	a_scan(v_type_directory_entry);
	a_scan(v_type_directory);
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
	return t_define(this)
		(L"Path"sv, static_cast<t_object*>(v_type_path))
		(L"FileType"sv, t_type_of<std::filesystem::file_type>::f_define(this))
		(L"Permissions"sv, t_type_of<std::filesystem::perms>::f_define(this))
		(L"DirectoryEntry"sv, static_cast<t_object*>(v_type_directory_entry))
		(L"Directory"sv, static_cast<t_object*>(v_type_directory))
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
