#ifndef XEMMAI__SHARABLE_H
#define XEMMAI__SHARABLE_H

#include "object.h"
#include <shared_mutex>

namespace xemmai
{

struct t_owned
{
	decltype(t_slot::t_increments::v_instance) v_owner = t_slot::t_increments::v_instance;

	bool f_owned() const
	{
		return v_owner == t_slot::t_increments::v_instance;
	}
	void f_owned_or_throw()
	{
		if (!f_owned()) f_throw(L"not owned."sv);
	}
};

struct t_sharable : t_owned
{
	std::shared_mutex v_mutex;

	bool f_shared() const
	{
		return !v_owner;
	}
	void f_own();
	void f_share();
	template<template<typename> typename T_lock>
	auto f_owned_or_shared(auto a_do)
	{
		if (f_owned()) return a_do();
		if (!f_shared()) f_throw(L"owned by another thread."sv);
		T_lock lock(v_mutex);
		if (!f_shared()) f_throw(L"owned by another thread."sv);
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
