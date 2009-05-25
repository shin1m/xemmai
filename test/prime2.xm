system = Module("system");
print = system.out.write_line;

generator = @(f) {
	fiber = Fiber(@(other) {
		f(@(x) {
			:other = other(x);
		});
	});
	@{
		fiber(Fiber.current());
	};
};

sequence = @(n) {
	generator(@(yield) {
		while (true) {
			yield(n);
			:n = n + 1;
		}
	});
};

sieve = @(n, ns) {
	generator(@(yield) {
		while (true) {
			m = ns();
			if (m % n != 0) yield(m);
		}
	});
};

primes = generator(@(yield) {
	ns = sequence(2);
	while (true) {
		n = ns();
		yield(n);
		ns = sieve(n, ns);
	}
});

i = 0;
while (i < 30) {
	print(primes());
	i = i + 1;
}
