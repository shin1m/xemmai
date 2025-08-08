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
#define CONTAINER__TYPES(_)\
	_(pair)\
	_(queue)
	CONTAINER__TYPES(XEMMAI__TYPE__DECLARE)

	using t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_container, t_global, f_global())
#define XEMMAI__TYPE__LIBRARY t_container
CONTAINER__TYPES(XEMMAI__TYPE__DEFINE)
#undef XEMMAI__TYPE__LIBRARY

#endif
