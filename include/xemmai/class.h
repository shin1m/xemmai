#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_type> : t_bears<t_type>
{
	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_base(a_ids, a_super)
	{
		t_value::f_increments()->f_push(v_this);
		t_value::f_increments()->f_push(v_this);
		static_cast<t_object*>(v_this)->v_type = static_cast<t_object*>(v_super->v_this)->v_type = this;
		v_fixed = v_shared = true;
		v_derive = &t_type::f_dont_derive;
		f_scan = f_do_scan;
		f_finalize = f_do_finalize;
		v_instantiate = static_cast<void (t_type::*)(t_stacked*, size_t)>(&t_type_of::f_do_instantiate);
		v_get_nonowned = static_cast<void (t_type::*)(t_object*, t_object*, t_stacked*)>(&t_type_of::f_do_get_nonowned);
		v_get = static_cast<t_scoped (t_type::*)(t_object*, t_object*)>(&t_type_of::f_do_get);
		f_put = f_do_put;
		f_remove = f_do_remove;
		f_call_nonowned = f_do_call_nonowned;
		f_call = f_do_call;
		f_send = f_do_send;
	}
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	static void f_do_finalize(t_object* a_this)
	{
		f_as<t_type&>(a_this).f_delete();
	}
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
	return static_cast<t_class*>(static_cast<t_object*>(v_this)->v_type)->f_do_get(v_this, a_key);
}

}

#endif
