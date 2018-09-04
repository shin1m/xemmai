#ifndef QUEUE_H
#define QUEUE_H

#include <xemmai/string.h>

using namespace xemmai;

struct t_container;

struct t_pair
{
	t_slot v_value;
	t_slot v_next;

	t_pair(t_scoped&& a_value) : v_value(std::move(a_value))
	{
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_value);
		a_scan(v_next);
	}
};

class t_queue
{
	mutable t_lock v_lock;
	t_slot v_head;

public:
	t_queue()
	{
		std::printf("t_queue::t_queue()\n");
	}
	t_queue(const t_string& a_message)
	{
		std::printf("t_queue::t_queue(a_message = %ls)\n", a_message.f_wstring().c_str());
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_head);
	}
	std::wstring f_string() const;
	bool f_empty() const;
	void f_push(t_container* a_extension, t_scoped&& a_value);
	t_scoped f_pop();
};

#endif
