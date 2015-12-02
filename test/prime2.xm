system = Module("system"
print = system.out.write_line

generator = @(f)
	fiber = Fiber(@(other) f(@(x) :other = other(x
	@() fiber(Fiber.current(

sequence = @(n) generator(@(yield)
	while true
		yield(n
		:n = n + 1

sieve = @(n, ns) generator(@(yield)
	while true
		m = ns(
		yield(m if m % n != 0

primes = generator(@(yield)
	ns = sequence(2
	while true
		n = ns(
		yield(n
		ns = sieve(n, ns

print(primes() for i = 0; i < 30; i = i + 1
