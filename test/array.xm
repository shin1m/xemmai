a = ["one", 2];
print(a);
print("\n");

a.push(3.0);
print(a);
print("\n");

a.push("four");
print(a);
print("\n");

print("[" + a[0] + ", " + a[1] + ", " + a[2] + ", " + a[3] + "]\n");

a.push(5.0);
print(a);
print("\n");

print(a.shift());
print("\n");
print(a);
print("\n");

print(a.pop());
print("\n");
print(a);
print("\n");

print("\n");

b = [];
b.push(1);
b.push(2);
b.push(3);
print(b);
print("\n");

b.insert(2, "four");
print(b);
print("\n");

print(b.remove(2));
print("\n");
print(b);
print("\n");

b.push(4);
b.shift();
print(b);
print("\n");

b.insert(2, "five");
print(b);
print("\n");

print(b.remove(2));
print("\n");
print(b);
print("\n");

b.push(5);
b.shift();
print(b);
print("\n");

b.insert(2, "six");
print(b);
print("\n");

print(b.remove(2));
print("\n");
print(b);
print("\n");

b.push(6);
b.shift();
print(b);
print("\n");

b.insert(2, "seven");
print(b);
print("\n");

print(b.remove(2));
print("\n");
print(b);
print("\n");

b.push(7);
b.shift();
print(b);
print("\n");

b.insert(2, "eight");
print(b);
print("\n");

print(b.remove(2));
print("\n");
print(b);
print("\n");

b.push(8);
b.shift();
print(b);
print("\n");

b.insert(1, "five");
print(b);
print("\n");

print(b.remove(1));
print("\n");
print(b);
print("\n");

b.unshift(5);
b.pop();
print(b);
print("\n");

b.insert(1, "four");
print(b);
print("\n");

print(b.remove(1));
print("\n");
print(b);
print("\n");

b.unshift(4);
b.pop();
print(b);
print("\n");

b.insert(1, "three");
print(b);
print("\n");

print(b.remove(1));
print("\n");
print(b);
print("\n");

b.unshift(3);
b.pop();
print(b);
print("\n");

b.insert(1, "two");
print(b);
print("\n");

print(b.remove(1));
print("\n");
print(b);
print("\n");

b.unshift(2);
b.pop();
print(b);
print("\n");

b.insert(1, "one");
print(b);
print("\n");

print(b.remove(1));
print("\n");
print(b);
print("\n");
