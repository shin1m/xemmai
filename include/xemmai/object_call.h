	t_transfer f_call(XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TRANSFER_A_N, XEMMAI__MACRO__N))
	{
		t_scoped_stack stack(XEMMAI__MACRO__N + 1);
		XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__CONSTRUCT_A_N, XEMMAI__MACRO__N)
		f_call_and_return(t_value(), stack, XEMMAI__MACRO__N);
		return stack.f_return();
	}
