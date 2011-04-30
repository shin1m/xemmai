system = Module("system");
print = system.out.write_line;
assert = @(x) if (!x) throw Throwable("Assertion failed.");;

d = {"one": 1, 2: "two"};
print(d);
assert(d.size() == 2);
assert(d.has("one") && d["one"] == 1);
assert(d.has(2) && d[2] == "two");

d["three"] = 3.0;
print(d);
assert(d.size() == 3);
assert(d.has("three") && d["three"] == 3.0);

d[4.0] = "four";
print(d);
assert(d.size() == 4);
assert(d.has(4.0) && d[4.0] == "four");

d.remove("one");
print(d);
assert(d.size() == 3);
assert(!d.has("one"));

d.remove(2);
print(d);
assert(d.size() == 2);
assert(!d.has(2));

d.clear();
print(d);
assert(d.size() == 0);

d = {};
i = 0;
while (i < 1000) {
	d[i] = i;
	i = i + 1;
}
assert(d.size() == 1000);
