#ifndef XEMMAI__DICTIONARY_H
#define XEMMAI__DICTIONARY_H

#include "object.h"

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
		template<typename T, size_t A_size> friend class t_shared_pool;
		friend class t_local_pool<t_entry>;
		friend class t_engine;

		static t_entry* f_allocate();

		t_entry* v_next;
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
		friend struct t_type_of<t_table>;

		static const size_t v_capacities[];

		size_t v_rank;
		size_t v_capacity;
		size_t v_size;

		void* operator new(size_t a_size, size_t a_rank)
		{
			size_t n = v_capacities[a_rank];
			char* p = new char[a_size + sizeof(t_entry*) * n];
			reinterpret_cast<size_t*>(p)[0] = a_rank;
			reinterpret_cast<size_t*>(p)[1] = n;
			return p;
		}
		void operator delete(void* a_p)
		{
			delete[] static_cast<char*>(a_p);
		}
		void operator delete(void* a_p, size_t)
		{
			delete[] static_cast<char*>(a_p);
		}

		static t_scoped f_instantiate(size_t a_rank);

		t_table() : v_size(0)
		{
			t_entry** entries = f_entries();
			for (size_t i = 0; i < v_capacity; ++i) entries[i] = nullptr;
		}
		~t_table();
		t_entry** f_entries() const
		{
			return const_cast<t_entry**>(reinterpret_cast<t_entry* const*>(this + 1));
		}
		void f_scan(t_scan a_scan);
		void f_clear();
		t_entry** f_bucket(size_t a_key) const
		{
			return &f_entries()[a_key % v_capacity];
		}
	};
	friend struct t_type_of<t_table>;

	t_slot v_table;

	t_dictionary() : v_table(t_table::f_instantiate(0))
	{
	}
	void f_rehash(size_t a_rank);
	t_entry* f_find(const t_value& a_key) const;

public:
	class t_iterator
	{
		const t_table& v_table;
		size_t v_i;
		t_entry* v_entry;

	public:
		t_iterator(const t_dictionary& a_dictionary);
		t_entry* f_entry() const
		{
			return v_entry;
		}
		void f_next()
		{
			v_entry = v_entry->v_next;
			if (v_entry) return;
			t_entry** entries = v_table.f_entries();
			while (++v_i < v_table.v_capacity) {
				v_entry = entries[v_i];
				if (v_entry) break;
			}
		}
	};
	friend class t_iterator;

	static t_scoped f_instantiate();

	void f_clear();
	size_t f_size() const;
	const t_value& f_get(const t_value& a_key) const;
	t_scoped f_put(const t_value& a_key, t_scoped&& a_value);
	bool f_has(const t_value& a_key) const
	{
		return f_find(a_key) != nullptr;
	}
	t_scoped f_remove(const t_value& a_key);
};

template<>
struct t_type_of<t_dictionary::t_table> : t_type
{
	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_dictionary> : t_type
{
	static void f_construct(t_object* a_module, const t_value& a_self, t_slot* a_stack, size_t a_n);
	static std::wstring f_string(const t_value& a_self);
	static ptrdiff_t f_hash(const t_value& a_self);
	static bool f_equals(const t_value& a_self, const t_value& a_other);
	static bool f_not_equals(const t_value& a_self, const t_value& a_other)
	{
		return !f_equals(a_self, a_other);
	}
	static void f_each(const t_value& a_self, const t_value& a_callable);
	static void f_define();

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_hash(t_object* a_this, t_slot* a_stack);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
	virtual void f_set_at(t_object* a_this, t_slot* a_stack);
	virtual void f_equals(t_object* a_this, t_slot* a_stack);
	virtual void f_not_equals(t_object* a_this, t_slot* a_stack);
};

inline t_dictionary::t_iterator::t_iterator(const t_dictionary& a_dictionary) : v_table(f_as<const t_table&>(a_dictionary.v_table)), v_i(0), v_entry(nullptr)
{
	t_entry** entries = v_table.f_entries();
	do {
		v_entry = entries[v_i];
		if (v_entry) break;
	} while (++v_i < v_table.v_capacity);
}

inline void t_dictionary::f_clear()
{
	f_as<t_table&>(v_table).f_clear();
	v_table = t_table::f_instantiate(0);
}

inline size_t t_dictionary::f_size() const
{
	return f_as<t_table&>(v_table).v_size;
}

}

#endif
