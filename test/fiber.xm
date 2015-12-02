system = Module("system"
print = system.out.write_line
assert = @(x)
	throw Throwable("Assertion failed." if !x

f0 = f1 = f2 = null
fm = Fiber.current(

f0 = Fiber(@(x)
	print("f0: " + x
	assert(x == 0
	x = f1(x + 1
	print("f0: " + x
	assert(x == 3
	fm(x + 1

f1 = Fiber(@(x)
	print("f1: " + x
	assert(x == 1
	f2(x + 1

f2 = Fiber(@(x)
	print("f2: " + x
	assert(x == 2
	f0(x + 1

a = f0(0)
print("fm: " + a
assert(a == 4
