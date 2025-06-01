system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

print("!false = " + !false
assert(!false === true
print("!true = " + !true
assert(!true === false
print("false & false = " + (false & false)
assert((false & false) === false
print("true & false = " + (true & false)
assert((true & false) === false
print("false & true = " + (false & true)
assert((false & true) === false
print("true & true = " + (true & true)
assert((true & true) === true
print("false | false = " + (false | false)
assert((false | false) === false
print("true | false = " + (true | false)
assert((true | false) === true
print("false | true = " + (false | true)
assert((false | true) === true
print("true | true = " + (true | true)
assert((true | true) === true
print("false ^ false = " + (false ^ false)
assert((false ^ false) === false
print("true ^ false = " + (true ^ false)
assert((true ^ false) === true
print("false ^ true = " + (false ^ true)
assert((false ^ true) === true
print("true ^ true = " + (true ^ true)
assert((true ^ true) === false

FALSE = false
TRUE = true

print("!FALSE = " + !FALSE
assert(!FALSE === true
print("!TRUE = " + !TRUE
assert(!TRUE === false
print("FALSE & false = " + (FALSE & false)
assert((FALSE & false) === false
print("TRUE & false = " + (TRUE & false)
assert((TRUE & false) === false
print("FALSE & true = " + (FALSE & true)
assert((FALSE & true) === FALSE
print("TRUE & true = " + (TRUE & true)
assert((TRUE & true) === true
print("FALSE | false = " + (FALSE | false)
assert((FALSE | false) === false
print("TRUE | false = " + (TRUE | false)
assert((TRUE | false) === true
print("FALSE | true = " + (FALSE | true)
assert((FALSE | true) === true
print("TRUE | true = " + (TRUE | true)
assert((TRUE | true) === true
print("FALSE ^ false = " + (FALSE ^ false)
assert((FALSE ^ false) === false
print("TRUE ^ false = " + (TRUE ^ false)
assert((TRUE ^ false) === true
print("FALSE ^ true = " + (FALSE ^ true)
assert((FALSE ^ true) === true
print("TRUE ^ true = " + (TRUE ^ true)
assert((TRUE ^ true) === false

print("!false = " + !false
assert(!false === TRUE
print("!true = " + !true
assert(!true === FALSE
print("false & FALSE = " + (false & FALSE)
assert((false & FALSE) === FALSE
print("true & FALSE = " + (true & FALSE)
assert((true & FALSE) === FALSE
print("false & TRUE = " + (false & TRUE)
assert((false & TRUE) === FALSE
print("true & TRUE = " + (true & TRUE)
assert((true & TRUE) === TRUE
print("false | FALSE = " + (false | FALSE)
assert((false | FALSE) === FALSE
print("true | FALSE = " + (true | FALSE)
assert((true | FALSE) === TRUE
print("false | TRUE = " + (false | TRUE)
assert((false | TRUE) === TRUE
print("true | TRUE = " + (true | TRUE)
assert((true | TRUE) === TRUE
print("false ^ FALSE = " + (false ^ FALSE)
assert((false ^ FALSE) === FALSE
print("true ^ FALSE = " + (true ^ FALSE)
assert((true ^ FALSE) === TRUE
print("false ^ TRUE = " + (false ^ TRUE)
assert((false ^ TRUE) === TRUE
print("true ^ TRUE = " + (true ^ TRUE)
assert((true ^ TRUE) === FALSE

print("!FALSE = " + !FALSE
assert(!FALSE === TRUE
print("!TRUE = " + !TRUE
assert(!TRUE === FALSE
print("FALSE & FALSE = " + (FALSE & FALSE)
assert((FALSE & FALSE) === FALSE
print("TRUE & FALSE = " + (TRUE & FALSE)
assert((TRUE & FALSE) === FALSE
print("FALSE & TRUE = " + (FALSE & TRUE)
assert((FALSE & TRUE) === FALSE
print("TRUE & TRUE = " + (TRUE & TRUE)
assert((TRUE & TRUE) === TRUE
print("FALSE | FALSE = " + (FALSE | FALSE)
assert((FALSE | FALSE) === FALSE
print("TRUE | FALSE = " + (TRUE | FALSE)
assert((TRUE | FALSE) === TRUE
print("FALSE | TRUE = " + (FALSE | TRUE)
assert((FALSE | TRUE) === TRUE
print("TRUE | TRUE = " + (TRUE | TRUE)
assert((TRUE | TRUE) === TRUE
print("FALSE ^ FALSE = " + (FALSE ^ FALSE)
assert((FALSE ^ FALSE) === FALSE
print("TRUE ^ FALSE = " + (TRUE ^ FALSE)
assert((TRUE ^ FALSE) === TRUE
print("FALSE ^ TRUE = " + (FALSE ^ TRUE)
assert((FALSE ^ TRUE) === TRUE
print("TRUE ^ TRUE = " + (TRUE ^ TRUE)
assert((TRUE ^ TRUE) === FALSE

foo = "foo"
assert((false && foo) === false
assert((true && foo) === foo
assert((false || foo) === foo
assert((foo || "bar") === foo

print("+(0 + 1) = " + +(0 + 1)
assert(+(0 + 1) == 1
print("-(0 + 1) = " + -(0 + 1)
assert(-(0 + 1) == -1
print("~(1 - 1) = " + ~(1 - 1)
assert(~(1 - 1) == ~0
print("-10 / 9 = " + (-10 / 9)
assert(-10 / 9 == -1
print("-10 % 9 = " + (-10 % 9)
assert(-10 % 9 == -1
bits = 0
for x = 1; x != 0; x = x << 1; bits = bits + 1
print("-1 >> 1 = " + (-1 >> 1)
assert(-1 >> 1 == ~(1 << bits - 1)
print("0x1f = " + 0x1f
assert(0x1f == 31
print("0x000a = " + 0x000a
assert(0x000a == 10

print("+(0.0 + 1.0) = " + +(0.0 + 1.0)
assert(+(0.0 + 1.0) == 1.0
print("-(0.0 + 1.0) = " + -(0.0 + 1.0)
assert(-(0.0 + 1.0) == -1.0
print("1 + 2.5 = " + (1 + 2.5)
assert(1 + 2.5 == 3.5
print("1.5 + 2 = " + (1.5 + 2)
assert(1.5 + 2 == 3.5
print("1.0e-1 = " + 1.0e-1
assert(1.0e-1 == 0.1
print("10.e-1 = " + 10.e-1
assert(10.e-1 == 1.0
print("0.1e1 = " + 0.1e1
assert(0.1e1 == 1.0

math = Module("math"
print("sqrt(2) = " + math.sqrt(2)
assert(math.sqrt(2) == math.sqrt(2.0)

Foo = Integer + @
	$f = @(x) $ + x
print("Foo(1).@ === Integer = " + (Foo(1).@ === Integer)
assert(Foo(1).@ !== Integer
print("Foo(1).f(2) = " + Foo(1).f(2)
assert(Foo(1).f(2) == 3
print("Foo(1) == Foo(2) = " + (Foo(1) == Foo(2))
assert(Foo(1) != Foo(2)
print("1 == Foo(2) = " + (1 == Foo(2))
assert(1 != Foo(2)
print("Foo(1) == 2 = " + (Foo(1) == 2)
assert(Foo(1) != 2
print("Foo(1) == Foo(1) = " + (Foo(1) == Foo(1))
assert(Foo(1) == Foo(1)
print("1 == Foo(1) = " + (1 == Foo(1))
assert(1 == Foo(1)
print("Foo(1) == 1 = " + (Foo(1) == 1)
assert(Foo(1) == 1
print("(1).__equals(1) = " + (1).__equals(1)
assert((1).__equals(1)
print("(1).__equals(2) = " + (1).__equals(2)
assert(!(1).__equals(2)
print("Foo(1).__equals(1) = " + Foo(1).__equals(1)
assert(Foo(1).__equals(1)
print("Foo(1).__equals(2) = " + Foo(1).__equals(2)
assert(!Foo(1).__equals(2)

Bar = Float + @
	$f = @(x) $ + x
print("Bar(1.0).@ === Float = " + (Bar(1.0).@ === Float)
assert(Bar(1.0).@ !== Float
print("Bar(1.0).f(2.0) = " + Bar(1.0).f(2.0)
assert(Bar(1.0).f(2.0) == 3.0
print("Bar(1.0) == Bar(2.0) = " + (Bar(1.0) == Bar(2.0))
assert(Bar(1.0) != Bar(2.0)
print("1.0 == Bar(2.0) = " + (1.0 == Bar(2.0))
assert(1.0 != Bar(2.0)
print("Bar(1.0) == 2.0 = " + (Bar(1.0) == 2.0)
assert(Bar(1.0) != 2.0
print("Bar(1.0) == Bar(1.0) = " + (Bar(1.0) == Bar(1.0))
assert(Bar(1.0) == Bar(1.0)
print("1.0 == Bar(1.0) = " + (1.0 == Bar(1.0))
assert(1.0 == Bar(1.0)
print("Bar(1.0) == 1.0 = " + (Bar(1.0) == 1.0)
assert(Bar(1.0) == 1.0
print("(1.0).__equals(1.0) = " + (1.0).__equals(1.0)
assert((1.0).__equals(1.0)
print("(1.0).__equals(2.0) = " + (1.0).__equals(2.0)
assert(!(1.0).__equals(2.0)
print("Bar(1.0).__equals(1.0) = " + Bar(1.0).__equals(1.0)
assert(Bar(1.0).__equals(1.0)
print("Bar(1.0).__equals(2.0) = " + Bar(1.0).__equals(2.0)
assert(!Bar(1.0).__equals(2.0)

assert(!"" === false
assert("" === ""
assert("" + "" === ""
assert(foo + "" === foo
assert("" + foo === foo
