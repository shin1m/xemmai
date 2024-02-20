system = Module("system"
print = system.out.write_line

Cell = Object + @
	$value
	$next
	$__initialize = @(value) $value = value

Ring = Object + @
	$ring
	$__initialize = @
		$ring = null
	$__string = @
		if $ring
			return "(" + $string($ring.next) + ")"
		else
			return "()"
	$string = @(cell)
		cell === $ring && return cell.value.__string(
		return cell.value.__string() + " " + $string(cell.next)
	$push = @(value)
		cell = Cell(value
		if $ring
			cell.next = $ring.next
			$ring.next = cell
		else
			cell.next = cell
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
	n > 0 && rings(n - 1
	print(ring(n

rings(10
