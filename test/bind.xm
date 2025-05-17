system = Module("system"
print = system.error.write_line
assert = @(x) x || throw Throwable("Assertion failed."

greet = @(x) $ + ", " + x + "!"

hello = greet["Hello"]
assert(hello("foo") == "Hello, foo!"

assert(greet["Bye"]("foo") == "Bye, foo!"
