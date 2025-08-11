system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

assert('(*'()) == '(
assert('(*'(), *'()) == '(
assert('("a", *'(), "b", *'(), "c") == '("a", "b", "c"
assert('("a", *'("b", "c"), "d") == '("a", "b", "c", "d"
assert('(*'("a", "b"), *'("c", "d")) == '("a", "b", "c", "d"
assert((@ '(*'("a", "b"), *'("c", "d")))() == '("a", "b", "c", "d"

assert('(*["a", "b"], *["c", "d"]) == '("a", "b", "c", "d"
assert('(*
	[*["a", "b"], *["c", "d"]]
) == '("a", "b", "c", "d"
assert((@(*xs) xs)(*'("a", "b"), *'("c", "d")) == '("a", "b", "c", "d"

assert('(*(Object + @
	$size = @ 3
	$__get_at = @(i) "" + i
)()) == '("0", "1", "2"

try
	'(*'("a", "b"), *null, *'("c", "d"
	assert(false
catch Throwable t
	print(t
	t.dump(
	assert(true
