#ifndef XEMMAI__SHARABLE_H
#define XEMMAI__SHARABLE_H

#include "object.h"
#include "atomic.h"

namespace xemmai
{

struct t_owned
{
	decltype(t_slot::t_increments::v_instance) v_owner = t_slot::t_increments::v_instance;

	bool f_owned() const
	{
		return v_owner == t_slot::t_increments::v_instance;
	}
	template<typename T>
	auto f_owned_or_throw(T a_do) -> decltype(a_do())
	{
		if (!f_owned()) f_throw(L"not owned."sv);
		return a_do();
	}
};

struct t_sharable : t_owned
{
	t_lock v_lock;

	bool f_shared() const
	{
		return !v_owner;
	}
	void f_own();
	void f_share();
	template<typename T_lock, typename T>
	auto f_owned_or_shared(T a_do) -> decltype(a_do())
	{
		if (f_owned()) return a_do();
		if (!f_shared()) f_throw(L"owned by another thread."sv);
		T_lock lock(v_lock);
		return a_do();
	}
};

template<>
struct t_type_of<t_sharable> : t_bears<t_sharable>
{
	static void f_define();

	using t_base::t_base;
};

}

#endif
