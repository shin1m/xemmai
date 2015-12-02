system = Module("system"
print = system.out.write_line

Cell = Class() :: @
	$__initialize = @(value) $value = value

Ring = Class() :: @
	$__initialize = @
		$ring = null
	$__string = @
		if $ring === null
			return "()"
		else
			return "(" + $string($ring.next) + ")"
	$string = @(cell)
		return cell.value.__string() if cell == $ring
		return cell.value.__string() + " " + $string(cell.next)
	$push = @(value)
		cell = Cell(value
		if $ring === null
			cell.next = cell
		else
			cell.next = $ring.next
			$ring.next = cell
		$ring = cell
	$pop = @
		cell = $ring.next
		$ring.next = cell.next
		return cell.value

ring = @(n)
	if n > 0
		r = ring(n - 1
		r.push(n
		return r
	else
		return Ring(

rings = @(n)
	rings(n - 1 if n > 0
	print(ring(n

rings(10
