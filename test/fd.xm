io = Module("io"

writer = io.Writer(io.File(1, "w"), "utf-8"
try
	writer.write_line("Hello, world!"
finally
	writer.close(
