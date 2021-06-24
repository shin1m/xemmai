assert = @(x) x || throw Throwable("Assertion failed."

f = @(x) Object + (return x + 2)
assert(f(1) == 3

g = @ Object + (return f(4))
assert(g() == 6

h = @ Object + (return Class)
assert(h() === Class
