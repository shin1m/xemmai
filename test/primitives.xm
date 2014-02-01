system = Module("system");
print = system.out.write_line;
assert = @(x) if (!x) throw Throwable("Assertion failed.");;

print("!true = " + (!true));
assert(!true == false);
print("true & true = " + (true & true));
assert(true & true == true);
print("true | true = " + (true | true));
assert(true | true == true);
print("true ^ true = " + (true ^ true));
assert(true ^ true == false);

print("-10 / 9 = " + (-10 / 9));
assert(-10 / 9 == -1);
print("-10 % 9 = " + (-10 % 9));
assert(-10 % 9 == -1);
bits = 0;
for (x = 1; x != 0; x = x << 1) bits = bits + 1;
print("-1 >> 1 = " + (-1 >> 1));
assert(-1 >> 1 == ~(1 << bits - 1));
print("0x1f = " + 0x1f);
assert(0x1f == 31);
print("0x000a = " + 0x000a);
assert(0x000a == 10);

print("1 + 2.5 = " + (1 + 2.5));
assert(1 + 2.5 == 3.5);
print("1.5 + 2 = " + (1.5 + 2));
assert(1.5 + 2 == 3.5);
print("1.0e-1 = " + 1.0e-1);
assert(1.0e-1 == 0.1);
print("10.e-1 = " + 10.e-1);
assert(10.e-1 == 1.0);
print("0.1e1 = " + 0.1e1);
assert(0.1e1 == 1.0);

math = Module("math");
print("sqrt(2) = " + math.sqrt(2));
assert(math.sqrt(2) == math.sqrt(2.0));

Foo = Class(Integer);
Foo.f = @(x) $ + x;
print("Foo(1).: === Integer = " + (Foo(1).: === Integer));
assert(Foo(1).: !== Integer);
print("Foo(1).f(2) = " + Foo(1).f(2));
assert(Foo(1).f(2) == 3);
print("Foo(1) == Foo(2) = " + (Foo(1) == Foo(2)));
assert(Foo(1) != Foo(2));
print("1 == Foo(2) = " + (1 == Foo(2)));
assert(1 != Foo(2));
print("Foo(1) == 2 = " + (Foo(1) == 2));
assert(Foo(1) != 2);
print("Foo(1) == Foo(1) = " + (Foo(1) == Foo(1)));
assert(Foo(1) == Foo(1));
print("1 == Foo(1) = " + (1 == Foo(1)));
assert(1 == Foo(1));
print("Foo(1) == 1 = " + (Foo(1) == 1));
assert(Foo(1) == 1);
print("(1).__equals(1) = " + (1).__equals(1));
assert((1).__equals(1));
print("(1).__equals(2) = " + (1).__equals(2));
assert(!(1).__equals(2));
print("Foo(1).__equals(1) = " + Foo(1).__equals(1));
assert(Foo(1).__equals(1));
print("Foo(1).__equals(2) = " + Foo(1).__equals(2));
assert(!Foo(1).__equals(2));

Bar = Class(Float);
Bar.f = @(x) $ + x;
print("Bar(1.0).: === Float = " + (Bar(1.0).: === Float));
assert(Bar(1.0).: !== Float);
print("Bar(1.0).f(2.0) = " + Bar(1.0).f(2.0));
assert(Bar(1.0).f(2.0) == 3.0);
print("Bar(1.0) == Bar(2.0) = " + (Bar(1.0) == Bar(2.0)));
assert(Bar(1.0) != Bar(2.0));
print("1.0 == Bar(2.0) = " + (1.0 == Bar(2.0)));
assert(1.0 != Bar(2.0));
print("Bar(1.0) == 2.0 = " + (Bar(1.0) == 2.0));
assert(Bar(1.0) != 2.0);
print("Bar(1.0) == Bar(1.0) = " + (Bar(1.0) == Bar(1.0)));
assert(Bar(1.0) == Bar(1.0));
print("1.0 == Bar(1.0) = " + (1.0 == Bar(1.0)));
assert(1.0 == Bar(1.0));
print("Bar(1.0) == 1.0 = " + (Bar(1.0) == 1.0));
assert(Bar(1.0) == 1.0);
print("(1.0).__equals(1.0) = " + (1.0).__equals(1.0));
assert((1.0).__equals(1.0));
print("(1.0).__equals(2.0) = " + (1.0).__equals(2.0));
assert(!(1.0).__equals(2.0));
print("Bar(1.0).__equals(1.0) = " + Bar(1.0).__equals(1.0));
assert(Bar(1.0).__equals(1.0));
print("Bar(1.0).__equals(2.0) = " + Bar(1.0).__equals(2.0));
assert(!Bar(1.0).__equals(2.0));

Foo = Class(String) :: @{
	$__construct = @(*xs) {
		s = "";
		xs.each(@(x) :s = s + x);
		:$^__construct[$](s);
	};
	$__initialize = @(*xs) {
	};
};

foo = Foo(0, "a", 1, "b");
print(foo);
assert(foo == "0a1b");
