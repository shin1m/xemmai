system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

test = @(x, error)
	try
		x(
		assert(false
	catch Throwable e
		print(e
		e.dump(
		assert(e.__string() == error

test_not_supported = @(x) test(x, "not supported."
test_not_supported(@ null.a = ""
test_not_supported(@ null.~a
test_not_supported(@ null("", ""
test_not_supported(@ null[""]
test_not_supported(@ null[""] = ""
test_not_supported(@ null + ""
test_not_supported(@ 0 + ""
test_not_supported(@ 0.0 & ""
test_not_supported(@ null :: ""

test_owned = @(x) test(x, "owned by another thread."
foo = Object(
foo.share(
Thread(@ foo.own()).join(
test_owned(@ foo.a = ""
test_owned(@ foo.~a

test_immutable = @(x) test(x, "immutable."
test_immutable(@ 'foo.a = ""
test_immutable(@ 'foo.~a
