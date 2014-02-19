system = Module("system");
print = system.out.write_line;
time = Module("time");
assert = @(x) if (!x) throw Throwable("Assertion failed.");;

t = time.compose('(1970, 1, 1, 1, 1, 0.5));
print(t);
assert(t == 3660.5);

t = time.decompose(3660.5);
print(t);
assert(t == '(1970, 1, 1, 1, 1, 0.5, 4, 1));

t = time.parse_rfc2822("Tue, 18 Feb 2014 21:31:41 +0900");
print(t);
assert(t == '(2014, 2, 18, 21, 31, 41, 9 * 60 * 60));

t = time.format_rfc2822('(2014, 2, 18, 21, 31, 41, 2), 9 * 60 * 60);
print(t);
assert(t == "Tue, 18 Feb 2014 21:31:41 +0900");

t = time.parse_http("Tue, 18 Feb 2014 21:31:41 GMT");
print(t);
assert(t == '(2014, 2, 18, 21, 31, 41));

t = time.parse_http("Tuesday, 18-Feb-14 21:31:41 GMT");
print(t);
assert(t == '(2014, 2, 18, 21, 31, 41));

t = time.parse_http("Tue Feb 18 21:31:41 2014");
print(t);
assert(t == '(2014, 2, 18, 21, 31, 41));

t = time.format_http('(2014, 2, 18, 21, 31, 41, 2));
print(t);
assert(t == "Tue, 18 Feb 2014 21:31:41 GMT");

t = time.parse_xsd("2014-2-18T21:31:41");
print(t);
assert(t == '(2014, 2, 18, 21, 31, 41));

t = time.parse_xsd("2014-2-18T21:31:41+0900");
print(t);
assert(t == '(2014, 2, 18, 21, 31, 41, 9 * 60 * 60));

t = time.format_xsd('(2014, 2, 18, 21, 31, 41), 0, 3);
print(t);
assert(t == "2014-02-18T21:31:41.000Z");

t = time.format_xsd('(2014, 2, 18, 21, 31, 41), 9 * 60 * 60, 0);
print(t);
assert(t == "2014-02-18T21:31:41+09:00");
