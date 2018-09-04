#include <xemmai/convert.h>

#include "callback.h"

using namespace xemmai;

struct t_callback_extension;

namespace xemmai
{

template<>
struct t_type_of<t_client> : t_derivable<t_holds<t_client>>
{
	typedef t_callback_extension t_extension;

	static void f_define(t_callback_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_server> : t_derivable<t_holds<t_server>>
{
	typedef t_callback_extension t_extension;

	static void f_define(t_callback_extension* a_extension);
	static void f_post(t_server& a_self, std::wstring a_message)
	{
		a_self.f_post(a_message);
	}

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

struct t_callback_extension : t_extension
{
	t_slot v_symbol_on_message;
	t_slot_of<t_type> v_type_client;
	t_slot_of<t_type> v_type_server;

	t_callback_extension(t_object* a_module) : t_extension(a_module)
	{
		v_symbol_on_message = t_symbol::f_instantiate(L"on_message");
		t_type_of<t_client>::f_define(this);
		t_type_of<t_server>::f_define(this);
	}
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_symbol_on_message);
		a_scan(v_type_client);
		a_scan(v_type_server);
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_callback_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
	t_scoped f_as(t_client* a_value) const;
};

template<>
inline t_slot_of<t_type>& t_callback_extension::f_type_slot<t_client>()
{
	return v_type_client;
}

template<>
inline t_slot_of<t_type>& t_callback_extension::f_type_slot<t_server>()
{
	return v_type_server;
}

class t_client_wrapper : public t_client
{
	friend struct t_callback_extension;

	t_object* v_self;

public:
	static t_scoped f_construct(t_type* a_class)
	{
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_client_wrapper(object));
		return object;
	}
	static void f_super__on_message(t_client* a_self, std::wstring a_message)
	{
		if (dynamic_cast<t_client_wrapper*>(a_self))
			a_self->t_client::f_on_message(a_message);
		else
			a_self->f_on_message(a_message);
	}

	t_client_wrapper(t_object* a_self) : v_self(a_self)
	{
	}
	virtual void f_on_message(const std::wstring& a_message)
	{
		auto extension = f_extension<t_callback_extension>(v_self->f_type()->v_module);
		v_self->f_invoke(extension->v_symbol_on_message, extension->f_as(a_message));
	}
};

namespace xemmai
{

void t_type_of<t_client>::f_define(t_callback_extension* a_extension)
{
	t_define<t_client, t_object>(a_extension, L"Client")
		(t_construct_with<t_scoped(*)(t_type*), t_client_wrapper::f_construct>())
		(a_extension->v_symbol_on_message, t_member<void(*)(t_client*, std::wstring), t_client_wrapper::f_super__on_message>())
		(L"remove", t_member<void(t_client::*)(), &t_client::f_remove>())
	;
}

t_scoped t_type_of<t_client>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), t_client_wrapper::f_construct>::t_bind<t_client>::f_do(this, a_stack, a_n);
}

void t_type_of<t_server>::f_define(t_callback_extension* a_extension)
{
	t_define<t_server, t_object>(a_extension, L"Server")
		(t_construct<>())
		(L"add", t_member<void(t_server::*)(t_client&), &t_server::f_add>())
		(L"post", t_member<void(*)(t_server&, std::wstring), f_post>())
		(L"run", t_member<void(t_server::*)(), &t_server::f_run>())
	;
}

t_scoped t_type_of<t_server>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<t_server>::f_do(this, a_stack, a_n);
}

}

t_scoped t_callback_extension::f_as(t_client* a_value) const
{
	auto p = dynamic_cast<t_client_wrapper*>(a_value);
	if (p) return p->v_self;
	t_scoped object = t_object::f_allocate(v_type_client);
	object.f_pointer__(a_value);
	return object;
}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new t_callback_extension(a_module);
}
