system = Module("system");
print = system.out.write_line;

object = Object();
if (object.?hello) throw Throwable("Hello");
if (object.?world) throw Throwable("World");
try {
	print(object.hello + ", " + object.world + "!");
} catch (e) {
	e.dump();
}
object.hello = "Hello";
object.world = "World";
if (!object.?hello) throw Throwable("Hello");
if (!object.?world) throw Throwable("World");
print(object.hello + ", " + object.world + "!");
print(object.~hello + ", " + object.~world + "!");
if (object.?hello) throw Throwable("Hello");
if (object.?world) throw Throwable("World");
try {
	print(object.hello + ", " + object.world + "!");
} catch (e) {
	e.dump();
}
object.('hello) = "Hello";
object.('world) = "World";
if (!object.?('hello)) throw Throwable("Hello");
if (!object.?('world)) throw Throwable("World");
print(object.('hello) + ", " + object.('world) + "!");
print(object.~('hello) + ", " + object.~('world) + "!");
if (object.?('hello)) throw Throwable("Hello");
if (object.?('world)) throw Throwable("World");
try {
	print(object.('hello) + ", " + object.('world) + "!");
} catch (e) {
	e.dump();
}

if ($.?hello) throw Throwable("Hello");
if ($.?world) throw Throwable("World");
try {
	print($hello + ", " + $world + "!");
} catch (e) {
	e.dump();
}
$hello = "Hello";
$world = "World";
if (!$.?hello) throw Throwable("Hello");
if (!$.?world) throw Throwable("World");
print($hello + ", " + $world + "!");
print($.~hello + ", " + $.~world + "!");
if ($.?hello) throw Throwable("Hello");
if ($.?world) throw Throwable("World");
try {
	print($hello + ", " + $world + "!");
} catch (e) {
	e.dump();
}
$.('hello) = "Hello";
$.('world) = "World";
if (!$.?('hello)) throw Throwable("Hello");
if (!$.?('world)) throw Throwable("World");
print($.('hello) + ", " + $.('world) + "!");
print($.~('hello) + ", " + $.~('world) + "!");
if ($.?('hello)) throw Throwable("Hello");
if ($.?('world)) throw Throwable("World");
try {
	print($.('hello) + ", " + $.('world) + "!");
} catch (e) {
	e.dump();
}
