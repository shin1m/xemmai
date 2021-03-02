system = Module("system"
io = Module("io"
print = system.out.write_line
try
	io.File("foo", "rb"
catch Throwable e
	print(e
