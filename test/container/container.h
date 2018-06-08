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
	static t_scoped f_instantiate(t_container* a_extension, t_scoped&& a_value);
	static t_type* f_define(t_container* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_queue> : t_type
{
	typedef t_container t_extension;

	static void f_define(t_container* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

struct t_container : t_extension
{
	t_slot_of<t_type> v_type_pair;
	t_slot_of<t_type> v_type_queue;

	template<typename T>
	void f_type__(t_type* a_type);

	t_container(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_type* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
};

template<>
inline void t_container::f_type__<t_pair>(t_type* a_type)
{
	v_type_pair = a_type->v_this;
}

template<>
inline void t_container::f_type__<t_queue>(t_type* a_type)
{
	v_type_queue = a_type->v_this;
}

template<>
inline t_type* t_container::f_type<t_pair>() const
{
	return v_type_pair;
}

template<>
inline t_type* t_container::f_type<t_queue>() const
{
	return v_type_queue;
}

#endif
