#include <xemmai/dictionary.h>

#include <xemmai/convert.h>

namespace xemmai
{

#ifdef XEMMAI__DICTIONARY__PRIME
namespace
{

template<size_t A_capacity>
constexpr std::integral_constant<size_t, A_capacity> R;

}

const t_dictionary::t_table::t_rank t_dictionary::t_table::v_ranks[] = {
	{R<11>, 11 / 2, 0}, R<23>, R<53>, R<97>, R<193>, R<389>, R<769>,
	R<1543>, R<3079>, R<6151>, R<12289>, R<24593>, R<49157>, R<98317>,
	R<196613>, R<393241>, R<786433>, R<1572869>, R<3145739>, R<6291469>, R<12582917>,
	R<25165843>, R<50331653>, R<100663319>, R<201326611>, R<402653189>, R<805306457>, {R<1610612741>, size_t(-1)}
};

t_scoped t_dictionary::t_table::f_instantiate(const t_rank& a_rank)
#else
t_scoped t_dictionary::t_table::f_instantiate(size_t a_rank)
#endif
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_table>());
	object.f_pointer__(new(a_rank) t_table(a_rank));
	return object;
}

#ifdef XEMMAI__DICTIONARY__PRIME
void t_dictionary::f_rehash(const t_table::t_rank& a_rank)
#else
void t_dictionary::f_rehash(size_t a_rank)
#endif
{
	auto& table0 = f_as<t_table&>(v_table);
	t_scoped table = t_table::f_instantiate(a_rank);
	auto& table1 = f_as<t_table&>(table);
	table1.v_size = table0.v_size;
	auto entries = table1.f_entries();
	for (auto p = table0.f_entries(); p != table0.v_end; ++p) {
		if (!p->v_hash) continue;
#ifdef XEMMAI__DICTIONARY__PRIME
		auto q = entries + table1.v_slot(p->v_hash);
		while (q->v_hash) if (++q >= table1.v_end) q = entries;
#else
		size_t i = p->v_hash & table1.v_mask;
		while (entries[i].v_hash) i = (i + 1) & table1.v_mask;
		auto q = entries + i;
#endif
		q->v_hash = p->v_hash;
		q->v_key.f_construct(std::move(p->v_key));
		q->v_value.f_construct(std::move(p->v_value));
	}
	v_table = std::move(table);
}

t_scoped t_dictionary::f_instantiate()
{
	t_scoped object = t_object::f_allocate(f_global()->f_type<t_dictionary>());
	object.f_pointer__(new t_dictionary());
	return object;
}

t_scoped t_dictionary::f_put(const t_value& a_key, t_scoped&& a_value)
{
	size_t hash = f_as<size_t>(a_key.f_hash());
	{
		auto& table = f_as<t_table&>(v_table);
		auto p = table.f_find(a_key, hash);
		if (p->v_hash) return p->v_value = std::move(a_value);
#ifdef XEMMAI__DICTIONARY__PRIME
		if (table.v_size >= table.v_rank.v_upper) f_rehash(*(&table.v_rank + 1));
#else
		if (table.v_size >= table.v_upper) f_rehash(table.v_rank + 1);
#endif
	}
	auto& table = f_as<t_table&>(v_table);
	auto p = table.f_find(a_key, hash);
	p->v_hash = ~(~0u >> 1) | hash;
	p->v_key.f_construct(a_key);
	p->v_value.f_construct(std::move(a_value));
	++table.v_size;
	return p->v_value;
}

t_scoped t_dictionary::f_remove(const t_value& a_key)
{
	auto& table = f_as<t_table&>(v_table);
	auto p = table.f_find(a_key);
	if (!p->v_hash) f_throw(L"key not found.");
	p->v_hash = 0;
	p->v_key = nullptr;
	t_scoped value = std::move(p->v_value);
	auto entries = table.f_entries();
#ifdef XEMMAI__DICTIONARY__PRIME
	auto q = p;
#else
	size_t i = p - entries;
	size_t j = i;
#endif
	while (true) {
#ifdef XEMMAI__DICTIONARY__PRIME
		if (++q >= table.v_end) q = entries;
		if (!q->v_hash) break;
		size_t k = entries + table.v_slot(q->v_hash) - p;
		if (k > 0 && k <= q - p) continue;
#else
		j = (j + 1) & table.v_mask;
		auto q = entries + j;
		if (!q->v_hash) break;
		size_t k = (q->v_hash & table.v_mask) - i;
		if (k > 0 && k <= j - i) continue;
#endif
		p->v_hash = q->v_hash;
		q->v_hash = 0;
		p->v_key = std::move(q->v_key);
		p->v_value = std::move(q->v_value);
		p = q;
#ifndef XEMMAI__DICTIONARY__PRIME
		i = j;
#endif
	}
#ifdef XEMMAI__DICTIONARY__PRIME
	if (--table.v_size < table.v_rank.v_lower) f_rehash(*(&table.v_rank - 1));
#else
	if (--table.v_size < table.v_lower) f_rehash(table.v_rank - 1);
#endif
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
	t_dictionary::t_iterator i(f_as<const t_dictionary&>(a_self));
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
	f_check<const std::wstring&>(x, L"key");
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
		f_check<const std::wstring&>(x, L"key");
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
	t_dictionary::t_iterator i(f_as<const t_dictionary&>(a_self));
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
