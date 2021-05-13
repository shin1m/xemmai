system = Module("system"
print = system.out.write_line

object = Object(
!object.?hello || throw Throwable("Hello"
!object.?world || throw Throwable("World"
try: print(object.hello + ", " + object.world + "!" catch Throwable e: e.dump(
object = (Object + @
	$hello
	$world
)(
object.hello = "Hello"
object.world = "World"
object.?hello || throw Throwable("Hello"
object.?world || throw Throwable("World"
print(object.hello + ", " + object.world + "!"
object.('hello) = "Hello"
object.('world) = "World"
object.?('hello) || throw Throwable("Hello"
object.?('world) || throw Throwable("World"
print(object.('hello) + ", " + object.('world) + "!"

$hello = "Hello"
$hello === null || throw Throwable("Hello"
$.('world) = "World"
$.('world) === null || throw Throwable("World"
