system = Module("system");
print = system.out.write_line;

fm = Fiber.current();

f0 = Fiber(@(x) {
	x = fm("Hello, " + x + "!!\n");
	x = fm(x + "Good!!\n");
	fm("Good bye, " + x + ".\n");
});

print(@{
	f0(f0("foo") + "How are you?\n");
}());

print(@{
	f0(f0("bar") + "Bye.\n");
}());

f1 = Fiber(@(x) {
	x = fm("Hello, " + x + "!!\n");
	throw Throwable(x + "Oops, sorry.\n");
});

try {
	print(@{
		f1(f1("zot") + "How are you?\n");
	}());
} catch (Throwable e) {
	print("caught: " + e);
	e.dump();
}

f2 = Fiber(@(x) {
	throw Throwable(x + "Oops, sorry.\n");
});

try {
	f2("How are you?\n");
} catch (Throwable e) {
	print("caught: " + e);
	e.dump();
}
