system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."
assert_sequence = @(xs, ys)
	xs.size() == ys.size() || throw Throwable("xs.size() must be " + ys.size() + " but was " + xs.size() + "."
	for i = 0; i < xs.size(); i = i + 1
		xs[i] == ys[i] || throw Throwable("xs[" + i + "] must be " + ys[i] + " but was " + xs[i] + "."

a = ["one", 2
print(a
assert(a.size() == 2
assert(a[0] == "one"
assert(a[1] == 2

a.push(3.0
print(a
assert(a.size() == 3
assert(a[2] == 3.0

a.push("four"
print(a
assert(a.size() == 4
assert(a[3] == "four"

a.push(5.0
print(a
assert(a.size() == 5
assert(a[4] == 5.0

x = a.shift(
print(x
print(a
assert(x == "one"
assert(a.size() == 4

x = a.pop(
print(x
print(a
assert(x == 5.0
assert(a.size() == 3

print(

b = [
for i = 0; i < 5; i = i + 1: b.push(i.__string(
print(b
assert_sequence(b, ["0", "1", "2", "3", "4"
# 01234.

b.insert(3, "x"
print(b
assert_sequence(b, ["0", "1", "2", "x", "3", "4"
# 012x34

print(b.remove(3
print(b
assert_sequence(b, ["0", "1", "2", "3", "4"
# 01234.

b.push("5"
b.shift(
print(b
assert_sequence(b, ["1", "2", "3", "4", "5"
# .12345

b.insert(3, "x"
print(b
assert_sequence(b, ["1", "2", "3", "x", "4", "5"
# 5123x4

print(b.remove(3
print(b
assert_sequence(b, ["1", "2", "3", "4", "5"
# .12345

b.push("6"
b.shift(
print(b
assert_sequence(b, ["2", "3", "4", "5", "6"
# 6.2345

b.insert(3, "x"
print(b
assert_sequence(b, ["2", "3", "4", "x", "5", "6"
# 56234x

print(b.remove(3
print(b
assert_sequence(b, ["2", "3", "4", "5", "6"
# 6.2345

b.push("7"
b.shift(
print(b
assert_sequence(b, ["3", "4", "5", "6", "7"
# 67.345

b.insert(3, "x"
print(b
assert_sequence(b, ["3", "4", "5", "x", "6", "7"
# x67345

print(b.remove(3
print(b
assert_sequence(b, ["3", "4", "5", "6", "7"
# 67.345

b.push("8"
b.shift(
print(b
assert_sequence(b, ["4", "5", "6", "7", "8"
# 678.45

b.insert(3, "x"
print(b
assert_sequence(b, ["4", "5", "6", "x", "7", "8"
# 6x7845

print(b.remove(3
print(b
assert_sequence(b, ["4", "5", "6", "7", "8"
# 678.45

b = [
for i = 0; i < 5; i = i + 1: b.unshift(i.__string(
print(b
assert_sequence(b, ["4", "3", "2", "1", "0"
# .43210

b.insert(2, "x"
print(b
assert_sequence(b, ["4", "3", "x", "2", "1", "0"
# 43x210

print(b.remove(2
print(b
assert_sequence(b, ["4", "3", "2", "1", "0"
# .43210

b.unshift("5"
b.pop(
print(b
assert_sequence(b, ["5", "4", "3", "2", "1"
# 54321.

b.insert(2, "x"
print(b
assert_sequence(b, ["5", "4", "x", "3", "2", "1"
# 4x3215

print(b.remove(2
print(b
assert_sequence(b, ["5", "4", "3", "2", "1"
# 54321.

b.unshift("6"
b.pop(
print(b
assert_sequence(b, ["6", "5", "4", "3", "2"
# 5432.6

b.insert(2, "x"
print(b
assert_sequence(b, ["6", "5", "x", "4", "3", "2"
# x43265

print(b.remove(2
print(b
assert_sequence(b, ["6", "5", "4", "3", "2"
# 5432.6

b.unshift("7"
b.pop(
print(b
assert_sequence(b, ["7", "6", "5", "4", "3"
# 543.76

b.insert(2, "x"
print(b
assert_sequence(b, ["7", "6", "x", "5", "4", "3"
# 54376x

print(b.remove(2
print(b
assert_sequence(b, ["7", "6", "5", "4", "3"
# 543.76

b.unshift("8"
b.pop(
print(b
assert_sequence(b, ["8", "7", "6", "5", "4"
# 54.876

b.insert(2, "x"
print(b
assert_sequence(b, ["8", "7", "x", "6", "5", "4"
# 5487x6

print(b.remove(2
print(b
assert_sequence(b, ["8", "7", "6", "5", "4"
# 54.876

b.sort(@(x, y) x < y
print(b
assert_sequence(b, ["4", "5", "6", "7", "8"

c = ["a", "b", "c", "d", "e"
c.unshift("f"
c.unshift("g"
print(c
assert_sequence(c, ["g", "f", "a", "b", "c", "d", "e"
c.sort(@(x, y) x < y
print(c
assert_sequence(c, ["a", "b", "c", "d", "e", "f", "g"

d = [
for i = 0; i < 23; i = i + 1
	d.push(i.__string(
print(d
assert(d.size() == 23
for i = 0; i < 17; i = i + 1
	d.shift(
print(d
assert_sequence(d, ["17", "18", "19", "20", "21", "22"
for i = 0; i < 6; i = i + 1
	d.shift(
print(d
assert_sequence(d, [

e = [
for i = 0; i < 3; i = i + 1: e.insert(0, i
print(e
assert_sequence(e, [2, 1, 0
for ; i < 7; i = i + 1: e.unshift(i
print(e
e.remove(0
print(e
e.remove(0
print(e
assert_sequence(e, [4, 3, 2, 1, 0

Foo = List + @
	$__initialize = @(*xs) $push(4

foo = Foo(1, 2, 3
print(foo
assert_sequence(foo, [1, 2, 3, 4
