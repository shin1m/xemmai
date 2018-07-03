#ifndef XEMMAI__THROWABLE_H
#define XEMMAI__THROWABLE_H

#include "fiber.h"

namespace xemmai
{

class t_throwable
{
	friend struct t_backtrace;
	friend struct t_type_of<t_throwable>;

	t_backtrace* v_backtrace = nullptr;
	std::wstring v_message;

protected:
	XEMMAI__PORTABLE__EXPORT virtual ~t_throwable();

public:
	XEMMAI__PORTABLE__EXPORT static t_scoped f_instantiate(const std::wstring& a_message);
	XEMMAI__PORTABLE__EXPORT static void f_throw [[noreturn]] (const std::wstring& a_message);
	static void f_throw [[noreturn]] (t_stacked* a_stack, size_t a_n, const std::wstring& a_message)
	{
		t_destruct_n(a_stack, a_n);
		f_throw(a_message);
	}

	t_throwable(const std::wstring& a_message) : v_message(a_message)
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
	static constexpr auto V_ids = f_ids<t_throwable, t_object>();

	static void f_define();

	using t_type::t_type;
	XEMMAI__PORTABLE__EXPORT virtual t_type* f_derive();
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual void f_finalize(t_object* a_this);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
