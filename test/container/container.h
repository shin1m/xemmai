#ifndef CONTAINER_H
#define CONTAINER_H

#include <xemmai/convert.h>

#include "queue.h"

using namespace xemmai;

struct t_container;

namespace xemmai
{

template<>
struct t_type_of<t_pair> : t_type
{
	static t_transfer f_instantiate(t_container* a_extension, const t_transfer& a_value);
	static t_transfer f_define(t_container* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, size_t a_n);
};

template<>
struct t_type_of<t_queue> : t_type
{
	typedef t_container t_extension;

	static t_transfer f_define(t_container* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, size_t a_n, t_stack& a_stack);
};

}

struct t_container : t_extension
{
	t_slot v_type_pair;
	t_slot v_type_queue;

	t_container(t_object* a_module);
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
};

template<>
inline t_object* t_container::f_type<t_pair>() const
{
	return v_type_pair;
}

template<>
inline t_object* t_container::f_type<t_queue>() const
{
	return v_type_queue;
}

#endif
