system = Module("system"
print = system.out.write_line

counter = 0

hello = @(name, n)
	while n > 0
		n = n - 1
		i = :counter
		:counter = i + 1
		print(name + ": " + i
	print("done."

good_bye = @(name, n)
	return @
		hello(name, n

foo = Thread(good_bye("foo", 30
bar = Thread(good_bye("bar", 20
zot = Thread(good_bye("zot", 10
fooo = Thread(good_bye("fooo", 30
baar = Thread(good_bye("baar", 20
zoot = Thread(good_bye("zoot", 10
foooo = Thread(good_bye("foooo", 30
baaar = Thread(good_bye("baaar", 20
zooot = Thread(good_bye("zooot", 10
fooooo = Thread(good_bye("fooooo", 30
baaaar = Thread(good_bye("baaaar", 20
zoooot = Thread(good_bye("zoooot", 10

foo.join(
bar.join(
zot.join(
fooo.join(
baar.join(
zoot.join(
foooo.join(
baaar.join(
zooot.join(
fooooo.join(
baaaar.join(
zoooot.join(

print("counter: " + counter
