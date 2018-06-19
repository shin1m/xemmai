system = Module("system"
print = system.out.write_line
io = Module("io"

writer = io.Writer(Object(), "utf-8"
try
	writer.write_line("Hello, world!"
catch Throwable e
	print(e
	e.dump(
