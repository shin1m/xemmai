#include <xemmai/dictionary.h>

#include <xemmai/convert.h>
#include <xemmai/derived.h>

namespace xemmai
{

t_dictionary::t_entry* t_dictionary::t_entry::f_allocate()
{
	return f_engine()->v_dictionary__entry__pool.f_allocate(t_engine::V_POOL__ALLOCATION__UNIT);
}

t_dictionary::t_table* t_dictionary::t_table::f_allocate(size_t a_rank)
{
	return f_engine()->f_dictionary__table__allocate(a_rank);
}

void t_dictionary::t_table::f_free(t_dictionary::t_table* a_p)
{
	f_engine()->f_dictionary__table__free(a_p);
}

void t_dictionary::t_table::f_scan(t_scan a_scan)
{
	for (size_t i = 0; i < v_capacity; ++i) {
		for (t_entry* p = v_entries[i]; p; p = p->v_next) {
			if (!p->v_key) return;
			a_scan(p->v_key);
			a_scan(p->v_value);
		}
	}
}

void t_dictionary::t_table::f_finalize()
{
	for (size_t i = 0; i < v_capacity; ++i) {
		t_entry* p = v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			t_local_pool<t_entry>::f_free(p);
			p = q;
		}
	}
	f_engine()->f_dictionary__table__free(this);
}

void t_dictionary::t_table::f_clear()
{
	for (size_t i = 0; i < v_capacity; ++i) {
		t_entry* p = v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			p->v_key = 0;
			p->v_value = 0;
			t_local_pool<t_entry>::f_free(p);
			p = q;
		}
	}
	f_engine()->f_dictionary__table__free(this);
}

void t_dictionary::f_rehash(size_t a_rank)
{
	t_table* table = v_table;
	v_table = t_table::f_allocate(a_rank);
	v_table->v_size = table->v_size;
	size_t n = table->v_capacity;
	for (size_t i = 0; i < n; ++i) {
		t_entry* p = table->v_entries[i];
		while (p) {
			t_entry* q = p->v_next;
			t_entry** bucket = v_table->f_bucket(f_as<size_t>(p->v_key.f_hash()));
			p->v_next = *bucket;
			t_scoped(p->v_key);
			t_scoped(p->v_value);
			*bucket = p;
			p = q;
		}
	}
	t_table::f_free(table);
}

t_dictionary::t_entry* t_dictionary::f_find(const t_value& a_key) const
{
	t_entry* p = *v_table->f_bucket(f_as<size_t>(a_key.f_hash()));
	while (p) {
		if (f_as<bool>(p->v_key.f_equals(a_key))) return p;
		p = p->v_next;
	}
	return 0;
}

t_transfer t_dictionary::f_instantiate()
{
	t_transfer object = t_object::f_allocate(f_global()->f_type<t_dictionary>());
	object.f_pointer__(new t_dictionary());
	return object;
}

const t_value& t_dictionary::f_get(const t_value& a_key) const
{
	t_entry* field = f_find(a_key);
	if (!field) t_throwable::f_throw(L"key not found.");
	return field->v_value;
}

t_transfer t_dictionary::f_put(const t_value& a_key, const t_transfer& a_value)
{
	t_entry* p = f_find(a_key);
	if (p) return p->v_value = a_value;
	if (v_table->v_rank < t_table::V_POOLS__SIZE - 1 && v_table->v_size >= v_table->v_capacity) {
		f_rehash(v_table->v_rank + 1);
	}
	t_entry** bucket = v_table->f_bucket(f_as<size_t>(a_key.f_hash()));
	p = t_local_pool<t_entry>::f_allocate(t_entry::f_allocate);
	p->v_next = *bucket;
	p->v_key.f_construct(a_key);
	p->v_value.f_construct(a_value);
	*bucket = p;
	++v_table->v_size;
	return p->v_value;
}

t_transfer t_dictionary::f_remove(const t_value& a_key)
{
	t_entry** bucket = v_table->f_bucket(f_as<size_t>(a_key.f_hash()));
	while (true) {
		t_entry* p = *bucket;
		if (!p) t_throwable::f_throw(L"key not found.");
		if (f_as<bool>(p->v_key.f_equals(a_key))) break;
		bucket = &p->v_next;
	}
	t_entry* p = *bucket;
	*bucket = p->v_next;
	p->v_key = 0;
	t_transfer value = p->v_value.f_transfer();
	t_local_pool<t_entry>::f_free(p);
	--v_table->v_size;
	if (v_table->v_rank > 0 && v_table->v_size < v_table->v_capacity / 2) f_rehash(v_table->v_rank - 1);
	return value;
}

std::wstring t_type_of<t_dictionary>::f_string(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& dictionary = f_as<const t_dictionary&>(a_self);
	t_dictionary::t_iterator i(dictionary);
	t_transfer x;
	t_transfer y;
	{
		t_with_lock_for_read lock(a_self);
		if (!i.f_entry()) return L"{}";
		x = i.f_entry()->f_key();
		y = i.f_entry()->v_value;
	}
	x = x.f_get(f_global()->f_symbol_string())();
	f_check<const std::wstring&>(x, L"value");
	y = y.f_get(f_global()->f_symbol_string())();
	f_check<const std::wstring&>(y, L"value");
	std::wstring s = f_as<const std::wstring&>(x) + L": " + f_as<const std::wstring&>(y);
	while (true) {
		{
			t_with_lock_for_read lock(a_self);
			i.f_next();
			if (!i.f_entry()) break;
			x = i.f_entry()->f_key();
			y = i.f_entry()->v_value;
		}
		x = x.f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(x, L"value");
		y = y.f_get(f_global()->f_symbol_string())();
		f_check<const std::wstring&>(y, L"value");
		s += L", " + f_as<const std::wstring&>(x) + L": " + f_as<const std::wstring&>(y);
	}
	return L'{' + s + L'}';
}

int t_type_of<t_dictionary>::f_hash(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	const t_dictionary& dictionary = f_as<const t_dictionary&>(a_self);
	int n = 0;
	t_dictionary::t_iterator i(dictionary);
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
		f_check<int>(x, L"value");
		n ^= f_as<int>(x);
		y = y.f_hash();
		f_check<int>(y, L"value");
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
	t_dictionary::t_iterator i(d0);
	while (true) {
		t_transfer x;
		t_transfer y;
		{
			t_with_lock_for_read lock0(a_self);
			t_with_lock_for_read lock1(a_other);
			if (!i.f_entry()) break;
			t_dictionary::t_entry* field = d1.f_find(i.f_entry()->f_key());
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
	t_dictionary::t_iterator i(d0);
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
	f_as<t_dictionary&>(a_this).f_scan(a_scan);
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
	t_with_lock_for_read lock(a_this);
	a_stack[0].f_construct(f_as<const t_dictionary&>(a_this).f_get(a0));
	context.f_done();
}

void t_type_of<t_dictionary>::f_set_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	t_transfer a0 = a_stack[1].f_transfer();
	t_transfer a1 = a_stack[2].f_transfer();
	t_with_lock_for_write lock(a_this);
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
