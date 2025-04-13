#include "callback.h"
#include <xemmai/convert.h>

using namespace xemmai;

struct t_callback_library;

namespace xemmai
{

template<>
struct t_type_of<t_client> : t_derivable<t_bears<t_client>>
{
	using t_library = t_callback_library;

	template<typename T>
	struct t_cast : t_type::t_cast<T>
	{
		static T f_as(auto&& a_object)
		{
			return *static_cast<t_object*>(a_object)->f_as<t_client*>();
		}
	};
	template<typename T>
	struct t_cast<T*> : t_type::t_cast<T*>
	{
		static T* f_as(auto&& a_object)
		{
			auto p = static_cast<t_object*>(a_object);
			return p ? p->f_as<t_client*>() : nullptr;
		}
	};

	static t_pvalue f_transfer(t_library* a_library, t_client* a_value);
	static void f_define(t_callback_library* a_library);

	t_type_of(auto&&... a_xs) : t_base(std::forward<decltype(a_xs)>(a_xs)...)
	{
		f_finalize = f_do_finalize;
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	static void f_do_finalize(t_object* a_this, t_scan a_scan);
};

template<>
struct t_type_of<t_server> : t_derivable<t_holds<t_server>>
{
	using t_library = t_callback_library;

	static void f_define(t_callback_library* a_library);
	static void f_post(t_server& a_self, std::wstring_view a_message)
	{
		a_self.f_post(a_message);
	}

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

struct t_callback_library : t_library
{
	t_slot v_symbol_on_message;
	t_slot_of<t_type> v_type_client;
	t_slot_of<t_type> v_type_server;

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_callback_library, t_global, f_global())
XEMMAI__LIBRARY__TYPE(t_callback_library, client)
XEMMAI__LIBRARY__TYPE(t_callback_library, server)

class t_client_wrapper : public t_client
{
	friend struct t_type_of<t_client>;

	t_object* v_self;

public:
	static void f_super__on_message(t_client* a_self, std::wstring_view a_message)
	{
		if (dynamic_cast<t_client_wrapper*>(a_self))
			a_self->t_client::f_on_message(a_message);
		else
			a_self->f_on_message(a_message);
	}

	virtual void f_on_message(std::wstring_view a_message)
	{
		auto& library = v_self->f_type()->v_module->f_as<t_callback_library>();
		static size_t index;
		v_self->f_invoke(library.v_symbol_on_message, index, library.f_as(a_message));
	}
};

namespace xemmai
{

t_pvalue t_type_of<t_client>::f_transfer(t_library* a_library, t_client* a_value)
{
	auto p = dynamic_cast<t_client_wrapper*>(a_value);
	return p ? p->v_self : a_library->f_type<t_client>()->f_new<t_client*>(a_value);
}

void t_type_of<t_client>::f_define(t_callback_library* a_library)
{
	t_define{a_library}
		(a_library->v_symbol_on_message, t_member<void(*)(t_client*, std::wstring_view), t_client_wrapper::f_super__on_message>())
		(L"remove"sv, t_member<void(t_client::*)(), &t_client::f_remove>())
	.f_derive<t_client, t_object>();
}

t_pvalue t_type_of<t_client>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*), [](auto a_class)
	{
		auto object = a_class->template f_new<t_client*>(new t_client_wrapper);
		object->template f_as<t_client_wrapper*>()->v_self = object;
		return object;
	}>::f_do(this, a_stack, a_n);
}

void t_type_of<t_client>::f_do_finalize(t_object* a_this, t_scan a_scan)
{
	delete dynamic_cast<t_client_wrapper*>(a_this->f_as<t_client*>());
}

void t_type_of<t_server>::f_define(t_callback_library* a_library)
{
	t_define{a_library}
		(L"add"sv, t_member<void(t_server::*)(t_client&), &t_server::f_add>())
		(L"post"sv, t_member<void(*)(t_server&, std::wstring_view), f_post>())
		(L"run"sv, t_member<void(t_server::*)(), &t_server::f_run>())
	.f_derive<t_server, t_object>();
}

t_pvalue t_type_of<t_server>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<t_server>::f_do(this, a_stack, a_n);
}

}

void t_callback_library::f_scan(t_scan a_scan)
{
	a_scan(v_symbol_on_message);
	a_scan(v_type_client);
	a_scan(v_type_server);
}

std::vector<std::pair<t_root, t_rvalue>> t_callback_library::f_define()
{
	v_symbol_on_message = t_symbol::f_instantiate(L"on_message"sv);
	t_type_of<t_client>::f_define(this);
	t_type_of<t_server>::f_define(this);
	return t_define(this)
		(L"Client"sv, static_cast<t_object*>(v_type_client))
		(L"Server"sv, static_cast<t_object*>(v_type_server))
	;
}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<t_callback_library>(a_handle);
}
