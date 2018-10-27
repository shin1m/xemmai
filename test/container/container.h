#ifndef CONTAINER_H
#define CONTAINER_H

#include <xemmai/convert.h>

#include "queue.h"

using namespace xemmai;

struct t_container;

namespace xemmai
{

template<>
struct t_type_of<t_pair> : t_uninstantiatable<t_underivable<t_holds<t_pair>>>
{
	static t_scoped f_instantiate(t_container* a_extension, t_scoped&& a_value);
	static t_scoped f_define(t_container* a_extension);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
};

template<>
struct t_type_of<t_queue> : t_derivable<t_holds<t_queue>>
{
	typedef t_container t_extension;

	static void f_define(t_container* a_extension);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

struct t_container : t_extension
{
	t_slot_of<t_type> v_type_pair;
	t_slot_of<t_type> v_type_queue;

	t_container(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_container*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		return f_global()->f_as(std::forward<T>(a_value));
	}
};

template<>
inline t_slot_of<t_type>& t_container::f_type_slot<t_pair>()
{
	return v_type_pair;
}

template<>
inline t_slot_of<t_type>& t_container::f_type_slot<t_queue>()
{
	return v_type_queue;
}

#endif
