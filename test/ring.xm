system = Module("system");
print = system.out.write_line;

Cell = Class() :: @{
	$__initialize = @(a_value) {
		$v_value = a_value;
	};
};

Ring = Class() :: @{
	$__initialize = @{
		$v_ring = null;
	};
	$__string = @{
		if ($v_ring === null) {
			return "()";
		} else {
			return "(" + $string($v_ring.v_next) + ")";
		}
	};
	$string = @(a_cell) {
		if (a_cell == $v_ring) return a_cell.v_value.__string();
		return a_cell.v_value.__string() + " " + $string(a_cell.v_next);
	};
	$push = @(a_value) {
		cell = Cell(a_value);
		if ($v_ring === null) {
			cell.v_next = cell;
		} else {
			cell.v_next = $v_ring.v_next;
			$v_ring.v_next = cell;
		}
		$v_ring = cell;
	};
	$pop = @{
		cell = $v_ring.v_next;
		$v_ring.v_next = cell.v_next;
		return cell.v_value;
	};
};

ring = @(a_n) {
	if (a_n > 0) {
		r = ring(a_n - 1);
		r.push(a_n);
		return r;
	} else {
		return Ring();
	}
};

rings = @(a_n) {
	if (a_n > 0) rings(a_n - 1);
	print(ring(a_n));
};

rings(10);
