#ifndef XEMMAI__MAP_H
#define XEMMAI__MAP_H

#include "sharable.h"
#include "boolean.h"
#include "integer.h"

namespace xemmai
{

class t_map : public t_sharable
{
	friend struct t_type_of<t_object>;
	friend struct t_type_of<t_map>;
	friend class t_global;

public:
	class t_iterator;
	class t_entry
	{
		friend class t_map;
		friend class t_iterator;

		size_t v_gap = 0;
		size_t v_hash;
		t_svalue v_key;

	public:
		t_svalue v_value;

		const t_svalue& f_key() const
		{
			return v_key;
		}
	};

private:
	struct t_table
	{
		struct t_rank
		{
			template<size_t A_capacity>
			static size_t f_slot(size_t a_hash)
			{
				return a_hash % A_capacity;
			}

			size_t v_capacity;
			size_t v_upper;
			size_t v_lower;
			size_t(*v_slot)(size_t);

			template<size_t A_capacity>
			constexpr t_rank(std::integral_constant<size_t, A_capacity>, size_t a_lower = A_capacity / 4) : v_capacity(A_capacity), v_upper(A_capacity / 2), v_lower(a_lower), v_slot(f_slot<A_capacity>)
			{
			}
		};
		static const t_rank v_ranks[];

		size_t(*v_slot)(size_t);
		t_entry* v_end;
		const t_rank& v_rank;

		static t_object* f_instantiate(t_type* a_type, const t_rank& a_rank)
		{
			auto p = f_engine()->f_allocate(sizeof(t_table) + sizeof(t_entry) * a_rank.v_capacity);
			new(p->f_data()) t_table(a_rank);
			p->f_be(a_type);
			return p;
		}

		t_table(const t_rank& a_rank) : v_slot(a_rank.v_slot), v_end(f_entries() + a_rank.v_capacity), v_rank(a_rank)
		{
			std::uninitialized_default_construct(f_entries(), v_end);
		}
		t_entry* f_entries() const
		{
			return const_cast<t_entry*>(reinterpret_cast<const t_entry*>(this + 1));
		}
		void f_scan(t_scan a_scan)
		{
			for (auto p = f_entries(); p != v_end; ++p) {
				a_scan(p->v_key);
				a_scan(p->v_value);
			}
		}
		static bool f_equals(const t_entry& a_p, size_t a_hash, const t_pvalue& a_key)
		{
			return a_p.v_hash == a_hash && f_as<bool>(a_p.v_key.f_equals(a_key));
		}
		XEMMAI__PORTABLE__ALWAYS_INLINE t_entry* f_find(const t_pvalue& a_key) const
		{
			size_t hash = f_as<size_t>(a_key.f_hash());
			auto p = f_entries() + v_slot(hash);
			size_t gap = 1;
			for (; p->v_gap > gap; ++gap) if (++p >= v_end) p = f_entries();
			for (; p->v_gap == gap; ++gap) {
				if (f_equals(*p, hash, a_key)) return p;
				if (++p >= v_end) p = f_entries();
			}
			return nullptr;
		}
		std::pair<t_entry*, size_t> f_find(size_t a_hash, const t_pvalue& a_key) const
		{
			auto p = f_entries() + v_slot(a_hash);
			size_t gap = 1;
			for (; p->v_gap > gap; ++gap) if (++p >= v_end) p = f_entries();
			for (; p->v_gap >= gap && !f_equals(*p, a_hash, a_key); ++gap) if (++p >= v_end) p = f_entries();
			return {p, gap};
		}
		XEMMAI__LOCAL void f_put(t_entry* a_p, size_t a_gap, size_t a_hash, t_pvalue a_key, t_pvalue a_value);
	};
	friend struct t_type_of<t_table>;

	t_slot v_table;
	size_t v_size = 0;

	t_map(t_type* a_table) : v_table(t_table::f_instantiate(a_table, t_table::v_ranks[0]))
	{
	}
	XEMMAI__LOCAL void f_rehash(const t_table::t_rank& a_rank);

public:
	class t_iterator
	{
		t_object* v_table;
		t_entry* v_entry;
		t_entry* v_end;

	public:
		t_iterator(const t_map& a_map) : v_table(a_map.v_table), v_entry(v_table->f_as<t_table>().f_entries()), v_end(v_table->f_as<t_table>().v_end)
		{
			do if (v_entry->v_gap) return; while (++v_entry < v_end);
			v_entry = nullptr;
		}
		t_entry* f_entry() const
		{
			return v_entry;
		}
		void f_next()
		{
			while (++v_entry < v_end) if (v_entry->v_gap) return;
			v_entry = nullptr;
		}
	};

	static t_object* f_instantiate();

	void f_clear()
	{
		v_table = t_table::f_instantiate(v_table->f_type(), t_table::v_ranks[0]);
		v_size = 0;
	}
	size_t f_size() const
	{
		return v_size;
	}
	const t_svalue& f_get(const t_pvalue& a_key) const
	{
		auto p = v_table->f_as<t_table>().f_find(a_key);
		if (!p) f_throw(L"key not found."sv);
		return p->v_value;
	}
	t_pvalue f_put(const t_pvalue& a_key, const t_pvalue& a_value);
	bool f_has(const t_pvalue& a_key) const
	{
		return v_table->f_as<t_table>().f_find(a_key);
	}
	t_pvalue f_remove(const t_pvalue& a_key);
};

template<>
struct t_type_of<t_map::t_table> : t_uninstantiatable<t_finalizes<t_derives<t_map::t_table>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_this->f_as<t_map::t_table>().f_scan(a_scan);
	}
};

template<>
struct t_type_of<t_map> : t_derivable<t_holds<t_map, t_type_of<t_sharable>>>
{
	XEMMAI__LOCAL static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan)
	{
		a_scan(a_this->f_as<t_map>().v_table);
	}
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_pvalue* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_pvalue* a_stack);
};

}

#endif
