bytes = Bytes(16);
i = 0;
while (i < bytes.size()) {
	bytes[i] = i * 17;
	i = i + 1;
}
print(bytes);
print("\n");

i = 0;
while (i < bytes.size()) {
	if (bytes[i] != i * 17) throw "bytes[" + i + "] must be " + (i * 17) + ".";
	i = i + 1;
}
