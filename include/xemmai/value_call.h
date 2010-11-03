inline t_transfer t_value::operator()(XEMMAI__MACRO__JOIN(XEMMAI__MACRO__TRANSFER_A_N, XEMMAI__MACRO__N)) const
{
	t_scoped_stack stack(XEMMAI__MACRO__N + 1);
	stack->f_push();
	XEMMAI__MACRO__REPEAT(XEMMAI__MACRO__PUSH_A_N, XEMMAI__MACRO__N)
	f_call_and_return(t_value(), XEMMAI__MACRO__N);
	return stack->f_pop();
}
