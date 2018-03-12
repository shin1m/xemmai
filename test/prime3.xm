system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

delay = @(f)
	g = @
		x = f(
		:g = @ x
		x
	@ g(

Pair = Class() :: @
	$__initialize = @(first, second)
		$first = first
		$second = second

sequence = @(n) delay(@ Pair(n, sequence(n + 1

sieve = @(n, ns) delay(@ ns().first % n == 0 ? sieve(n, ns().second)() : Pair(ns().first, sieve(n, ns().second))

primes = @(ns) delay(@ Pair(ns().first, primes(sieve(ns().first, ns().second)))

ns = primes(sequence(2
assert(2 == ns().first
ns = ns().second
assert(3 == ns().first
ns = ns().second
assert(5 == ns().first
ns = ns().second
assert(7 == ns().first
ns = ns().second
assert(11 == ns().first

ns = primes(sequence(2
for i = 0; i < 30; i = i + 1
	print(ns().first
	ns = ns().second
