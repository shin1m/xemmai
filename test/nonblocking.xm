system = Module("system"
print = system.out.write_line
io = Module("io"
os = Module("os"
assert = @(x) x || throw Throwable("Assertion failed."

fds = os.pipe(
in = io.File(fds[0], true
out = io.File(fds[1], true
try
	try
		in.blocking__(false
		out.blocking__(false
		buffer = Bytes(1
		assert(in.read(buffer, 0, 1) == -1
		for i = 0;; i = i + 1
			buffer[0] = i
			out.write(buffer, 0, 1) < 1 && break
		print(i
		assert(i > 0
		for j = 0; j < i; j = j + 1
			assert(in.read(buffer, 0, 1) == 1
			assert(buffer[0] == j % 0x100
		assert(in.read(buffer, 0, 1) == -1
	finally
		out.close(
	assert(in.read(buffer, 0, 1) == 0
finally
	in.close(

eof = @(buffer, offset, size) 0
one = @(x)@(buffer, offset, size)
	x === null && return -1
	buffer[offset] = x
	:x = null
	1
reader = io.Reader(@(buffer, offset, size) read(buffer, offset, size), "utf-8"
read = one(0x24
assert(reader.read(1) == "$"
assert(reader.read(1) === null
read = one(0xd0
assert(reader.read(1) === null
read = eof
try
	reader.read(1
	assert(false
catch Throwable t
	assert(t.__string() == "incomplete sequence."
read = one(0x98
assert(reader.read(1) == String.from_code(0x418
read = eof
assert(reader.read(1) == ""
read = one(0x24
assert(reader.read_line() == "$"
assert(reader.read_line() === null
read = eof
assert(reader.read_line() == ""

writer = io.Writer(@(buffer, offset, size) write(buffer, offset, size), "utf-8"
write = @(buffer, offset, size)
	:size = size
	:write = eof
	0
for i = 0; writer.write("$"); i = i + 1
assert(i == size
assert(write === eof
try
	writer.write("$"
	assert(false
catch Throwable t
	assert(t.__string() == "resume required."
assert(!writer.resume(
write = @(buffer, offset, size)
	assert(size == :size
	:write = @(buffer, offset, size)
		assert(size == 1
		assert(buffer[offset] == 0x24
		::write = eof
		size
	size - 1
assert(!writer.resume(
assert(write !== eof
assert(writer.resume(
assert(write === eof
for i = 2; i < size; i = i + 1; assert(writer.write("$"
assert(!writer.write(String.from_code(0x418
write = @(buffer, offset, size)
	assert(size == :size - 1
	:write = @(buffer, offset, size)
		assert(size == 2
		assert(buffer[offset] == 0xd0
		assert(buffer[offset + 1] == 0x98
		::write = eof
		size
	size
assert(writer.resume(
assert(write !== eof
assert(writer.flush(
assert(write === eof
for i = 0; i < size; i = i + 1; assert(writer.write("$"
assert(!writer.write_line("$"
try
	writer.write_line("$"
	assert(false
catch Throwable t
	assert(t.__string() == "resume required."
write = @(buffer, offset, size)
	assert(size == :size
	:write = @(buffer, offset, size)
		assert(size == 2
		assert(buffer[offset] == 0x24
		assert(buffer[offset + 1] == 0xa
		::write = eof
		size
	size
assert(writer.resume(
assert(write === eof
for i = 1; i < size; i = i + 1; assert(writer.write("$"
assert(!writer.write_line("$"
write = @(buffer, offset, size)
	assert(size == :size
	assert(buffer[offset + size - 1] == 0x24
	:write = @(buffer, offset, size)
		assert(size == 1
		assert(buffer[offset] == 0xa
		::write = eof
		size
	size
assert(writer.resume(
assert(write === eof
writer.flush(
