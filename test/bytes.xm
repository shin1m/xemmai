system = Module("system");
print = system.out.write_line;

assert_bytes = @(xs, ys) {
	if (xs.size() != ys.size()) throw Throwable("must be same size.");
	i = 0;
	while (i < xs.size()) {
		if (xs[i] != ys[i]) throw Throwable("xs[" + i + "] must be " + ys[i] + ".");
		i = i + 1;
	}
};

bytes = Bytes(16);
i = 0;
while (i < bytes.size()) {
	bytes[i] = i * 17;
	i = i + 1;
}
print(bytes);
assert_bytes(bytes, [0, 17, 2 * 17, 3 * 17, 4 * 17, 5 * 17, 6 * 17, 7 * 17, 8 * 17, 9 * 17, 10 * 17, 11 * 17, 12 * 17, 13 * 17, 14 * 17, 15 * 17]);

Foo = Class(Bytes) :: @{
	$__construct = @(*xs) :$^__construct[$](xs.size());
	$__initialize = @(*xs) {
		i = 0;
		while (i < xs.size()) {
			$[i] = xs[i];
			i = i + 1;
		}
	};
};

foo = Foo(0, 1, 2, 3);
print(foo);
assert_bytes(foo, [0, 1, 2, 3]);
