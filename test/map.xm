system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

d = {"one": 1, 2: "two"
print(d
assert(d.size() == 2
assert(d.has("one") && d["one"] == 1
assert(d.has(2) && d[2] == "two"

d["three"] = 3.0
print(d
assert(d.size() == 3
assert(d.has("three") && d["three"] == 3.0

d[4.0] = "four"
print(d
assert(d.size() == 4
assert(d.has(4.0) && d[4.0] == "four"

d.remove("one"
print(d
assert(d.size() == 3
assert(!d.has("one")

d.remove(2
print(d
assert(d.size() == 2
assert(!d.has(2)

d.clear(
print(d
assert(d.size() == 0

d = {
for i = 0; i < 1000; i = i + 1: d[i] = i
assert(d.size() == 1000
for i = 0; i < 1000; i = i + 1: assert(d[i] == i

Foo = Map + @
	$__initialize = @(*xs) $[3] = "three"

foo = Foo(1, "one", "two", 2
print(foo
assert(foo.size() == 3
assert(foo[1] == "one"
assert(foo["two"] == 2
assert(foo[3] == "three"

{null: ""
print({null: "", null: ""
