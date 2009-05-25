system = Module("system");
print = system.out.write_line;

print("array: " + ["one", 2]);

a = ["one", 2];
print(a);

a.push(3.0);
print(a);

a.push("four");
print(a);

print("[" + a[0] + ", " + a[1] + ", " + a[2] + ", " + a[3] + "]");

a.push(5.0);
print(a);

print(a.shift());
print(a);

print(a.pop());
print(a);

print();

b = [];
b.push(1);
b.push(2);
b.push(3);
print(b);

b.insert(2, "four");
print(b);

print(b.remove(2));
print(b);

b.push(4);
b.shift();
print(b);

b.insert(2, "five");
print(b);

print(b.remove(2));
print(b);

b.push(5);
b.shift();
print(b);

b.insert(2, "six");
print(b);

print(b.remove(2));
print(b);

b.push(6);
b.shift();
print(b);

b.insert(2, "seven");
print(b);

print(b.remove(2));
print(b);

b.push(7);
b.shift();
print(b);

b.insert(2, "eight");
print(b);

print(b.remove(2));
print(b);

b.push(8);
b.shift();
print(b);

b.insert(1, "five");
print(b);

print(b.remove(1));
print(b);

b.unshift(5);
b.pop();
print(b);

b.insert(1, "four");
print(b);

print(b.remove(1));
print(b);

b.unshift(4);
b.pop();
print(b);

b.insert(1, "three");
print(b);

print(b.remove(1));
print(b);

b.unshift(3);
b.pop();
print(b);

b.insert(1, "two");
print(b);

print(b.remove(1));
print(b);

b.unshift(2);
b.pop();
print(b);

b.insert(1, "one");
print(b);

print(b.remove(1));
print(b);
