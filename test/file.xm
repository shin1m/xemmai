system = Module("system"
io = Module("io"
print = system.out.write_line

bytes = Bytes(26
for i = 0; i < bytes.size(); i = i + 1; bytes[i] = 65 + i

file = io.File("file.xm.test", "w"
try
	size = bytes.size(
	while true
		n = file.write(bytes, bytes.size() - size, size
		n < 0 && continue
		size = size - n
		size > 0 || break
finally
	file.close(

bytes = Bytes(4
file = io.File("file.xm.test", "r"
try
	while true
		n = file.read(bytes, 0, bytes.size(
		n < 0 && continue
		n > 0 || break
		print(bytes
finally
	file.close(
