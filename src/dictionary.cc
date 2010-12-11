#include <xemmai/dictionary.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

size_t t_dictionary::t_hash_traits::f_hash(const t_value& a_key)
{
	return f_as<size_t>(a_key.f_hash());
}

bool t_dictionary::t_hash_traits::f_equals(const t_value& a_x, const t_value& a_y)
{
	return f_as<bool>(a_x.f_equals(a_y));
}

t_transfer t_dictionary::f_instantiate()
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_dictionary>());
	object.f_pointer__(new t_dictionary());
	return object;
}

const t_value& t_dictionary::f_get(const t_value& a_key) const
{
	t_hash::t_entry* field = v_hash.f_find<t_hash_traits>(a_key);
	if (!field) t_throwable::f_throw(L"key not found.");
	return field->v_value;
}

t_transfer t_dictionary::f_remove(const t_value& a_key)
{
	std::pair<bool, t_transfer> pair = v_hash.f_remove<t_hash_traits>(a_key);
	if (!pair.first) t_throwable::f_throw(L"key not found.");
	--v_size;
	return pair.second;
}

std::wstring t_type_of<t_dictionary>::f_string(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& dictionary = f_as<const t_dictionary&>(a_self);
	t_hash::t_iterator i(dictionary.v_hash);
	t_transfer x;
	t_transfer y;
	{
		t_with_lock_for_read lock(a_self);
		if (!i.f_entry()) return L"{}";
		x = i.f_entry()->f_key();
		y = i.f_entry()->v_value;
	}
	x = x.f_get(f_global()->f_symbol_string())();
	if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
	y = y.f_get(f_global()->f_symbol_string())();
	if (!f_is<std::wstring>(y)) t_throwable::f_throw(L"argument must be string.");
	std::wstring s = f_as<std::wstring>(x) + L": " + f_as<std::wstring>(y);
	while (true) {
		{
			t_with_lock_for_read lock(a_self);
			i.f_next();
			if (!i.f_entry()) break;
			x = i.f_entry()->f_key();
			y = i.f_entry()->v_value;
		}
		x = x.f_get(f_global()->f_symbol_string())();
		if (!f_is<std::wstring>(x)) t_throwable::f_throw(L"argument must be string.");
		y = y.f_get(f_global()->f_symbol_string())();
		if (!f_is<std::wstring>(y)) t_throwable::f_throw(L"argument must be string.");
		s += L", " + f_as<std::wstring>(x) + L": " + f_as<std::wstring>(y);
	}
	return L'{' + s + L'}';
}

int t_type_of<t_dictionary>::f_hash(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& dictionary = f_as<const t_dictionary&>(a_self);
	int n = 0;
	t_hash::t_iterator i(dictionary.v_hash);
	while (true) {
		t_transfer x;
		t_transfer y;
		{
			t_with_lock_for_read lock(a_self);
			if (!i.f_entry()) break;
			x = i.f_entry()->f_key();
			y = i.f_entry()->v_value;
			i.f_next();
		}
		x = x.f_hash();
		if (!f_is<int>(x)) t_throwable::f_throw(L"argument must be integer.");
		n ^= f_as<int>(x);
		y = y.f_hash();
		if (!f_is<int>(y)) t_throwable::f_throw(L"argument must be integer.");
		n ^= f_as<int>(y);
	}
	return n;
}

bool t_type_of<t_dictionary>::f_equals(const t_value& a_self, const t_value& a_other)
{
	if (a_self == a_other) return true;
	f_check<t_dictionary>(a_self, L"this");
	if (!f_is<t_dictionary>(a_other)) return false;
	const t_dictionary& d0 = f_as<const t_dictionary&>(a_self);
	const t_dictionary& d1 = f_as<const t_dictionary&>(a_other);
	if (d0.f_size() != d1.f_size()) return false;
	t_hash::t_iterator i(d0.v_hash);
	while (true) {
		t_transfer x;
		t_transfer y;
		{
			t_with_lock_for_read lock0(a_self);
			t_with_lock_for_read lock1(a_other);
			if (!i.f_entry()) break;
			t_hash::t_entry* field = d1.v_hash.f_find<t_dictionary::t_hash_traits>(i.f_entry()->f_key());
			if (!field) return false;
			x = i.f_entry()->v_value;
			y = field->v_value;
			i.f_next();
		}
		if (!f_as<bool>(x.f_equals(y))) return false;
	}
	return false;
}

void t_type_of<t_dictionary>::f_each(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& d0 = f_as<const t_dictionary&>(a_self);
	t_hash::t_iterator i(d0.v_hash);
	while (true) {
		t_transfer key;
		t_transfer value;
		{
			t_with_lock_for_read lock0(a_self);
			if (!i.f_entry()) break;
			key = i.f_entry()->f_key();
			value = i.f_entry()->v_value;
			i.f_next();
		}
		a_callable(key, value);
	}
}

void t_type_of<t_dictionary>::f_define()
{
	t_define<t_dictionary, t_object>(f_global(), L"Dictionary")
		(f_global()->f_symbol_string(), t_member<std::wstring (*)(const t_value&), f_string>())
		(f_global()->f_symbol_hash(), t_member<int (*)(const t_value&), f_hash>())
		(f_global()->f_symbol_get_at(), t_member<const t_value& (t_dictionary::*)(const t_value&) const, &t_dictionary::f_get, t_with_lock_for_read>())
		(f_global()->f_symbol_set_at(), t_member<t_transfer (t_dictionary::*)(const t_value&, const t_transfer&), &t_dictionary::f_put, t_with_lock_for_write>())
		(f_global()->f_symbol_equals(), t_member<bool (*)(const t_value&, const t_value&), f_equals>())
		(f_global()->f_symbol_not_equals(), t_member<bool (*)(const t_value&, const t_value&), f_not_equals>())
		(L"clear", t_member<void (t_dictionary::*)(), &t_dictionary::f_clear, t_with_lock_for_write>())
		(L"size", t_member<size_t (t_dictionary::*)() const, &t_dictionary::f_size, t_with_lock_for_read>())
		(L"has", t_member<bool (t_dictionary::*)(const t_value&) const, &t_dictionary::f_has, t_with_lock_for_read>())
		(L"remove", t_member<t_transfer (t_dictionary::*)(const t_value&), &t_dictionary::f_remove, t_with_lock_for_write>())
		(L"each", t_member<void (*)(const t_value&, const t_value&), f_each>())
	;
}

t_type* t_type_of<t_dictionary>::f_derive(t_object* a_this)
{
	return new t_derived<t_type_of>(v_module, a_this);
}

void t_type_of<t_dictionary>::f_scan(t_object* a_this, t_scan a_scan)
{
	t_dictionary& p = f_as<t_dictionary&>(a_this);
	p.v_hash.f_scan(a_scan);
}

void t_type_of<t_dictionary>::f_finalize(t_object* a_this)
{
	delete &f_as<t_dictionary&>(a_this);
}

void t_type_of<t_dictionary>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_transfer p = t_dictionary::f_instantiate();
	t_dictionary& dictionary = f_as<t_dictionary&>(p);
	for (size_t i = 1; i <= a_n; ++i) {
		t_transfer x = a_stack[i].f_transfer();
		if (++i > a_n) {
			dictionary.f_put(x, t_transfer());
			break;
		}
		t_transfer y = a_stack[i].f_transfer();
		dictionary.f_put(x, y);
	}
	a_stack[0].f_construct(p);
}

void t_type_of<t_dictionary>::f_hash(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	a_stack[0].f_construct(f_hash(t_value(a_this)));
	context.f_done();
}

void t_type_of<t_dictionary>::f_get_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	portable::t_scoped_lock_for_read lock(a_this->v_lock);
	a_stack[0].f_construct(f_as<const t_dictionary&>(a_this).f_get(a0));
	context.f_done();
}

void t_type_of<t_dictionary>::f_set_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	t_transfer a1 = a_stack[2].f_transfer();
	portable::t_scoped_lock_for_write lock(a_this->v_lock);
	a_stack[0].f_construct(f_as<t_dictionary&>(a_this).f_put(a0, a1));
	context.f_done();
}

void t_type_of<t_dictionary>::f_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_equals(a_this, a0));
	context.f_done();
}

void t_type_of<t_dictionary>::f_not_equals(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	a_stack[0].f_construct(f_not_equals(a_this, a0));
	context.f_done();
}

}
