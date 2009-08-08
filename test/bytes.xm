system = Module("system");
print = system.out.write_line;

bytes = Bytes(16);
i = 0;
while (i < bytes.size()) {
	bytes[i] = i * 17;
	i = i + 1;
}
print(bytes);

i = 0;
while (i < bytes.size()) {
	if (bytes[i] != i * 17) throw Throwable("bytes[" + i + "] must be " + (i * 17) + ".");
	i = i + 1;
}
