system = Module("system");
print = system.out.write_line;

fix = @(f) @(*x) f(fix(f))(*x);

print("factor: " + fix(@(f) @(x) x == 0 ? 1 : x * f(x - 1))(5));
