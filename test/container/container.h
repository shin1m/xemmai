#ifndef CONTAINER_H
#define CONTAINER_H

#include "queue.h"
#include <xemmai/convert.h>

using namespace xemmai;

struct t_container;

namespace xemmai
{

template<>
struct t_type_of<t_pair> : t_uninstantiatable<t_holds<t_pair>>
{
	static t_object* f_instantiate(t_container* a_library, const t_pvalue& a_value);
	static t_object* f_define(t_container* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_pair>().f_scan(a_scan);
	}
};

template<>
struct t_type_of<t_queue> : t_derivable<t_holds<t_queue>>
{
	using t_library = t_container;

	static void f_define(t_container* a_library);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_queue>().f_scan(a_scan);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

struct t_container : t_library
{
	t_slot_of<t_type> v_type_pair;
	t_slot_of<t_type> v_type_queue;

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_container, t_global, f_global())
XEMMAI__LIBRARY__TYPE(t_container, pair)
XEMMAI__LIBRARY__TYPE(t_container, queue)

#endif
