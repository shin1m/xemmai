	t_transfer f_call(XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TRANSFER_A_N, XEMMAI__MACRO__N))
	{
		t_slot slots[] = {XEMMAI__MACRO__REVERSE_REPEAT(XEMMAI__MACRO__A_N_COMMA, XEMMAI__MACRO__N) t_slot()};
		t_scoped_stack stack(slots, slots + XEMMAI__MACRO__N + 1);
		f_call_and_return(t_value(), XEMMAI__MACRO__N, stack);
		return stack.f_pop();
	}
