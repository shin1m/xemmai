system = Module("system");
print = system.out.write_line;

assert_bytes = @(xs, ys) {
	if (xs.size() != ys.size()) throw Throwable("must be same size.");
	for (i = 0; i < xs.size(); i = i + 1)
		if (xs[i] != ys[i])
			throw Throwable("xs[" + i + "] must be " + ys[i] + ".");
};

bytes = Bytes(16);
for (i = 0; i < bytes.size(); i = i + 1) bytes[i] = i * 17;
print(bytes);
assert_bytes(bytes, [0, 17, 2 * 17, 3 * 17, 4 * 17, 5 * 17, 6 * 17, 7 * 17, 8 * 17, 9 * 17, 10 * 17, 11 * 17, 12 * 17, 13 * 17, 14 * 17, 15 * 17]);

Foo = Class(Bytes) :: @{
	$__construct = @(*xs) :$^__construct[$](xs.size());
	$__initialize = @(*xs) {
		for (i = 0; i < xs.size(); i = i + 1) $[i] = xs[i];
	};
};

foo = Foo(0, 1, 2, 3);
print(foo);
assert_bytes(foo, [0, 1, 2, 3]);
