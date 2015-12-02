system = Module("system"
list = Module("list"
print = system.out.write_line

Tower = Class(list.List) :: @
	$__initialize = @(height)
		:$^__initialize[$](
		$build(height
	$build = @(height)
		if height > 0
			$push(height
			$build(height - 1

Towers = Class() :: @
	$__initialize = @(x, y, z)
		$x = x
		$y = y
		$z = z
	$__string = @() "(" + $x + " " + $y + " " + $z + ")"

hanoi = @(height, from, via, to, move)
	hanoi(height - 1, from, to, via, move if height > 1
	move(from, to
	hanoi(height - 1, via, from, to, move if height > 1

n = 5
towers = Towers(Tower(n), Tower(0), Tower(0)
print(towers
hanoi(n, towers.x, towers.y, towers.z, @(from, to)
	to.push(from.pop(
	print(towers
