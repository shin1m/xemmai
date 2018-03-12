system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

fix = @(f) @(*x) f(fix(f))(*x
print("factor: " + fix(@(f) @(x) x == 0 ? 1 : x * f(x - 1))(5)
assert(fix(@(f) @(x) x == 0 ? 1 : x * f(x - 1))(5) == 120

Z = @(f) (@(x) f(@(y) x(x)(y)))(@(x) f(@(y) x(x)(y)))
print("factor: " + Z(@(f) @(x) x == 0 ? 1 : x * f(x - 1))(5)
assert(Z(@(f) @(x) x == 0 ? 1 : x * f(x - 1))(5) == 120
