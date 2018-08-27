#ifndef XEMMAI__DICTIONARY_H
#define XEMMAI__DICTIONARY_H

#include "boolean.h"
#include "integer.h"

namespace xemmai
{

class t_dictionary
{
	friend class t_engine;
	friend class t_global;
	friend struct t_type_of<t_dictionary>;

public:
	class t_iterator;
	class t_entry
	{
		friend class t_dictionary;
		friend class t_iterator;

		size_t v_gap = 0;
		size_t v_hash;
		t_slot v_key;

	public:
		t_slot v_value;

		const t_slot& f_key() const
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
			size_t (*v_slot)(size_t);

			template<size_t A_capacity>
			constexpr t_rank(std::integral_constant<size_t, A_capacity>, size_t a_lower = A_capacity / 4) : v_capacity(A_capacity), v_upper(A_capacity / 2), v_lower(a_lower), v_slot(f_slot<A_capacity>)
			{
			}
		};
		static const t_rank v_ranks[];

		size_t (*v_slot)(size_t);
		t_entry* v_end;
		size_t v_size = 0;
		const t_rank& v_rank;

		void* operator new(size_t a_size, const t_rank& a_rank)
		{
			return new char[a_size + sizeof(t_entry) * a_rank.v_capacity];
		}
		void operator delete(void* a_p)
		{
			delete[] static_cast<char*>(a_p);
		}
		void operator delete(void* a_p, const t_rank&)
		{
			delete[] static_cast<char*>(a_p);
		}

		static t_scoped f_instantiate(const t_rank& a_rank);

		t_table(const t_rank& a_rank) : v_slot(a_rank.v_slot), v_end(f_entries() + a_rank.v_capacity), v_rank(a_rank)
		{
			for (auto p = f_entries(); p != v_end; ++p) new(p) t_entry();
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
		static bool f_equals(const t_entry& a_p, size_t a_hash, const t_value& a_key)
		{
			return a_p.v_hash == a_hash && f_as<bool>(a_p.v_key.f_equals(a_key));
		}
		XEMMAI__PORTABLE__ALWAYS_INLINE t_entry* f_find(const t_value& a_key) const
		{
			size_t hash = f_as<size_t>(a_key.f_hash());
			auto entries = f_entries();
			auto p = entries + v_slot(hash);
			for (size_t gap = 1; p->v_gap >= gap; ++gap) {
				if (f_equals(*p, hash, a_key)) return p;
				if (++p >= v_end) p = entries;
			}
			return nullptr;
		}
		std::pair<t_entry*, size_t> f_find(size_t a_hash, const t_value& a_key) const
		{
			auto entries = f_entries();
			auto p = entries + v_slot(a_hash);
			size_t gap = 1;
			for (; p->v_gap >= gap && !f_equals(*p, a_hash, a_key); ++gap) if (++p >= v_end) p = entries;
			return {p, gap};
		}
		void f_put(t_entry* a_p, size_t a_gap, size_t a_hash, t_scoped&& a_key, t_scoped&& a_value);
	};
	friend struct t_type_of<t_table>;

	t_table* v_table;
	t_slot v_slot;

	t_dictionary();
	void f_rehash(const t_table::t_rank& a_rank);

public:
	class t_iterator
	{
		t_scoped v_slot;
		t_entry* v_entry;
		t_entry* v_end;

	public:
		t_iterator(const t_dictionary& a_dictionary) : v_slot(a_dictionary.v_slot), v_entry(a_dictionary.v_table->f_entries()), v_end(a_dictionary.v_table->v_end)
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
	friend class t_iterator;

	static t_scoped f_instantiate();

	void f_clear();
	size_t f_size() const
	{
		return v_table->v_size;
	}
	const t_value& f_get(const t_value& a_key) const
	{
		auto p = v_table->f_find(a_key);
		if (!p) f_throw(L"key not found.");
		return p->v_value;
	}
	t_scoped f_put(const t_value& a_key, t_scoped&& a_value);
	bool f_has(const t_value& a_key) const
	{
		return v_table->f_find(a_key);
	}
	t_scoped f_remove(const t_value& a_key);
};

template<>
struct t_type_of<t_dictionary::t_table> : t_uninstantiatable<t_underivable<t_finalizes<t_derives<t_dictionary::t_table>>>>
{
	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
};

template<>
struct t_type_of<t_dictionary> : t_derivable<t_holds<t_dictionary>>
{
	static void f__construct(xemmai::t_extension* a_extension, t_stacked* a_stack, size_t a_n);
	static std::wstring f_string(const t_value& a_self);
	static void f_clear(const t_value& a_self);
	static size_t f_size(const t_value& a_self);
	static t_scoped f__get_at(const t_value& a_self, const t_value& a_key);
	static t_scoped f__set_at(const t_value& a_self, const t_value& a_key, t_scoped&& a_value);
	static bool f_has(const t_value& a_self, const t_value& a_key);
	static t_scoped f_remove(const t_value& a_self, const t_value& a_key);
	static void f_each(const t_value& a_self, const t_value& a_callable);
	static void f_define();

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_stacked* a_stack);
};

inline t_dictionary::t_dictionary() : v_slot(t_table::f_instantiate(t_table::v_ranks[0]))
{
	v_table = &f_as<t_table&>(v_slot);
}

inline void t_dictionary::f_clear()
{
	t_scoped p = t_table::f_instantiate(t_table::v_ranks[0]);
	v_table = &f_as<t_table&>(p);
	v_slot = std::move(p);
}

}

#endif
