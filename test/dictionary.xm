print("dictionary: " + {"one": 1, 2: "two"} + "\n");

d = {"one": 1, 2: "two"};
print(d);
print("\n");

d["three"] = 3.0;
print(d);
print("\n");

d[4.0] = "four";
print(d);
print("\n");

print("" + d["one"] + ", " + d[2] + ", " + d["three"] + ", " + d[4.0] + "\n");

d.remove("one");
print(d);
print("\n");

d.remove(2);
print(d);
print("\n");
