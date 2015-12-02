system = Module("system"
print = system.out.write_line
assert = @(x)
	throw Throwable("Assertion failed." if !x

print("variadic function"

variadic = @(x, y, *z)
	print(x
	assert(x == "first"
	print(y
	assert(y == "second"
	print(z
	assert(z == '("third", "fourth", "fifth")
try
	variadic("first"
	assert(false
catch Throwable t
	print(t
	t.dump(
	assert(true
variadic("first", "second", "third", "fourth", "fifth"

variadic = @(x, y, *z)
	print(x
	assert(x == "first"
	print(y
	assert(y == "second"
	print(z
	assert(z == '()
variadic("first", "second"

variadic = @(*x)
	print(x
	assert(x == '("first", "second", "third", "fourth", "fifth")
variadic("first", "second", "third", "fourth", "fifth"

variadic = @(*x)
	print(x
	assert(x == '()
variadic(

f = @(x, y, z)
	print(x
	assert(x == "first"
	print(y
	assert(y == "second"
	print(z
	assert(z == "third"
tuple = '("first", "second", "third"
f(*tuple

print("default arguments"

variadic = @(x, y = "y", z = "z")
	print(x
	assert(x == "first"
	print(y
	assert(y == "y"
	print(z
	assert(z == "z"
try
	variadic(
	assert(false
catch Throwable t
	print(t
	t.dump(
	assert(true
variadic("first"

variadic = @(x, y = "y", z = "z")
	print(x
	assert(x == "first"
	print(y
	assert(y == "second"
	print(z
	assert(z == "z"
variadic("first", "second"

variadic = @(x, y = "y", z = "z")
	print(x
	assert(x == "first"
	print(y
	assert(y == "second"
	print(z
	assert(z == "third"
variadic("first", "second", "third"
try
	variadic("first", "second", "third", "fourth"
	assert(false
catch Throwable t
	print(t
	t.dump(
	assert(true

print("default arguments and variadic function"

variadic = @(x, y = "y", z = "z", *w)
	print(x
	assert(x == "first"
	print(y
	assert(y == "y"
	print(z
	assert(z == "z"
	print(w
	assert(w == '()
variadic("first"

variadic = @(x, y = "y", z = "z", *w)
	print(x
	assert(x == "first"
	print(y
	assert(y == "second"
	print(z
	assert(z == "third"
	print(w
	assert(w == '("fourth", "fifth")
variadic("first", "second", "third", "fourth", "fifth"
