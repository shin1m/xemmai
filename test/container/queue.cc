#include "container.h"

std::wstring t_queue::f_string() const
{
	t_scoped_lock_for_read lock(v_lock);
	std::wstring s;
	if (v_head) {
		t_object* pair = f_as<t_pair&>(v_head).v_next;
		while (true) {
			t_scoped p = f_as<t_pair&>(pair).v_value.f_invoke(f_global()->f_symbol_string());
			s += f_is<t_string>(p) ? f_as<std::wstring>(p) : L"<unprintable>";
			s += L'\n';
			if (pair == static_cast<t_object*>(v_head)) break;
			pair = f_as<t_pair&>(pair).v_next;
		}
	}
	return s;
}

bool t_queue::f_empty() const
{
	t_scoped_lock_for_read lock(v_lock);
	return !v_head;
}

void t_queue::f_push(t_container* a_extension, t_scoped&& a_value)
{
	t_scoped_lock_for_write lock(v_lock);
	t_scoped pair = t_type_of<t_pair>::f_instantiate(a_extension, std::move(a_value));
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
	if (!v_head) f_throw(L"empty queue.");
	t_scoped pair = std::move(f_as<t_pair&>(v_head).v_next);
	if (pair == v_head)
		v_head = nullptr;
	else
		f_as<t_pair&>(v_head).v_next = std::move(f_as<t_pair&>(pair).v_next);
	return f_as<t_pair&>(pair).v_value;
}
