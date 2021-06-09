#include <xemmai/convert.h>

namespace xemmai
{

namespace
{

template<size_t A_capacity>
constexpr std::integral_constant<size_t, A_capacity> R;

}

const t_map::t_table::t_rank t_map::t_table::v_ranks[] = {
	{R<11>, 0}, R<23>, R<53>, R<97>, R<193>, R<389>, R<769>,
	R<1543>, R<3079>, R<6151>, R<12289>, R<24593>, R<49157>, R<98317>,
	R<196613>, R<393241>, R<786433>, R<1572869>, R<3145739>, R<6291469>, R<12582917>,
	R<25165843>, R<50331653>, R<100663319>, R<201326611>, R<402653189>, R<805306457>, R<1610612741>
};

void t_map::t_table::f_put(t_entry* a_p, size_t a_gap, size_t a_hash, t_pvalue a_key, t_pvalue a_value)
{
	while (a_p->v_gap) {
		std::swap(a_p->v_gap, a_gap);
		std::swap(a_p->v_hash, a_hash);
		t_pvalue key = a_p->v_key;
		a_p->v_key = a_key;
		a_key = key;
		t_pvalue value = a_p->v_value;
		a_p->v_value = a_value;
		a_value = value;
		do if (++a_p >= v_end) a_p = f_entries(); while (a_p->v_gap >= ++a_gap);
	}
	a_p->v_gap = a_gap;
	a_p->v_hash = a_hash;
	a_p->v_key = a_key;
	a_p->v_value = a_value;
}

void t_map::f_rehash(const t_table::t_rank& a_rank)
{
	auto object = t_table::f_instantiate(v_table->f_type(), a_rank);
	auto& table0 = v_table->f_as<t_table>();
	auto& table1 = object->f_as<t_table>();
	table1.v_size = table0.v_size;
	auto entries = table1.f_entries();
	for (auto p = table0.f_entries(); p != table0.v_end; ++p) {
		if (!p->v_gap) continue;
		size_t gap = 0;
		auto q = entries + table1.v_slot(p->v_hash);
		while (q->v_gap >= ++gap) if (++q >= table1.v_end) q = entries;
		table1.f_put(q, gap, p->v_hash, std::move(p->v_key), std::move(p->v_value));
	}
	v_table = std::move(object);
}

t_object* t_map::f_instantiate()
{
	return f_new<t_map>(f_global(), f_global()->f_type<t_map::t_table>());
}

t_pvalue t_map::f_put(const t_pvalue& a_key, const t_pvalue& a_value)
{
	auto table = &v_table->f_as<t_table>();
	auto hash = f_as<size_t>(a_key.f_hash());
	auto [p, gap] = table->f_find(hash, a_key);
	if (p->v_gap == gap) return p->v_value = a_value;
	if (table->v_size >= table->v_rank.v_upper) {
		auto rank = &table->v_rank + 1;
		if (rank >= t_table::v_ranks + sizeof(t_table::v_ranks) / sizeof(t_table::t_rank)) f_throw(L"cannot grow."sv);
		f_rehash(*rank);
		table = &v_table->f_as<t_table>();
		std::tie(p, gap) = table->f_find(hash, a_key);
	}
	table->f_put(p, gap, hash, a_key, a_value);
	++table->v_size;
	return p->v_value;
}

t_pvalue t_map::f_remove(const t_pvalue& a_key)
{
	auto& table = v_table->f_as<t_table>();
	auto p = table.f_find(a_key);
	if (!p) f_throw(L"key not found."sv);
	p->v_gap = 0;
	p->v_key = nullptr;
	t_pvalue value = p->v_value;
	p->v_value = nullptr;
	auto entries = table.f_entries();
	while (true) {
		auto q = p;
		if (++q >= table.v_end) q = entries;
		size_t gap = q->v_gap;
		if (gap <= 1) break;
		for (size_t g = gap;;) {
			auto r = q;
			if (++r >= table.v_end) r = entries;
			if (r->v_gap < ++g) break;
			q = r;
		}
		p->v_gap = gap - 1;
		q->v_gap = 0;
		p->v_hash = q->v_hash;
		p->v_key = q->v_key;
		q->v_key = nullptr;
		p->v_value = q->v_value;
		q->v_value = nullptr;
		p = q;
	}
	if (--table.v_size < table.v_rank.v_lower) f_rehash(*(&table.v_rank - 1));
	return value;
}

t_object* t_type_of<t_map>::f__string(t_map& a_self)
{
	t_map::t_iterator i(a_self);
	std::vector<wchar_t> cs{L'{'};
	t_pvalue x;
	t_pvalue y;
	auto get = [&]
	{
		if (!i.f_entry()) return false;
		x = i.f_entry()->f_key();
		y = i.f_entry()->v_value;
		return true;
	};
	if (a_self.f_owned_or_shared<std::shared_lock>(get)) {
		auto push = [&](t_pvalue& x, const wchar_t* name)
		{
			x = x.f_string();
			f_check<t_string>(x, name);
			auto& s = x->f_as<t_string>();
			auto p = static_cast<const wchar_t*>(s);
			cs.insert(cs.end(), p, p + s.f_size());
		};
		while (true) {
			push(x, L"key");
			cs.push_back(L':');
			cs.push_back(L' ');
			push(y, L"value");
			if (!a_self.f_owned_or_shared<std::shared_lock>([&]
			{
				i.f_next();
				return get();
			})) break;
			cs.push_back(L',');
			cs.push_back(L' ');
		}
	}
	cs.push_back(L'}');
	return t_string::f_instantiate(cs.data(), cs.size());
}

void t_type_of<t_map>::f_clear(t_map& a_self)
{
	a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		a_self.f_clear();
	});
}

size_t t_type_of<t_map>::f_size(t_map& a_self)
{
	return a_self.f_owned_or_shared<std::shared_lock>([&]
	{
		return a_self.f_size();
	});
}

t_pvalue t_type_of<t_map>::f__get_at(t_map& a_self, const t_pvalue& a_key)
{
	return a_self.f_owned_or_shared<std::shared_lock>([&]
	{
		return t_pvalue(a_self.f_get(a_key));
	});
}

t_pvalue t_type_of<t_map>::f__set_at(t_map& a_self, const t_pvalue& a_key, const t_pvalue& a_value)
{
	return a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		return a_self.f_put(a_key, a_value);
	});
}

bool t_type_of<t_map>::f_has(t_map& a_self, const t_pvalue& a_key)
{
	return a_self.f_owned_or_shared<std::shared_lock>([&]
	{
		return a_self.f_has(a_key);
	});
}

t_pvalue t_type_of<t_map>::f_remove(t_map& a_self, const t_pvalue& a_key)
{
	return a_self.f_owned_or_shared<std::lock_guard>([&]
	{
		return a_self.f_remove(a_key);
	});
}

void t_type_of<t_map>::f_each(t_map& a_self, const t_pvalue& a_callable)
{
	t_map::t_iterator i(a_self);
	while (true) {
		t_pvalue key;
		t_pvalue value;
		if (!a_self.f_owned_or_shared<std::shared_lock>([&]
		{
			if (!i.f_entry()) return false;
			key = i.f_entry()->f_key();
			value = i.f_entry()->v_value;
			i.f_next();
			return true;
		})) break;
		a_callable(key, value);
	}
}

void t_type_of<t_map>::f_define()
{
	t_define{f_global()}.f_derive<t_map::t_table, t_object>();
	t_define{f_global()}
		(f_global()->f_symbol_string(), t_member<t_object*(*)(t_map&), f__string>())
		(L"clear"sv, t_member<void(*)(t_map&), f_clear>())
		(f_global()->f_symbol_size(), t_member<size_t(*)(t_map&), f_size>())
		(f_global()->f_symbol_get_at(), t_member<t_pvalue(*)(t_map&, const t_pvalue&), f__get_at>())
		(f_global()->f_symbol_set_at(), t_member<t_pvalue(*)(t_map&, const t_pvalue&, const t_pvalue&), f__set_at>())
		(L"has"sv, t_member<bool(*)(t_map&, const t_pvalue&), f_has>())
		(L"remove"sv, t_member<t_pvalue(*)(t_map&, const t_pvalue&), f_remove>())
		(L"each"sv, t_member<void(*)(t_map&, const t_pvalue&), f_each>())
	.f_derive<t_map, t_sharable>();
}

t_pvalue t_type_of<t_map>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	auto object = f_new<t_map>(f_global()->f_type<t_map::t_table>());
	auto& map = object->f_as<t_map>();
	a_n += 2;
	for (size_t i = 2; i < a_n; ++i) {
		const auto& x = a_stack[i];
		if (++i >= a_n) {
			map.f_put(x, {});
			break;
		}
		map.f_put(x, a_stack[i]);
	}
	return object;
}

size_t t_type_of<t_map>::f_do_get_at(t_object* a_this, t_pvalue* a_stack)
{
	auto& map = a_this->f_as<t_map>();
	map.f_owned_or_shared<std::shared_lock>([&]
	{
		a_stack[0] = map.f_get(a_stack[2]);
	});
	return -1;
}

size_t t_type_of<t_map>::f_do_set_at(t_object* a_this, t_pvalue* a_stack)
{
	auto& map = a_this->f_as<t_map>();
	map.f_owned_or_shared<std::lock_guard>([&]
	{
		a_stack[0] = map.f_put(a_stack[2], a_stack[3]);
	});
	return -1;
}

}
