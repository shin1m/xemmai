system = Module("system"
print = system.out.write_line

object = Object(
!object.?hello || throw Throwable("Hello"
!object.?world || throw Throwable("World"
try: print(object.hello + ", " + object.world + "!" catch Throwable e: e.dump(
object.hello = "Hello"
object.world = "World"
object.?hello || throw Throwable("Hello"
object.?world || throw Throwable("World"
print(object.hello + ", " + object.world + "!"
print(object.~hello + ", " + object.~world + "!"
!object.?hello || throw Throwable("Hello"
!object.?world || throw Throwable("World"
try: print(object.hello + ", " + object.world + "!" catch Throwable e: e.dump(
object.('hello) = "Hello"
object.('world) = "World"
object.?('hello) || throw Throwable("Hello"
object.?('world) || throw Throwable("World"
print(object.('hello) + ", " + object.('world) + "!"
print(object.~('hello) + ", " + object.~('world) + "!"
!object.?('hello) || throw Throwable("Hello"
!object.?('world) || throw Throwable("World"
try: print(object.('hello) + ", " + object.('world) + "!" catch Throwable e: e.dump(

!$.?hello || throw Throwable("Hello"
!$.?world || throw Throwable("World"
try: print($hello + ", " + $world + "!" catch Throwable e: e.dump(
$hello = "Hello"
$world = "World"
$.?hello || throw Throwable("Hello"
$.?world || throw Throwable("World"
print($hello + ", " + $world + "!"
print($.~hello + ", " + $.~world + "!"
!$.?hello || throw Throwable("Hello"
!$.?world || throw Throwable("World"
try: print($hello + ", " + $world + "!" catch Throwable e: e.dump(
$.('hello) = "Hello"
$.('world) = "World"
$.?('hello) || throw Throwable("Hello"
$.?('world) || throw Throwable("World"
print($.('hello) + ", " + $.('world) + "!"
print($.~('hello) + ", " + $.~('world) + "!"
!$.?('hello) || throw Throwable("Hello"
!$.?('world) || throw Throwable("World"
try: print($.('hello) + ", " + $.('world) + "!" catch Throwable e: e.dump(
