system = Module("system"
print = system.error.write_line

test = @(f)
	try
		f(
	catch Throwable e
		print(e
		e.dump(
		return
	throw Throwable("should not reach here."

test(@ print(*null
test(@ null["x"](
test(@ null * "x"
test(@ null["x"]
test(@ null["x"] = null
