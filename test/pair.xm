Pair = Object + @
	$first
	$second
	$__initialize = @(first, second)
		$first = first
		$second = second
	$__string = @ $second ? $first.__string() + " " + $second : $first.__string()

List = Object + @
	$list
	$__initialize = @ $list = null
	$__string = @ $list ? "(" + $list + ")" : "()"
	$push = @(value) $list = Pair(value, $list
	$pop = @
		value = $list.first
		$list = $list.second
		value
$List = List
