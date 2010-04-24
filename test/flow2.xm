system = Module("system");
print = system.out.write_line;

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

@{
	try {
		throw null;
	} catch (Null e) {
	}
}();
