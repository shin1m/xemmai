system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

if true
	a =
		1 +
2
	a = a + 3
assert(a == 6

a = (Object + @
	$
		.f = @(x)@(y) x + y
)(
assert(a.f(1)(2) == 3
assert(a
	.f
		(1
		(2) == 3
assert(3 ==
a
	.f
		(1
		(2
assert("c" == "abc"
	.substring(1
	.substring(1
assert("ef" ==
	"abcdef"
		.substring(a
			.f
				(1
				(2
		.substring(1

a = [["foo"
assert("foo" == a
	[0
	[0

f = @(
	x,
y
	z
) x + y + z
assert(f(1
	2,
3
) == 6
a = f(
	1,
2
	3
assert(a == 6
a = f(1
, 2
	3
assert(a == 6
a = f(1
	2
, 3
assert(a == 6

a = {1: "one"
	"two": 2,
3: "three"
}
assert(a[1] == "one"
a = {
	1: "one",
"two": 2
	3: "three"
assert(a["two"] == 2
a = {1: "one"
, "two": 2
	3: "three"
assert(a[3] == "three"
a = {1: "one"
	"two": 2
, 3: "three"
assert(a[3] == "three"

a = {
	'key: 'value
	"string key": "This is a string"
	'list: [
		'one
		"two"
		3
	'map: {
		'one: "one"
		'two: 2
		'three: 3.0
assert(a['list][1] == "two"
