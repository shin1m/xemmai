#include "container.h"

t_object* t_queue::f_string() const
{
	using namespace std::literals;
	std::shared_lock lock(v_mutex);
	std::vector<wchar_t> cs;
	if (v_head) {
		t_object* pair = f_as<t_pair&>(v_head).v_next;
		while (true) {
			auto p = f_as<t_pair&>(pair).v_value.f_string();
			if (f_is<t_string>(p)) {
				auto& s = f_as<const t_string&>(p);
				cs.insert(cs.end(), static_cast<const wchar_t*>(s), s + s.f_size());
			} else {
				auto s = L"<unprintable>"sv;
				cs.insert(cs.end(), s.data(), s.data() + s.size());
			}
			cs.push_back(L'\n');
			if (pair == v_head) break;
			pair = f_as<t_pair&>(pair).v_next;
		}
	}
	return t_string::f_instantiate(cs.data(), cs.size());
}

bool t_queue::f_empty() const
{
	std::shared_lock lock(v_mutex);
	return !v_head;
}

void t_queue::f_push(t_container* a_library, const t_pvalue& a_value)
{
	std::lock_guard lock(v_mutex);
	auto pair = t_type_of<t_pair>::f_instantiate(a_library, a_value);
	if (v_head) {
		f_as<t_pair&>(pair).v_next = f_as<t_pair&>(v_head).v_next;
		f_as<t_pair&>(v_head).v_next = pair;
	} else {
		f_as<t_pair&>(pair).v_next = pair;
	}
	v_head = pair;
}

t_pvalue t_queue::f_pop()
{
	std::lock_guard lock(v_mutex);
	if (!v_head) f_throw(L"empty queue."sv);
	auto& pair = f_as<t_pair&>(v_head).v_next;
	if (pair == v_head)
		v_head = nullptr;
	else
		f_as<t_pair&>(v_head).v_next = f_as<t_pair&>(pair).v_next;
	return f_as<t_pair&>(pair).v_value;
}
