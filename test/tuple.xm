system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

t = '(
print(t
assert(t.size() == 0
assert(t != ""
assert(t < '(null
assert(t <= '(
assert('(null) > t
assert(t >= '(
assert(t == '(
assert(t != '(null
assert(t == Tuple(

t = '("one", 2, 'three, 4.0
print(t
assert(t.size() == 4
assert(t[0] == "one"
assert(t[1] == 2
assert(t[2] == 'three
assert(t[3] == 4.0
try
	t[4] == null
	assert(false
catch Throwable e
	print(e
	e.dump(
	assert(true
assert(t < '("one", 2, 'three, 4.0, 5
assert(t < '("one", 3
assert(t <= '("one", 2, 'three, 4.0
assert(t <= '("one", 2, 'three, 5.0
assert(t > '("one", 2, 'three
assert(t > '("one", 2, 'three, 3.0, 5
assert(t >= '("one", 2, 'three, 4.0
assert(t >= '("on"
assert(t == '("one", 2, 'three, 4.0
assert(t != '("one", 2, 'three
assert(t != '("one", 2, 'four, 4.0
assert(t == Tuple("one", 2, 'three, 4.0
s = ""
t.each(@(x) :s = s + " " + x
print(s
assert(s == " one 2 three 4"
