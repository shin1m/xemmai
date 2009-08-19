system = Module("system");
print = system.out.write_line;

f = @(x) x * 2;
if (f(1) > 0) {
	print("1");
} else {
	print("ng");
}
if (f(-1) > 0) {
	print("ng");
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
while (true) {
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

i = 0;
while (true) {
	try {
		if (i > 1) {
			i = 3;
			break;
		}
		if (i > 0) throw 2;
		i = 1;
		print("try");
	} catch (Float e) {
		throw Throwable("never reach here.");
	} catch (Integer e) {
		print("catch");
		i = e;
	} finally {
		print("finally");
		print(i);
	}
}
