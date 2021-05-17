system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

d = {
	10: "10"
	0: "0"
	21: "21"
print(d
assert(d.size() == 3
assert(d.__string() == "{21: 21, 0: 0, 10: 10}"
d[11] = "11"
print(d
assert(d.size() == 4
assert(d.__string() == "{21: 21, 0: 0, 11: 11, 10: 10}"
d[32] = "32"
print(d
assert(d.size() == 5
assert(d.__string() == "{21: 21, 32: 32, 11: 11, 0: 0, 10: 10}"

d.remove(10
print(d
assert(d.size() == 4
assert(d.__string() == "{21: 21, 0: 0, 11: 11, 32: 32}"
d.remove(32
print(d
assert(d.size() == 3
assert(d.__string() == "{11: 11, 0: 0, 21: 21}"
d.remove(21
print(d
assert(d.size() == 2
assert(d.__string() == "{11: 11, 0: 0}"
d.remove(11
print(d
assert(d.size() == 1
assert(d.__string() == "{0: 0}"
d.remove(0
print(d
assert(d.size() == 0
assert(d.__string() == "{}"
