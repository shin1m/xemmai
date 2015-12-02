system = Module("system"
print = system.out.write_line

object = Object(
throw Throwable("Hello" if object.?hello
throw Throwable("World" if object.?world
try
	print(object.hello + ", " + object.world + "!"
catch Throwable e
	e.dump(
object.hello = "Hello"
object.world = "World"
throw Throwable("Hello" if !object.?hello
throw Throwable("World" if !object.?world
print(object.hello + ", " + object.world + "!"
print(object.~hello + ", " + object.~world + "!"
throw Throwable("Hello" if object.?hello
throw Throwable("World" if object.?world
try
	print(object.hello + ", " + object.world + "!"
catch Throwable e
	e.dump(
object.('hello) = "Hello"
object.('world) = "World"
throw Throwable("Hello" if !object.?('hello)
throw Throwable("World" if !object.?('world)
print(object.('hello) + ", " + object.('world) + "!"
print(object.~('hello) + ", " + object.~('world) + "!"
throw Throwable("Hello" if object.?('hello)
throw Throwable("World" if object.?('world)
try
	print(object.('hello) + ", " + object.('world) + "!"
catch Throwable e
	e.dump(

throw Throwable("Hello" if $.?hello
throw Throwable("World" if $.?world
try
	print($hello + ", " + $world + "!"
catch Throwable e
	e.dump(
$hello = "Hello"
$world = "World"
throw Throwable("Hello" if !$.?hello
throw Throwable("World" if !$.?world
print($hello + ", " + $world + "!"
print($.~hello + ", " + $.~world + "!"
throw Throwable("Hello" if $.?hello
throw Throwable("World" if $.?world
try
	print($hello + ", " + $world + "!"
catch Throwable e
	e.dump(
$.('hello) = "Hello"
$.('world) = "World"
throw Throwable("Hello" if !$.?('hello)
throw Throwable("World" if !$.?('world)
print($.('hello) + ", " + $.('world) + "!"
print($.~('hello) + ", " + $.~('world) + "!"
throw Throwable("Hello" if $.?('hello)
throw Throwable("World" if $.?('world)
try
	print($.('hello) + ", " + $.('world) + "!"
catch Throwable e
	e.dump(
