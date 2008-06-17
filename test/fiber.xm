f0 = f1 = f2 = null;
fm = Fiber.current();

f0 = Fiber(@(x) {
	print("f0: " + x + "\n");
	x = f1(x + 1);
	print("f0: " + x + "\n");
	fm(x + 1);
});

f1 = Fiber(@(x) {
	print("f1: " + x + "\n");
	f2(x + 1);
});

f2 = Fiber(@(x) {
	print("f2: " + x + "\n");
	f0(x + 1);
});

print("fm: " + f0(0) + "\n");
