sequence = @(n) {
	@{
		:n = n + 1;
	};
};

sieve = @(n, ns) {
	@{
		while (true) {
			m = ns();
			if (m % n != 0) break m;
		}
	};
};

primes = @{
	ns = sequence(1);
	@{
		n = ns();
		:ns = sieve(n, ns);
		n;
	};
}();

i = 0;
while (i < 30) {
	print(primes());
	print("\n");
	i = i + 1;
}
