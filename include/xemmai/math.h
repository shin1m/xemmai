#ifndef XEMMAI__MATH_H
#define XEMMAI__MATH_H

#include "global.h"

namespace xemmai
{

class t_math : public t_extension
{
public:
	t_math(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(T a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

}

#endif
