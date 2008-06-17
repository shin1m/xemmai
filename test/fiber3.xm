f0 = Fiber(@(co) {
	co("Hello everyone,\n");
	co("This is shin.\n");
	"Good bye.\n";
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
