#include <xemmai/tuple.h>

#include <xemmai/derived.h>

namespace xemmai
{

t_transfer t_tuple::f_instantiate(size_t a_size)
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_tuple>());
	object->v_pointer = new(a_size) t_tuple();
	return object;
}

t_type* t_type_of<t_tuple>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_tuple>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_tuple* p = f_as<t_tuple*>(a_this);
	for (size_t i = 0; i < p->f_size(); ++i) a_scan((*p)[i]);
}

void t_type_of<t_tuple>::f_finalize(t_object* a_this)
{
	delete f_as<t_tuple*>(a_this);
}

}
