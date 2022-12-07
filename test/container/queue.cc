#include "container.h"

t_object* t_queue::f_string()
{
	return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
	{
		std::vector<wchar_t> cs;
		if (v_head) {
			t_object* pair = v_head->f_as<t_pair>().v_next;
			while (true) {
				if (auto p = f_string_or_null(pair->f_as<t_pair>().v_value)) {
					auto& s = p->f_as<t_string>();
					cs.insert(cs.end(), static_cast<const wchar_t*>(s), s + s.f_size());
				} else {
					auto s = L"<unprintable>"sv;
					cs.insert(cs.end(), s.begin(), s.end());
				}
				cs.push_back(L'\n');
				if (pair == v_head) break;
				pair = pair->f_as<t_pair>().v_next;
			}
		}
		return t_string::f_instantiate(cs.data(), cs.size());
	});
}

void t_queue::f_push(t_container* a_library, const t_pvalue& a_value)
{
	f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		auto pair = t_type_of<t_pair>::f_instantiate(a_library, a_value);
		if (v_head) {
			pair->f_as<t_pair>().v_next = v_head->f_as<t_pair>().v_next;
			v_head->f_as<t_pair>().v_next = pair;
		} else {
			pair->f_as<t_pair>().v_next = pair;
		}
		v_head = pair;
	});
}

t_pvalue t_queue::f_pop()
{
	return f_owned_or_shared<t_lock_with_safe_region>([&]
	{
		if (!v_head) f_throw(L"empty queue."sv);
		auto& pair = v_head->f_as<t_pair>().v_next;
		if (pair == v_head)
			v_head = nullptr;
		else
			v_head->f_as<t_pair>().v_next = pair->f_as<t_pair>().v_next;
		return t_pvalue(pair->f_as<t_pair>().v_value);
	});
}
