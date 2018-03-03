system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

fm = Fiber.current(

f0 = Fiber(@(x)
	x = fm("Hello, " + x + "!!\n"
	x = fm(x + "Good!!\n"
	fm("Good bye, " + x + ".\n"

a = (@ f0(f0("foo") + "How are you?\n"))(
print(a
assert(a == "Hello, foo!!
How are you?
Good!!
"

a = (@ f0(f0("bar") + "Bye.\n"))(
print(a
assert(a == "Good bye, bar.
Bye.
"

f1 = Fiber(@(x)
	x = fm("Hello, " + x + "!!\n"
	throw Throwable(x + "Oops, sorry.\n"

try
	print((@
		f1(f1("zot") + "How are you?\n"
	)(
	throw Throwable("never reach here."
catch Throwable e
	print("caught: " + e
	e.dump(

f2 = Fiber(@(x)
	throw Throwable(x + "Oops, sorry.\n"

try
	f2("How are you?\n"
	throw Throwable("never reach here."
catch Throwable e
	print("caught: " + e
	e.dump(
