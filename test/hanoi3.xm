system = Module("system"
print = system.out.write_line

tower = @(height)
	a = [
	while height > 0
		a.unshift(height
		height = height - 1
	a

hanoi = @(height, from, via, to, move)
	height > 1 && hanoi(height - 1, from, to, via, move
	move(from, to
	height > 1 && hanoi(height - 1, via, from, to, move

do_hanoi = @(n, move)
	towers = [tower(n), tower(0), tower(0)
	print(towers
	hanoi(n, towers[0], towers[1], towers[2], @(from, to)
		move(from, to
		print(towers
Thread(@
	do_hanoi(3, @(from, to)
		to.unshift(from.shift(
do_hanoi(2, @(from, to)
	to.unshift(from.shift(
