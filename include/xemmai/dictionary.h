#ifndef XEMMAI__DICTIONARY_H
#define XEMMAI__DICTIONARY_H

#include "object.h"

namespace xemmai
{

class t_dictionary
{
	friend struct t_type_of<t_dictionary>;
	struct t_hash_traits
	{
		static size_t f_hash(const t_value& a_key);
		static bool f_equals(const t_value& a_x, const t_value& a_y);
	};

	t_hash v_hash;
	size_t v_size;

	t_dictionary() : v_size(0)
	{
	}
	~t_dictionary()
	{
		v_hash.f_finalize();
	}

public:
	static t_transfer f_instantiate();

	void f_clear()
	{
		v_hash.f_clear();
		v_size = 0;
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_value& f_get(const t_value& a_key) const;
	t_transfer f_put(const t_value& a_key, const t_transfer& a_value)
	{
		t_value p = a_value;
		if (v_hash.f_put<t_hash_traits>(a_key, a_value).first) ++v_size;
		return p;
	}
	bool f_has(const t_value& a_key) const
	{
		return v_hash.f_find<t_hash_traits>(a_key) != 0;
	}
	t_transfer f_remove(const t_value& a_key);
};

template<>
struct t_type_of<t_dictionary> : t_type
{
	static std::wstring f_string(const t_value& a_self);
	static int f_hash(const t_value& a_self);
	static bool f_equals(const t_value& a_self, const t_value& a_other);
	static bool f_not_equals(const t_value& a_self, const t_value& a_other)
	{
		return !f_equals(a_self, a_other);
	}
	static void f_each(const t_value& a_self, const t_value& a_callable);
	static void f_define();

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_slot* a_stack);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
	virtual void f_set_at(t_object* a_this, t_slot* a_stack);
	virtual void f_equals(t_object* a_this, t_slot* a_stack);
	virtual void f_not_equals(t_object* a_this, t_slot* a_stack);
};

}

#endif
