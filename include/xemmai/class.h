#ifndef XEMMAI__CLASS_H
#define XEMMAI__CLASS_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<t_type> : t_type
{
	static constexpr auto V_ids = f_ids<t_type, t_object>();

	template<size_t A_n>
	t_type_of(const std::array<t_type_id, A_n>& a_ids, t_type* a_super) : t_type(a_ids, a_super)
	{
		t_value::f_increments()->f_push(v_this);
		t_value::f_increments()->f_push(v_this);
		static_cast<t_object*>(v_this)->v_type = static_cast<t_object*>(v_super->v_this)->v_type = this;
		v_fixed = v_shared = true;
	}
	virtual t_type* f_derive();
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_stacked* a_stack, size_t a_n);
	virtual void f_get_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack);
	virtual t_scoped f_get(t_object* a_this, t_object* a_key);
	virtual void f_put(t_object* a_this, t_object* a_key, t_scoped&& a_value);
	virtual t_scoped f_remove(t_object* a_this, t_object* a_key);
	virtual void f_call_nonowned(t_object* a_this, t_object* a_key, t_stacked* a_stack, size_t a_n);
	virtual size_t f_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
	virtual size_t f_send(t_object* a_this, t_stacked* a_stack);
};

typedef t_type_of<t_type> t_class;

}

#endif
