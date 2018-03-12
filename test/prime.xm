system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

sequence = @(n) @ :n = n + 1

sieve = @(n, ns) @ while true
	m = ns(
	m % n != 0 && break m

primes = @
	ns = sequence(1
	@
		n = ns(
		:ns = sieve(n, ns
		n

ns = primes(
assert(2 == ns(
assert(3 == ns(
assert(5 == ns(
assert(7 == ns(
assert(11 == ns(

ns = primes(
for i = 0; i < 30; i = i + 1
	print(ns(
