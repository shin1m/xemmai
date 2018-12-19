system = Module("system"
print = system.out.write_line

hanoi = @(towers, move)
	step = @(height, from, via, to)
		height > 1 && step(height - 1, from, to, via
		move(from, to
		height > 1 && step(height - 1, via, from, to
	step(towers[0].size(), towers[0], towers[1], towers[2]

towers = '([1, 2, 3, 4, 5], [], []
print(towers
hanoi(towers, @(from, to)
	to.unshift(from.shift(
	print(towers
