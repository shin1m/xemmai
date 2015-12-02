io = Module("io"

writer = io.Writer(io.File("text.xm.test", "w"), "utf-8"
try
	writer.write("Hello, "
	writer.write_line("world!"
	writer.write("This is shin."
	writer.write_line(
	writer.write_line("Good bye."
finally
	writer.close(

reader = io.Reader(io.File("text.xm.test", "r"), "utf-8"
try
	s = reader.read_line(
	throw Throwable(s if s != "Hello, world!\n"
	s = reader.read_line(
	throw Throwable(s if s != "This is shin.\n"
	s = reader.read(4
	throw Throwable(s if s != "Good"
	s = reader.read_line(
	throw Throwable(s if s != " bye.\n"
	s = reader.read_line(
	throw Throwable(s if s != ""
finally
	reader.close(
