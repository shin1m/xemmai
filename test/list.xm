Pair = Class() :: @{
	$__initialize = @(a_first, a_second) {
		$v_first = a_first;
		$v_second = a_second;
	};
	$__string = @{
		$v_second === null ? $v_first.__string() : $v_first.__string() + " " + $v_second;
	};
};

List = Class() :: @{
	$__initialize = @{
		$v_list = null;
	};
	$__string = @{
		$v_list === null ? "()" : "(" + $v_list + ")";
	};
	$push = @(a_value) {
		$v_list = Pair(a_value, $v_list);
	};
	$pop = @{
		value = $v_list.v_first;
		$v_list = $v_list.v_second;
		value;
	};
};
$List = List;
