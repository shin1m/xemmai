system = Module("system"
print = system.out.write_line

sequence = @(n) @() :n = n + 1

sieve = @(n, ns) @
	while true
		m = ns(
		if m % n != 0: break m

primes = (@
	ns = sequence(1
	@
		n = ns(
		:ns = sieve(n, ns
		n
)(

for i = 0; i < 30; i = i + 1: print(primes(
