system = Module("system");
print = system.out.write_line;
assert = @(x) if (!x) throw Throwable("Assertion failed.");;

t = '();
print(t);
assert(t.size() == 0);
assert(t == Tuple());

t = '("one", 2, 'three, 4.0);
print(t);

assert(t.size() == 4);
assert(t[0] == "one");
assert(t[1] == 2);
assert(t[2] == 'three);
assert(t[3] == 4.0);
assert(t == Tuple("one", 2, 'three, 4.0));
