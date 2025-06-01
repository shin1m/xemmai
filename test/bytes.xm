system = Module("system"
print = system.out.write_line

assert_sequence = @(xs, ys)
	xs.size() == ys.size() || throw Throwable("xs.size() must be " + ys.size() + " but was " + xs.size() + "."
	for i = 0; i < xs.size(); i = i + 1
		xs[i] == ys[i] || throw Throwable("xs[" + i + "] must be " + ys[i] + " but was " + xs[i] + "."

bytes = Bytes(16
for i = 0; i < bytes.size(); i = i + 1; bytes[i] = i * 17
print(bytes
assert_sequence(bytes, [0, 17, 2 * 17, 3 * 17, 4 * 17, 5 * 17, 6 * 17, 7 * 17, 8 * 17, 9 * 17, 10 * 17, 11 * 17, 12 * 17, 13 * 17, 14 * 17, 15 * 17]
