assert = @(x) x || throw Throwable("Assertion failed."

f = @(x) 1 + (return x + 2)
assert(f(1) == 3

g = @(x) (return f(4)) + 5
assert(g(1) == 6
