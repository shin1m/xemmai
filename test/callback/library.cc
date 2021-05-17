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

	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *f_object(std::forward<T1>(a_object))->template f_as<t_client*>();
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return p ? p->template f_as<t_client*>() : nullptr;
		}
	};

	static void f_define(t_callback_library* a_library);

	template<typename... T_an>
	t_type_of(T_an&&... a_an) : t_base(std::forward<T_an>(a_an)...)
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
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_symbol_on_message);
		a_scan(v_type_client);
		a_scan(v_type_server);
	}
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define()
	{
		v_symbol_on_message = t_symbol::f_instantiate(L"on_message"sv);
		t_type_of<t_client>::f_define(this);
		t_type_of<t_server>::f_define(this);
		return t_export(this)
			(L"Client"sv, t_object::f_of(v_type_client))
			(L"Server"sv, t_object::f_of(v_type_server))
		;
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_callback_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
	t_pvalue f_as(t_client* a_value) const;
};

template<>
inline t_slot_of<t_type>& t_callback_library::f_type_slot<t_client>()
{
	return v_type_client;
}

template<>
inline t_slot_of<t_type>& t_callback_library::f_type_slot<t_server>()
{
	return v_type_server;
}

class t_client_wrapper : public t_client
{
	friend struct t_callback_library;

	t_object* v_self;

public:
	static t_pvalue f_construct(t_type* a_class)
	{
		auto object = a_class->f_new<t_client*>(new t_client_wrapper());
		object->f_as<t_client_wrapper*>()->v_self = object;
		return object;
	}
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
		v_self->f_invoke(library.v_symbol_on_message, library.f_as(a_message));
	}
};

namespace xemmai
{

void t_type_of<t_client>::f_define(t_callback_library* a_library)
{
	t_define<t_client, t_object>{a_library}
		(a_library->v_symbol_on_message, t_member<void(*)(t_client*, std::wstring_view), t_client_wrapper::f_super__on_message>())
		(L"remove"sv, t_member<void(t_client::*)(), &t_client::f_remove>())
	.f_derive();
}

t_pvalue t_type_of<t_client>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), t_client_wrapper::f_construct>::t_bind<t_client>::f_do(this, a_stack, a_n);
}

void t_type_of<t_client>::f_do_finalize(t_object* a_this, t_scan a_scan)
{
	delete dynamic_cast<t_client_wrapper*>(a_this->f_as<t_client*>());
}

void t_type_of<t_server>::f_define(t_callback_library* a_library)
{
	t_define<t_server, t_object>{a_library}
		(L"add"sv, t_member<void(t_server::*)(t_client&), &t_server::f_add>())
		(L"post"sv, t_member<void(*)(t_server&, std::wstring_view), f_post>())
		(L"run"sv, t_member<void(t_server::*)(), &t_server::f_run>())
	.f_derive();
}

t_pvalue t_type_of<t_server>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<t_server>::f_do(this, a_stack, a_n);
}

}

t_pvalue t_callback_library::f_as(t_client* a_value) const
{
	auto p = dynamic_cast<t_client_wrapper*>(a_value);
	return p ? p->v_self : v_type_client->f_new<t_client*>(a_value);
}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<t_callback_library>(a_handle);
}
