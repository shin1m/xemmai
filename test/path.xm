system = Module("system"
io = Module("io"
print = system.out.write_line

path = io.Path(system.script) / ".."
print(path
(path / "path.xm").__string() == system.script || throw Throwable("must equal."
(io.Path(".") / "..").__string() == io.Path("..").__string() || throw Throwable("must equal."
