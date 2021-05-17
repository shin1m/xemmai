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
	.f = @(x) x * x
)(
assert(a.f(2) == 4
assert(a
.f(2) == 4

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
