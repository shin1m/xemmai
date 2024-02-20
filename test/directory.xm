system = Module("system"
print = system.out.write_line
os = Module("os"
assert = @(x) x || throw Throwable("Assertion failed."

directory = os.Directory("" + os.Path(system.script) / ".."
try
	while entry = directory.read()
		special = entry.permissions >> 9
		owner = entry.permissions >> 6 & 7
		group = entry.permissions >> 3 & 7
		others = entry.permissions & 7
		print(entry.name + ": " + entry.type + " " + special + owner + group + others
		if entry.name == "CMakeLists.txt"
			cmake = entry
		else if entry.name == "enum"
			enum = entry
		assert(entry.permissions != os.Permissions.NONE
	assert(cmake !== null
	assert(cmake.type == os.FileType.REGULAR
	assert(enum !== null
	assert(enum.type == os.FileType.DIRECTORY
finally
	directory.close(
