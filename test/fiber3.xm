system = Module("system"
print = system.out.write_line
assert = @(x) if !x: throw Throwable("Assertion failed."

f0 = Fiber(@(co)
	co("Hello everyone,"
	co("This is shin."
	"Good bye."

foo = Object(
foo.__string = @
	f0(Fiber.current(

a = "" + foo
print(a
assert(a == "Hello everyone,"
a = "" + foo
print(a
assert(a == "This is shin."
a = "" + foo
print(a
assert(a == "Good bye."
a = "" + foo
print(a
assert(a == "Hello everyone,"
a = "" + foo
print(a
assert(a == "This is shin."
a = "" + foo
print(a
assert(a == "Good bye."
a = "" + foo
print(a
assert(a == "Hello everyone,"
