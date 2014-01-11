#ifndef XEMMAI__NATIVE_H
#define XEMMAI__NATIVE_H

#include "object.h"

namespace xemmai
{

class t_native
{
	friend struct t_type_of<t_native>;

public:
	typedef void (*t_function)(t_object*, t_slot*, size_t);

private:
	t_slot v_module;
	t_function v_function;

	t_native(t_scoped&& a_module, t_function a_function) : v_module(std::move(a_module)), v_function(a_function)
	{
	}
	~t_native() = default;

public:
	XEMMAI__PORTABLE__EXPORT static t_scoped f_instantiate(t_scoped&& a_module, t_function a_function);
};

template<>
struct t_type_of<t_native> : t_type
{
	t_type_of(t_scoped&& a_module, t_scoped&& a_super) : t_type(std::move(a_module), std::move(a_super))
	{
		v_shared = true;
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_call(t_object* a_this, t_slot* a_stack, size_t a_n);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
};

}

#endif
