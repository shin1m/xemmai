#ifndef QUEUE_H
#define QUEUE_H

#include <shared_mutex>
#include <xemmai/string.h>

using namespace xemmai;

struct t_container;

struct t_pair
{
	t_svalue v_value;
	t_slot v_next;

	t_pair(const t_pvalue& a_value) : v_value(a_value)
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
	mutable std::shared_mutex v_mutex;
	t_slot v_head;

public:
	t_queue()
	{
		std::printf("t_queue::t_queue()\n");
	}
	t_queue(std::wstring_view a_message)
	{
		std::printf("t_queue::t_queue(a_message = %.*ls)\n", static_cast<int>(a_message.size()), a_message.data());
	}
	void f_scan(t_scan a_scan)
	{
		a_scan(v_head);
	}
	t_object* f_string() const;
	bool f_empty() const;
	void f_push(t_container* a_library, const t_pvalue& a_value);
	t_pvalue f_pop();
};

#endif
