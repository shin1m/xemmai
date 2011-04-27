#ifndef XEMMAI__DICTIONARY_H
#define XEMMAI__DICTIONARY_H

#include "object.h"

namespace xemmai
{

class t_dictionary
{
	friend class t_engine;
	friend struct t_type_of<t_dictionary>;

public:
	class t_iterator;
	class t_entry
	{
		friend class t_dictionary;
		friend class t_iterator;
		template<typename T_base> friend class t_shared_pool;
		template<typename T, size_t A_size> friend class t_fixed_pool;
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
		typedef t_entry* t_variable;

		static const size_t V_POOLS__SIZE = 4;

		size_t v_rank;
		size_t v_capacity;
		union
		{
			t_table* v_next;
			size_t v_size;
		};
		t_entry* v_entries[1];

		static t_table* f_allocate(size_t a_rank);
		static void f_free(t_table* a_p);

		t_table()
		{
		}
		void f_scan(t_scan a_scan);
		void f_finalize();
		void f_clear();
		t_entry** f_bucket(size_t a_key)
		{
			return &v_entries[a_key % v_capacity];
		}
	};

	t_table* v_table;

	t_dictionary() : v_table(t_table::f_allocate(0))
	{
		v_table->v_size = 0;
	}
	~t_dictionary()
	{
		v_table->f_finalize();
	}
	void f_rehash(size_t a_rank);
	t_entry* f_find(const t_value& a_key) const;

public:
	class t_iterator
	{
		t_table* v_table;
		size_t v_i;
		t_entry* v_entry;

	public:
		t_iterator(const t_dictionary& a_dictionary) : v_table(a_dictionary.v_table), v_i(0), v_entry(0)
		{
			size_t n = v_table->v_capacity;
			do {
				v_entry = v_table->v_entries[v_i];
				if (v_entry) break;
			} while (++v_i < n);
		}
		t_entry* f_entry() const
		{
			return v_entry;
		}
		void f_next()
		{
			v_entry = v_entry->v_next;
			if (v_entry) return;
			size_t n = v_table->v_capacity;
			while (++v_i < n) {
				v_entry = v_table->v_entries[v_i];
				if (v_entry) break;
			}
		}
	};
	friend class t_iterator;

	static t_transfer f_instantiate();

	void f_scan(t_scan a_scan)
	{
		v_table->f_scan(a_scan);
	}
	void f_clear()
	{
		v_table->f_clear();
		v_table = t_table::f_allocate(0);
		v_table->v_size = 0;
	}
	size_t f_size() const
	{
		return v_table->v_size;
	}
	const t_value& f_get(const t_value& a_key) const;
	t_transfer f_put(const t_value& a_key, const t_transfer& a_value);
	bool f_has(const t_value& a_key) const
	{
		return f_find(a_key) != 0;
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
