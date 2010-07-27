#include <xemmai/convert.h>

#include "callback.h"

using namespace xemmai;

struct t_callback_extension;

namespace xemmai
{

template<>
struct t_type_of<t_client> : t_type
{
	typedef t_callback_extension t_extension;

	static void f_define(t_callback_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

template<>
struct t_type_of<t_server> : t_type
{
	typedef t_callback_extension t_extension;

	static void f_define(t_callback_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

struct t_callback_extension : t_extension
{
	t_slot v_symbol_on_message;
	t_slot v_type_client;
	t_slot v_type_server;

	template<typename T>
	void f_type__(const t_transfer& a_type);

	t_callback_extension(t_object* a_module) : t_extension(a_module)
	{
		v_symbol_on_message = t_symbol::f_instantiate(L"on_message");
		t_type_of<t_client>::f_define(this);
		t_type_of<t_server>::f_define(this);
	}
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(T a_value) const
	{
		return f_global()->f_as(a_value);
	}
	t_transfer f_as(t_client* a_value) const;
};

template<>
inline void t_callback_extension::f_type__<t_client>(const t_transfer& a_type)
{
	v_type_client = a_type;
}

template<>
inline void t_callback_extension::f_type__<t_server>(const t_transfer& a_type)
{
	v_type_server = a_type;
}

template<>
inline t_object* t_callback_extension::f_type<t_client>() const
{
	return v_type_client.f_object();
}

template<>
inline t_object* t_callback_extension::f_type<t_server>() const
{
	return v_type_server.f_object();
}

class t_client_wrapper : public t_client
{
	friend struct t_callback_extension;

	t_object* v_self;

public:
	static t_transfer f_construct(t_object* a_class)
	{
		t_transfer object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_client_wrapper(object.f_object()));
		return object;
	}
	static void f_super__on_message(t_client* a_self, const std::wstring& a_message)
	{
		if (dynamic_cast<t_client_wrapper*>(a_self))
			a_self->t_client::f_on_message(a_message);
		else
			a_self->f_on_message(a_message);
	}

	t_client_wrapper(t_object* a_self) : v_self(a_self)
	{
	}
	virtual void f_on_message(const std::wstring& a_message);
};

void t_client_wrapper::f_on_message(const std::wstring& a_message)
{
	t_callback_extension* extension = f_extension<t_callback_extension>(f_as<t_type&>(v_self->f_type()).v_module.f_object());
	v_self->f_get(extension->v_symbol_on_message.f_object())(extension->f_as(a_message));
}

namespace xemmai
{

void t_type_of<t_client>::f_define(t_callback_extension* a_extension)
{
	t_define<t_client, t_object>(a_extension, L"Client")
		(a_extension->v_symbol_on_message.f_object(), t_member<void (*)(t_client*, const std::wstring&), t_client_wrapper::f_super__on_message>())
		(L"remove", t_member<void (t_client::*)(), &t_client::f_remove>())
	;
}

t_type* t_type_of<t_client>::f_derive(t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

void t_type_of<t_client>::f_finalize(t_object* a_this)
{
	delete dynamic_cast<t_client_wrapper*>(&f_as<t_client&>(a_this));
}

void t_type_of<t_client>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct_with<t_transfer (*)(t_object*), t_client_wrapper::f_construct>::f_call(a_class, a_n, a_stack);
}

void t_type_of<t_server>::f_define(t_callback_extension* a_extension)
{
	t_define<t_server, t_object>(a_extension, L"Server")
		(L"add", t_member<void (t_server::*)(t_client&), &t_server::f_add>())
		(L"post", t_member<void (t_server::*)(const std::wstring&), &t_server::f_post>())
		(L"run", t_member<void (t_server::*)(), &t_server::f_run>())
	;
}

t_type* t_type_of<t_server>::f_derive(t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

void t_type_of<t_server>::f_finalize(t_object* a_this)
{
	delete &f_as<t_server&>(a_this);
}

void t_type_of<t_server>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_construct<t_server>::f_call(a_class, a_n, a_stack);
}

}

void t_callback_extension::f_scan(t_scan a_scan)
{
	a_scan(v_symbol_on_message);
	a_scan(v_type_client);
	a_scan(v_type_server);
}

t_transfer t_callback_extension::f_as(t_client* a_value) const
{
	t_client_wrapper* p = dynamic_cast<t_client_wrapper*>(a_value);
	if (p) return p->v_self;
	t_transfer object = t_object::f_allocate(v_type_client.f_object());
	object.f_pointer__(a_value);
	return object;
}

#ifdef _MSC_VER
extern "C" __declspec(dllexport) t_extension* f_factory(t_object* a_module)
#else
extern "C" t_extension* f_factory(t_object* a_module)
#endif
{
	return new t_callback_extension(a_module);
}
