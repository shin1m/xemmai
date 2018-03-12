system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

generator = @(f)
	fiber = Fiber(@(other) f(@(x) :other = other(x
	@ fiber(Fiber.current(

sequence = @(n) generator(@(yield) while true
	yield(n
	:n = n + 1

sieve = @(n, ns) generator(@(yield) while true
	m = ns(
	m % n != 0 && yield(m

primes = @ generator(@(yield)
	ns = sequence(2
	while true
		n = ns(
		yield(n
		ns = sieve(n, ns

ns = primes(
assert(2 == ns(
assert(3 == ns(
assert(5 == ns(
assert(7 == ns(
assert(11 == ns(

ns = primes(
for i = 0; i < 30; i = i + 1
	print(ns(
