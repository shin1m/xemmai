if (true) {
	print("1\n");
} else {
	print("ng\n");
}
if (false) {
	print("ng\n");
} else {
	print("2\n");
}

i = 2;
while (i > 0) {
	if (i > 1) {
		print("3\n");
		i = 1;
	} else if (i > 0) {
		print("4\n");
		i = 0;
	}
}
print("5\n");

i = 0;
while (true) {
	if (i > 1) {
		print("8\n");
		break;
	}
	if (i > 0) {
		print("7\n");
		i = 2;
		continue;
	}
	print("6\n");
	i = 1;
}
print("9\n");

a = @{
	i = 0;
	while (true) {
		if (i > 1) return "12\n";
		if (i > 0) {
			print("11\n");
			i = 2;
			continue;
		}
		print("10\n");
		i = 1;
	}
	print("9\n");
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
		print("try\n");
	} catch (e) {
		print("catch\n");
		i = e;
	} finally {
		print("finally\n");
		print(i);
		print("\n");
	}
}
