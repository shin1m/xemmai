system = Module("system");
print = system.out.write_line;

print("dictionary: " + {"one": 1, 2: "two"});

d = {"one": 1, 2: "two"};
print(d);

d["three"] = 3.0;
print(d);

d[4.0] = "four";
print(d);

print("" + d["one"] + ", " + d[2] + ", " + d["three"] + ", " + d[4.0]);

d.remove("one");
print(d);

d.remove(2);
print(d);
