#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_type> : t_underivable<t_bears<t_type>>
{
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_base(a_ids, a_super, {})
	{
		auto p = t_object::f_of(this);
		t_value::f_increments()->f_push(p);
		t_value::f_increments()->f_push(p);
		p->v_type = t_object::f_of(v_super)->v_type = this;
		v_fixed = true;
		v_get_nonowned = static_cast<void (t_type::*)(t_object*, t_object*, t_stacked*)>(&t_type_of::f_do_get_nonowned);
		v_get = static_cast<t_scoped (t_type::*)(t_object*, t_object*)>(&t_type_of::f_do_get);
		f_put = f_do_put;
		f_remove = f_do_remove;
		f_call_nonowned = f_do_call_nonowned;
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	void f_do_instantiate(t_stacked* a_stack, size_t a_n);
	void f_do_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack);
	t_scoped f_do_get(t_object* a_this, t_object* a_key);
	static void f_do_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	static t_scoped f_do_remove(t_object* a_this, t_object* a_key);
	static void f_do_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n);
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	static size_t f_do_send(t_object* a_this, t_stacked* a_stack);
};

typedef t_type_of<t_type> t_class;

inline t_scoped t_type::f_get_of_type(t_object* a_key)
{
	auto p = t_object::f_of(this);
	return static_cast<t_class*>(p->v_type)->f_do_get(p, a_key);
}

}

#endif
