io = Module("io"
assert = @(x) x || throw Throwable("Assertion failed."

writer = io.Writer(io.File("text.xm.test", "w"), "utf-8"
try
	assert(writer.write("Hello, "
	assert(writer.write_line("world!"
	assert(writer.write("This is shin."
	assert(writer.write_line(""
	assert(writer.write_line("Good bye."
finally
	writer.close(

reader = io.Reader(io.File("text.xm.test", "r"), "utf-8"
try
	s = reader.read_line(
	s == "Hello, world!\n" || throw Throwable(s
	s = reader.read_line(
	s == "This is shin.\n" || throw Throwable(s
	s = reader.read(4
	s == "Good" || throw Throwable(s
	s = reader.read_line(
	s == " bye.\n" || throw Throwable(s
	s = reader.read_line(
	s == "" || throw Throwable(s
finally
	reader.close(
