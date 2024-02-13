system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."

test = @(x, match)
	try
		x(
		assert(false
	catch Throwable e
		print(e
		e.dump(
		assert(match(e.__string(

test_not_supported = @(x) test(x, @(x) x == "not supported."
test_not_supported(@ null.a = ""
test_not_supported(@ null("", ""
test_not_supported(@ null[""]
test_not_supported(@ null[""] = ""
test_not_supported(@ null + ""
test_not_supported(@ 0 + ""
test_not_supported(@ 0.0 & ""

test_owned = @(x) test(x, @(x) x == "owned by another thread."
foo = [
foo.share(
Thread(@ foo.own()).join(
test_owned(@ foo.push(""

test_undefined_field = @(x) test(x, @(x) x == "undefined field: foo"
test_undefined_field(@ Object().foo
test_undefined_field(@ Object().foo = ""
test_invalid_key = @(x) test(x, @(x) x.substring(0, 12) == "key must be "
test_invalid_key(@ Object().("foo")
test_invalid_key(@ Object().("foo") = ""
test_invalid_key(@ Object + @ $.("foo")
test_invalid_key(@ Object + @ $.("foo") = ""

test_out_of_range = @(x) test(x, @(x) x == "out of range."
test_out_of_range(@ "".substring(1
test_out_of_range(@ "".substring(1, 1
test_out_of_range(@ "".code_at(0
test_out_of_range(@ '()[0]
test_out_of_range(@ [][0]
test_out_of_range(@ [][-1]
test_out_of_range(@ Bytes(0)[0]
test_out_of_range(@ Bytes(0)[-1]
test_out_of_range(@ Bytes(0)[0] = 0
test_out_of_range(@ Bytes(0).copy(0, 1, Bytes(1), 0
test_out_of_range(@ Bytes(0).copy(-1, 0, Bytes(1), 0
test_out_of_range(@ Bytes(1).copy(0, 1, Bytes(0), 0
