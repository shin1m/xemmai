system = Module("system"
pair = Module("pair"
print = system.out.write_line

Tower = pair.List + @
	$__initialize = @(height)
		pair.List.__initialize[$](
		$build(height
	$build = @(height)
		if height > 0
			$push(height
			$build(height - 1

Towers = Object + @
	$x
	$y
	$z
	$__initialize = @(x, y, z)
		$x = x
		$y = y
		$z = z
	$__string = @ "(" + $x + " " + $y + " " + $z + ")"

hanoi = @(height, from, via, to, move)
	height > 1 && hanoi(height - 1, from, to, via, move
	move(from, to
	height > 1 && hanoi(height - 1, via, from, to, move

n = 5
towers = Towers(Tower(n), Tower(0), Tower(0)
print(towers
hanoi(n, towers.x, towers.y, towers.z, @(from, to)
	to.push(from.pop(
	print(towers
