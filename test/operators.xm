assert = @(x) if !x: throw Throwable("Assertion failed."

Foo = Class() :: @
	$__call = @(x, y) "a(" + x + ", " + y + ")"
	$__get_at = @(x) "a[" + x + "]"
	$__set_at = @(x, y) "a[" + x + "] = " + y
	$__plus = @ "+a"
	$__minus = @ "-a"
	$__not = @ "!a"
	$__complement = @ "~a"
	$__multiply = @(x) "a * " + x
	$__divide = @(x) "a / " + x
	$__modulus = @(x) "a % " + x
	$__add = @(x) "a + " + x
	$__subtract = @(x) "a - " + x
	$__left_shift = @(x) "a << " + x
	$__right_shift = @(x) "a >> " + x
	$__less = @(x) "a < " + x
	$__less_equal = @(x) "a <= " + x
	$__greater = @(x) "a > " + x
	$__greater_equal = @(x) "a >= " + x
	$__equals = @(x) "a == " + x
	$__not_equals = @(x) "a != " + x
	$__and = @(x) "a & " + x
	$__xor = @(x) "a ^ " + x
	$__or = @(x) "a | " + x
	$__send = @(x) "a :: " + x

a = Foo(
assert(a("x", "y") == "a(x, y)"
assert(a["x"] == "a[x]"
assert((a["x"] = "y") == "a[x] = y"
assert(+a == "+a"
assert(-a == "-a"
assert(!a == "!a"
assert(~a == "~a"
assert(a * "x" == "a * x"
assert(a / "x" == "a / x"
assert(a % "x" == "a % x"
assert(a + "x" == "a + x"
assert(a - "x" == "a - x"
assert(a << "x" == "a << x"
assert(a >> "x" == "a >> x"
assert(a < "x" == "a < x"
assert(a <= "x" == "a <= x"
assert(a > "x" == "a > x"
assert(a >= "x" == "a >= x"
assert(a == "x" == "a == x"
assert(a != "x" == "a != x"
assert((a & "x") == "a & x"
assert((a ^ "x") == "a ^ x"
assert((a | "x") == "a | x"
assert((a :: "x") == "a :: x"

a = Object(
a.__call = @(x, y) "a(" + x + ", " + y + ")"
a.__get_at = @(x) "a[" + x + "]"
a.__set_at = @(x, y) "a[" + x + "] = " + y
a.__plus = @ "+a"
a.__minus = @ "-a"
a.__not = @ "!a"
a.__complement = @ "~a"
a.__multiply = @(x) "a * " + x
a.__divide = @(x) "a / " + x
a.__modulus = @(x) "a % " + x
a.__add = @(x) "a + " + x
a.__subtract = @(x) "a - " + x
a.__left_shift = @(x) "a << " + x
a.__right_shift = @(x) "a >> " + x
a.__less = @(x) "a < " + x
a.__less_equal = @(x) "a <= " + x
a.__greater = @(x) "a > " + x
a.__greater_equal = @(x) "a >= " + x
a.__equals = @(x) "a == " + x
a.__not_equals = @(x) "a != " + x
a.__and = @(x) "a & " + x
a.__xor = @(x) "a ^ " + x
a.__or = @(x) "a | " + x
a.__send = @(x) "a :: " + x
assert(a("x", "y") == "a(x, y)"
assert(a["x"] == "a[x]"
assert((a["x"] = "y") == "a[x] = y"
assert(+a == "+a"
assert(-a == "-a"
assert(!a == "!a"
assert(~a == "~a"
assert(a * "x" == "a * x"
assert(a / "x" == "a / x"
assert(a % "x" == "a % x"
assert(a + "x" == "a + x"
assert(a - "x" == "a - x"
assert(a << "x" == "a << x"
assert(a >> "x" == "a >> x"
assert(a < "x" == "a < x"
assert(a <= "x" == "a <= x"
assert(a > "x" == "a > x"
assert(a >= "x" == "a >= x"
assert(a == "x" == "a == x"
assert(a != "x" == "a != x"
assert((a & "x") == "a & x"
assert((a ^ "x") == "a ^ x"
assert((a | "x") == "a | x"
assert((a :: "x") == "a :: x"

Bar = Class() :: @
	$__call = @(x, y) x > 0 ? $(x - 1, x + y) : y
	$__add = @(x) x > 0 ? $ + (x - 1) : "zero"
a = Bar(
assert(a(10000, 0) == 10001 * 10000 / 2
assert(a + 10000 == "zero"

f = @(x) x + x
assert(f("x") == "xx"
