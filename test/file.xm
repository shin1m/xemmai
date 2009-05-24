io = Module("io");

bytes = Bytes(26);
i = 0;
while (i < bytes.size()) {
	bytes[i] = 65 + i;
	i = i + 1;
}

file = io.File("file.xm.test", "w");
try {
	file.write(bytes, 0, bytes.size());
} finally {
	file.close();
}

bytes = Bytes(4);
file = io.File("file.xm.test", "r");
try {
	while (true) {
		n = file.read(bytes, 0, bytes.size());
		if (n <= 0) break;
		print(bytes);
		print("\n");
	}
} finally {
	file.close();
}
