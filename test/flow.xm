system = Module("system");
print = system.out.write_line;

f = @(x) x * 2;
if (f(1) > 0) {
	print("1");
} else {
	throw Throwable("never reach here.");
}
if (f(-1) > 0) {
	throw Throwable("never reach here.");
} else {
	print("2");
}

print(f(1) > 0 ? "true" : "ng");
print(f(-1) > 0 ? "ng" : "false");
print(f(1) > 0 ? "true" : f(1) > 0 ? "ng" : "ng");
print(f(-1) > 0 ? "ng" : f(1) > 0 ? "false->true" : "ng");
print(f(-1) > 0 ? "ng" : f(-1) > 0 ? "ng" : "false->false");

i = 2;
while (i > 0) {
	if (i > 1) {
		print("3");
		i = 1;
	} else if (i > 0) {
		print("4");
		i = 0;
	}
}
print("5");

i = 0;
for (;;) {
	if (i > 1) {
		print("8");
		break;
	}
	if (i > 0) {
		print("7");
		i = 2;
		continue;
	}
	print("6");
	i = 1;
}
print("9");

a = @{
	i = 0;
	while (true) {
		if (i > 1) return "12";
		if (i > 0) {
			print("11");
			i = 2;
			continue;
		}
		print("10");
		i = 1;
	}
	print("9");
}();
print(a);

j = 0;
for (i = 0; i < 2; i = i + 1) {
	print(i);
	if (i != j) throw Throwable("i must be j.");
	j = j + 1;
}

for (i = 0, j = 0; i < 2; i = i + 1, j = j + 1) {
	print(i);
	if (i == j) continue;
	throw Throwable("never reach here.");
}

i = 0;
for (; true;) {
	if (i >= 2) break;
	print(i);
	i = i + 1;
	continue;
	throw Throwable("never reach here.");
}

for (i = 0; i < 2; i = i + 1);
print(i);
if (i != 2) throw Throwable("i must be 2.");
