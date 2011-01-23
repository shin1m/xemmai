system = Module("system");
print = system.out.write_line;
assert = @(x) if (!x) throw Throwable("Assertion failed.");;

variadic = @(x, y, *z) {
	print(x);
	assert(x == "first");
	print(y);
	assert(y == "second");
	print(z);
	assert(z == '("third", "fourth", "fifth"));
};
variadic("first", "second", "third", "fourth", "fifth");

variadic = @(x, y, *z) {
	print(x);
	assert(x == "first");
	print(y);
	assert(y == "second");
	print(z);
	assert(z == '());
};
variadic("first", "second");

variadic = @(*x) {
	print(x);
	assert(x == '("first", "second", "third", "fourth", "fifth"));
};
variadic("first", "second", "third", "fourth", "fifth");

variadic = @(*x) {
	print(x);
	assert(x == '());
};
variadic();

f = @(x, y, z) {
	print(x);
	assert(x == "first");
	print(y);
	assert(y == "second");
	print(z);
	assert(z == "third");
};
tuple = '("first", "second", "third");
f(*tuple);
