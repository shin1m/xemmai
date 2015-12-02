system = Module("system"
print = system.out.write_line

tower = @(height)
	a = [
	while height > 0
		a.unshift(height
		height = height - 1
	a

hanoi = @(height, from, via, to, move)
	hanoi(height - 1, from, to, via, move if height > 1
	move(from, to
	hanoi(height - 1, via, from, to, move if height > 1

n = 5
towers = [tower(n), tower(0), tower(0)
print(towers
hanoi(n, towers[0], towers[1], towers[2], @(from, to)
	to.unshift(from.shift(
	print(towers
