#ifndef QUEUE_H
#define QUEUE_H

#include <xemmai/engine.h>
#include <xemmai/sharable.h>

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

class t_queue : public t_sharable
{
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
	t_object* f_string();
	bool f_empty()
	{
		return f_owned_or_shared<t_shared_lock_with_safe_region>([&]
		{
			return !v_head;
		});
	}
	void f_push(t_container* a_library, const t_pvalue& a_value);
	t_pvalue f_pop();
};

#endif
