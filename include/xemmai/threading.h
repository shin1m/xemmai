#ifndef XEMMAI__THREADING_H
#define XEMMAI__THREADING_H

#include "global.h"

namespace xemmai
{

class t_threading;

template<>
struct t_type_of<portable::t_mutex> : t_type
{
	typedef t_threading t_extension;

	static void f_acquire(portable::t_mutex& a_self);
	static void f_release(portable::t_mutex& a_self);
	static void f_define(t_threading* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

template<>
struct t_type_of<portable::t_condition> : t_type
{
	typedef t_threading t_extension;

	static void f_wait(portable::t_condition& a_self, portable::t_mutex& a_mutex);
	static void f_wait(portable::t_condition& a_self, portable::t_mutex& a_mutex, size_t a_milliseconds);
	static void f_signal(portable::t_condition& a_self);
	static void f_broadcast(portable::t_condition& a_self);
	static void f_define(t_threading* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

class t_threading : public t_extension
{
	template<typename T, typename T_super> friend class t_define;

	t_slot v_type_mutex;
	t_slot v_type_condition;

	template<typename T>
	void f_type__(const t_transfer& a_type);

public:
	t_threading(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

template<>
inline void t_threading::f_type__<portable::t_mutex>(const t_transfer& a_type)
{
	v_type_mutex = a_type;
}

template<>
inline void t_threading::f_type__<portable::t_condition>(const t_transfer& a_type)
{
	v_type_condition = a_type;
}

template<>
inline t_object* t_threading::f_type<portable::t_mutex>() const
{
	return v_type_mutex;
}

template<>
inline t_object* t_threading::f_type<portable::t_condition>() const
{
	return v_type_condition;
}

}

#endif
