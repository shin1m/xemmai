Pair = Class() :: @
	$__initialize = @(first, second)
		$first = first
		$second = second
	$__string = @ $second === null ? $first.__string() : $first.__string() + " " + $second

List = Class() :: @
	$__initialize = @ $list = null
	$__string = @ $list === null ? "()" : "(" + $list + ")"
	$push = @(value) $list = Pair(value, $list
	$pop = @
		value = $list.first
		$list = $list.second
		value
$List = List
