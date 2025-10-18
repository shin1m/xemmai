system = Module("system"
io = Module("io"
os = Module("os"
assert = @(x) x || throw Throwable("Assertion failed."

child = os.Child(system.executable, '(os.Path(system.script) / "../psmtp.xm", "foo"), '(), '(0, 1, 1
try
	in = io.Writer(child.pipe(0).write, "utf-8"
	out = io.Reader(child.pipe(1).read, "utf-8"
	child.pipe(2).blocking__(false
	error = io.Reader(child.pipe(2).read, "utf-8"
	log = ""
	read_log = @ while true
		x = error.read_line(
		(x === null || x == "") && break
		:log = log + x
		system.out.write("2: " + x
	read = @
		read_log(
		x = out.read_line(
		system.out.write("1: " + x
		system.out.flush(
		x
	write = @(x)
		system.out.write_line("0: " + x
		in.write_line(x
	try
		assert(read() == "220 SMTP pseudo server\n"
		write("HELO localhost"
		assert(read() == "250 Hello localhost\n"
		write("MAIL FROM: foo@foo.com"
		assert(read() == "250 Sender foo@foo.com\n"
		write("RCPT TO: bar@bar.com"
		assert(read() == "250 Recipient bar@bar.com\n"
		write("RCPT TO: zot@zot.com"
		assert(read() == "250 Recipient zot@zot.com\n"
		write("DATA"
		assert(read() == "354 Enter mail, end with \".\"\n"
		write("Hello everyone,"
		write("This is shin."
		write("Good bye."
		write("."
		assert(read() == "250 Accepted\n"
		write("QUIT"
		assert(read() == "221 SMTP pseudo server closing\n"
	catch Throwable t
		system.error.write_line(t
		t.dump(
	finally
		child.wait(
	read_log(
	system.out.flush(
	assert(log == "start: foo\nqueued:\n\tReceived: from localhost\n\tFrom: foo@foo.com\n\tTo: bar@bar.com\n\tTo: zot@zot.com\n\t\n\tHello everyone,\n\tThis is shin.\n\tGood bye.\n\ndone.\n"
finally
	child.close(
assert(child.exited() == 0
assert(child.signaled() === null
