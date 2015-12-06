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
	if s != "Hello, world!\n": throw Throwable(s
	s = reader.read_line(
	if s != "This is shin.\n": throw Throwable(s
	s = reader.read(4
	if s != "Good": throw Throwable(s
	s = reader.read_line(
	if s != " bye.\n": throw Throwable(s
	s = reader.read_line(
	if s != "": throw Throwable(s
finally
	reader.close(
