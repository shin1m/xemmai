#ifndef XEMMAI__THROWABLE_H
#define XEMMAI__THROWABLE_H

#include "fiber.h"

namespace xemmai
{

class t_throwable
{
	friend struct t_backtrace;
	friend struct t_finalizes<t_throwable, t_bears<t_throwable>>;
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
struct t_type_of<t_throwable> : t_derivable<t_holds<t_throwable>>
{
	static void f_define();

	using t_base::t_base;
	XEMMAI__PORTABLE__EXPORT virtual void f_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
