#ifndef XEMMAI__THROWABLE_H
#define XEMMAI__THROWABLE_H

#include "object.h"

namespace xemmai
{

struct t_backtrace
{
	XEMMAI__LOCAL static void f_push(t_object* a_value, t_object* a_lambda, void** a_pc);

	t_backtrace* v_next;
	t_slot v_lambda;
	void** v_pc;

	t_backtrace(t_backtrace* a_next, t_object* a_lambda, void** a_pc) : v_next(a_next), v_lambda(a_lambda), v_pc(a_pc)
	{
	}
	XEMMAI__LOCAL void f_dump() const;
};

class t_throwable
{
	friend struct t_backtrace;
	friend struct t_finalizes<t_bears<t_throwable>>;
	friend struct t_type_of<t_throwable>;

	std::atomic<t_backtrace*> v_backtrace;
	std::wstring v_message;

protected:
	XEMMAI__PUBLIC virtual ~t_throwable();

public:
	XEMMAI__PUBLIC static t_object* f_instantiate(std::wstring_view a_message);

	t_throwable(std::wstring_view a_message) : v_message(a_message)
	{
	}
	const std::wstring& f_string() const
	{
		return v_message;
	}
	XEMMAI__PUBLIC virtual void f_dump() const;
};

template<>
struct t_type_of<t_throwable> : t_derivable<t_holds<t_throwable>>
{
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		for (auto p = a_this->f_as<t_throwable>().v_backtrace.load(std::memory_order_acquire); p; p = p->v_next) a_scan(p->v_lambda);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
