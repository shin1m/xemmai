system = Module("system");
print = system.out.write_line;

object = Object();
try {
	print(object.hello + ", " + object.world + "!");
} catch (e) {
	e.dump();
}
object.hello = "Hello";
object.world = "World";
print(object.hello + ", " + object.world + "!");
print(object.~hello + ", " + object.~world + "!");
try {
	print(object.hello + ", " + object.world + "!");
} catch (e) {
	e.dump();
}
object.('hello) = "Hello";
object.('world) = "World";
print(object.('hello) + ", " + object.('world) + "!");
print(object.~('hello) + ", " + object.~('world) + "!");
try {
	print(object.('hello) + ", " + object.('world) + "!");
} catch (e) {
	e.dump();
}

try {
	print($hello + ", " + $world + "!");
} catch (e) {
	e.dump();
}
$hello = "Hello";
$world = "World";
print($hello + ", " + $world + "!");
print($.~hello + ", " + $.~world + "!");
try {
	print($hello + ", " + $world + "!");
} catch (e) {
	e.dump();
}
$.('hello) = "Hello";
$.('world) = "World";
print($.('hello) + ", " + $.('world) + "!");
print($.~('hello) + ", " + $.~('world) + "!");
try {
	print($.('hello) + ", " + $.('world) + "!");
} catch (e) {
	e.dump();
}
