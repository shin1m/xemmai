system = Module("system"
print = system.out.write_line
print_int32 = @(x) system.out.write(x < 0x80000000 ? x : ~0x7FFFFFFF | x
print_int32s = @(xs)
	system.out.write("["
	if xs.size() > 0
		print_int32(xs[0]
		for i = 1; i < xs.size(); i = i + 1
			system.out.write(", "
			print_int32(xs[i]
	print("]"

ascii2i = {
(@
	asciis = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
	for i = 0; i < asciis.size(); i = i + 1: ascii2i[asciis.code_at(i)] = i
)(

base32_decode = @(source)
	bs = [
	c = 0
	j = 8
	for i = 0; i < source.size(); i = i + 1
		try
			d = ascii2i[source.code_at(i)]
			if j < 5
				bs.push(c << j | d >> 5 - j
				c = d & (1 << 5 - j) - 1
				j = j + 3
			else
				c = (c << 5) + d
				j = j - 5
		catch Throwable e
	j < 8 && bs.push(c << j
	bs

sha1 = @
	rotate_left = @(x, b) x << b | (x & 0xFFFFFFFF) >> 32 - b
	V = 0x67452301
	W = 0xEFCDAB89
	X = 0x98BADCFE
	Y = 0x10325476
	Z = 0xC3D2E1F0
	xs = [
	step = @
		a = V
		b = W
		c = X
		d = Y
		e = Z
		I = @(f, k, x)
			t = rotate_left(a, 5) + f + e + k + xs[x]
			:e = d
			:d = c
			:c = rotate_left(b, 30)
			:b = a
			:a = t
		for i = 13; i < 77; i = i + 1: xs.push(rotate_left(xs[i] ^ xs[i - 5] ^ xs[i - 11] ^ xs[i - 13], 1
		for i = 0; i < 20; i = i + 1: I(b & c | ~b & d, 0x5A827999, i
		for ; i < 40; i = i + 1: I(b ^ c ^ d, 0x6ED9EBA1, i
		for ; i < 60; i = i + 1: I(b & c | b & d | c & d, 0x8F1BBCDC, i
		for ; i < 80; i = i + 1: I(b ^ c ^ d, 0xCA62C1D6, i
		:V = V + a
		:W = W + b
		:X = X + c
		:Y = Y + d
		:Z = Z + e
		:xs = [
	x = 0
	n = 0
	put = @(b)
		:x = (x << 8) + b
		:n = n + 1
		n < 4 && return
		xs.push(x
		xs.size() >= 16 && step(
		:x = 0
		:n = 0
	length = 0
	self = Object(
	self.byte = @(b)
		put(b
		:length = length + 1
		self
	self.bytes = @(bs)
		for i = 0; i < bs.size(); i = i + 1: put(bs[i]
		:length = length + bs.size()
		self
	self.done = @
		put(0x80
		while n > 0: put(0
		while xs.size() != 14: put(0
		xs.push(0
		xs.push(length * 8
		step(
		[V & 0xFFFFFFFF, W & 0xFFFFFFFF, X & 0xFFFFFFFF, Y & 0xFFFFFFFF, Z & 0xFFFFFFFF
	self

hmac = @(hash, key, text)
	key.size() > 64 && (key = hash().bytes(key).done())
	h0 = hash(
	h1 = hash(
	for i = 0; i < key.size(); i = i + 1
		h0.byte(key[i] ^ 0x36
		h1.byte(key[i] ^ 0x5C
	for ; i < 64; i = i + 1
		h0.byte(0x36
		h1.byte(0x5C
	#h1.byte(key[i] for ; i < key.size(); i = i + 1
	#n = (key.size() + 3) / 4 * 4
	#h1.byte(0 for ; i < n; i = i + 1
	h = h0.bytes(text).done(
	for i = 0; i < h.size(); i = i + 1: h1.byte(h[i] >> 24 & 0xFF).byte(h[i] >> 16 & 0xFF).byte(h[i] >> 8 & 0xFF).byte(h[i] & 0xFF
	h1.done(

hotp = @(key, c)
	s = hmac(sha1, key, [0, 0, 0, 0, c >> 24 & 0xFF, c >> 16 & 0xFF, c >> 8 & 0xFF, c & 0xFF]
	o = s[4] & 0xF
	ox = o >> 2
	ob = o & 3
	(s[ox] << 8 * ob | (ob != 0 ? s[ox + 1] >> 8 * (4 - ob) : 0)) & 0x7FFFFFFF

assert = @(x) x || throw Throwable("Assertion failed."
assert_sequence = @(xs, ys)
	xs.size() == ys.size() || throw Throwable("xs.size() must be " + ys.size() + " but was " + xs.size() + "."
	for i = 0; i < xs.size(); i = i + 1
		xs[i] == ys[i] || throw Throwable("xs[" + i + "] must be " + ys[i] + " but was " + xs[i] + "."

test_base32_decode = @(input, expected)
	x = base32_decode(input
	#print_int32s(x
	#print_int32s(expected
	assert_sequence(x, expected
test_base32_decode("", [
test_base32_decode("MY======", [0x66, 0x00
test_base32_decode("MZXQ====", [0x66, 0x6F, 0x00
test_base32_decode("MZXW6===", [0x66, 0x6F, 0x6F, 0x00
test_base32_decode("MZXW6YQ=", [0x66, 0x6F, 0x6F, 0x62, 0x00
test_base32_decode("MZXW6YTB", [0x66, 0x6F, 0x6F, 0x62, 0x61
test_base32_decode("QZXW6YUBOI======", [0x86, 0x6F, 0x6F, 0x62, 0x81, 0x72, 0x00

test_sha1 = @(input, expected)
	xs = [
	for i = 0; i < input.size(); i = i + 1: xs.push(input.code_at(i
	x = sha1().bytes(xs).done(
	#print_int32s(x
	#print_int32s(expected
	assert_sequence(x, expected
test_sha1("", [0xda39a3ee, 0x5e6b4b0d, 0x3255bfef, 0x95601890, 0xafd80709
test_sha1("The quick brown fox jumps over the lazy dog", [0x2fd4e1c6, 0x7a2d28fc, 0xed849ee1, 0xbb76e739, 0x1b93eb12
test_sha1("The quick brown fox jumps over the lazy cog", [0xde9f2c7f, 0xd25e1b3a, 0xfad3e85a, 0xbd17d9b, 0x100db4b3

if system.arguments.size() < 1
	key = [
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30
	'(
		'(59, 94287082
		'(1111111109, 7081804
		'(1111111111, 14050471
		'(1234567890, 89005924
		'(2000000000, 69279037
		'(20000000000, 65353130
	.each(@(x) assert(hotp(key, x[0] / 30) % 100000000 == x[1]
	system.error.write_line(system.script + " <secret>"
else
	key = base32_decode(system.arguments[0]
	math = Module("math"
	time = Module("time"
	t = Integer(math.floor(time.now())) / 30
	print(t
	print(hotp(key, t) % 1000000
