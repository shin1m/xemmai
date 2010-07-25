#include <xemmai/object.h>

#include <xemmai/engine.h>
#include <xemmai/class.h>

namespace xemmai
{

XEMMAI__PORTABLE__THREAD t_object* t_object::v_roots;
XEMMAI__PORTABLE__THREAD size_t t_object::v_release;
XEMMAI__PORTABLE__THREAD size_t t_object::v_collect;

void t_object::f_decrement(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_decrement();
	a_slot.v_p = 0;
}

void t_object::f_mark_gray(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_mark_gray();
	--p->v_cyclic;
}

void t_object::f_scan_gray(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_scan_gray();
}

void t_object::f_scan_black(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_scan_black();
}

void t_object::f_collect_white(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	p->f_collect_white();
}

void t_object::f_scan_red(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	if (p->v_color == e_color__RED && p->v_cyclic > 0) --p->v_cyclic;
}

void t_object::f_cyclic_decrement(t_slot& a_slot)
{
	t_object* p = a_slot.v_p;
	if (reinterpret_cast<size_t>(p) < t_value::e_tag__OBJECT) return;
	if (p->v_color != e_color__RED) {
		if (p->v_color == e_color__ORANGE) {
			--p->v_count;
			--p->v_cyclic;
		} else {
			p->f_decrement();
		}
	}
	a_slot.v_p = 0;
}

void t_object::f_collect()
{
	std::vector<t_object*>& cycles = f_engine()->v_object__cycles;
	for (std::vector<t_object*>::reverse_iterator i = cycles.rbegin(); i != cycles.rend(); ++i) {
		portable::t_scoped_lock lock(f_engine()->v_object__reviving__mutex);
		t_object* cycle = *i;
		t_object* p = cycle;
		do {
			p = p->v_next;
			if (p->v_color != e_color__ORANGE || p->v_cyclic > 0 || f_engine()->v_object__reviving && f_as<t_type&>(p->v_type).v_revive) {
				p = 0;
				break;
			}
		} while (p != cycle);
		if (p == cycle) {
			do {
				p = p->v_next;
				p->v_color = e_color__RED;
			} while (p != cycle);
			do {
				p = p->v_next;
				p->f_cyclic_decrement();
			} while (p != cycle);
			do {
				p = cycle->v_next;
				cycle->v_next = p->v_next;
				delete &f_as<t_type&>(p);
				++v_collect;
				t_local_pool<t_object>::f_free(p);
			} while (p != cycle);
		} else {
			p = cycle->v_next;
			t_object* q = p->v_next;
			if (p->v_color == e_color__ORANGE) {
				p->v_color = e_color__PURPLE;
				f_append(v_roots, p);
			} else if (p->v_color == e_color__PURPLE) {
				f_append(v_roots, p);
			} else {
				p->v_color = e_color__BLACK;
				p->v_next = 0;
			}
			while (p != cycle) {
				p = q;
				q = p->v_next;
				if (p->v_color == e_color__PURPLE) {
					f_append(v_roots, p);
				} else {
					p->v_color = e_color__BLACK;
					p->v_next = 0;
				}
			}
		}
	}
	cycles.clear();
	{
		t_library::t_handle*& finalizing = f_engine()->v_library__handle__finalizing;
		while (finalizing) {
			t_library::t_handle* p = finalizing;
			finalizing = p->v_next;
			delete p;
		}
	}
	if (!v_roots) return;
	{
		bool b = ++f_engine()->v_collector__skip >= t_engine::V_COLLECTOR__SKIP;
		if (b) {
			f_engine()->v_collector__skip = 0;
			++f_engine()->v_collector__collect;
		}
		t_object* p = v_roots;
		while (true) {
			t_object* q = p->v_next;
			if (q->v_color == e_color__PURPLE && q->v_count > 0) {
				if (b) q->f_mark_gray();
				if (q == v_roots) break;
				p = q;
			} else {
				p->v_next = q->v_next;
				if (q->v_count <= 0) {
					++v_release;
					t_local_pool<t_object>::f_free(q);
				} else {
					q->v_next = 0;
				}
				if (q == v_roots) {
					if (p == q) p = 0;
					v_roots = p;
					break;
				}
			}
		}
		if (!b) return;
	}
	if (!v_roots) return;
	{
		t_object* p = v_roots;
		do {
			p = p->v_next;
			p->f_scan_gray();
		} while (p != v_roots);
	}
	t_object*& cycle = f_engine()->v_object__cycle;
	while (true) {
		t_object* p = v_roots->v_next;
		v_roots->v_next = p->v_next;
		if (p->v_color == e_color__WHITE) {
			cycle = 0;
			p->f_collect_white();
			cycles.push_back(cycle);
		} else {
			p->v_next = 0;
		}
		if (p == v_roots) break;
	}
	v_roots = 0;
	for (std::vector<t_object*>::const_iterator i = cycles.begin(); i != cycles.end(); ++i) {
		t_object* cycle = *i;
		t_object* p = cycle;
		do {
			p = p->v_next;
			p->v_color = e_color__RED;
			p->v_cyclic = p->v_count;
		} while (p != cycle);
		do {
			p = p->v_next;
			p->v_fields.f_scan(f_scan_red);
			f_as<t_type&>(p->v_type).f_scan(p, f_scan_red);
			f_scan_red(p->v_type);
		} while (p != cycle);
		do {
			p = p->v_next;
			p->v_color = e_color__ORANGE;
		} while (p != cycle);
	}
}

t_object* t_object::f_pool__allocate()
{
	return f_engine()->f_object__pool__allocate();
}

void t_object::f_mark_gray()
{
	if (v_color == e_color__GRAY) return;
	v_color = e_color__GRAY;
	v_cyclic = v_count;
	v_fields.f_scan(f_mark_gray);
	f_as<t_type&>(v_type).f_scan(this, f_mark_gray);
	f_mark_gray(v_type);
}

void t_object::f_scan_gray()
{
	if (v_color == e_color__GRAY && v_cyclic <= 0) {
		v_color = e_color__WHITE;
		v_fields.f_scan(f_scan_gray);
		f_as<t_type&>(v_type).f_scan(this, f_scan_gray);
		f_scan_gray(v_type);
	} else if (v_color != e_color__WHITE) {
		f_scan_black();
	}
}

void t_object::f_collect_white()
{
	if (v_color != e_color__WHITE) return;
	v_color = e_color__ORANGE;
	f_append(f_engine()->v_object__cycle, this);
	v_fields.f_scan(f_collect_white);
	f_as<t_type&>(v_type).f_scan(this, f_collect_white);
	f_collect_white(v_type);
}

void t_object::f_cyclic_decrement()
{
	v_fields.f_scan(f_cyclic_decrement);
	f_as<t_type&>(v_type).f_scan(this, f_cyclic_decrement);
	v_fields.f_finalize();
	if (v_type != f_engine()->v_type_class) {
		f_as<t_type&>(v_type).f_finalize(this);
		v_type.v_pointer = 0;
	}
	f_cyclic_decrement(v_type);
}

#ifndef XEMMAI__PORTABLE__SUPPORTS_THREAD_EXPORT
t_transfer t_object::f_allocate_uninitialized(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	return t_transfer(p, t_transfer::t_pass());
}

t_transfer t_object::f_allocate(t_object* a_type)
{
	t_object* p = t_local_pool<t_object>::f_allocate(f_pool__allocate);
	p->v_next = 0;
	p->v_count = 1;
	p->v_type.f_construct(a_type);
	p->v_pointer = 0;
	return t_transfer(p, t_transfer::t_pass());
}
#endif

void t_object::f_call_and_return(const t_value& a_self, size_t a_n, t_stack& a_stack)
{
	f_call(a_self, a_n, a_stack);
	if (f_context()->v_native <= 0) a_stack.f_return(t_code::f_loop());
}

t_transfer t_object::f_call(size_t a_n, t_slot* a_slots)
{
	std::vector<t_slot> slots(a_n + 1);
	for (size_t i = 0; i < a_n; ++i) slots[i] = a_slots[i];
	t_scoped_stack stack(&slots[0], &slots[0] + a_n + 1);
	f_call_and_return(t_value(), a_n, stack);
	return stack.f_pop();
}

}
