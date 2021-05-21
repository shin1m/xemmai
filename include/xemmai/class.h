#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_type> : t_uninstantiatable<t_bears<t_type>>
{
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_base(a_ids, a_super, nullptr, sizeof(t_type), 0, std::vector<std::pair<t_root, t_rvalue>>{}, std::map<t_object*, size_t>{})
	{
		f_finalize = f_do_scan;
		v_get = static_cast<t_pvalue (t_type::*)(t_object*, t_object*, size_t&)>(&t_type_of::f_do_get);
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	XEMMAI__PORTABLE__EXPORT t_pvalue f_do_get(t_object* a_this, t_object* a_key, size_t& a_index);
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
	static size_t f_do_add(t_object* a_this, t_pvalue* a_stack);
};

using t_class = t_type_of<t_type>;

struct t_builder : t_owned
{
	template<typename T>
	static void f_do(t_fields& a_fields, T a_do);

	t_fields* v_fields;
};

template<>
struct t_type_of<t_builder> : t_uninstantiatable<t_finalizes<t_derives<t_builder>>>
{
	template<typename... T_an>
	t_type_of(T_an&&... a_an) : t_base(std::forward<T_an>(a_an)...)
	{
		v_get = static_cast<t_pvalue (t_type::*)(t_object*, t_object*, size_t&)>(&t_type_of::f_do_get);
		v_put = f_do_put;
	}
	t_pvalue f_do_get(t_object* a_this, t_object* a_key, size_t& a_index);
	static void f_do_put(t_object* a_this, t_object* a_key, const t_pvalue& a_value);
};

}

#endif
