#ifndef XEMMAI__THROWABLE_H
#define XEMMAI__THROWABLE_H

#include "fiber.h"

namespace xemmai
{

class t_throwable
{
	friend struct t_fiber;
	friend struct t_type_of<t_throwable>;

	t_fiber::t_context* v_context;
	std::wstring v_message;

protected:
	XEMMAI__PORTABLE__EXPORT virtual ~t_throwable();

public:
	XEMMAI__PORTABLE__EXPORT static t_transfer f_instantiate(const std::wstring& a_message);
	XEMMAI__PORTABLE__EXPORT static void f_throw(const std::wstring& a_message);

	t_throwable(const std::wstring& a_message) : v_context(0), v_message(a_message)
	{
	}
	const std::wstring& f_string() const
	{
		return v_message;
	}
	XEMMAI__PORTABLE__EXPORT virtual void f_dump() const;
};

template<>
struct t_type_of<t_throwable> : t_type
{
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	XEMMAI__PORTABLE__EXPORT virtual t_type* f_derive(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
