system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

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
b.push(1
b.push(2
b.push(3
print(b
assert(b == [1, 2, 3]

b.insert(2, "four"
print(b
assert(b == [1, 2, "four", 3]

print(b.remove(2
print(b
assert(b == [1, 2, 3]

b.push(4
b.shift(
print(b
assert(b == [2, 3, 4]

b.insert(2, "five"
print(b
assert(b == [2, 3, "five", 4]

print(b.remove(2
print(b
assert(b == [2, 3, 4]

b.push(5
b.shift(
print(b
assert(b == [3, 4, 5]

b.insert(2, "six"
print(b
assert(b == [3, 4, "six", 5]

print(b.remove(2
print(b
assert(b == [3, 4, 5]

b.push(6
b.shift(
print(b
assert(b == [4, 5, 6]

b.insert(2, "seven"
print(b
assert(b == [4, 5, "seven", 6]

print(b.remove(2
print(b
assert(b == [4, 5, 6]

b.push(7
b.shift(
print(b
assert(b == [5, 6, 7]

b.insert(2, "eight"
print(b
assert(b == [5, 6, "eight", 7]

print(b.remove(2
print(b
assert(b == [5, 6, 7]

b.push(8
b.shift(
print(b
assert(b == [6, 7, 8]

b.insert(1, "five"
print(b
assert(b == [6, "five", 7, 8]

print(b.remove(1
print(b
assert(b == [6, 7, 8]

b.unshift(5
b.pop(
print(b
assert(b == [5, 6, 7]

b.insert(1, "four"
print(b
assert(b == [5, "four", 6, 7]

print(b.remove(1
print(b
assert(b == [5, 6, 7]

b.unshift(4
b.pop(
print(b
assert(b == [4, 5, 6]

b.insert(1, "three"
print(b
assert(b == [4, "three", 5, 6]

print(b.remove(1
print(b
assert(b == [4, 5, 6]

b.unshift(3
b.pop(
print(b
assert(b == [3, 4, 5]

b.insert(1, "two"
print(b
assert(b == [3, "two", 4, 5]

print(b.remove(1
print(b
assert(b == [3, 4, 5]

b.unshift(2
b.pop(
print(b
assert(b == [2, 3, 4]

b.insert(1, "one"
print(b
assert(b == [2, "one", 3, 4]

print(b.remove(1
print(b
assert(b == [2, 3, 4]

Foo = Class(Array) :: @
	$__construct = @(*xs) :$^__construct[$](0, *xs
	$__initialize = @(*xs) $push(4

foo = Foo(1, 2, 3
print(foo
assert(foo == [0, 1, 2, 3, 4]
