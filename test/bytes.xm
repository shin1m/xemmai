system = Module("system"
print = system.out.write_line

assert_bytes = @(xs, ys)
	throw Throwable("must be same size." if xs.size() != ys.size()
	throw Throwable("xs[" + i + "] must be " + ys[i] + "." if xs[i] != ys[i] for i = 0; i < xs.size(); i = i + 1

bytes = Bytes(16
bytes[i] = i * 17 for i = 0; i < bytes.size(); i = i + 1
print(bytes
assert_bytes(bytes, [0, 17, 2 * 17, 3 * 17, 4 * 17, 5 * 17, 6 * 17, 7 * 17, 8 * 17, 9 * 17, 10 * 17, 11 * 17, 12 * 17, 13 * 17, 14 * 17, 15 * 17]

Foo = Class(Bytes) :: @
	$__construct = @(*xs) :$^__construct[$](xs.size(
	$__initialize = @(*xs)
		$[i] = xs[i] for i = 0; i < xs.size(); i = i + 1

foo = Foo(0, 1, 2, 3
print(foo
assert_bytes(foo, [0, 1, 2, 3]
