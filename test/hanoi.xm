list = Module("list");

Tower = Class(list.List) :: @{
	$__initialize = @(a_height) {
		:$^__initialize[$]();
		$build(a_height);
	};
	$build = @(a_height) {
		if (a_height > 0) {
			$push(a_height);
			$build(a_height - 1);
		}
	};
};

Towers = Class() :: @{
	$__initialize = @(a_x, a_y, a_z) {
		$v_x = a_x;
		$v_y = a_y;
		$v_z = a_z;
	};
	$__string = @{
		"(" + $v_x + " " + $v_y + " " + $v_z + ")";
	};
};

hanoi = @(a_height, a_from, a_via, a_to, a_move) {
	if (a_height > 1) hanoi(a_height - 1, a_from, a_to, a_via, a_move);
	a_move(a_from, a_to);
	if (a_height > 1) hanoi(a_height - 1, a_via, a_from, a_to, a_move);
};

n = 5;
towers = Towers(Tower(n), Tower(0), Tower(0));
print(towers);
print("\n");
hanoi(n, towers.v_x, towers.v_y, towers.v_z, @(a_from, a_to) {
	a_to.push(a_from.pop());
	print(towers);
	print("\n");
});
