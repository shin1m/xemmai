object = Object();
try {
	print(object.hello + ", " + object.world + "!\n");
} catch (e) {
	e.dump();
}
object.hello = "Hello";
object.world = "World";
print(object.hello + ", " + object.world + "!\n");
print(object.~hello + ", " + object.~world + "!\n");
try {
	print(object.hello + ", " + object.world + "!\n");
} catch (e) {
	e.dump();
}
object.('hello) = "Hello";
object.('world) = "World";
print(object.('hello) + ", " + object.('world) + "!\n");
print(object.~('hello) + ", " + object.~('world) + "!\n");
try {
	print(object.('hello) + ", " + object.('world) + "!\n");
} catch (e) {
	e.dump();
}

try {
	print($hello + ", " + $world + "!\n");
} catch (e) {
	e.dump();
}
$hello = "Hello";
$world = "World";
print($hello + ", " + $world + "!\n");
print($.~hello + ", " + $.~world + "!\n");
try {
	print($hello + ", " + $world + "!\n");
} catch (e) {
	e.dump();
}
$.('hello) = "Hello";
$.('world) = "World";
print($.('hello) + ", " + $.('world) + "!\n");
print($.~('hello) + ", " + $.~('world) + "!\n");
try {
	print($.('hello) + ", " + $.('world) + "!\n");
} catch (e) {
	e.dump();
}
