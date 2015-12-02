system = Module("system"
print = system.out.write_line

sequence = @(n) @() :n = n + 1

sieve = @(n, ns) @
	while true
		m = ns(
		break m if m % n != 0

primes = (@
	ns = sequence(1
	@
		n = ns(
		:ns = sieve(n, ns
		n
)(

print(primes() for i = 0; i < 30; i = i + 1
