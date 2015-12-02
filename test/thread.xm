system = Module("system"
print = system.out.write_line

hello = @(name, n)
	if n > 0
		print("Hello, " + name + ": " + n
		hello(name, n - 1
		print("Good bye, " + name + ": " + n

good_bye = @(name, n)
	@
		hello(name, n

foo = Thread(good_bye("foo", 30
bar = Thread(good_bye("bar", 20
zot = Thread(good_bye("zot", 10

foo.join(
bar.join(
zot.join(
