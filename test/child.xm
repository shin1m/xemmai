system = Module("system"
print = system.out.write_line

if system.arguments.size() > 0
	print("Hello, " + system.arguments[0] + "!"
	line = system.in.read_line(
	print("Good bye, " + line.substring(0, line.size() - 1
else
	io = Module("io"
	os = Module("os"
	assert = @(x) x || throw Throwable("Assertion failed."
	child = os.Child(system.executable, '(system.script, "World"), '(), '(0, 1
	try
		in = io.Writer(child.pipe(0).write, "utf-8"
		out = io.Reader(child.pipe(1).read, "utf-8"
		read = @
			line = out.read_line()
			print("1: " + line
			line
		assert(read() == "Hello, World!\n"
		in.write_line("foo"
		assert(read() == "Good bye, foo\n"
		assert(read() == ""
	finally
		child.wait(
		child.close(
	assert(child.exited() == 0
	assert(child.signaled() === null
