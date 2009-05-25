system = Module("system");
print = system.out.write_line;

f0 = f1 = f2 = null;
fm = Fiber.current();

f0 = Fiber(@(x) {
	print("f0: " + x);
	x = f1(x + 1);
	print("f0: " + x);
	fm(x + 1);
});

f1 = Fiber(@(x) {
	print("f1: " + x);
	f2(x + 1);
});

f2 = Fiber(@(x) {
	print("f2: " + x);
	f0(x + 1);
});

print("fm: " + f0(0));
