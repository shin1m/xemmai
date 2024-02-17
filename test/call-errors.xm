system = Module("system"
print = system.error.write_line
assert = @(x) x || throw Throwable("Assertion failed."

try
	[].unshift(
catch Throwable e
	print(e
	assert(e.__string() == "must be called with 1 argument(s)."

try
	"".code_at("", ""
catch Throwable e
	print(e
	assert(e.__string() == "must be called with 1 argument(s)."
