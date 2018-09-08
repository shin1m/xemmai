#ifndef XEMMAI__THROWABLE_H
#define XEMMAI__THROWABLE_H

#include "fiber.h"
#include "string.h"

namespace xemmai
{

struct t_backtrace
{
	static void f_push(const t_value& a_throwable, const t_scoped& a_lambda, void** a_pc);

	t_backtrace* v_next;
	t_slot v_lambda;

	t_backtrace(t_backtrace* a_next, const t_scoped& a_lambda, void** a_pc) : v_next(a_next), v_lambda(a_lambda)
	{
		*reinterpret_cast<void***>(&v_lambda.v_integer) = a_pc;
	}
	void** const& f_pc() const
	{
		return *reinterpret_cast<void** const*>(&v_lambda.v_integer);
	}
	void f_dump() const;
};

class t_throwable
{
	friend struct t_backtrace;
	friend struct t_finalizes<t_bears<t_throwable>>;
	friend struct t_type_of<t_throwable>;

	t_backtrace* v_backtrace = nullptr;
	std::wstring v_message;

protected:
	XEMMAI__PORTABLE__EXPORT virtual ~t_throwable();

public:
	XEMMAI__PORTABLE__EXPORT static t_scoped f_instantiate(std::wstring_view a_message);

	t_throwable(std::wstring_view a_message) : v_message(a_message)
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
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
