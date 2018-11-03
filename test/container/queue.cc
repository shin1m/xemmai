#include "container.h"

t_scoped t_queue::f_string() const
{
	using namespace std::literals;
	t_scoped_lock_for_read lock(v_lock);
	std::vector<wchar_t> cs;
	if (v_head) {
		t_object* pair = f_as<t_pair&>(v_head).v_next;
		while (true) {
			t_scoped p = f_as<t_pair&>(pair).v_value.f_invoke(f_global()->f_symbol_string());
			if (f_is<t_string>(p)) {
				auto& s = f_as<const t_string&>(p);
				cs.insert(cs.end(), static_cast<const wchar_t*>(s), s + s.f_size());
			} else {
				auto s = L"<unprintable>"sv;
				cs.insert(cs.end(), s.data(), s.data() + s.size());
			}
			cs.push_back(L'\n');
			if (pair == static_cast<t_object*>(v_head)) break;
			pair = f_as<t_pair&>(pair).v_next;
		}
	}
	return t_string::f_instantiate(cs.data(), cs.size());
}

bool t_queue::f_empty() const
{
	t_scoped_lock_for_read lock(v_lock);
	return !v_head;
}

void t_queue::f_push(t_container* a_extension, t_scoped&& a_value)
{
	t_scoped_lock_for_write lock(v_lock);
	auto pair = t_type_of<t_pair>::f_instantiate(a_extension, std::move(a_value));
	if (v_head) {
		f_as<t_pair&>(pair).v_next = std::move(f_as<t_pair&>(v_head).v_next);
		f_as<t_pair&>(v_head).v_next = pair;
	} else {
		f_as<t_pair&>(pair).v_next = pair;
	}
	v_head = std::move(pair);
}

t_scoped t_queue::f_pop()
{
	t_scoped_lock_for_write lock(v_lock);
	if (!v_head) f_throw(L"empty queue."sv);
	t_scoped pair = std::move(f_as<t_pair&>(v_head).v_next);
	if (pair == v_head)
		v_head = nullptr;
	else
		f_as<t_pair&>(v_head).v_next = std::move(f_as<t_pair&>(pair).v_next);
	return f_as<t_pair&>(pair).v_value;
}
