system = Module("system");
print = system.out.write_line;

print("dictionary: " + {"one": 1, 2: "two"});

d = {"one": 1, 2: "two"};
if (!d.has("one")) throw Throwable("one");
if (!d.has(2)) throw Throwable(2);
print(d);

d["three"] = 3.0;
if (!d.has("three")) throw Throwable("three");
print(d);

d[4.0] = "four";
if (!d.has(4.0)) throw Throwable(4.0);
print(d);

print("" + d["one"] + ", " + d[2] + ", " + d["three"] + ", " + d[4.0]);

d.remove("one");
if (d.has("one")) throw Throwable("one");
print(d);

d.remove(2);
if (d.has(2)) throw Throwable(2);
print(d);
