system = Module("system");
print = system.out.write_line;

f0 = Fiber(@(co) {
	co("Hello everyone,");
	co("This is shin.");
	"Good bye.";
});

foo = Object();
foo.__string = @{
	f0(Fiber.current());
};

print(foo);
print(foo);
print(foo);
print(foo);
print(foo);
print(foo);
print(foo);
