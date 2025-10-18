io = Module("io"
assert = @(x) x || throw Throwable("Assertion failed."

file = io.File("text.xm.test", "w"
try
	writer = io.Writer(file.write, "utf-8"
	assert(writer.write("Hello, "
	assert(writer.write_line("world!"
	assert(writer.write("This is shin."
	assert(writer.write_line(""
	assert(writer.write_line("Good bye."
	writer.flush(
finally
	file.close(

file = io.File("text.xm.test", "r"
try
	reader = io.Reader(file.read, "utf-8"
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
	file.close(
