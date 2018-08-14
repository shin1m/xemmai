#include <xemmai/dictionary.h>

#include <xemmai/convert.h>

namespace xemmai
{

const size_t t_dictionary::t_table::v_capacities[] = {
	11, 23, 53, 97, 193, 389, 769,
	1543, 3079, 6151, 12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917,
	25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741
};

t_scoped t_dictionary::t_table::f_instantiate(size_t a_rank)
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_table>());
	object.f_pointer__(new(a_rank) t_table(a_rank));
	return object;
}

void t_dictionary::t_table::f_scan(t_scan a_scan)
{
	t_entry** entries = f_entries();
	bool b = false;
	for (size_t i = 0; i < v_capacity; ++i) {
		for (t_entry* p = entries[i]; p; p = p->v_next) {
			if (p->v_key) {
				a_scan(p->v_key);
			} else {
				if (b) return;
				b = true;
			}
			a_scan(p->v_value);
		}
	}
}

void t_dictionary::t_table::f_clear()
{
	t_entry** entries = f_entries();
	for (size_t i = 0; i < v_capacity; ++i) {
		t_entry* p = entries[i];
		entries[i] = nullptr;
		while (p) {
			t_entry* q = p->v_next;
			p->v_key = nullptr;
			p->v_value = nullptr;
			t_local_pool<t_entry>::f_free(p);
			p = q;
		}
	}
}

void t_dictionary::f_rehash(size_t a_rank)
{
	auto& table0 = f_as<t_table&>(v_table);
	t_scoped table = t_table::f_instantiate(a_rank);
	auto& table1 = f_as<t_table&>(table);
	table1.v_size = table0.v_size;
	t_entry** entries = table0.f_entries();
	for (size_t i = 0; i < table0.v_capacity; ++i) {
		t_entry* p = entries[i];
		entries[i] = nullptr;
		while (p) {
			t_entry* q = p->v_next;
			t_entry** bucket = table1.f_bucket(f_as<size_t>(p->v_key.f_hash()));
			p->v_next = *bucket;
			t_scoped(p->v_key);
			t_scoped(p->v_value);
			*bucket = p;
			p = q;
		}
	}
	v_table = std::move(table);
}

t_dictionary::t_entry* t_dictionary::f_find(const t_value& a_key) const
{
	t_entry* p = *f_as<const t_table&>(v_table).f_bucket(f_as<size_t>(a_key.f_hash()));
	while (p) {
		if (f_as<bool>(p->v_key.f_equals(a_key))) return p;
		p = p->v_next;
	}
	return nullptr;
}

t_scoped t_dictionary::f_instantiate()
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_dictionary>());
	object.f_pointer__(new t_dictionary());
	return object;
}

t_scoped t_dictionary::f_put(const t_value& a_key, t_scoped&& a_value)
{
	t_entry* p = f_find(a_key);
	if (p) return p->v_value = std::move(a_value);
	{
		auto& table = f_as<t_table&>(v_table);
		if (table.v_rank < sizeof(t_table::v_capacities) / sizeof(size_t) - 1 && table.v_size >= table.v_capacity) f_rehash(table.v_rank + 1);
	}
	auto& table = f_as<t_table&>(v_table);
	t_entry** bucket = table.f_bucket(f_as<size_t>(a_key.f_hash()));
	p = t_local_pool<t_entry>::f_allocate(t_entry::f_allocate);
	p->v_next = *bucket;
	p->v_key.f_construct(a_key);
	p->v_value.f_construct(std::move(a_value));
	*bucket = p;
	++table.v_size;
	return p->v_value;
}

t_scoped t_dictionary::f_remove(const t_value& a_key)
{
	auto& table = f_as<t_table&>(v_table);
	t_entry** bucket = table.f_bucket(f_as<size_t>(a_key.f_hash()));
	while (true) {
		t_entry* p = *bucket;
		if (!p) f_throw(L"key not found.");
		if (f_as<bool>(p->v_key.f_equals(a_key))) break;
		bucket = &p->v_next;
	}
	t_entry* p = *bucket;
	*bucket = p->v_next;
	p->v_key = nullptr;
	t_scoped value = std::move(p->v_value);
	t_local_pool<t_entry>::f_free(p);
	--table.v_size;
	if (table.v_rank > 0 && table.v_size < table.v_capacity / 2) f_rehash(table.v_rank - 1);
	return value;
}

void t_type_of<t_dictionary::t_table>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	f_as<t_dictionary::t_table&>(a_this).f_scan(a_scan);
}

void t_type_of<t_dictionary>::f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n)
{
	if (a_stack[1].f_type() != f_global()->f_type<t_class>()) f_throw(a_stack, a_n, L"must be class.");
	t_scoped p = t_object::f_allocate(&f_as<t_type&>(a_stack[1]));
	a_stack[1].f_destruct();
	auto dictionary = new t_dictionary();
	p.f_pointer__(dictionary);
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) {
		t_scoped x = std::move(a_stack[i]);
		if (++i >= a_n) {
			dictionary->f_put(x, {});
			break;
		}
		try {
			dictionary->f_put(x, t_scoped(std::move(a_stack[i])));
		} catch (...) {
			while (++i < a_n) a_stack[i].f_destruct();
			throw;
		}
	}
	a_stack[0].f_construct(std::move(p));
}

std::wstring t_type_of<t_dictionary>::f_string(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	auto& dictionary = f_as<const t_dictionary&>(a_self);
	t_dictionary::t_iterator i(dictionary);
	t_scoped x;
	t_scoped y;
	if (!f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		if (!i.f_entry()) return false;
		x = i.f_entry()->f_key();
		y = i.f_entry()->v_value;
		return true;
	})) return L"{}";
	x = x.f_invoke(f_global()->f_symbol_string());
	f_check<const std::wstring&>(x, L"value");
	y = y.f_invoke(f_global()->f_symbol_string());
	f_check<const std::wstring&>(y, L"value");
	std::wstring s = f_as<const std::wstring&>(x) + L": " + f_as<const std::wstring&>(y);
	while (f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		i.f_next();
		if (!i.f_entry()) return false;
		x = i.f_entry()->f_key();
		y = i.f_entry()->v_value;
		return true;
	})) {
		x = x.f_invoke(f_global()->f_symbol_string());
		f_check<const std::wstring&>(x, L"value");
		y = y.f_invoke(f_global()->f_symbol_string());
		f_check<const std::wstring&>(y, L"value");
		s += L", " + f_as<const std::wstring&>(x) + L": " + f_as<const std::wstring&>(y);
	}
	return L'{' + s + L'}';
}

void t_type_of<t_dictionary>::f_clear(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		f_as<t_dictionary&>(a_self).f_clear();
	});
}

size_t t_type_of<t_dictionary>::f_size(const t_value& a_self)
{
	f_check<t_dictionary>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return f_as<const t_dictionary&>(a_self).f_size();
	});
}

t_scoped t_type_of<t_dictionary>::f__get_at(const t_value& a_self, const t_value& a_key)
{
	f_check<t_dictionary>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return t_scoped(f_as<const t_dictionary&>(a_self).f_get(a_key));
	});
}

t_scoped t_type_of<t_dictionary>::f__set_at(const t_value& a_self, const t_value& a_key, t_scoped&& a_value)
{
	f_check<t_dictionary>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_dictionary&>(a_self).f_put(a_key, std::move(a_value));
	});
}

bool t_type_of<t_dictionary>::f_has(const t_value& a_self, const t_value& a_key)
{
	f_check<t_dictionary>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
	{
		return f_as<t_dictionary&>(a_self).f_has(a_key);
	});
}

t_scoped t_type_of<t_dictionary>::f_remove(const t_value& a_self, const t_value& a_key)
{
	f_check<t_dictionary>(a_self, L"this");
	return f_owned_or_shared<t_with_lock_for_write>(a_self, [&]
	{
		return f_as<t_dictionary&>(a_self).f_remove(a_key);
	});
}

void t_type_of<t_dictionary>::f_each(const t_value& a_self, const t_value& a_callable)
{
	f_check<t_dictionary>(a_self, L"this");
	auto& dictionary = f_as<const t_dictionary&>(a_self);
	t_dictionary::t_iterator i(dictionary);
	while (true) {
		t_scoped key;
		t_scoped value;
		if (!f_owned_or_shared<t_with_lock_for_read>(a_self, [&]
		{
			if (!i.f_entry()) return false;
			key = i.f_entry()->f_key();
			value = i.f_entry()->v_value;
			i.f_next();
			return true;
		})) break;
		a_callable(std::move(key), std::move(value));
	}
}

void t_type_of<t_dictionary>::f_define()
{
	f_global()->v_type_dictionary__table.f_construct((new t_type_of<t_dictionary::t_table>(t_type_of<t_dictionary::t_table>::V_ids, f_global()->f_type<t_object>(), f_global()->f_module()))->v_this);
	t_define<t_dictionary, t_object>(f_global(), L"Dictionary")
		(f_global()->f_symbol_construct(), f__construct)
		(f_global()->f_symbol_string(), t_member<std::wstring(*)(const t_value&), f_string>())
		(L"clear", t_member<void(*)(const t_value&), f_clear>())
		(f_global()->f_symbol_size(), t_member<size_t(*)(const t_value&), f_size>())
		(f_global()->f_symbol_get_at(), t_member<t_scoped(*)(const t_value&, const t_value&), f__get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_scoped(*)(const t_value&, const t_value&, t_scoped&&), f__set_at>())
		(L"has", t_member<bool(*)(const t_value&, const t_value&), f_has>())
		(L"remove", t_member<t_scoped(*)(const t_value&, const t_value&), f_remove>())
		(L"each", t_member<void(*)(const t_value&, const t_value&), f_each>())
	;
}

void t_type_of<t_dictionary>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_dictionary&>(a_this).v_table);
}

t_scoped t_type_of<t_dictionary>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	t_scoped p = t_object::f_allocate(this);
	auto dictionary = new t_dictionary();
	p.f_pointer__(dictionary);
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) {
		const auto& x = a_stack[i];
		if (++i >= a_n) {
			dictionary->f_put(x, {});
			break;
		}
		dictionary->f_put(x, t_scoped(a_stack[i]));
	}
	return p;
}

size_t t_type_of<t_dictionary>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_owned_or_shared<t_with_lock_for_read>(a_this, [&]
	{
		a_stack[0].f_construct(f_as<const t_dictionary&>(a_this).f_get(a0.v_p));
	});
	return -1;
}

size_t t_type_of<t_dictionary>::f_do_set_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	t_scoped a1 = std::move(a_stack[3]);
	f_owned_or_shared<t_with_lock_for_write>(a_this, [&]
	{
		a_stack[0].f_construct(f_as<t_dictionary&>(a_this).f_put(a0.v_p, std::move(a1)));
	});
	return -1;
}

}
