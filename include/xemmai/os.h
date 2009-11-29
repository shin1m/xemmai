#ifndef XEMMAI__OS_H
#define XEMMAI__OS_H

#include "module.h"
#include "global.h"

namespace xemmai
{

class t_os : public t_extension
{
public:
	t_os(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		return f_global()->f_as(a_value);
	}
};

}

#endif
