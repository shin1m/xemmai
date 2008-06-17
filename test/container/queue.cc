#include "container.h"

std::wstring t_queue::f_string() const
{
	portable::t_scoped_lock_for_read lock(v_lock);
	std::wstring s;
	if (v_head) {
		t_object* pair = f_as<t_pair*>(v_head)->v_next;
		while (true) {
			t_scoped p = f_as<t_pair*>(pair)->v_value->f_get(f_global()->f_symbol_string())->f_call();
			s += f_is<std::wstring>(p) ? f_as<const std::wstring&>(p) : L"<unprintable>";
			s += L'\n';
			if (pair == v_head) break;
			pair = f_as<t_pair*>(pair)->v_next;
		}
	}
	return s;
}

bool t_queue::f_empty() const
{
	portable::t_scoped_lock_for_read lock(v_lock);
	return !v_head;
}

void t_queue::f_push(t_container* a_extension, const t_transfer& a_value)
{
	portable::t_scoped_lock_for_write lock(v_lock);
	t_scoped pair = t_type_of<t_pair>::f_instantiate(a_extension, a_value);
	if (v_head) {
		f_as<t_pair*>(pair)->v_next = f_as<t_pair*>(v_head)->v_next.f_transfer();
		f_as<t_pair*>(v_head)->v_next = pair;
	} else {
		f_as<t_pair*>(pair)->v_next = pair;
	}
	v_head = pair.f_transfer();
}

t_transfer t_queue::f_pop()
{
	portable::t_scoped_lock_for_write lock(v_lock);
	if (!v_head) t_throwable::f_throw(L"empty queue.");
	t_transfer pair = f_as<t_pair*>(v_head)->v_next;
	if (pair == v_head)
		v_head = 0;
	else
		f_as<t_pair*>(v_head)->v_next = f_as<t_pair*>(pair)->v_next;
	return f_as<t_pair*>(pair)->v_value.f_transfer();
}
