#include <xemmai/global.h>

namespace xemmai
{

void t_object::f_field_add(t_object* a_structure, const t_pvalue& a_value)
{
	if (!v_fields) {
		auto fields = new(4) t_structure::t_fields(4);
		new(*fields) t_svalue(a_value);
		for (size_t i = 1; i < 4; ++i) new(*fields + i) t_svalue();
		v_fields = fields;
	} else {
		size_t index = v_structure->v_size;
		if (index < v_fields->v_size) {
			auto fields = new(index + 4) t_structure::t_fields(index + 4);
			for (size_t i = 0; i < index; ++i) new(*fields + i) t_svalue((*v_fields)[i]);
			new(*fields + index) t_svalue(a_value);
			for (size_t i = index + 1; i < index + 4; ++i) new(*fields + i) t_svalue();
			f_new<std::unique_ptr<t_structure::t_fields>>(f_global(), true, std::exchange(v_fields, fields));
		} else {
			new(*v_fields + index) t_svalue(a_value);
		}
	}
	auto structure0 = t_object::f_of(v_structure);
	t_slot::t_increments::f_push(a_structure);
	v_structure = &a_structure->f_as<t_structure>();
	t_slot::t_decrements::f_push(structure0);
}

void t_object::f_own()
{
	if (f_type()->v_fixed) f_throw(L"thread mode is fixed."sv);
	{
		t_scoped_lock_for_write lock(v_lock);
		if (v_owner) f_throw(L"already owned."sv);
		v_owner = t_slot::t_increments::v_instance;
	}
	auto p = v_structure->f_fields();
	for (size_t i = 0; i < v_structure->v_size; ++i) {
		t_object* key = p[i];
		size_t j = t_thread::t_cache::f_index(this, key);
		auto& cache = t_thread::v_cache[j];
		if (cache.v_object == this && cache.v_key == key) cache.v_object = cache.v_key = cache.v_value = nullptr;
		cache.v_revision = t_thread::t_cache::f_revise(j);
		cache.v_key_revision = key->f_as<t_symbol>().v_revision;
	}
}

void t_object::f_share()
{
	if (f_type()->v_fixed) f_throw(L"thread mode is fixed."sv);
	if (v_owner != t_slot::t_increments::v_instance) f_throw(L"not owned."sv);
	t_scoped_lock_for_write lock(v_lock);
	v_owner = nullptr;
}

void t_object::f_field_put(t_object* a_key, const t_pvalue& a_value)
{
	intptr_t index = v_structure->f_index(a_key);
	if (index < 0)
		f_field_add(v_structure->f_append(a_key), a_value);
	else
		(*v_fields)[index] = a_value;
}

void t_object::f_field_remove(size_t a_index)
{
	auto structure1 = v_structure->f_remove(a_index);
	t_slot::t_increments::f_push(structure1);
	auto structure0 = t_object::f_of(v_structure);
	v_structure = &structure1->f_as<t_structure>();
	t_slot::t_decrements::f_push(structure0);
	size_t size = v_structure->v_size;
	if (size + 8 > v_fields->v_size) {
		for (; a_index < size; ++a_index) (*v_fields)[a_index] = (*v_fields)[a_index + 1];
		(*v_fields)[size] = nullptr;
	} else {
		auto fields = new(size + 4) t_structure::t_fields(size + 4);
		for (size_t i = 0; i < a_index; ++i) new(*fields + i) t_svalue((*v_fields)[i]);
		for (size_t i = a_index; i < size; ++i) new(*fields + i) t_svalue((*v_fields)[i + 1]);
		for (size_t i = size; i < size + 4; ++i) new(*fields + i) t_svalue();
		f_new<std::unique_ptr<t_structure::t_fields>>(f_global(), true, std::exchange(v_fields, fields));
	}
}

}
