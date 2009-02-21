#include <xemmai/dictionary.h>

#include <xemmai/boolean.h>
#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

size_t t_dictionary::t_hash_traits::f_hash(t_object* a_key)
{
	return f_as<size_t>(a_key->f_get(f_global()->f_symbol_hash())->f_call());
}

size_t t_dictionary::t_hash_traits::f_equals(t_object* a_x, t_object* a_y)
{
	return f_as<bool>(a_x->f_get(f_global()->f_symbol_equals())->f_call(a_y));
}

t_transfer t_dictionary::f_instantiate()
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_dictionary>());
	object->v_pointer = new t_dictionary();
	return object;
}

t_object* t_dictionary::f_get(t_object* a_key) const
{
	t_hash::t_entry* field = v_hash.f_find<t_hash_traits>(a_key);
	if (!field) t_throwable::f_throw(L"key not found.");
	return field->v_value;
}

t_transfer t_dictionary::f_remove(t_object* a_key)
{
	t_transfer value = v_hash.f_remove<t_hash_traits>(a_key);
	if (!value) t_throwable::f_throw(L"key not found.");
	--v_size;
	return value;
}

std::wstring t_type_of<t_dictionary>::f_string(t_object* a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& dictionary = f_as<const t_dictionary&>(a_self);
	t_hash::t_iterator i(dictionary.v_hash);
	t_transfer x;
	t_transfer y;
	{
		portable::t_scoped_lock_for_read lock(a_self->v_lock);
		if (!i.f_entry()) return L"{}";
		x = i.f_entry()->f_key();
		y = i.f_entry()->v_value;
	}
	x = x->f_get(f_global()->f_symbol_string())->f_call();
	if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
	y = y->f_get(f_global()->f_symbol_string())->f_call();
	if (!f_is<std::wstring>(y)) t_throwable::f_throw(L"argument must be string.");
	std::wstring s = f_as<std::wstring>(x) + L": " + f_as<std::wstring>(y);
	while (true) {
		{
			portable::t_scoped_lock_for_read lock(a_self->v_lock);
			i.f_next();
			if (!i.f_entry()) break;
			x = i.f_entry()->f_key();
			y = i.f_entry()->v_value;
		}
		x = x->f_get(f_global()->f_symbol_string())->f_call();
		if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
		y = y->f_get(f_global()->f_symbol_string())->f_call();
		if (!f_is<std::wstring>(y)) t_throwable::f_throw(L"argument must be string.");
		s += L", " + f_as<std::wstring>(x) + L": " + f_as<std::wstring>(y);
	}
	return L'{' + s + L'}';
}

int t_type_of<t_dictionary>::f_hash(t_object* a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& dictionary = f_as<const t_dictionary&>(a_self);
	int n = 0;
	t_hash::t_iterator i(dictionary.v_hash);
	while (true) {
		t_transfer x;
		t_transfer y;
		{
			portable::t_scoped_lock_for_read lock(a_self->v_lock);
			i.f_next();
			if (!i.f_entry()) break;
			x = i.f_entry()->f_key();
			y = i.f_entry()->v_value;
		}
		x = x->f_get(f_global()->f_symbol_hash())->f_call();
		if (!f_is<int>(x)) t_throwable::f_throw(L"argument must be integer.");
		n ^= f_as<int>(x);
		y = y->f_get(f_global()->f_symbol_hash())->f_call();
		if (!f_is<int>(y)) t_throwable::f_throw(L"argument must be integer.");
		n ^= f_as<int>(y);
	}
	return n;
}

t_object* t_type_of<t_dictionary>::f_get_at(t_object* a_self, t_object* a_key)
{
	f_check<t_dictionary>(a_self, L"this");
	portable::t_scoped_lock_for_read lock(a_self->v_lock);
	return f_as<const t_dictionary&>(a_self).f_get(a_key);
}

t_object* t_type_of<t_dictionary>::f_set_at(t_object* a_self, t_object* a_key, const t_transfer& a_value)
{
	f_check<t_dictionary>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	t_object* p = a_value;
	f_as<t_dictionary&>(a_self).f_put(a_key, a_value);
	return p;
}

bool t_type_of<t_dictionary>::f_equals(t_object* a_self, t_object* a_other)
{
	if (a_self == a_other) return true;
	f_check<t_dictionary>(a_self, L"this");
	if (!f_is<t_dictionary>(a_other)) return false;
	const t_dictionary& d0 = f_as<const t_dictionary&>(a_self);
	const t_dictionary& d1 = f_as<const t_dictionary&>(a_other);
	{
		portable::t_scoped_lock_for_read lock0(a_self->v_lock);
		portable::t_scoped_lock_for_read lock1(a_other->v_lock);
		if (d0.f_size() != d1.f_size()) return false;
	}
	t_hash::t_iterator i(d0.v_hash);
	while (true) {
		t_transfer x;
		t_transfer y;
		{
			portable::t_scoped_lock_for_read lock0(a_self->v_lock);
			portable::t_scoped_lock_for_read lock1(a_other->v_lock);
			i.f_next();
			if (!i.f_entry()) break;
			t_hash::t_entry* field = d1.v_hash.f_find<t_dictionary::t_hash_traits>(i.f_entry()->f_key());
			if (!field) return false;
			x = i.f_entry()->v_value;
			y = field->v_value;
		}
		if (!f_as<bool>(x->f_get(f_global()->f_symbol_equals())->f_call(y))) return false;
	}
	return false;
}

void t_type_of<t_dictionary>::f_clear(t_object* a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	f_as<t_dictionary&>(a_self).f_clear();
}

size_t t_type_of<t_dictionary>::f_size(t_object* a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	portable::t_scoped_lock_for_read lock(a_self->v_lock);
	return f_as<t_dictionary&>(a_self).f_size();
}

t_transfer t_type_of<t_dictionary>::f_remove_at(t_object* a_self, t_object* a_key)
{
	f_check<t_dictionary>(a_self, L"this");
	portable::t_scoped_lock_for_write lock(a_self->v_lock);
	return f_as<t_dictionary&>(a_self).f_remove(a_key);
}

t_transfer t_type_of<t_dictionary>::f_define()
{
	return t_define<t_dictionary, t_object>(f_global(), L"Dictionary")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(t_object*), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(t_object*), f_hash>())
		(f_global()->f_symbol_get_at(), t_member<t_object* (*)(t_object*, t_object*), f_get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_object* (*)(t_object*, t_object*, const t_transfer&), f_set_at>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(t_object*, t_object*), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(t_object*, t_object*), f_not_equals>())
		(L"clear", t_member<void (*)(t_object*), f_clear>())
		(L"size", t_member<size_t (*)(t_object*), f_size>())
		(L"remove", t_member<t_transfer (*)(t_object*, t_object*), f_remove_at>())
	;
}

t_type* t_type_of<t_dictionary>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_dictionary>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_dictionary* p = f_as<t_dictionary*>(a_this);
	p->v_hash.f_scan(a_scan);
}

void t_type_of<t_dictionary>::f_finalize(t_object* a_this)
{
	delete f_as<t_dictionary*>(a_this);
}

void t_type_of<t_dictionary>::f_construct(t_object* a_class, size_t a_n, t_stack& a_stack)
{
	t_transfer p = t_dictionary::f_instantiate();
	t_dictionary& dictionary = f_as<t_dictionary&>(p);
	{
		size_t i = a_n;
		while (i > 0) {
			t_transfer x = a_stack.f_at(--i).f_transfer();
			if (i <= 0) {
				dictionary.f_put(x, f_global()->f_null());
				break;
			}
			t_transfer y = a_stack.f_at(--i).f_transfer();
			dictionary.f_put(x, y);
		}
	}
	while (a_n > 0) {
		--a_n;
		a_stack.f_pop();
	}
	a_stack.f_return(p);
}

void t_type_of<t_dictionary>::f_hash(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	a_stack.f_return(f_global()->f_as(f_hash(a_this)));
	context.f_done();
}

void t_type_of<t_dictionary>::f_get_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_get_at(a_this, a0));
	context.f_done();
}

void t_type_of<t_dictionary>::f_set_at(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a1 = a_stack.f_pop();
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_set_at(a_this, a0, a1));
	context.f_done();
}

void t_type_of<t_dictionary>::f_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_equals(a_this, a0)));
	context.f_done();
}

void t_type_of<t_dictionary>::f_not_equals(t_object* a_this, t_stack& a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack.f_pop();
	a_stack.f_return(f_global()->f_as(f_not_equals(a_this, a0)));
	context.f_done();
}

}
